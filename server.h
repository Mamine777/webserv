/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 14:53:36 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/26 14:06:50 by mokariou         ###   ########.fr       */
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
#include "ConfigParse.h"
#include "Request.h"
#include <fstream>
#include <algorithm>
#include "Config/ParseConfig.hpp"
#include "Config/Config.hpp"
#include "ConfigParse.h"
#include "Response.h"
#include <string>
#include "cgi.h"


class Response;
class Request;
class cgi;

class server
{
	private:
		Config	_config;
		ParseConfig	configParse;
		std::string _configFile;
		std::vector<int> _serverSockets;
		std::vector<int>    sockets;
		std::vector<struct pollfd> _pollfds;
		std::map<int, ServerConfig*> _serverConfigs;
	
		void setupServer();
		void acceptConnection();
		void handleClient(int clientSocket);
	public:
		server	(Config &config, ParseConfig &parser);
		~server();
		std::string getContentType(const std::string &filePath);

		void start();
};
void	handleMethod(LocConfig *location, Response &response, Request &req, cgi &CGI);




#endif