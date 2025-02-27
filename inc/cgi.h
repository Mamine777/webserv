/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 16:33:32 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/23 18:41:22 by mokariou         ###   ########.fr       */
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
		std::string    executeCgi(std::string path, const std::string &query);


};

void	error(const char *str);
bool	dup(int *fd, int file, int end);

#endif