/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 12:13:26 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/23 12:42:31 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
#define REQUEST_H


#include "server.h"
#include <sstream>

class Request
{
	private:
		std::map<std::string, std::string> _headers;//contain the the rest of the info
		std::string	_method;
		std::string	_path;
		std::string	_version;
		std::string	_body;
	public:
		Request(){};
		~Request(){};
		void	parse(std::string &rawRequest);
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getHeader(const std::string &key) const;
		std::string getBody() const;
};


#endif