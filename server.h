/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 14:53:36 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/25 13:09:23 by mokariou         ###   ########.fr       */
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



// struct Route {
// 	std::string path;
// 	std::string root;
// 	bool directory_listing;
// 	std::string default_file;
// 	std::vector<std::string> allowed_methods;
// };

// struct ServerConfig {
// 	std::vector<int> ports;
// 	std::string host;
// 	std::string CGI_file;
// 	std::vector<Route> routes;
// 	std::map<int, std::string> error_pages;
// 	size_t client_body_size_limit;
// };

// class ConfigParser {
// 	private:
// 		std::string _configFile;
// 		ServerConfig _config;

// 		void parseConfig();

// 	public:
// 		ConfigParser(const std::string &configFile);
// 		ServerConfig getConfig() const;
// };


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




#endif