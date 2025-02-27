/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 12:35:32 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/23 13:37:52 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.h"

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
	//set the body if it exist
	if (_headers.find("Content-Length") != _headers.end()) {
			size_t contentLength = std::strtoul(_headers["Content-Length"].c_str(), NULL, 10);
			_body = rawRequest.substr(rawRequest.find("\r\n\r\n") + 4, contentLength);
	   }
}