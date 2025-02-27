/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 17:19:18 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/27 16:52:56 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include <errno.h>
#include <poll.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <vector>
#include <fstream>

#include "Header.hpp"
#include "MessageException.hpp"
#include "Request.hpp"

void Server::dispatchRequest(Request &req, Response &res) {
	std::map<std::string, void (*)(Request &, Response &)>::iterator it;
	std::string longest = this->getLongestLoc(req.getHeader().getRessource());
	std::cout << "Longest ====>>> " << longest << std::endl;

	if (req.getHeader().getMethod() == "GET") {
		std::vector<StaticHandler>::iterator statit = this->statics.begin();
		for (; statit != this->statics.end(); ++statit) {
			if ((*statit).getUriPath() == longest) {
				(*statit).execute(req, res);
				return;
			}
		}
		std::vector<AutoIndexHandler>::iterator indexit = this->autoindexs.begin();
		for (; indexit != this->autoindexs.end(); ++indexit) {
			if ((*indexit).getUriPath() == longest) {
				(*indexit).execute(req, res);
				return;
			}
		}
	}
	if (req.getHeader().getMethod() == "POST") {
		std::map<std::string, std::string>::iterator	postit = this->postLocations.find(longest);
		std::cout << "\"" << this->postLocations[longest] << "\"" << std::endl;
		if (postit != this->postLocations.end()) {
			std::string path = req.getHeader().getRessource().replace(0, (*postit).first.length(), (*postit).second + "/");
			std::ofstream	file(path.c_str());
			if (file.fail()) {
				std::cout << strerror(errno) << std::endl;
				res.status(500).sendText("Internal Server Error");
				return ;
			}
			file << req.getRawBody();
			res.status(200).send();
			return ;
		}
	}
		res.status(404).sendText("404 Ressource not Found");
}

std::vector<int> &Server::getServerSocks() {
	return (this->serverSocks);
}

void Server::get(std::string path, void (*f)(Request &req, Response &res)) {
	this->getMap.insert(std::make_pair(path, f));
}

void Server::serveStatic(std::string urlPath, std::string rootPath, std::string defhtml) {
	this->statics.push_back(StaticHandler(urlPath, rootPath, defhtml));
	if (std::find(this->locations.begin(), this->locations.end(), urlPath) == this->locations.end())
		this->locations.push_back(urlPath);
}

void Server::serveAutoIndex(std::string urlPath, std::string rootPath) {
	this->autoindexs.push_back(AutoIndexHandler(urlPath, rootPath));
	if (std::find(this->locations.begin(), this->locations.end(), urlPath) == this->locations.end())
		this->locations.push_back(urlPath);
}

void Server::servePost(std::string urlPath, std::string uploadPath) {
	std::cout << "Added post for route " << urlPath << " to save at " << uploadPath << std::endl;
	this->postLocations[urlPath] = uploadPath;
	if (std::find(this->locations.begin(), this->locations.end(), urlPath) == this->locations.end())
		this->locations.push_back(urlPath);
}

std::string Server::getLongestLoc(std::string ressource) {
	std::string longest = "";
	std::vector<std::string>::iterator it = this->locations.begin();
	for (; it != this->locations.end(); ++it) {
		if (!ressource.compare(0, (*it).size(), (*it)) && (*it).length() > longest.length())
			longest = (*it);
	}
	return (longest);
}

ServerConfig *Server::getConfig() {
	return this->config;
}

Server::Server(ServerConfig *config) : config(config) {}

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
