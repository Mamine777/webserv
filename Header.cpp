/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 12:48:43 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/18 17:05:05 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Header.hpp"
#include <sstream>
#include "MessageException.hpp"

Header::Header() {
}

Header::~Header()
{
}

std::string Header::getHttpVer() {
	return this->httpVer;
}

std::map<std::string, std::string> &Header::getParams() {
	return this->parameters;
}

std::string Header::getField(std::string key) {
	std::map<std::string, std::string>::iterator it = this->parameters.find(key);
	if (it == this->parameters.end())
		return ("");
	return ((*it).second);
}

std::string Header::toString()
{
	return std::string();
}

std::ostream &operator<<(std::ostream &os, ReqHeader head) {
	os << "Method: " << head.getMethod() << std::endl;
	os << "Ressource: " << head.getRessource() << std::endl;
	os << "HttpVer: " << head.getHttpVer() << std::endl;

	std::map<std::string, std::string>::iterator	it = head.getParams().begin();
	for (;it != head.getParams().end(); ++it) {
		os << (*it).first << ": " << (*it).second << std::endl;
	}
	return (os);
}

ReqHeader::ReqHeader(std::string header) {
	if (header.empty())
		return ;

	std::istringstream	ss(header);
	std::string buff;

	std::getline(ss, buff);
	if (!buff.empty() && *(buff.end() - 1) == '\r')
		buff.erase(buff.end() - 1);
	size_t	spaces[2];
	spaces[0] = buff.find(' ');
	spaces[1] = buff.find(' ', spaces[0] + 1);
	this->method = buff.substr(0, spaces[0]);
	this->ressource = buff.substr(spaces[0] + 1, spaces[1] - spaces[0] - 1);
	this->httpVer = buff.substr(spaces[1] + 1);

	while (!ss.eof()) {
		std::getline(ss, buff);
		if (buff == "\r" || buff.empty())
			break;
		if (!buff.empty() && *(buff.end() - 1) == '\r')
			buff.erase(buff.end() - 1);
		std::string key = buff.substr(0, buff.find(": "));
		std::string val = buff.substr(buff.find(": ") + 2);
		this->parameters[key] = val;
	}
}

ReqHeader::~ReqHeader() {}

std::string ReqHeader::getMethod() {
	return this->method;
}

std::string ReqHeader::getRessource() {
	return this->ressource;
}

std::string ReqHeader::toString()
{
	std::string	buff;
	buff += this->method + " " + this->ressource + " " + this->httpVer + "\r\n";

	std::map<std::string, std::string>::iterator	it = this->parameters.begin();
	for (; it != this->parameters.end(); ++it)
		buff += (*it).first + ": " + (*it).second + "\r\n";
	buff += "\r\n";
	return buff;
}

ResHeader::ResHeader(std::string httpVer): status(200) {
	this->httpVer = httpVer;
}

ResHeader::~ResHeader() {}

void ResHeader::setParam(std::string key, std::string value)
{
	this->parameters[key] = value;
}

void ResHeader::setStatus(unsigned int status){
	this->status = status;
}

void ResHeader::setHttpVer(std::string ver) {
	this->httpVer = ver;
}

//!	change status message
std::string ResHeader::toString() {
	std::stringstream ss;
	ss << std::dec << this->status;

	std::string buff = this->httpVer + " " + ss.str() + " OK\r\n";
	
	std::map<std::string, std::string>::iterator	it = this->parameters.begin();
	for (; it != this->parameters.end(); ++it)
		buff += (*it).first + ": " + (*it).second + "\r\n";
	buff += "\r\n";
	return buff;
}
