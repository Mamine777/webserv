/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 17:19:18 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/18 17:28:54 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <exception>
#include "MessageException.hpp"
#include <string.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <poll.h>
#include "Header.hpp"
#include "Request.hpp"

void Server::dispatchRequest(Request &req, Response &res) {
	std::map<std::string, void(*)(Request&, Response&)>::iterator it;
	if (req.getHeader().getMethod() == "GET")
		it = this->getMap.find(req.getHeader().getRessource());
	if (req.getHeader().getMethod() == "POST")
		it = this->postMap.find(req.getHeader().getRessource());
	if (it == this->postMap.end() || it == this->getMap.end())
		std::cout << "404 Ressource not Found" << std::endl;
	else
		(*it).second(req, res);
}

Server::Server() : serverSocket(socket(AF_INET, SOCK_STREAM, 0))
{
	if (this->serverSocket < 0) {
		throw MessageException(strerror(errno));
	}

	int opt = 1;
	setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

Server::~Server() {}

static std::string	recieveData(int clientSocket) {
	char		buff[1024];
	std::string	ret;
	int			bytesRead;

	while ((bytesRead = recv(clientSocket, buff, sizeof(buff), 0)) > 0) {
		ret.append(buff, bytesRead);
		if (ret.find("\r\n\r\n") != std::string::npos)
			break ;
	}
	if (bytesRead == 0)
		std::clog << "Client closed connection: " << clientSocket << std::endl;
	else if (bytesRead < 0)
		std::cerr << "Error recieving data" << std::endl;
	return (ret);
}

void Server::listen(uint16_t port) {
	this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_port = htons(port);
	this->serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr)) < 0) {
		close(serverSocket);
		throw MessageException(strerror(errno));
	}
	if (::listen(this->serverSocket, 5) < 0) {
		close(serverSocket);
		throw MessageException(strerror(errno));
	}

	std::clog << "Server listening on port " << port << std::endl;

	//TODO: pushback all the sockets from the serverSocket Vector
	std::vector<pollfd> fds;
	fds.push_back((pollfd) {this->serverSocket, POLLIN, 0});
	while (true) {
		if (poll(fds.data(), fds.size(), -1) < 0)
			throw MessageException(strerror(errno));
		
		for (size_t i = 0; i < fds.size(); i++) {
			if (fds[i].fd == this->serverSocket && fds[i].revents & POLLIN) {
				sockaddr_in	clientAddr;
				socklen_t	clientLen = sizeof(clientAddr);
				int			clientSocket = accept(this->serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
				
				if (clientSocket == -1) {
					std::cerr << "Failed to accept client" << std::endl;
					continue;
				}
				std::clog << "New client connected: " << clientSocket << std::endl;
				fds.push_back((pollfd) {clientSocket, POLLIN, 0});
			} else if (fds[i].revents & POLLIN) {
				std::string	reqbuff = recieveData(fds[i].fd);
				Request		req(reqbuff);
				Response	res(fds[i].fd, req.getHeader().getHttpVer());
				this->dispatchRequest(req, res);
				/* std::string response =
					"HTTP/1.1 404 OK\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: 13\r\n"
					"Connection: close\r\n"
					"\r\n"
					"Hello, world!";
				send(fds[i].fd, response.c_str(), response.size(), 0); */
				close(fds[i].fd);
				fds.erase(fds.begin() + i);
				i--;
			}
		}
	}
}

void Server::get(std::string path, void (*f)(Request &req, Response &res)) {
	this->getMap.insert(std::make_pair(path, f));
}
