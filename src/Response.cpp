/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 15:19:16 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/28 23:10:21 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.h"
#include "Response.h"
Response::Response() : _version("HTTP/1.1"), _statusCode(200), _statusMessage("OK"), isfinished(false), ind(0)
{}

Response::Response(std::string ver): _version(ver), _statusCode(200), _statusMessage("OK"), isfinished(false), ind(0) {}

std::string Response::getStatusMessage(int code)
{
	switch (code) {
		case 100:
			return "Continue";
		case 101:
			return "Switching Protocols";
		case 102:
			return "Processing";
		case 103:
			return "Early Hints";
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 202:
			return "Accepted";
		case 203:
			return "Non-Authoritative Information";
		case 204:
			return "No Content";
		case 205:
			return "Reset Content";
		case 206:
			return "Partial Content";
		case 207:
			return "Multi-Status";
		case 208:
			return "Already Reported";
		case 226:
			return "IM Used";
		case 300:
			return "Multiple Choices";
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 303:
			return "See Other";
		case 304:
			return "Not Modified";
		case 307:
			return "Temporary Redirect";
		case 308:
			return "Permanent Redirect";
		case 400:
			return "Bad Request";
		case 401:
			return "Unauthorized";
		case 402:
			return "Payment Required";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 406:
			return "Not Acceptable";
		case 407:
			return "Proxy Authentication Required";
		case 408:
			return "Request Timeout";
		case 409:
			return "Conflict";
		case 410:
			return "Gone";
		case 411:
			return "Length Required";
		case 412:
			return "Precondition Failed";
		case 413:
			return "Content Too Large";
		case 414:
			return "URI Too Long";
		case 415:
			return "Unsupported Media Type";
		case 416:
			return "Range Not Satisfiable";
		case 417:
			return "Expectation Failed";
		case 418:
			return "I'm a teapot";
		case 421:
			return "Misdirected Request";
		case 422:
			return "Unprocessable Content";
		case 423:
			return "Locked";
		case 424:
			return "Failed Dependency";
		case 425:
			return "Too Early";
		case 426:
			return "Upgrade Required";
		case 428:
			return "Precondition Required";
		case 429:
			return "Too Many Requests";
		case 431:
			return "Request Header Fields Too Large";
		case 451:
			return "Unavailable For Legal Reasons";
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 502:
			return "Bad Gateway";
		case 503:
			return "Service Unavailable";
		case 504:
			return "Gateway Timeout";
		case 505:
			return "HTTP Version Not Supported";
		case 506:
			return "Variant Also Negotiates";
		case 507:
			return "Insufficient Storage";
		case 508:
			return "Loop Detected";
		case 510:
			return "Not Extended";
		case 511:
			return "Network Authentication Required";
		default:
			return "Un-known Code";
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

void Response::done() { this->isfinished = true; }

bool Response::isdone() { return this->isfinished; }

void Response::setRetVal(std::string retval) {this->returnval = retval;}

std::string Response::getRetVal() { return this->returnval; }

void Response::setIndex(size_t val) {this->ind = val;}

size_t Response::getIndex() { return this->ind; }

std::string Response::expectHeader(std::string httpVer, unsigned int code) {
    std::stringstream	ss;
	ss << std::dec << code;
	
	return (httpVer + " " + ss.str() + " " + getStatusMessage(code)) + "\r\n\r\n";
}
int	Response::getStatus()
{
	return this->_statusCode;
}