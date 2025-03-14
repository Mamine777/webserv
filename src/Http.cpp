#include "Http.h"

Http::Http() {}

Http::~Http() {
    std::vector<pollfd>::iterator fdIt = this->fds.begin();
    for (; fdIt != this->fds.end(); ++fdIt) {
        close(fdIt->fd);
    }

    std::map<int, std::vector<server *> >::iterator servIt =
        this->socket_server.begin();
    for (; servIt != this->socket_server.end(); ++servIt) {
        std::vector<server *>::iterator servIit = servIt->second.begin();
        for (; servIit != servIt->second.end(); ++servIit) {
            server *servptr = *servIit;
            delete servptr;
        }
    }
}

void Http::addport(uint16_t port, server *serv) {
    int serverSocket;
    if (this->port_socket.find(port) == this->port_socket.end()) {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            throw std::runtime_error(strerror(errno));
        }

        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(serverSocket, (struct sockaddr *)&serverAddr,
                 sizeof(serverAddr)) < 0) {
            close(serverSocket);
            throw std::runtime_error(strerror(errno));
        }
        this->port_socket[port] = serverSocket;
    } else {
        serverSocket = (*this->port_socket.find(port)).second;
    }
    this->socket_server[serverSocket].push_back(serv);
}

void closeSockets(std::map<int, std::vector<server *> > &socket_server) {
    std::map<int, std::vector<server *> >::iterator it = socket_server.begin();
    for (; it != socket_server.end(); ++it) {
        close(it->first);
    }
}

bool isNumeric(const std::string &s) {
    for (size_t i = 0; i < s.length(); i++) {
        if (!isdigit(s[i])) return false;
    }
    return !s.empty();
}

bool isIPv4(const std::string &host) {
    std::istringstream stream(host);
    std::string part;
    int count = 0;

    while (std::getline(stream, part, '.')) {
        if (++count > 4) return false;
        if (!isNumeric(part)) return false;
        int num = std::atoi(part.c_str());
        if (num < 0 || num > 255) return false;
    }

    return count == 4;
}

std::string getHostName(Request &req) {
    if (req.getHeader("Host").empty()) return ("");
    std::string hostname = req.getHeader("Host");
    hostname = hostname.substr(0, hostname.find(":"));
    if (hostname == "localhost") return ("");
    if (isIPv4(hostname)) return ("");
    return (hostname);
}

server *getServerFromSocket(std::vector<server *> &servers, Request &req) {
    server *serv = NULL;
    std::string saveLoc = "";
    std::string hostname = getHostName(req);
    std::vector<server *>::iterator servIt = servers.begin();
    for (; servIt != servers.end(); ++servIt) {
        if (!(*servIt)->getConfig().server_name.empty() && !hostname.empty() &&
            hostname != (*servIt)->getConfig().server_name)
            continue;
        std::vector<LocConfig>::iterator locIt =
            (*servIt)->getConfig().locations.begin();
        for (; locIt != (*servIt)->getConfig().locations.end(); ++locIt) {
            if ((!req.getPath().compare(0, locIt->path.size(), locIt->path) &&
                 (locIt->path.size() == req.getPath().size() ||
                  req.getPath()[locIt->path.size()] == '/') &&
                 locIt->path.size() > saveLoc.size()) ||
                (locIt->path == "/" && saveLoc.empty())) {
                serv = (*servIt);
                saveLoc = (*locIt).path;
            }
        }
    }
    return serv;
}

static void sendResponse(int clientSocket, Response &res,
                         std::map<int, Http::clientSteps> &steps) {
    size_t actbytes = res.getRetVal().size() - res.getIndex();
    actbytes = actbytes > 2048 ? 2048 : actbytes;
    if (actbytes <= 0) {
        res.done();
        return;
    }
    if (res.getIndex() > res.getRetVal().size()) {
        res.done();
        return;
    }
    int bytes;
    if ((bytes = send(clientSocket,
                      res.getRetVal().substr(res.getIndex(), 2048).c_str(),
                      actbytes, 0)) <= 0) {
        if (bytes == -1) {
            steps[clientSocket] = Http::CS_ERROR;
            std::cerr << "Failed to sent data" << std::endl;
            return;
        } else if (bytes == 0) {
            std::clog << "Warning: Client closed connection when more bytes "
                         "were supposed to be sent"
                      << std::endl;
            res.done();
            return;
        }
    }

    res.setIndex(res.getIndex() + actbytes);
    if (actbytes <= 0) res.done();
}

