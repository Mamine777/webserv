/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 12:10:58 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/18 15:47:00 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "Header.hpp"

class Request {
private:
	ReqHeader	head;
public:
	Request(std::string req);
	~Request();

	ReqHeader	&getHeader();
};