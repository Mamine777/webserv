/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 18:55:28 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/28 22:43:22 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../inc/Response.h"
#include <string>
#include "../inc/cgi.h"

server::server(ServerConfig &config) : _config(config) {
}

server::~server() {}

bool	findout(std::string Method, std::vector<std::string> allowed_method){

	for (size_t i = 0 ; i < allowed_method.size(); i++)
		if (Method == allowed_method[i])
			return true;
	return false;

}

bool	request_checking(std::string path)
{
	if (path.find("src") != std::string::npos || path.find("inc") != std::string::npos ||
		path.find("Makefile") != std::string::npos)
		return false;
	return true;
}

void handleMethod(LocConfig *location, Response &response, Request &req, cgi &CGI)
{
	if (req.getMethod() == "GET" && findout("GET", location->allowed_methods)){
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
	else if (req.getMethod() == "POST" && findout("POST", location->allowed_methods)) {
	//implement Post
	}
	else if (req.getMethod() == "DELETE" && findout("DELETE", location->allowed_methods)){
		std::string filePath = "." + req.getPath();
		if (!request_checking(req.getPath())){
			response.setStatus(403);
			response.setBody("Forbidden");
		}
		else{
			if (remove(filePath.c_str()) == 0) {
			response.setStatus(200);
			response.setBody("File deleted successfully");}
		}
	}
	else
	{
		response.setStatus(405);
		response.setBody("405 Method Not Allowed");
	}
}

void server::setupServer(uint16_t port){
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) {
		throw std::runtime_error(strerror(errno));
	}

	int opt = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
		close(serverSocket);
		throw std::runtime_error(strerror(errno));
	}
	this->_serverSockets.push_back(serverSocket);
}

void server::dispatchRequest(Request& req, Response& res) {
    std::string	longest = "";
	LocConfig	*loc;
	for (size_t i = 0; i < this->_config.locations.size(); i++)
	{
		if (req.getPath().substr(0, this->_config.locations[i].path.size()) == this->_config.locations[i].path)
		{
			if (this->_config.locations[i].path.size() > longest.size()) {
				loc = &this->_config.locations[i];
				longest = this->_config.locations[i].path;
			}
		}
	}
	if (!loc) {
		res.setStatus(404);
		res.setBody("404 Not Found");
		return ;
	}
	
	cgi	CGI;
	if (req.getMethod() == "GET" || req.getMethod() == "POST" || req.getMethod() == "DELETE")
		handleMethod(loc, res, req, CGI);
	else {
		res.setStatus(405);
		res.setBody("405 Method Not Allowed");
		return ;
	}
	if (req.getPath() == "/" && loc->path == "/")
	{
		std::string contentType = getContentType(loc->index);
		res.setHeader("Content-Type", contentType);
	}
	else
	{
		std::string contentType = getContentType(req.getPath());
		res.setHeader("Content-Type", contentType);
	}
	
	res.setRetVal(res.toString());
}

