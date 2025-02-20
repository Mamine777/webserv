/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:25:34 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/20 23:46:35 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <poll.h>
#include <string.h>
#include <errno.h>
#include "MessageException.hpp"
#include <algorithm>
#include <map>
#include "unistd.h"
#include "Http.hpp"

Http::Http() {
}

Http::~Http() {
}

void Http::addServer(Server *server) {
    this->servers.push_back(server);
}

//TODO: close all sockets when theres an error
void closeSockets(std::vector<Server *>::iterator servers) {
	(void)servers;
}

bool isServerSock(std::vector<int> fds, int fd) {
	std::vector<int>::iterator	it = fds.begin();
	for (; it != fds.end(); ++it) {
		if (*it == fd) return true;
	}
	return false;
}

std::vector<Server *>::iterator	getServerFromSocket(std::vector<Server *> servers, int fd) {
	std::vector<Server *>::iterator	it = servers.begin();
	for (; it != servers.end(); ++it) {
		Server	*serv = (*it);
		if (std::find(serv->getServerSocks().begin(), serv->getServerSocks().end(), fd) != serv->getServerSocks().end())
			return (it);
	}
	return (servers.end());
}

static std::string recieveData(int clientSocket) {
	char buff[1024];
	std::string ret;
	int bytesRead;

	while ((bytesRead = recv(clientSocket, buff, sizeof(buff), 0)) > 0) {
		ret.append(buff, bytesRead);
		if (ret.find("\r\n\r\n") != std::string::npos) break;
	}
	if (bytesRead == 0)
		std::clog << "Client closed connection: " << clientSocket << std::endl;
	else if (bytesRead < 0)
		std::cerr << "Error recieving data" << std::endl;
	return (ret);
}

void Http::start() {
	std::vector<pollfd> fds;
	std::vector<int> serverfds;
	std::vector<Server *>::iterator it = this->servers.begin();
	std::map<int, Server *>	requestServer;
	for (; it != this->servers.end(); ++it) {
		std::vector<int>::iterator iit = (*it)->getServerSocks().begin();
		for (; iit != (*it)->getServerSocks().end(); ++iit) {
			if (listen((*iit), 5) < 0) {
				closeSockets(this->servers.begin());
				throw MessageException(strerror(errno));
			}
			fds.push_back((pollfd){(*iit), POLLIN, 0});
			serverfds.push_back((*iit));
			std::clog << "Server listening on new port" << std::endl;
		}
	}
	while (true) {
		if (poll(fds.data(), fds.size(), -1) < 0)
			throw MessageException(strerror(errno));

		for (size_t i = 0; i < fds.size(); i++) {
			if (isServerSock(serverfds, fds[i].fd) && fds[i].revents & POLLIN) {
				sockaddr_in clientAddr;
				socklen_t clientLen = sizeof(clientAddr);
				int clientSocket =
					accept(fds[i].fd, (struct sockaddr *)&clientAddr,
						   &clientLen);

				if (clientSocket == -1) {
					std::cerr << "Failed to accept client" << std::endl;
					continue;
				}
				std::clog << "New client connected: " << clientSocket
						  << std::endl;
				fds.push_back((pollfd){clientSocket, POLLIN, 0});
				requestServer.insert(std::pair<int, Server *>(clientSocket, *(getServerFromSocket(this->servers, fds[i].fd))));
			} else if (fds[i].revents & POLLIN) {
				std::string reqbuff = recieveData(fds[i].fd);
				Request req(reqbuff);
				Response res(fds[i].fd, req.getHeader().getHttpVer());
				std::map<int, Server *>::iterator	servIt = requestServer.find(fds[i].fd);
				if (servIt == requestServer.end())
					throw MessageException(std::string("Mixup in server sockets"));
				(*servIt).second->dispatchRequest(req, res);
				close(fds[i].fd);
				fds.erase(fds.begin() + i);
				i--;
			}
		}
	}
}