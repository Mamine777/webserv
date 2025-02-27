/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 14:21:21 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/23 16:35:28 by mokariou         ###   ########.fr       */
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
		std::string getStatusMessage(int code) const;
	public:
		Response();
		~Response(){};
		void setStatus(int code);
    	void setHeader(const std::string &key, const std::string &value);
   		void setBody(const std::string &body);
   		void setBodyFromFile(const std::string &filePath);
		int	getStatus();
    	std::string toString() const;
};

#endif