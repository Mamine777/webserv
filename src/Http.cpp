/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:25:34 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/27 16:41:36 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"

#include <errno.h>
#include <poll.h>
#include <string.h>

#include <algorithm>
#include <map>
#include <sstream>

#include "MessageException.hpp"
#include "unistd.h"

Http::Http() {
}

Http::~Http() {
}

/* static std::string recieveData(int clientSocket) {
	char buff[1024];
	std::string ret;
	int bytesRead = 0;
	while ((bytesRead = recv(clientSocket, buff, sizeof(buff), 0)) > 0) {
		ret.append(buff, bytesRead);
		if (ret.find("\r\n\r\n") != std::string::npos)
			break;
	}
	if (bytesRead == 0)
		std::clog << "Client closed connection: " << clientSocket << std::endl;
	else if (bytesRead < 0)
		std::cerr << "Error receiving data" << std::endl;
	return ret;
} */

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
		req.getHeader().parseHead(req.getRawReq());

		std::cout << "Header: " << req.getHeader().getHttpVer() << " " << req.getHeader().getRessource();
		req.setBodySize(req.getRawReq().size() - req.getRawReq().find("\r\n\r\n") + 4);

		if (req.getHeader().getField("Content-Length").empty()) {
			req.setFinishBody(true);
			fd.events = POLLOUT;
		}

		std::string expect(req.getHeader().getField("Expect"));
		unsigned int code = 0;
		if (!expect.empty()) {
			std::stringstream expss(expect);
			expss >> code;
			std::string tmpHeader = ResHeader::expectHeader(req.getHeader().getHttpVer(), code);
			std::cout << tmpHeader << std::endl;
			send(clientSocket, tmpHeader.c_str(), tmpHeader.length(), 0);
		}
	}
}

static void recieveBody(int clientSocket, Request &req, pollfd &fd) {
	std::cout << "Ready to read body for ress" << req.getHeader().getRessource() <<std::endl;
	std::string	contLenStr = req.getHeader().getField("Content-Length");
	unsigned int contLen = 0;
	if (!contLenStr.empty()) {
		std::stringstream ss(contLenStr);
		ss >> contLen;
	} else {
		fd.events = POLLIN | POLLOUT;
		req.setFinishBody(true);
		return ;
	}
	
	char buff[1024];
	int bytesRead;
	
	if (req.getBodySize() < static_cast<size_t>(contLen)) {
		bytesRead = recv(clientSocket, buff, sizeof(buff), 0);
		std::cout << "recv return value (body) = " << bytesRead << std::endl;
		if (bytesRead == 0) {
			std::clog << "finised reading" << std::endl;
			req.setFinishBody(true);
			fd.events = POLLIN | POLLOUT;
			req.getRawBody() = req.getRawReq().substr(req.getRawReq().find("\r\n\r\n") + 4);
			return ;	
		} if (bytesRead < 0) {
			fd.events = POLLIN | POLLOUT;
			std::cerr << "Error when geting body from request" << std::endl;
			return ;
		}
		req.getRawReq().append(buff, bytesRead);
		req.setBodySize(req.getBodySize() + bytesRead);
	} if (req.getBodySize() >= static_cast<size_t>(contLen)) {
		std::clog << "finised reading" << std::endl;
		req.setFinishBody(true);
		fd.events = POLLIN | POLLOUT;
		req.getRawBody() = req.getRawReq().substr(req.getRawReq().find("\r\n\r\n") + 4);
	}
}

std::vector<Server *>::iterator getServerFromSocket(std::vector<Server *> &servers, int fd) {
	std::vector<Server *>::iterator it;
	for (it = servers.begin(); it != servers.end(); ++it) {
		Server *serv = *it;
		std::vector<int> serverSocks = serv->getServerSocks();
		if (std::find(serverSocks.begin(), serverSocks.end(), fd) != serverSocks.end())
			return it;
	}
	return servers.end();
}

