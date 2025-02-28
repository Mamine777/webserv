/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 12:35:32 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/28 17:43:15 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.h"
#include "Request.h"

Request::Request(): finishHead(false), finishBody(false) {}

std::string Request::getMethod()const {return this->_method;}

std::string Request::getPath() const { return this->_path;}

std::string Request::getVersion() const {return this->_version;}

std::string Request::getBody() const {return this->_body;}

std::string Request::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);
	if (it != this->_headers.end())
    {
        return it->second;
    }
	else
		return "";
}

std::string& Request::getRawReq() {
	return this->raw;
}

void Request::setBody(std::string body) { this->_body = body; }

void    Request::parse(std::string &rawRequest)
{
	std::istringstream requestStream(rawRequest);
	std::string line;

	std::getline(requestStream, line);
	if (line.empty())
		return ;
	std::istringstream requestline(line);
	requestline >> _method >> _path >> _version;

	//std::cout << "=====.....>>>>>>" << _method << "====>>>>>>" << _path << _version;

	//setting the damn map
	while (std::getline(requestStream, line))
	{
		if (line.empty() || line == "\r")
			break ;
		size_t colonPos = line.find(":");
		if (colonPos != std::string::npos){
			
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 2);

			_headers[key] = value;
		}
	}
}

bool Request::getFinishBody() const { return this->finishBody; }
bool Request::getFinishHead() const { return this->finishHead; }
size_t Request::getBodySize() const { return this->bodySize; }
void Request::setBodySize(size_t i) {this->bodySize = i;}
void Request::setFinishBody(bool v) { this->finishBody = v; }
void Request::setFinishHead(bool v) {this->finishHead = v;}
