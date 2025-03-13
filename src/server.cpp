/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 18:55:28 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/13 16:03:40 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../inc/Response.h"
#include <string>
#include "../inc/cgi.h"
#include "sys/stat.h"
#include "sys/types.h"
#include <dirent.h>
#include "server.h"

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
bool	hasDirTraversal(std::string path) {
	size_t	first = 0;
	for (; first < path.length();) {
		size_t	second = path.find('/', first + 1);
		std::string	temp = path.substr(first + 1, second - first - 1);
		if (temp == "..")
			return true;
		first = second;
	}
	return false;
}

void handleMethod(LocConfig *location, Response &response, Request &req, cgi &CGI)
{
	if (hasDirTraversal(req.getPath())) {
		response.setStatus(403);
		response.setBody("Forbidden Request");
		return ;
	}
	if (req.getMethod() == "GET" && findout("GET", location->allowed_methods)){
		/* std::string filePath = "." + req.getPath();
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
		} */
		if (!location->cgi_pass.empty()) {
			(void)CGI; //mute compilation error
			//TODO: Do something better than whats above, next time if you dont know just ask
		} else if (location->directory_listing) {
			std::string path = req.getPath().replace(0, location->path.size(), location->root + "/");
			struct stat s;
			if (stat(path.c_str(), &s) != 0) {
				response.setStatus(404);
				response.setBody("Couldn't find File");
			}
			if (s.st_mode & S_IFREG) {
				response.setBodyFromFile(path);
				response.setType(path.substr(path.find_last_of("/") + 1));
				return ;
			} else if (s.st_mode & S_IFDIR) {
				std::string ret = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n	<meta charset=\"UTF-8\">\n	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n	<title>Document</title>\n</head>\n<body>\n";
				ret += "	<h1>AutoIndex for: " + req.getPath() + "</h1>\n";
				DIR *dir;
				struct dirent *ent;
				if ((dir = opendir(path.c_str())) == NULL) {
					response.setStatus(500);
					response.setBody("Internal Error test");
					closedir(dir);
					return ;
				}
				ret += "	<ul>\n";
				while ((ent = readdir(dir)) != NULL) {
					if (location->path == req.getPath() && std::string(ent->d_name) == "..")
						continue ; 
					if (std::string(ent->d_name) == ".")
						continue ; 
					std::string	filename = ent->d_name;
					if (ent->d_type == DT_DIR)
						filename += "/";
					ret += "		<li><a href=\"" + req.getPath() + "/" + ent->d_name + "\">" + filename + "</a></li>\n";
				}
				ret += "	</ul>\n</body>\n</html>";
				closedir(dir);
				response.setType("text/html");
				std::cout << "ran this" << std::endl;
				response.setBody(ret);
			}
		}
		else if (!location->directory_listing && !location->root.empty()) {
			std::string path = req.getPath().replace(0, location->path.size(), location->root + "/");
			std::string filename = path.substr(path.find_last_of("/"));

			if (filename.find('.') == std::string::npos)
				path += "/" + location->index;
			response.setBodyFromFile(path);
			response.setType(path.substr(path.find_last_of('/') + 1));
		}
		else if (!location->redirect_url.empty()) {
			response.setHeader("Connection", "close");
			response.setHeader("Location", location->redirect_url);
			response.setStatus(301);
		}
	}
	else if (req.getMethod() == "POST" && findout("POST", location->allowed_methods)) {
		std::string	path = req.getPath().replace(0, location->path.size(), location->upload_store + "/");
		std::ofstream file(path.c_str());
		if (file.fail()) {
			response.setStatus(500);
			response.setBody("Internal Server Error");
			return ;
		}
		file << req.getBody();
		if (!location->redirect_url.empty()) {
			response.setHeader("Connection", "close");
			response.setHeader("Location", location->redirect_url);
			response.setStatus(301);
		} else {
			response.setHeader("Connection", "close");
			response.setStatus(200);
		}
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

void server::dispatchRequest(Request& req, Response& res) {
    std::string	longest = "";
	LocConfig	*loc;
	/* for (size_t i = 0; i < this->_config.locations.size(); i++)
	{
		if (req.getPath().substr(0, this->_config.locations[i].path.size()) == this->_config.locations[i].path)
		{
			if (this->_config.locations[i].path.size() > longest.size()) {
				loc = &this->_config.locations[i];
				longest = this->_config.locations[i].path;
			}
		}
	} */
	std::vector<LocConfig>::iterator locIt = this->_config.locations.begin();
	for (; locIt != this->_config.locations.end(); ++locIt) {
		if ((!req.getPath().compare(0, locIt->path.size(), locIt->path) && (locIt->path.size() == req.getPath().size() || req.getPath()[locIt->path.size()] == '/') && locIt->path.size() > longest.size()) || (locIt->path == "/" && longest.empty())) {
			loc = &(*locIt);
			longest = loc->path;
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
	res.setRetVal(res.toString());
}

ServerConfig &server::getConfig() {
	return this->_config;
}