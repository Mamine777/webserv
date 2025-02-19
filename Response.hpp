/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 15:41:15 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/19 16:45:03 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "Header.hpp"

class Response
{
private:
	int			clientSocket;
	ResHeader	head;
public:
	Response(int clientSocket, std::string httpVer);
	~Response();

	int	getClientSock();

	void		append(std::string field);
	void		append(std::string field, std::string value);
	void		attachment();
	void		attachment(std::string path);
	void		cookie(std::string name, std::string value);
	Response	&status(unsigned int status);
	void		type(std::string type);
	
	void	sendText(std::string str);
	void	sendFile(std::string path);
};

