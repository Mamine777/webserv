/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 17:19:18 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/26 00:08:49 by fghysbre         ###   ########.fr       */
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
	
	if (req.getHeader().getMethod() == "GET") {
		std::vector<StaticHandler>::iterator statit = this->statics.begin();
		for (; statit != this->statics.end(); ++statit) {
			if (!req.getHeader().getRessource().compare(0, (*statit).getUriPath().length(), (*statit).getUriPath())) {
				(*statit).execute(req, res);
				return ;
			}
		}
		std::vector<AutoIndexHandler>::iterator indexit = this->autoindexs.begin();
		for (; indexit != this->autoindexs.end(); ++ indexit) {
			if (!req.getHeader().getRessource().compare(0, (*indexit).getUriPath().length(), (*indexit).getUriPath())) {
				(*indexit).execute(req, res);
				return ;
			}
		}
		it = this->getMap.find(req.getHeader().getRessource());
	}
	if (req.getHeader().getMethod() == "POST")
		it = this->postMap.find(req.getHeader().getRessource());
	if (it == this->postMap.end() || it == this->getMap.end())
		res.status(404).sendText("404 Ressource not Found");
	else
		(*it).second(req, res);
}

std::vector<int> &Server::getServerSocks() {
	return (this->serverSocks);
}

void Server::get(std::string path, void (*f)(Request &req, Response &res)) {
	this->getMap.insert(std::make_pair(path, f));
}

void Server::serveStatic(std::string urlPath, std::string rootPath, std::string defhtml) {
	this->statics.push_back(StaticHandler(urlPath, rootPath, defhtml));
	this->locations.push_back(urlPath);
}

void Server::serveAutoIndex(std::string urlPath, std::string rootPath) {
	this->autoindexs.push_back(AutoIndexHandler(urlPath, rootPath));
	this->locations.push_back(urlPath);
}

ServerConfig &Server::getConfig() {
    return this->config;
}

Server::Server(ServerConfig &config): config(config) {}

Server::~Server() {}

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
	} 
	this->serverSocks.push_back(serverSocket);
}
