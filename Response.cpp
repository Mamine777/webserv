/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 15:19:16 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/23 16:38:36 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.h"
#include "server.h"
Response::Response() : _version("HTTP/1.1"), _statusCode(200), _statusMessage("OK")
{}

std::string Response::getStatusMessage(int code) const
{
	 switch (code) {
		case 200: return "OK";
		case 201: return "Created";
		case 204: return "No Content";
		case 400: return "Bad Request";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 500: return "Internal Server Error";
		default: return "Unknown";
	}
}

void    Response::setStatus(int code)
{
	_statusCode = code;
	_statusMessage = getStatusMessage(code);
}

void    Response::setHeader(const std::string &key, const std::string &value)
{
	_headers[key] = value;
}

void Response::setBody(const std::string &body)
{
    _body = body;

    std::ostringstream lengthStream;
    lengthStream << _body.size();
    _headers["Content-Length"] = lengthStream.str();
}

void Response::setBodyFromFile(const std::string &filePath)
{
    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        setStatus(404);
        setBody("404 Not Found");
        return;
    }


    std::ostringstream contentStream;
    contentStream << file.rdbuf();
    _body = contentStream.str();
    

    std::ostringstream lengthStream;
    lengthStream << _body.size();
    _headers["Content-Length"] = lengthStream.str();
}


std::string Response::toString() const {
    std::ostringstream oss;
    oss << _statusCode;
    std::string response = _version + " " + oss.str() + " " + _statusMessage + "\r\n";
    
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
        response += it->first + ": " + it->second + "\r\n";
    }
    
    response += "\r\n" + _body;
    return response;
}


int	Response::getStatus()
{
	return this->_statusCode;
}