/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:01:29 by fghysbre          #+#    #+#             */
/*   Updated: 2025/03/13 16:56:09 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <server.h>
#include <vector>
#include <map>

extern bool	runServ;

class Http
{
private:
	std::map<int, std::vector<server *> >	socket_server;
	std::map<uint16_t, int>					port_socket;
	std::vector<pollfd>						fds;
public:
	Http();
	~Http();
	
	void	addport(uint16_t port, server *serv);
	void	start();
	
	enum	clientSteps {
		CS_INIT,
		CS_READ,
		CS_WRITE,
	};
};
