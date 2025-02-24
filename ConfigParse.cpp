/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 15:51:38 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/24 19:43:04 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParse.h"

ConfigParser::ConfigParser(const std::string &configFile) : _configFile(configFile)
{
	parseConfig();
}

void    ConfigParser::parseConfig(){

	Route defaultRoute;

	_config.ports.push_back(8080);
	_config.ports.push_back(7070);
	_config.ports.push_back(8081);
	
	_config.host = "127.0.0.1";
	_config.client_body_size_limit = 1000000;
	_config.CGI_file = "./a.out";

	defaultRoute.path = "/";
	defaultRoute.root = "www";
	defaultRoute.directory_listing = false;
    defaultRoute.default_file = "index.html";
    defaultRoute.allowed_methods.push_back("GET");

	_config.routes.push_back(defaultRoute);
}

ServerConfig ConfigParser::getConfig() const {
    return _config;
}