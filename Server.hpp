/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 16:54:44 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/17 17:52:45 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <map>

class Response;
class Request;

class Server
{
private:
	int			serverSocket;
	sockaddr_in	serverAddr;
	std::map	<std::string, void(Request, Response)>map;
public:
	Server();
	~Server();

	void	listen(uint16_t port, void f(void));
};