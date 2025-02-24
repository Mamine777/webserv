/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 15:09:08 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/24 14:32:28 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"
#include "ConfigParse.h"
#include "Response.h"
#include "cgi.h"

server::server(const std::string &configFile) : _configFile(configFile), _serverSocket(-1) {
	ConfigParser parser(configFile);
	_config = parser.getConfig();
	setupServer();
}

server::~server() {
	if (_serverSocket != -1)
		close(_serverSocket);
}

void    server::setupServer(){

	
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
		throw std::runtime_error("Failed to create socket");

	struct sockaddr_in serverAddr;

	int opt = 1;
	 setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(_config.port);

	if (bind(_serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) {
		close(_serverSocket);
		throw std::runtime_error("Failed to bind socket");
	}

	if (listen(_serverSocket, 10)) {
		close(_serverSocket);
		throw std::runtime_error("Failed to listen on socket");
	}

	struct pollfd serverPollfd;
	serverPollfd.fd = _serverSocket;
	serverPollfd.events = POLLIN;
	_pollfds.push_back(serverPollfd);
	std::cout << "Server is listening on port " << _config.port << "..." << std::endl;	
}

void	server::acceptConnection() {
	struct sockaddr_in clientAdr;

	socklen_t	clientAddrlen = sizeof(clientAdr);
	int clientSocket = accept(_serverSocket, (struct sockaddr *)&clientAdr, &clientAddrlen);
	if (clientSocket < 0) {std::cerr << "Failed to accept connection" << std::endl;
		return;
	}

	struct pollfd clientPollfd;
    clientPollfd.fd = clientSocket;
    clientPollfd.events = POLLIN;
    _pollfds.push_back(clientPollfd);

	std::cout << "New connection accepted" << std::endl;
	// std::string response =
	// 				"HTTP/1.1 404 OK\r\n"
	// 				"Content-Type: text/html\r\n"
	// 				"Content-Length: 13\r\n"
	// 				"Connection: close\r\n"
	// 				"\r\n"
	// 				"byebyye ma G";
	// send(clientSocket, response.c_str(), response.size(), 0); 
	// close(clientSocket);
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
	
	//std::cout << req.getMethod() << std::endl;
	if (req.getMethod() != "GET" && req.getMethod() != "DELETE" && req.getMethod() != "POST")
	{
		response.setStatus(405);
		response.setBody("405 Method Not Allowed");
	}
	else
	{
		if (req.getMethod() == "GET"){
			std::string	path_cgi =  "." + req.getPath();
			if (path_cgi == _config.CGI_file)
			{
				std::string cgiOutput = CGI.executeCgi(_config.CGI_file, "");
				response.setStatus(200);
                response.setBody(cgiOutput);
			}
			else
			{
				std::string filePath = "." + req.getPath();
					response.setBodyFromFile(filePath);
					if (response.getStatus() == 404)
						response.setBody("404 Not Found");
					else
						response.setStatus(200);
			}
		}
	}
	std::string contentType = getContentType(req.getPath());
    response.setHeader("Content-Type", contentType);

	std::string responseStr = response.toString();
	std::cout << responseStr << std::endl;
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
                if (_pollfds[i].fd == _serverSocket) {
                    acceptConnection();
                } else {
                    handleClient(_pollfds[i].fd);
                }
            }
        }
    }
}