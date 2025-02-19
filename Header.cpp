/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 12:48:43 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/19 17:15:32 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"

#include <sstream>

#include "MessageException.hpp"

Header::Header() {}

Header::~Header() {}

std::string Header::getHttpVer() { return this->httpVer; }

std::multimap<std::string, std::string> &Header::getParams() {
	return this->parameters;
}

std::string Header::getCookie(std::string name) {
	std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> range = this->cookies.equal_range(name);
	if (range.first == this->cookies.end())
		return "";
	std::multimap<std::string, std::string>::iterator	last = range.second;
	return (*(--last)).second;
}

void Header::setCookie(std::string name, std::string val) {
	std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> range = this->cookies.equal_range(name);
	if (range.first == this->cookies.end()) {
		this->cookies.insert(std::pair<std::string, std::string>(name, val));
		return ;
	}
	std::multimap<std::string, std::string>::iterator	last = range.second;
	(*(--last)).second = val;
}

std::string Header::cookieToStr() {
	std::multimap<std::string, std::string>::iterator	it = this->cookies.begin();
	if (it == this->cookies.end())
		return "";

	std::string buff;
	for (; it != this->cookies.end(); ++it) {
		if (it != this->cookies.begin())
			buff += " ";
		buff.append((*it).first + "=" + (*it).second + ";");
	}
	return buff;
}

std::string Header::getField(std::string key) {
	std::map<std::string, std::string>::iterator it =
		this->parameters.find(key);
	if (it == this->parameters.end()) return ("");
	return ((*it).second);
}

std::string Header::toString() { return std::string(); }

std::ostream &operator<<(std::ostream &os, ReqHeader head) {
	os << "Method: " << head.getMethod() << std::endl;
	os << "Ressource: " << head.getRessource() << std::endl;
	os << "HttpVer: " << head.getHttpVer() << std::endl;

	std::map<std::string, std::string>::iterator it = head.getParams().begin();
	for (; it != head.getParams().end(); ++it) {
		os << (*it).first << ": " << (*it).second << std::endl;
	}
	return (os);
}

ReqHeader::ReqHeader(std::string header) {
	if (header.empty()) return;

	std::istringstream ss(header);
	std::string buff;

	std::getline(ss, buff);
	if (!buff.empty() && *(buff.end() - 1) == '\r') buff.erase(buff.end() - 1);
	size_t spaces[2];
	spaces[0] = buff.find(' ');
	spaces[1] = buff.find(' ', spaces[0] + 1);
	this->method = buff.substr(0, spaces[0]);
	this->ressource = buff.substr(spaces[0] + 1, spaces[1] - spaces[0] - 1);
	this->httpVer = buff.substr(spaces[1] + 1);

	while (!ss.eof()) {
		std::getline(ss, buff);
		if (buff == "\r" || buff.empty()) break;
		if (!buff.empty() && *(buff.end() - 1) == '\r')
			buff.erase(buff.end() - 1);
		std::string key = buff.substr(0, buff.find(": "));
		std::string val = buff.substr(buff.find(": ") + 2);
		this->parameters.insert(std::pair<std::string, std::string>(key, val));
	}
}

ReqHeader::~ReqHeader() {}

std::string ReqHeader::getMethod() { return this->method; }

std::string ReqHeader::getRessource() { return this->ressource; }

std::string ReqHeader::toString() {
	std::string buff;
	buff += this->method + " " + this->ressource + " " + this->httpVer + "\r\n";

	std::map<std::string, std::string>::iterator it = this->parameters.begin();
	for (; it != this->parameters.end(); ++it)
		buff += (*it).first + ": " + (*it).second + "\r\n";
	buff += "\r\n";
	return buff;
}

ResHeader::ResHeader(std::string httpVer) : status(200) {
	this->httpVer = httpVer;
}

ResHeader::~ResHeader() {}

static std::string getStatMsg(unsigned int status) {
	switch (status) {
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



void ResHeader::setParam(std::string key, std::string value) {
	std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator>	range = this->parameters.equal_range(key);
	if (range.first != this->parameters.end())
		this->parameters.erase(range.first, range.second);
	this->parameters.insert(std::pair<std::string, std::string>(key, value));
}

void ResHeader::setStatus(unsigned int status) { this->status = status; }

void ResHeader::setHttpVer(std::string ver) { this->httpVer = ver; }

std::string ResHeader::toString() {
	std::stringstream ss;
	ss << std::dec << this->status;

	std::string buff = this->httpVer + " " + ss.str() + " " + getStatMsg(this->status) + "\r\n";

	if (!this->cookies.empty() && this->getField("Cookie").empty())
		this->parameters.insert(std::pair<std::string, std::string>(std::string("Cookie"), this->cookieToStr()));
	std::map<std::string, std::string>::iterator it = this->parameters.begin();
	for (; it != this->parameters.end(); ++it)
		buff += (*it).first + ": " + (*it).second + "\r\n";
	buff += "\r\n";
	return buff;
}
