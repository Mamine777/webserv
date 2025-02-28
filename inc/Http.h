/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:01:29 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/28 16:06:15 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <server.h>
#include <vector>

class Http
{
private:
	std::vector<server *>	servers;
public:
	Http();
	~Http();

	void	addServer(server *server);
	void	start();
};
