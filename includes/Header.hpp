/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 12:44:24 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/27 13:50:40 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <iostream>

class Header
{
protected:
	std::multimap<std::string, std::string> cookies;
	std::multimap<std::string, std::string>	parameters;
	std::string								httpVer;
public:
	Header();
	virtual ~Header();

	std::string	getHttpVer();
	std::multimap<std::string, std::string>	&getParams();

	std::string	getCookie(std::string name);
	void		setCookie(std::string name, std::string val);
	std::string cookieToStr();

	std::string	getField(std::string key);
	virtual std::string toString() = 0;
};

class ReqHeader : public Header {
private:
	std::string	method;
	std::string	ressource;
public:
	ReqHeader();
	ReqHeader(std::string header);
	~ReqHeader();

	void	parseHead(std::string str);
	void	parseCookies();
	
	std::string	getMethod();
	std::string	getRessource();
	
	std::string	toString();
};

class ResHeader : public Header {
private:
	unsigned int status;
public:
	ResHeader(std::string httpVer);
	~ResHeader();

	void	setParam(std::string key, std::string value);
	void	setStatus(unsigned int status);
	void	setHttpVer(std::string ver);

	std::string	toString();

	static std::string	expectHeader(std::string httpVer, unsigned int code);
};

std::ostream	&operator<<(std::ostream &os, ReqHeader head);