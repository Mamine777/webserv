/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndexHandler.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 14:12:53 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/25 15:01:34 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Response.hpp"
#include "Request.hpp"

class AutoIndexHandler
{
private:
	std::string	urlPath;
	std::string	rootPath;
public:
	AutoIndexHandler(std::string urlPath, std::string rootPath);
	~AutoIndexHandler();

	std::string	getUriPath();

	void execute(Request& req, Response& res);
};
