/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 16:54:44 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/24 16:35:26 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <netinet/in.h>
#include <map>
#include <string>
#include <vector>
#include "Request.hpp"
#include "Response.hpp"
#include "StaticHandler.hpp"

class Server
{
private:
	std::vector<int>	serverSocks;
	std::map	<std::string, void(*)(Request&, Response&)>getMap;
	std::map	<std::string, void(*)(Request&, Response&)>postMap;
	std::vector <StaticHandler>statics;

public:
	Server();
	~Server();

	void	addPort(uint16_t port);

	void	dispatchRequest(Request &req, Response &res);
	void	get(std::string path, void (*f)(Request& req, Response& res));

	std::vector<int>	&getServerSocks();

	void	serveStatic(std::string urlPath, std::string rootPath, std::string defhtml);
};