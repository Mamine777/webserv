/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 15:41:15 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/19 15:26:58 by fghysbre         ###   ########.fr       */
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

	Response	&status(unsigned int status);
	
	void	sendText(std::string str);
	void	sendFile(std::string path);
};

