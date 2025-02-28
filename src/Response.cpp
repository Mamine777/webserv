/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 15:19:16 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/28 23:41:04 by fghysbre         ###   ########.fr       */
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

void Response::setType(std::string type) {
	if (type.find('/') != std::string::npos) {
		this->setHeader("Content-Type", type);
		return;
	}
	size_t pointpos = type.find_last_of('.');
	if (pointpos == std::string::npos) {		
		this->setHeader("Content-Type", "text/plain");
		return;
	}
	std::string fileExt = type.substr(pointpos + 1);
	// https://developer.mozilla.org/en-US/docs/Web/HTTP/MIME_types/Common_types
	if (fileExt == "aac")
		this->setHeader("Content-Type", "audio/aac");
	else if (fileExt == "abw")
		this->setHeader("Content-Type", "application/x-abiword");
	else if (fileExt == "apng")
		this->setHeader("Content-Type", "image/apng");
	else if (fileExt == "arc")
		this->setHeader("Content-Type", "application/x-freearc");
	else if (fileExt == "avif")
		this->setHeader("Content-Type", "image/avif");
	else if (fileExt == "avi")
		this->setHeader("Content-Type", "video/x-msvideo");
	else if (fileExt == "azw")
		this->setHeader("Content-Type", "application/vnd.amazon.ebook");
	else if (fileExt == "bin")
		this->setHeader("Content-Type", "application/octet-stream");
	else if (fileExt == "bmp")
		this->setHeader("Content-Type", "image/bmp");
	else if (fileExt == "bz")
		this->setHeader("Content-Type", "application/x-bzip");
	else if (fileExt == "bz2")
		this->setHeader("Content-Type", "application/x-bzip2");
	else if (fileExt == "cda")
		this->setHeader("Content-Type", "application/x-cdf");
	else if (fileExt == "csh")
		this->setHeader("Content-Type", "application/x-csh");
	else if (fileExt == "css")
		this->setHeader("Content-Type", "text/css");
	else if (fileExt == "csv")
		this->setHeader("Content-Type", "text/csv");
	else if (fileExt == "doc")
		this->setHeader("Content-Type", "application/msword");
	else if (fileExt == "docx")
		this->setHeader("Content-Type", "application/vnd.openxmlformats-officedocument.wordprocessingml.document");
	else if (fileExt == "eot")
		this->setHeader("Content-Type", "application/vnd.ms-fontobject");
	else if (fileExt == "epub")
		this->setHeader("Content-Type", "application/epub+zip");
	else if (fileExt == "gz")
		this->setHeader("Content-Type", "application/x-gzip");
	else if (fileExt == "gif")
		this->setHeader("Content-Type", "image/gif");
	else if (fileExt == "htm" || fileExt == "html")
		this->setHeader("Content-Type", "text/html");
	else if (fileExt == "ico")
		this->setHeader("Content-Type", "image/vnd.microsoft.icon");
	else if (fileExt == "ics")
		this->setHeader("Content-Type", "text/calendar");
	else if (fileExt == "jar")
		this->setHeader("Content-Type", "application/java-archive");
	else if (fileExt == "jpeg" || fileExt == "jpg")
		this->setHeader("Content-Type", "image/jpeg");
	else if (fileExt == "js")
		this->setHeader("Content-Type", "text/javascript");
	else if (fileExt == "json")
		this->setHeader("Content-Type", "application/json");
	else if (fileExt == "jsonld")
		this->setHeader("Content-Type", "application/ld+json");
	else if (fileExt == "mid" || fileExt == "midi")
		this->setHeader("Content-Type", "audio/x-midi");
	else if (fileExt == "mjs")
		this->setHeader("Content-Type", "text/javascript");
	else if (fileExt == "mp3")
		this->setHeader("Content-Type", "audio/mpeg");
	else if (fileExt == "mp4")
		this->setHeader("Content-Type", "video/mp4");
	else if (fileExt == "mpeg")
		this->setHeader("Content-Type", "video/mpeg");
	else if (fileExt == "mpkg")
		this->setHeader("Content-Type", "application/vnd.apple.installer+xml");
	else if (fileExt == "odp")
		this->setHeader("Content-Type", "application/vnd.oasis.opendocument.presentation");
	else if (fileExt == "ods")
		this->setHeader("Content-Type", "application/vnd.oasis.opendocument.spreadsheet");
	else if (fileExt == "odt")
		this->setHeader("Content-Type", "application/vnd.oasis.opendocument.text");
	else if (fileExt == "oga")
		this->setHeader("Content-Type", "audio/ogg");
	else if (fileExt == "ogv")
		this->setHeader("Content-Type", "video/ogg");
	else if (fileExt == "ogx")
		this->setHeader("Content-Type", "application/ogg");
	else if (fileExt == "opus")
		this->setHeader("Content-Type", "audio/ogg");
	else if (fileExt == "otf")
		this->setHeader("Content-Type", "font/otf");
	else if (fileExt == "png")
		this->setHeader("Content-Type", "image/png");
	else if (fileExt == "pdf")
		this->setHeader("Content-Type", "application/pdf");
	else if (fileExt == "php")
		this->setHeader("Content-Type", "application/x-httpd-php");
	else if (fileExt == "ppt")
		this->setHeader("Content-Type", "application/vnd.ms-powerpoint");
	else if (fileExt == "pptx")
		this->setHeader("Content-Type", "application/vnd.openxmlformats-officedocument.presentationml.presentation");
	else if (fileExt == "rar")
		this->setHeader("Content-Type", "application/vnd.rar");
	else if (fileExt == "rtf")
		this->setHeader("Content-Type", "application/rtf");
	else if (fileExt == "sh")
		this->setHeader("Content-Type", "application/x-sh");
	else if (fileExt == "svg")
		this->setHeader("Content-Type", "image/svg+xml");
	else if (fileExt == "tar")
		this->setHeader("Content-Type", "application/x-tar");
	else if (fileExt == "tif" || fileExt == "tiff")
		this->setHeader("Content-Type", "image/tiff");
	else if (fileExt == "ts")
		this->setHeader("Content-Type", "video/mp2t");
	else if (fileExt == "ttf")
		this->setHeader("Content-Type", "font/ttf");
	else if (fileExt == "txt")
		this->setHeader("Content-Type", "text/plain");
	else if (fileExt == "vsd")
		this->setHeader("Content-Type", "application/vnd.visio");
	else if (fileExt == "wav")
		this->setHeader("Content-Type", "audio/wav");
	else if (fileExt == "weba")
		this->setHeader("Content-Type", "audio/webm");
	else if (fileExt == "webm")
		this->setHeader("Content-Type", "video/webm");
	else if (fileExt == "webp")
		this->setHeader("Content-Type", "image/webp");
	else if (fileExt == "woff")
		this->setHeader("Content-Type", "font/woff");
	else if (fileExt == "woff2")
		this->setHeader("Content-Type", "font/woff2");
	else if (fileExt == "xhtml")
		this->setHeader("Content-Type", "application/xhtml+xml");
	else if (fileExt == "xls")
		this->setHeader("Content-Type", "application/vnd.ms-excel");
	else if (fileExt == "xlsx")
		this->setHeader("Content-Type", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
	else if (fileExt == "xml")
		this->setHeader("Content-Type", "application/xml");
	else if (fileExt == "xul")
		this->setHeader("Content-Type", "application/vnd.mozilla.xul+xml");
	else if (fileExt == "zip")
		this->setHeader("Content-Type", "application/zip");
	else if (fileExt == "3gp")
		this->setHeader("Content-Type", "video/3gpp");
	else if (fileExt == "3g2")
		this->setHeader("Content-Type", "video/3gpp2");
	else if (fileExt == "7z")
		this->setHeader("Content-Type", "application/x-7z-compressed");
	else
		this->setHeader("Content-Type", "text/plain");
}

std::string Response::expectHeader(std::string httpVer, unsigned int code) {
    std::stringstream	ss;
	ss << std::dec << code;
	
	return (httpVer + " " + ss.str() + " " + getStatusMessage(code)) + "\r\n\r\n";
}
int	Response::getStatus()
{
	return this->_statusCode;
}