/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:01:29 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/20 17:26:02 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include <vector>

class http
{
private:
	std::vector<Server &>	servers;
public:
	http();
	~http();

	void	addServer(Server &server);
	void	start();
};
