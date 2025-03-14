/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 16:33:32 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/14 20:29:15 by fghysbre         ###   ########.fr       */
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

class cgi
{		
    public:
        cgi(){};
        ~cgi(){};
		bool	isCGIrequest(const std::string& path);
		std::string executeCgi(std::string pathOr, const std::string &query, LocConfig *location, Request req);


};

void	error(const char *str);
bool	dup(int *fd, int file, int end);

#endif