void Http::recieveReq(int fd, Request &req,
                      std::map<int, Http::clientSteps> &steps,
                      std::map<int, Response> &responses, int serverSock) {
    std::string contLenStr;
    unsigned int contLen;
    if (req.getFinishHead()) {
        contLenStr = req.getHeader("Content-Length");
        if (!contLenStr.empty()) {
            std::stringstream ss(contLenStr);
            ss >> contLen;
        } else {
            req.setFinishBody(true);
            steps[fd] = Http::CS_WRITE;
            return;
        }
    }

    char buff[1024];
    int bytesRead;
    if (!req.getFinishHead() ||
        req.getBodySize() < static_cast<size_t>(contLen)) {
        bytesRead = recv(fd, buff, sizeof(buff), 0);
        if (bytesRead < 0) {
            steps[fd] = Http::CS_ERROR;
            std::cerr << "Error recieving data" << std::endl;
            return;
        }
        req.getRawReq().append(buff, bytesRead);
        if (!req.getFinishHead() &&
            (req.getRawReq().find("\r\n\r\n") != std::string::npos ||
             bytesRead == 0)) {
            req.setFinishHead(true);
            req.parse(req.getRawReq());

            req.setBodySize(req.getRawReq().size() -
                            req.getRawReq().find("\r\n\r\n") + 4);
            if (req.getHeader("Content-Length").empty()) {
                req.setFinishBody(true);
                steps[fd] = Http::CS_WRITE;
            }

            server *serv =
                getServerFromSocket(this->socket_server[serverSock], req);
            if (!serv) {
                responses[fd] = Response(req.getVersion());
                req.setFinishBody(true);
                steps[fd] = Http::CS_WRITE;
                this->errorPage(404, responses[fd], NULL);
                return;
            }

            contLenStr = req.getHeader("Content-Length");
            std::istringstream contLenss(contLenStr);
            contLenss >> contLen;
            if (static_cast<size_t>(contLen) >
                serv->getConfig().client_max_body_size) {
                responses[fd] = Response(req.getVersion());
                req.setFinishBody(true);
                steps[fd] = Http::CS_WRITE;
                this->errorPage(413, responses[fd], serv);
                return;
            }

            std::string expect(req.getHeader("Expect"));
            unsigned int code = 0;
            if (!expect.empty()) {
                responses[fd] = Response(req.getVersion());
                req.setFinishBody(true);
                steps[fd] = Http::CS_WRITE;
                Response &res = responses[fd];
                std::stringstream expss(expect);
                expss >> code;
                res.setRetVal(Response::expectHeader(req.getVersion(), code));
                res.setFinal(false);
                return;
            }
        } else if (req.getFinishHead() && bytesRead > 0)
            req.setBodySize(req.getBodySize() + bytesRead);
    }
    if (req.getFinishHead() &&
        (req.getBodySize() >= static_cast<size_t>(contLen) || bytesRead == 0)) {
        req.setFinishBody(true);
        steps[fd] = Http::CS_WRITE;
        if (req.getRawReq().find("\r\n\r\n") + 4 <= req.getRawReq().size())
            req.setBody(
                req.getRawReq().substr(req.getRawReq().find("\r\n\r\n") + 4));
    }
}

// TODO: return false if request conbt bigger than configed

