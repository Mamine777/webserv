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
    std::cout << "[" << serv->getConfig().server_name << "] added to port "
              << port << " (socket: " << serverSocket << ")" << std::endl;
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
	if (req.getHeader("Host").empty())
		return ("");
	std::string hostname = req.getHeader("Host");
	hostname = hostname.substr(0, hostname.find(":"));
	if (hostname == "localhost") 
		return ("");
	if (isIPv4(hostname))
		return ("");
	return (hostname);
}

server *getServerFromSocket(std::vector<server *> &servers, Request &req) {
    server *serv = NULL;
    std::string saveLoc = "";
	std::string hostname = getHostName(req);
    std::clog << "hostname of request" << hostname << std::endl;
    std::vector<server *>::iterator servIt = servers.begin();
    for (; servIt != servers.end(); ++servIt) {
		if (!hostname.empty() && hostname != (*servIt)->getConfig().server_name)
            continue;
        std::cout << "Checking server for location "
                  << (*servIt)->getConfig().server_name << std::endl;
        std::vector<LocConfig>::iterator locIt =
            (*servIt)->getConfig().locations.begin();
        for (; locIt != (*servIt)->getConfig().locations.end(); ++locIt) {
            std::cout << "Checked location " << req.getPath()
                      << " ~= " << locIt->path << std::endl;
            if ((!req.getPath().compare(0, locIt->path.size(), locIt->path) && (locIt->path.size() == req.getPath().size() || req.getPath()[locIt->path.size()] == '/') && locIt->path.size() > saveLoc.size()) || (locIt->path == "/" && saveLoc.empty())) {
				std::clog << "replaced location \"" << saveLoc << "\" with \"" << locIt->path << "\"" << std::endl; 
                serv = (*servIt);
                saveLoc = (*locIt).path;
            }
        }
    }
    return serv;
}

static void sendResponse(int clientSocket, Response &res) {
    size_t actbytes = res.getRetVal().size() - res.getIndex();
    actbytes = actbytes > 2048 ? 2048 : actbytes;
    if (actbytes <= 0) {
        res.done();
        return;
    }
    if (send(clientSocket, res.getRetVal().substr(res.getIndex(), 2048).c_str(),
             actbytes, 0) == -1) {
        std::cerr << "Failed to sent data" << std::endl;
        return;
    }
    res.setIndex(res.getIndex() + actbytes);
    if (actbytes <= 0) res.done();
}

