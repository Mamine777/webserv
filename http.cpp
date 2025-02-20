/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:25:34 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/20 17:54:42 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http.hpp"

#include <poll.h>
#include <string.h>

#include <errno.h">

#include "MessageException.hpp"

void http::addServer(Server &server) {
	this->addServer(server);
}

void closeSockets(std::vector<Server &>::iterator servers) {
}

bool isServerSock(std::vector<int> fds, int fd) {
	std::vector<int>::iterator	it = fds.begin();
	for (; it != fds.end(); ++it) {
		if (*it == fd) return true;
	}
	return false;
}

void http::start() {
	//TODO Change this to std::vector<std::pair<serv, pollfd>>
	std::vector<pollfd> fds;
	std::vector<int> serverfds;
	std::vector<Server &>::iterator it = this->servers.begin();
	for (; it != this->servers.end(); ++it) {
		std::vector<int>::iterator iit = (*it).getServerSocks().begin();
		for (; iit != (*it).getServerSocks().end(); ++iit) {
			if (listen((*iit), 5) < 0) {
				closeSockets(this->servers.begin());
				throw MessageException(strerror(errno));
			}
			fds.push_back((pollfd){(*iit), POLLIN, 0});
			serverfds.push_back((*iit));
			std::clog << "Server listening on new port" << port << std::endl;
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
					accept(this->serverSocket, (struct sockaddr *)&clientAddr,
						   &clientLen);

				if (clientSocket == -1) {
					std::cerr << "Failed to accept client" << std::endl;
					continue;
				}
				std::clog << "New client connected: " << clientSocket
						  << std::endl;
				fds.push_back((pollfd){clientSocket, POLLIN, 0});
			} else if (fds[i].revents & POLLIN) {
				std::string reqbuff = recieveData(fds[i].fd);
				Request req(reqbuff);
				Response res(fds[i].fd, req.getHeader().getHttpVer());
				this->dispatchRequest(req, res);
				/* std::string response =
					"HTTP/1.1 404 OK\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: 13\r\n"
					"Connection: close\r\n"
					"\r\n"
					"Hello, world!";
				send(fds[i].fd, response.c_str(), response.size(), 0);*/
				close(fds[i].fd);
				fds.erase(fds.begin() + i);
				i--;
			}
		}
	}
}