#include "Http.h"

Http::Http() {}

Http::~Http() {}

void Http::addServer(server* server) {
	this->servers.push_back(server);
}

void	closeSockets(std::vector<server *> servers) {
	//TODO: do this atp ffs
	(void)servers;
}

std::vector<server *>::iterator getServerFromSocket(std::vector<server *> &servers, int fd) {
	std::vector<server *>::iterator it;
	for (it = servers.begin(); it != servers.end(); ++it) {
		server *serv = *it;
		std::vector<int> serverSocks = serv->getServerSocks();
		if (std::find(serverSocks.begin(), serverSocks.end(), fd) != serverSocks.end())
			return it;
	}
	return servers.end();
}

static void recieveHead(int clientSocket, Request &req, pollfd &fd) {
	char buff[1024];
	int bytesRead = 0;

	bytesRead = recv(clientSocket, buff, sizeof(buff), 0);
	if (bytesRead < 0) {
		std::cerr << "Error recieving data" << std::endl;
		return ;
	}
	req.getRawReq().append(buff, bytesRead);
	if (req.getRawReq().find("\r\n\r\n") != std::string::npos || bytesRead == 0) {
		req.setFinishHead(true);
		req.parse(req.getRawReq());

		req.setBodySize(req.getRawReq().size() - req.getRawReq().find("\r\n\r\n") + 4);
		if (req.getHeader("Content-Length").empty()) {
			req.setFinishBody(true);
			fd.events = POLLOUT;
		}

		std::string	expect(req.getHeader("Expect"));
		unsigned int code = 0;
		if (!expect.empty()) {
			std::stringstream expss(expect);
			expss >> code;
			std::string	tmpHeader = Response::expectHeader(req.getVersion(), code);
			send(clientSocket, tmpHeader.c_str(), tmpHeader.size(), 0);
		}
	}
}

static void recieveBody(int clientSocket, Request &req, pollfd &fd) {
	std::string contLenStr = req.getHeader("Content-Length");
	unsigned int contLen = 0;
	if (!contLenStr.empty()) {
		std::stringstream ss(contLenStr);
		ss >> contLen;
	} else {
		fd.events = POLLOUT;
		req.setFinishBody(true);
		return ;
	}

	char buff[1024];
	int bytesRead;
	if (req.getBodySize() < static_cast<size_t>(contLen)) {
		bytesRead = recv(clientSocket, buff, sizeof(buff), 0);
		if (bytesRead < 0)
			std::cerr << "Error recieving data" << std::endl;
		else if (bytesRead > 0) {
			req.getRawReq().append(buff, bytesRead);
			req.setBodySize(req.getBodySize() + bytesRead);
		}
	} if (req.getBodySize() >= static_cast<size_t>(contLen) || bytesRead == 0) {
		req.setFinishBody(true);
		fd.events = POLLOUT;
		req.setBody(req.getRawReq().substr(req.getRawReq().find("\r\n\r\n") + 4));
	}
}

static void	sendResponse(int clientSocket, Response &res) {
	std::clog << "Sending response: " << std::endl
		<< "- index: " << res.getIndex() << std::endl
		<< "- bytesLeft: " << res.getRetVal().size() - res.getIndex() << std::endl; 
	size_t	actbytes = res.getRetVal().size() - res.getIndex();
	actbytes = actbytes > 2048 ? 2048 : actbytes;
	if (actbytes <= 0) {
		res.done();
		return ;
	}
	if (send(clientSocket, res.getRetVal().substr(res.getIndex(), 2048).c_str(), actbytes, 0) == -1) {
		std::cerr << "Failed to sent data" << std::endl;
		return ;
	}
	res.setIndex(res.getIndex() + actbytes);
	if (actbytes <= 0)
		res.done();
}

void Http::start() {
	std::vector<struct pollfd> fds;
	size_t					numServFds = 0;
	std::map<int, server *>	requestServer;
	std::map<int, Request> requests;
	std::map<int, Response> responses;

	std::vector<server *>::iterator	it = this->servers.begin();
	for (; it != this->servers.end(); ++it) {
		std::vector<int> serverSocks = (*it)->getServerSocks();
		std::vector<int>::iterator iit = serverSocks.begin();
		for (; iit != serverSocks.end(); ++iit) {
			if (listen(*iit, 20) < 0) {
				closeSockets(this->servers);
				throw std::runtime_error(strerror(errno));
			}
			fds.push_back((pollfd) {*iit, POLLIN | POLLOUT, 0});
			numServFds++;
		}
	}

	while (true) {
		if (poll(&fds[0], fds.size(), -1) < 0) {
			closeSockets(this->servers);
			throw std::runtime_error(strerror(errno));
		}

		for (size_t i = 0; i < fds.size();) {
			int currentFd = fds[i].fd;
			if (fds[i].revents & POLLIN) {
				if (i < numServFds) {
					sockaddr_in	clientAddr;
					socklen_t	clientLen = sizeof(clientAddr);
					int 		clientSock = accept(currentFd, (struct sockaddr *) &clientAddr, &clientLen);
					if (clientSock == -1) {
						++i;
						continue;
					}
					fds.push_back((pollfd) {clientSock, POLLIN, 0});

					std::vector<server *>::iterator serverIt = getServerFromSocket(this->servers, currentFd);
					if (serverIt == this->servers.end()) {
						closeSockets(this->servers);
						throw std::runtime_error("No server found for socket");
					}
					requestServer.insert(std::make_pair(clientSock, *serverIt));
					requests[clientSock] = Request();
				} else {
					std::map<int, Request>::iterator reqIt = requests.find(currentFd);
					if (reqIt == requests.end())
						continue ;
					Request &req = (*reqIt).second;
					if (!req.getFinishHead())
						recieveHead(currentFd, req, fds[i]);
					else if (!req.getFinishBody())
						recieveBody(currentFd, req, fds[i]);
				}
			}
			else if ((fds[i].revents & POLLOUT) && i >= numServFds) {
				std::map<int, Response>::iterator resIt = responses.find(currentFd);
				if (resIt == responses.end()) {
					std::map<int, Request>::iterator reqIt = requests.find(currentFd);
					if (reqIt == requests.end())
						continue ;
					Request &req = (*reqIt).second;
					if (!req.getFinishBody() || !req.getFinishHead())
						continue;
					responses[currentFd] = Response(req.getVersion());
					resIt = responses.find(currentFd);
					if (resIt == responses.end()) {
						throw std::runtime_error("Error creating new element in responses list");
					}
					Response &res = (*resIt).second;
					std::map<int, server*>::iterator servIt = requestServer.find(currentFd);
					if (servIt == requestServer.end())
					throw std::runtime_error(std::string("Mixup in server sockets"));
					servIt->second->dispatchRequest(req, res);
					
				}
				sendResponse(currentFd, (*resIt).second);
				if ((*resIt).second.isdone()) {
					close(currentFd);
					requestServer.erase(currentFd);
					requests.erase(currentFd);
					responses.erase(currentFd);
					fds[i] = fds.back();
					fds.pop_back();
					continue;
				}
			}
			++i;
		}
	}
}
