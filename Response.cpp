/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 16:31:48 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/18 17:06:31 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>

Response::Response(int clientSocket, std::string httpVer): clientSocket(clientSocket), head(ResHeader(httpVer)) {
}

Response::~Response() {
}

int Response::getClientSock() {
	return this->clientSocket;
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
