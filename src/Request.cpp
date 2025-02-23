/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 17:36:33 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/21 18:14:07 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>

Request::Request(std::string req, int clientSock): head(ReqHeader(req)), rawReq(req) {
	std::string 	contLenStr = head.getField("Content-Length");
	unsigned int	contLen = 0;
	if (!contLenStr.empty()) {
		std::stringstream	ss(contLenStr);
		ss >> contLen;
	}

	size_t	headerEnd = rawReq.find("\r\n\r\n") + 4;
	size_t	bodySize = rawReq.size() - headerEnd;

	char	buff[1024];
	while (bodySize < static_cast<size_t>(contLen)) {
		int	bytesRead = recv(clientSock, buff, sizeof(buff), 0);
		if (bytesRead <= 0) 
			break;
		rawReq.append(buff, bytesRead);
		bodySize += bytesRead;
	}
	this->rawBody = rawReq.substr(rawReq.find("\r\n\r\n") + 4);
}

Request::~Request() {
}


ReqHeader &Request::getHeader()
{
	return (this->head);
}