void Http::start() {
    size_t numServFds = 0;
    std::map<int, int> client_server;
    std::map<int, Request> requests;
    std::map<int, Response> responses;
    std::map<int, clientSteps> steps;

    std::map<int, std::vector<server *> >::iterator it =
        this->socket_server.begin();
    for (; it != this->socket_server.end(); ++it) {
        int servSock = it->first;
        if (listen(servSock, 20) < 0) {
            closeSockets(this->socket_server);
            throw std::runtime_error(strerror(errno));
        }
        this->fds.push_back((pollfd){servSock, POLLIN | POLLOUT, 0});
        numServFds++;
    }
    while (runServ) {
        if (poll(&this->fds[0], this->fds.size(), -1) < 0) {
            closeSockets(this->socket_server);
            throw std::runtime_error(strerror(errno));
        }

        for (size_t i = 0; i < this->fds.size();) {
            int currentFd = this->fds[i].fd;
            if (this->fds[i].revents & POLLIN &&
                (i < numServFds || steps[currentFd] == CS_READ)) {
                if (i < numServFds) {
                    sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
                    int clientSock = accept(
                        currentFd, (struct sockaddr *)&clientAddr, &clientLen);
                    if (clientSock == -1) {
                        std::cerr << "Failed to accept user connection..."
                                  << std::endl;
                        ++i;
                        continue;
                    }
                    this->fds.push_back(
                        (pollfd){clientSock, POLLIN | POLLOUT, 0});
                    steps[clientSock] = CS_INIT;
                    client_server.insert(std::make_pair(clientSock, currentFd));
                    requests[clientSock] = Request();
                    steps[clientSock] = CS_READ;
                } else {
                    std::map<int, Request>::iterator reqIt =
                        requests.find(currentFd);
                    if (reqIt == requests.end()) {
                        steps[currentFd] = CS_WRITE;
                        responses[currentFd] = Response("HTTP/1.1");
                        this->errorPage(500, responses[currentFd], NULL);
                        continue;
                    }
                    Request &req = reqIt->second;
                    if (!req.getFinishHead() || !req.getFinishBody())
                        recieveReq(currentFd, req, steps, responses,
                                   client_server[currentFd]);
                    if (steps[currentFd] == CS_ERROR) {
                        close(currentFd);
                        client_server.erase(currentFd);
                        requests.erase(currentFd);
                        this->fds[i] = this->fds.back();
                        this->fds.pop_back();
                        continue;
                    }
                }
            } else if (this->fds[i].revents & POLLOUT && i >= numServFds &&
                       steps[currentFd] == CS_WRITE) {
                std::map<int, Response>::iterator resIt =
                    responses.find(currentFd);
                if (resIt == responses.end()) {
                    std::map<int, Request>::iterator reqIt =
                        requests.find(currentFd);
                    if (reqIt == requests.end()) {
                        responses[currentFd] = Response("HTTP/1.1");
                        this->errorPage(500, responses[currentFd], NULL);
                        continue;
                    }
                    Request &req = reqIt->second;
                    if (!req.getFinishHead() || !req.getFinishBody()) continue;
                    responses[currentFd] = Response(req.getVersion());
                    resIt = responses.find(currentFd);
                    Response &res = resIt->second;
                    std::map<int, int>::iterator servIt =
                        client_server.find(currentFd);
                    if (servIt == client_server.end()) {
                        this->errorPage(500, res, NULL);
                        continue;
                    }
                    server *serv = getServerFromSocket(
                        this->socket_server[servIt->second], req);
                    if (serv == NULL) {
                        std::cerr << "Server Not Found" << std::endl;
                        this->errorPage(404, res, NULL);
                        continue;
                    }
                    serv->dispatchRequest(req, res);
                }
                sendResponse(currentFd, resIt->second, steps);
                if (resIt->second.isdone() || steps[currentFd] == CS_ERROR) {
                    if (resIt->second.isFinal() ||
                        steps[currentFd] == CS_ERROR) {
                        close(currentFd);
                        client_server.erase(currentFd);
                        requests.erase(currentFd);
                        responses.erase(currentFd);
                        this->fds[i] = this->fds.back();
                        this->fds.pop_back();
                        continue;
                    } else if (!resIt->second.isFinal()) {
                        steps[currentFd] = CS_READ;
                        requests.find(currentFd)->second.setFinishBody(false);
                        responses.erase(currentFd);
                    }
                }
            }
            ++i;
        }
    }
}

void Http::errorPage(int code, Response &res, server *server) {
    if (server) {
        server->errorPage(code, res);
        return;
    }

    std::string fileStr;
    std::string filePath = "errors/template.html";
    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
    std::stringstream ss;

    if (file.fail()) {
        std::cerr << "Failed to open template error page" << std::endl;
        return;
    }
    ss << file.rdbuf();
    file.close();
    fileStr = ss.str();

    std::stringstream codess;
    codess << code;
    std::string strCode = codess.str();
    fileStr.replace(fileStr.find("{{.code}}"), 9, strCode);
    fileStr.replace(fileStr.find("{{.message}}"), 12,
                    Response::getStatusMessage(code));
    res.setBody(fileStr);
    res.setStatus(code);
    res.setType("text/html");
    std::stringstream lengthss;
    lengthss << fileStr.size();
    res.setHeader("Content-Length", lengthss.str());
    res.setHeader("Connection", "close");
    res.setRetVal(res.toString());
}
