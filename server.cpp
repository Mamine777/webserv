/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 18:55:28 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/26 14:05:48 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"
#include "ConfigParse.h"
#include "Response.h"
#include <string>
#include "cgi.h"

server::server(Config &config, ParseConfig &parser) : _config(config), configParse(parser) {
    setupServer();
}

server::~server() {}


void	handleMethod(LocConfig *location, Response &response, Request &req, cgi &CGI)
{
	if (req.getMethod() == "GET")
	{
		std::string filePath = "." + req.getPath();
		if (!location->cgi_pass.empty() && req.getPath().find(location->path) == 0) {
			std::string cgiOutput = CGI.executeCgi(location->cgi_pass, "");
			response.setStatus(200);
			response.setBody(cgiOutput);
			}
			else if (filePath == "./")
			{
				filePath += location->index;
				response.setBodyFromFile(filePath);
				if (response.getStatus() == 404)
					response.setBody("404 Not Found");
			}
			else {
				response.setBodyFromFile(filePath);
				if (response.getStatus() == 404)
					response.setBody("404 Not Found");
				else
					response.setStatus(200);
			}
	}
}

void server::setupServer(){
	int port;
	for (size_t i = 0; i < _config.servers.size(); i++) {
		port = _config.servers[i].port;
		int servSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (servSocket == -1)
			throw std::runtime_error("Failed to create socket");

		struct sockaddr_in serverAddr;
		int opt = 1;
		setsockopt(servSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = htons(port);

		if (bind(servSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) {
			close(servSocket);
			throw std::runtime_error("Failed to bind socket");
		}

		if (listen(servSocket, 10)) {
			close(servSocket);
			throw std::runtime_error("Failed to listen on socket");
		}

		_serverSockets.push_back(servSocket);

		struct pollfd serverPollfd;
		serverPollfd.fd = servSocket;
		serverPollfd.events = POLLIN;
		_pollfds.push_back(serverPollfd);

		std::cout << "Server is listening on port " << port << "..." << std::endl;
	}
}


void server::acceptConnection() {
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].revents & POLLIN)
		{
			struct sockaddr_in clientAddr;
			socklen_t clientAddrLen = sizeof(clientAddr);
			int clientSocket = accept(_pollfds[i].fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
			if (clientSocket < 0) {
				std::cerr << "Failed to accept connection on socket " << _pollfds[i].fd << std::endl;
				continue;
			}

			struct pollfd clientPollfd;
			clientPollfd.fd = clientSocket;
			clientPollfd.events = POLLIN;
			_pollfds.push_back(clientPollfd);

	 		ServerConfig *serverConfig = &_config.servers[i];
            _serverConfigs[clientSocket] = serverConfig;
			std::cout << "New connection accepted on socket " << _pollfds[i].fd << std::endl;
		}
	}
}

void	server::handleClient(int clientSocket)
{
	char buffer[2048];
	Request req;
	Response	response;
	cgi				CGI;

	ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesRead <= 0) {
		//erase the damn socket if failed
		close(clientSocket);
		std::cout << "Client disconnected (socket: " << clientSocket << ")" << std::endl;
		for (size_t i = 0;i < _pollfds.size(); i++)
		{
			if (_pollfds[i].fd == clientSocket)
			{
				_pollfds.erase(_pollfds.begin() + i);
					break;
			}
		}
		return ;
	}
	std::string request(buffer, bytesRead);
	//std::cout << request << std::endl;
	req.parse(request);
	ServerConfig *serverConfig = NULL;

	if (_serverConfigs.find(clientSocket) == _serverConfigs.end()) {
		std::cerr << "Erreur : clientSocket introuvable dans _serverConfigs !" << std::endl;
		return;
	}
	serverConfig = _serverConfigs[clientSocket];
	for (size_t i = 0; i <_serverSockets.size(); i++)
	{
		if (std::find(_serverSockets.begin(), _serverSockets.end(), clientSocket) != _serverSockets.end())
		{
			serverConfig = _serverConfigs[clientSocket];	
			break;
    	}
	}
	if (!serverConfig){response.setStatus(505), response.setBody("500 Internal Server Error");}

	LocConfig *location = NULL;
	size_t longestMatch = 0;

	for (size_t i = 0; i < serverConfig->locations.size(); i++)
	{
		if (req.getPath().substr(0, serverConfig->locations[i].path.size()) == serverConfig->locations[i].path)
		{
			if (serverConfig->locations[i].path.size() > longestMatch) {
				location = &serverConfig->locations[i];
				longestMatch = serverConfig->locations[i].path.size();
			}
		}
	}
	if (!location)
	{
		response.setStatus(404);
		response.setBody("404 Not Found");
	}
	else
	{
		if (req.getMethod() == "GET" || req.getMethod() == "POST" || req.getMethod() == "DELETE")
			handleMethod(location, response, req, CGI);
		else
		{
			response.setStatus(405);
			response.setBody("405 Method Not Allowed");
		}
	}
	if (req.getPath() == "/" && location->path == "/")
	{
		std::string contentType = getContentType(location->index);
		response.setHeader("Content-Type", contentType);
	}
	else
	{
		std::string contentType = getContentType(req.getPath());
		response.setHeader("Content-Type", contentType);	
	}
		

	std::string responseStr = response.toString();
	//std::cout << responseStr << std::endl;
	send(clientSocket, responseStr.c_str(), responseStr.size(), 0);
}

void server::start() {
	while (true) {
		int ret = poll(_pollfds.data(), _pollfds.size(), -1);
		if (ret == -1) {
			std::cerr << "poll() error: " << std::endl;
			continue;
		}

		for (size_t i = 0; i < _pollfds.size(); i++) {
			if (_pollfds[i].revents & POLLIN) {
				if (std::find(_serverSockets.begin(), _serverSockets.end(), _pollfds[i].fd) != _serverSockets.end()) {
					acceptConnection();
				} else {
					handleClient(_pollfds[i].fd);
				}
			}
		}
	}
}