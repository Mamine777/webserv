/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 16:33:32 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/08 16:39:46 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_H
#define CGI_H
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include "Response.h"
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <fstream>
#include <cstdlib>
#include "server.h"

class cgi
{		
    public:
        cgi(){};
        ~cgi(){};
		bool	isCGIrequest(const std::string& path);
		std::string    executeCgi(std::string path, const std::string &query, LocConfig *location);


};

void	error(const char *str);
bool	dup(int *fd, int file, int end);

#endif