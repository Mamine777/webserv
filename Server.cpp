/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 17:19:18 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/17 17:55:19 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <exception>
#include "MessageException.hpp"
#include "string.h"
#include "errno.h"

Server::Server(): serverSocket(socket(AF_INET, SOCK_STREAM, 0)) {
	if (this->serverSocket < 0) {
		throw MessageException(strerror(errno));
	}

	int opt = 1;
	setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void Server::listen(uint16_t port, void f(void)) {
	this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_port = htons(port);
	this->serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr)) < 0) {
		close(serverSocket);
		throw MessageException(strerror(errno));
	}
	if (::listen(this->serverSocket, 5) < 0) {
		close(serverSocket);
		throw MessageException(strerror(errno));
	}
}
