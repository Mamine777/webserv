/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 16:35:07 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/24 16:39:17 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Response.hpp"
#include "Request.hpp"

class StaticHandler
{
private:
	std::string	uriPath;
	std::string	rootPath;
	std::string	defhtml;
public:
	StaticHandler(std::string urlPath, std::string rootPath, std::string defhtml);
	~StaticHandler();

	std::string getUriPath();

	void execute(Request &req, Response &res);
};
