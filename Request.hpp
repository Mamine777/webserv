/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 12:10:58 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/21 18:14:18 by fghysbre         ###   ########.fr       */
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
public:
	Request(std::string req, int clientSock);
	~Request();


	ReqHeader	&getHeader();
};