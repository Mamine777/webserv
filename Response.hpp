/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 15:41:15 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/18 17:14:30 by fghysbre         ###   ########.fr       */
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
	
	void	sendText(std::string str);
	void	
};

