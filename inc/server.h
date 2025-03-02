/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 14:53:36 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/02 21:32:21 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <map>
#include <vector>
#include <poll.h>
#include <sstream>
#include "Request.h"
#include <fstream>
#include <algorithm>
#include <Config/ParseConfig.hpp>
#include "Config/Config.hpp"
#include "Response.h"
#include <string>
#include "cgi.h"

class Response;
class Request;
class cgi;

class server
{
	private:
		ServerConfig	&_config;
		std::string _configFile;
		//std::vector<int> _serverSockets;
		//std::vector<int>    sockets;
		//std::vector<struct pollfd> _pollfds;
		//std::map<int, ServerConfig*> _serverConfigs;
	
	public:
		server	(ServerConfig &config);
		~server();
		//void setupServer(uint16_t port);
		std::string getContentType(const std::string &filePath);
		void	dispatchRequest(Request &req, Response &res);
		ServerConfig	&getConfig();
};
void	handleMethod(LocConfig *location, Response &response, Request &req, cgi &CGI);



#endif