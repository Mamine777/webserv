/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 14:53:36 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/13 19:57:14 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <Config/ParseConfig.hpp>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Config/Config.hpp"
#include "Request.h"
#include "Response.h"
#include "cgi.h"

class Response;
class Request;
class cgi;

class server {
   private:
    ServerConfig &_config;
    std::string _configFile;
    // std::vector<int> _serverSockets;
    // std::vector<int>    sockets;
    // std::vector<struct pollfd> _pollfds;
    // std::map<int, ServerConfig*> _serverConfigs;

   public:
    server(ServerConfig &config);
    ~server();
    // void setupServer(uint16_t port);
    std::string getContentType(const std::string &filePath);
    void dispatchRequest(Request &req, Response &res);
    ServerConfig &getConfig();
    void errorPage(int code, Response &res);
};
void handleMethod(LocConfig *location, Response &response, Request &req,
                  cgi &CGI);

#endif