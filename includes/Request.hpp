/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 12:10:58 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/27 14:17:57 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "Header.hpp"

class Request {
private:
	ReqHeader	head;
	std::string	rawReq;
	std::string	rawBody;
	bool		finishHead;
	bool		finishBody;
	size_t		bodySize;
public:
	Request();
	Request(std::string req, int clientSock);
	~Request();

	ReqHeader	&getHeader();
	std::string	&getRawBody();
	std::string	&getRawReq();

	bool		getFinishHead() const;
	bool		getFinishBody() const;
	void		setFinishHead(bool v);
	void		setFinishBody(bool v);
	void		setBodySize(size_t size);
	size_t		getBodySize() const;
};