static void recieveReq(int fd, Request &req,
                       std::map<int, Http::clientSteps> &steps,
                       std::map<int, Response> &responses) {
    std::clog << "Recieving Data for request " << fd << std::endl;
    std::string contLenStr;
    unsigned int contLen;
    if (req.getFinishHead()) {
        contLenStr = req.getHeader("Content-Length");
        if (!contLenStr.empty()) {
            std::stringstream ss(contLenStr);
            ss >> contLen;
        } else {
            std::clog << "Request has no content, passing to response"
                      << std::endl;
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
        std::clog << "Read " << bytesRead << " bytes for request " << fd
                  << std::endl;
        if (bytesRead < 0) {
            // TODO: Remove client if failed to read;
            std::cerr << "Error recieving data" << std::endl;
        }
        req.getRawReq().append(buff, bytesRead);
        if (!req.getFinishHead() &&
            (req.getRawReq().find("\r\n\r\n") != std::string::npos ||
             bytesRead == 0)) {
            std::clog << "Found end of header for request " << fd << std::endl;
            req.setFinishHead(true);
            req.parse(req.getRawReq());

            req.setBodySize(req.getRawReq().size() -
                            req.getRawReq().find("\r\n\r\n") + 4);
            if (req.getHeader("Content-Length").empty()) {
                req.setFinishBody(true);
                steps[fd] = Http::CS_WRITE;
                std::clog << "No content for expected for request" << fd
                          << std::endl;
            }

            // TODO: move things arround for only one send
            std::string expect(req.getHeader("Expect"));
            unsigned int code = 0;
            if (!expect.empty()) {
                responses[fd] = Response(req.getVersion());
                req.setFinishBody(true);
                Response &res = responses[fd];
                std::stringstream expss(expect);
                expss >> code;
                std::clog << "Expect header found with code " << code
                          << " for request " << fd << std::endl;
                res.setRetVal(Response::expectHeader(req.getVersion(), code));
                res.setFinal(false);
                steps[fd] = Http::CS_WRITE;
                return;
            }
        } else if (req.getFinishHead() && bytesRead > 0)
            req.setBodySize(req.getBodySize() + bytesRead);
    }
    if (req.getFinishHead() &&
        (req.getBodySize() >= static_cast<size_t>(contLen) || bytesRead == 0)) {
        req.setFinishBody(true);
        steps[fd] = Http::CS_WRITE;
        std::clog << "Finished reading content for request " << fd << std::endl;
        req.setBody(
            req.getRawReq().substr(req.getRawReq().find("\r\n\r\n") + 4));
    }
}

// TODO: Make server work based on hostname
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
    std::cout << "started listening" << std::endl;
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
                    std::cout << "Client intercepted" << std::endl;
                    steps[clientSock] = CS_INIT;
                    client_server.insert(std::make_pair(clientSock, currentFd));
                    requests[clientSock] = Request();
                    steps[clientSock] = CS_READ;
                } else {
                    std::map<int, Request>::iterator reqIt =
                        requests.find(currentFd);
                    if (reqIt == requests.end()) {
                        std::clog << 1 << std::endl;
                        continue;
                    }
                    Request &req = reqIt->second;
                    std::clog << "current getFinishBody status => "
                              << req.getFinishBody() << std::endl;
                    if (!req.getFinishHead() || !req.getFinishBody())
                        recieveReq(currentFd, req, steps, responses);
                }
            } else if (this->fds[i].revents & POLLOUT && i >= numServFds &&
                       steps[currentFd] == CS_WRITE) {
                std::map<int, Response>::iterator resIt =
                    responses.find(currentFd);
                if (resIt == responses.end()) {
                    std::map<int, Request>::iterator reqIt =
                        requests.find(currentFd);
                    if (reqIt == requests.end()) continue;
                    Request &req = reqIt->second;
                    std::clog << "started making response for request "
                              << currentFd << std::endl;
                    if (!req.getFinishHead() || !req.getFinishBody()) continue;
                    responses[currentFd] = Response(req.getVersion());
                    resIt = responses.find(currentFd);
                    if (resIt == responses.end()) {
                        // repond with server error;
                        continue;
                    }
                    Response &res = resIt->second;
                    std::map<int, int>::iterator servIt =
                        client_server.find(currentFd);
                    if (servIt == client_server.end()) {
                        res.setStatus(404);
                        res.setBody("404 Not Found\n");
                        res.setRetVal(res.toString());
                        continue;
                    }
                    server *serv = getServerFromSocket(
                        this->socket_server[servIt->second], req);
                    if (serv == NULL) {
                        // make this work cuz it doesnt send 404 or nuhin just
                        // closes connection
                        std::cerr << "Server Not Found" << std::endl;
                        res.setStatus(404);
                        res.setBody("404 Not Found\n");
                        res.setRetVal(res.toString());
                        continue;
                    }
                    serv->dispatchRequest(req, res);
                }
                sendResponse(currentFd, resIt->second);
                if (resIt->second.isdone()) {
                    std::clog << "done sending response to request "
                              << currentFd;
                    if (!resIt->second.isFinal()) {
                        std::clog
                            << ", it was temp response, back to getting request"
                            << std::endl;
                        steps[currentFd] = CS_READ;
                        requests.find(currentFd)->second.setFinishBody(false);
                        std::clog
                            << "setting request finish body back to false => "
                            << requests.find(currentFd)->second.getFinishBody();
                        responses.erase(currentFd);
                    } else {
                        std::clog << ", it was final, removing client"
                                  << std::endl;
                        close(currentFd);
                        client_server.erase(currentFd);
                        requests.erase(currentFd);
                        responses.erase(currentFd);
                        this->fds[i] = this->fds.back();
                        this->fds.pop_back();
                        continue;
                    }
                }
            }
            ++i;
        }
    }
}