/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 16:31:48 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/19 17:16:51 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <fstream>

Response::Response(int clientSocket, std::string httpVer): clientSocket(clientSocket), head(ResHeader(httpVer)) {
}

Response::~Response() {
}

int Response::getClientSock() {
	return this->clientSocket;
}

void Response::append(std::string field) {
	this->append(field, "");
}

void Response::append(std::string field, std::string value) {
	this->head.getParams().insert(std::pair<std::string, std::string>(field, value));
}

void Response::attachment() {
	this->head.setParam("Content-Disposition", "attachment");
}

void Response::attachment(std::string path) {
	std::string	filename = path.substr(path.find_last_of('/') + 1);
	this->head.setParam("Content-Disposition", "attachment; filename=\"" + filename + "\"");
	if (filename.find_last_of('.') != std::string::npos)
		this->type(filename.substr(filename.find_last_of('.') + 1));
}

void Response::cookie(std::string name, std::string value) {
	this->head.setCookie(name, value);
}

Response &Response::status(unsigned int status) {
	this->head.setStatus(status);
	return (*this);
}

void Response::type(std::string type) {
	//TODO: do this or whatever
	(void)type;
}

void Response::sendText(std::string str) {
	std::stringstream ss;
	ss << str.length();
	this->head.setParam("Content-Type", "text/plain");
	this->head.setParam("Content-Length", ss.str());
	this->head.setParam("Connection", "close");
	std::string	ret = this->head.toString();
	ret += str;
	send(this->clientSocket, ret.c_str(), ret.size(), 0);
}

void Response::sendFile(std::string path) {
	std::ifstream	file(path.c_str(), std::ios::binary | std::ios::ate);
	if (file.fail()) {
		std::cerr << "Failed to open file " << path << std::endl;
		return;
	}

	std::stringstream ss;
	ss << file.tellg();
	file.seekg(0, std::ios::beg);
	//! Change the type to smth better icl
	this->head.setParam("Content-Type", "text/html\n");
	this->head.setParam("Content-Length", ss.str());
	this->head.setParam("Connection", "close");
	std::string	ret = this->head.toString();
	if (send(this->clientSocket, ret.c_str(), ret.size(), 0) == -1) {
		std::cerr << "Failed to send header of file " << path << std::endl;
		file.close();
		return;
	}

	char	buffer[4098];
	while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
		ssize_t bytes_sent = send(this->clientSocket, buffer, file.gcount(), 0);
		std::cout << "file.gcount() = " << file.gcount() << std::endl; 
		if (bytes_sent == -1) {
			std::cerr << "Failed to send file" << path << std::endl;
			file.close();
			return ;
		}
	}
	file.close();
}
