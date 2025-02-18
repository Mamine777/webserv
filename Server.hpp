/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 16:54:44 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/18 17:29:00 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <map>
#include <string>
#include "Request.hpp"
#include "Response.hpp"

class Server
{
private:
	//TODO: Change serverSocket to a vector of ints to allow multiple servers
	int			serverSocket;
	sockaddr_in	serverAddr;
	std::map	<std::string, void(*)(Request&, Response&)>getMap;
	std::map	<std::string, void(*)(Request&, Response&)>postMap;

	void	dispatchRequest(Request &req, Response &res);
public:
	Server();
	~Server();

	void	listen(uint16_t port);

	void	get(std::string path, void (*f)(Request&, Response&));
};