/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 18:55:28 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/13 14:18:29 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../inc/Response.h"
#include <string>
#include "../inc/cgi.h"
#include "sys/stat.h"
#include "sys/types.h"
#include <dirent.h>
#include <cstdlib>

server::server(ServerConfig &config) : _config(config) {
}

server::~server() {}

bool	findout(std::string Method, std::vector<std::string> allowed_method){

	for (size_t i = 0 ; i < allowed_method.size(); i++)
		if (Method == allowed_method[i])
			return true;
	return false;
}

std::vector<std::string> split(const std::string &str, char delimiter)
{
    std::vector<std::string> result;
    std::string temp;
    
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == delimiter) {
            result.push_back(temp);
            temp = "";
        } else {
            temp += str[i];
        }
    }
    result.push_back(temp);
    
    return result;
}


bool	request_checking(std::string path)
{
	std::vector<std::string> splitted = split(path, '/');
	for (size_t i = 0 ; i < splitted.size(); i++)
	{
		if (splitted [i] == "src" || splitted[i] == "inc"
			|| splitted[i] == "Makefile" || splitted[i] == ".." || splitted[i] == "defaults" || splitted[i] == "obj")
				return false;	
	}
	return true;
}
std::string getFolder(std::string path, LocConfig *location){
	std::vector<std::string> splitted = split(path, '/');
	std::string rootPath;
	bool a = false;
	size_t pos = location->path.find('/');
    rootPath = location->path.substr(pos + 1);
	std::string folder = ".";
	for (size_t i = 0 ; i < splitted.size(); i++)
	{
	
		if (splitted[i] == rootPath)
		{
			splitted[i] = location->root;
			a = true;
		}
		folder += "/" + splitted[i];
	}
	
	return folder;
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

ParsedCgiOutput parseCgiOutput(const std::string& cgiOutput) {
    ParsedCgiOutput result;

    size_t pos = cgiOutput.find("\r\n\r\n");
    if (pos == std::string::npos) {
        result.headers = "Status: 500 Internal Server Error";
        result.body = "CGI output malformed";
        return result;
    }
    result.headers = cgiOutput.substr(0, pos);
    result.body = cgiOutput.substr(pos + 4);

	 std::istringstream stream(result.headers);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("Status:") == 0) {
            result.statusLine = line.substr(8);
            break;
        }
    }
    if (result.statusLine.empty()) {
        result.statusLine = "200 OK";
    }
    return result;
}

std::map<std::string, std::string> parseHeaders(const std::string& headers) {
    std::map<std::string, std::string> headerMap;
    std::istringstream stream(headers);
    std::string line;

    while (std::getline(stream, line) && !line.empty()) {
        size_t colon = line.find(": ");
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 2);
            headerMap[key] = value;
        }
    }

    return headerMap;
}

void handleMethod(LocConfig *location, Response &response, Request &req, cgi &CGI)
{
	if (hasDirTraversal(req.getPath())) {
		response.setStatus(403);
		response.setBody("Forbidden Request");
		return ;
	}
	if (req.getMethod() == "GET" && findout("GET", location->allowed_methods)){
		if (!location->cgi_pass.empty()) {
			std::string cgiOutput = CGI.executeCgi(req.getPath(), "", location, req);
            ParsedCgiOutput parsed = parseCgiOutput(cgiOutput);
            std::map<std::string, std::string> headerMap = parseHeaders(parsed.headers);
            response.setBody(parsed.body);
            if (headerMap.find("Status") != headerMap.end())
                response.setStatus(atoi(headerMap["Status"].c_str()));
            else
                response.setStatus(200);
            if (headerMap.find("Content-Type") != headerMap.end())
                response.setType(headerMap["Content-Type"]);
            else
                response.setType("text/html");
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
					response.setBody("Internal Error");
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
			response.setStatus(200);
		}
	}
	else if (req.getMethod() == "DELETE" && findout("DELETE", location->allowed_methods)){
		std::string filePath = "." + req.getPath();
		if (!request_checking(req.getPath())){
			response.setStatus(403);
			response.setBody("Forbidden and chill bro");
		}
		else {
			std::string folder = getFolder(req.getPath(), location);
			if (remove(folder.c_str()) == 0) {
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
	res.setRetVal(res.toString());
}

