/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 12:13:26 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/28 17:43:30 by fghysbre         ###   ########.fr       */
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
		std::string	raw;
		bool		finishHead;
		bool		finishBody;
		size_t		bodySize;
	public:
		Request();
		~Request(){};
		void	parse(std::string &rawRequest);
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::string getHeader(const std::string &key) const;
		std::string getBody() const;
		std::string&	getRawReq();
		void		setBody(std::string body);

		bool	getFinishBody() const;
		bool	getFinishHead() const;
		void	setFinishBody(bool v);
		void	setFinishHead(bool v);
		size_t	getBodySize() const;
		void	setBodySize(size_t i);
};


#endif