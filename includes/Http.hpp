/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:01:29 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/20 23:42:29 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include <vector>

class Http
{
private:
	std::vector<Server *>	servers;
public:
	Http();
	~Http();

	void	addServer(Server *server);
	void	start();
};