void closeSockets(std::vector<Server *>::iterator servers) {
	(void)servers;
}

void Http::addServer(Server *server) {
	this->servers.push_back(server);
}

void Http::start() {
	std::vector<struct pollfd> fds;
	std::vector<int> serverfds;
	std::map<int, Server *> requestServer;
	std::map<int, Request> requests;

	std::vector<Server *>::iterator it;
	for (it = this->servers.begin(); it != this->servers.end(); ++it) {
		std::vector<int> serverSocks = (*it)->getServerSocks();
		std::vector<int>::iterator iit;
		for (iit = serverSocks.begin(); iit != serverSocks.end(); ++iit) {
			if (listen(*iit, 5) < 0) {
				closeSockets(this->servers.begin());
				throw MessageException(strerror(errno));
			}
			fds.push_back((pollfd){*iit, POLLIN | POLLOUT, 0});
			serverfds.push_back(*iit);
		}
	}

	while (true) {
		if (poll(&fds[0], fds.size(), -1) < 0)
			throw MessageException(strerror(errno));

		for (size_t i = 0; i < fds.size();) {
			int currentFD = fds[i].fd;
			if ((fds[i].revents & POLLIN)) {
				if (std::find(serverfds.begin(), serverfds.end(), currentFD) != serverfds.end()) {
					sockaddr_in clientAddr;
					socklen_t clientLen = sizeof(clientAddr);
					int clientSocket = accept(currentFD, (struct sockaddr *)&clientAddr, &clientLen);
					if (clientSocket == -1) {
						++i;
						continue;
					}
					fds.push_back((pollfd){clientSocket, POLLIN, 0});

					std::vector<Server *>::iterator serverIt = getServerFromSocket(this->servers, currentFD);
					if (serverIt == this->servers.end())
						throw MessageException(std::string("No server found for socket"));
					requestServer.insert(std::make_pair(clientSocket, *serverIt));
					requests[clientSocket] = Request();
					std::cout << "New connection" << std::endl;
				} else {
					std::cout << "about to read shii" << std::endl;
					std::map<int, Request>::iterator reqIt = requests.find(currentFD);
					if (reqIt == requests.end())
						continue;
					Request &req = (*reqIt).second;
					if (!req.getFinishHead())
						recieveHead(currentFD, req, fds[i]);
					else if (!req.getFinishBody())
						recieveBody(currentFD, req, fds[i]);
					/* std::string reqbuff = recieveData(currentFD);
					if (!reqbuff.empty()) {
						Request req(reqbuff, currentFD);
						Response res(currentFD, req.getHeader().getHttpVer());
						std::map<int, Server*>::iterator servIt = requestServer.find(currentFD);
						if (servIt == requestServer.end())
							throw MessageException(std::string("Mixup in server sockets"));
						servIt->second->dispatchRequest(req, res);
					}
					close(currentFD);
					requestServer.erase(currentFD);
					fds[i] = fds.back();
					fds.pop_back();
					continue; */
				}
			}
			else if ((fds[i].revents & POLLOUT) && std::find(serverfds.begin(), serverfds.end(), currentFD) == serverfds.end()) {
				std::map<int, Request>::iterator reqIt = requests.find(currentFD);
				if (reqIt == requests.end())
					continue;
				Request &req = (*reqIt).second;
				if (!req.getFinishBody())
					continue;
				std::cout << "Finished reading, time to respond" << std::endl;
				
				Response res(currentFD, req.getHeader().getHttpVer());
				std::map<int, Server*>::iterator servIt = requestServer.find(currentFD);
				if (servIt == requestServer.end())
					throw MessageException(std::string("Mixup in server sockets"));
				servIt->second->dispatchRequest(req, res);
				close(currentFD);
				requestServer.erase(currentFD);
				requests.erase(currentFD);
				fds[i] = fds.back();
				fds.pop_back();
				continue ;
			}
			++i;
		}
	}
}