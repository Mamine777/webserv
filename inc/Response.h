/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 14:21:21 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/28 22:29:33 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_H
#define RESPONSE_H

#include "server.h"
#include "Request.h"

class Response
{
	private:
		std::string _version;
		int _statusCode;
		std::string _statusMessage;
		std::map<std::string, std::string> _headers;
		std::string _body;
		static std::string getStatusMessage(int code);
		bool		isfinished;
		std::string	returnval;
		size_t		ind;
	public:
		Response();
		Response(std::string ver);
		~Response(){};
		void setStatus(int code);
    	void setHeader(const std::string &key, const std::string &value);
   		void setBody(const std::string &body);
   		void setBodyFromFile(const std::string &filePath);
		int	getStatus();
    	std::string toString() const;
		void	done();
		bool	isdone();
		void	setRetVal(std::string retval);
		std::string	getRetVal();
		void	setIndex(size_t val);
		size_t	getIndex();

		static std::string	expectHeader(std::string httpVer, unsigned int code);
};

#endif