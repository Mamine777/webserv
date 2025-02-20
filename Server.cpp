/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 17:19:18 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/20 17:46:34 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include <errno.h>
#include <poll.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <exception>
#include <iostream>
#include <vector>

#include "Header.hpp"
#include "MessageException.hpp"
#include "Request.hpp"

void Server::dispatchRequest(Request &req, Response &res) {
	std::map<std::string, void (*)(Request &, Response &)>::iterator it;
	if (req.getHeader().getMethod() == "GET")
		it = this->getMap.find(req.getHeader().getRessource());
	if (req.getHeader().getMethod() == "POST")
		it = this->postMap.find(req.getHeader().getRessource());
	if (it == this->postMap.end() || it == this->getMap.end())
		std::cout << "404 Ressource not Found" << std::endl;
	else
		(*it).second(req, res);
}

std::vector<int> &Server::getServerSocks() {
	return (this->serverSocks);
}

Server::Server() {}

Server::~Server() {}

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

void Server::addPort(uint16_t port) {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) {
		throw MessageException(strerror(errno));
	}

	int opt = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		close(serverSocket);
		throw MessageException(strerror(errno));
	} /*
	 if (::listen(serverSocket, 5) < 0) {
		 close(serverSocket);
		 throw MessageException(strerror(errno));
	 } */

	this->serverSocks.push_back(serverSocket);

	//std::clog << "Server listening on port " << port << std::endl;

	/*// TODO: pushback all the sockets from the serverSocket Vector
	std::vector<pollfd> fds;
	fds.push_back((pollfd){this->serverSocket, POLLIN, 0});
	while (true) {
		if (poll(fds.data(), fds.size(), -1) < 0)
			throw MessageException(strerror(errno));

		for (size_t i = 0; i < fds.size(); i++) {
			if (fds[i].fd == this->serverSocket && fds[i].revents & POLLIN) {
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
				send(fds[i].fd, response.c_str(), response.size(), 0);
				close(fds[i].fd);
				fds.erase(fds.begin() + i);
				i--;
			}
		}
	}
	*/
}

void Server::get(std::string path, void (*f)(Request &req, Response &res)) {
	this->getMap.insert(std::make_pair(path, f));
}
