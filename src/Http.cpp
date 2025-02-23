/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:25:34 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/21 17:46:46 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <poll.h>
#include <string.h>
#include <errno.h>
#include "MessageException.hpp"
#include <algorithm>
#include <map>
#include "unistd.h"
#include "Http.hpp"

Http::Http() {
}

Http::~Http() {
}

static std::string recieveData(int clientSocket) {
    char buff[1024];
    std::string ret;
    int bytesRead = 0;
    while ((bytesRead = recv(clientSocket, buff, sizeof(buff), 0)) > 0) {
        ret.append(buff, bytesRead);
        if (ret.find("\r\n\r\n") != std::string::npos)
            break;
    }
    if (bytesRead == 0)
        std::clog << "Client closed connection: " << clientSocket << std::endl;
    else if (bytesRead < 0)
        std::cerr << "Error receiving data" << std::endl;
    return ret;
}

std::vector<Server*>::iterator getServerFromSocket(std::vector<Server*>& servers, int fd) {
    std::vector<Server*>::iterator it;
    for (it = servers.begin(); it != servers.end(); ++it) {
        Server* serv = *it;
        std::vector<int> serverSocks = serv->getServerSocks();
        if (std::find(serverSocks.begin(), serverSocks.end(), fd) != serverSocks.end())
            return it;
    }
    return servers.end();
}

void closeSockets(std::vector<Server*>::iterator servers) {
	(void)servers;
}

void Http::addServer(Server *server) {
    this->servers.push_back(server);
}

void Http::start() {
    std::vector<struct pollfd> fds;
    std::vector<int> serverfds;
    std::map<int, Server*> requestServer;

    std::vector<Server*>::iterator it;
    for (it = this->servers.begin(); it != this->servers.end(); ++it) {
        std::vector<int> serverSocks = (*it)->getServerSocks();
        std::vector<int>::iterator iit;
        for (iit = serverSocks.begin(); iit != serverSocks.end(); ++iit) {
            if (listen(*iit, 5) < 0) {
                closeSockets(this->servers.begin());
                throw MessageException(strerror(errno));
            }
            fds.push_back((pollfd) {*iit, POLLIN | POLLOUT, 0});
            serverfds.push_back(*iit);
        }
    }

    while (true) {
        if (poll(&fds[0], fds.size(), -1) < 0)
            throw MessageException(strerror(errno));

        for (size_t i = 0; i < fds.size(); ) {
            if ((fds[i].revents & POLLIN)) {
                int currentFD = fds[i].fd;
                if (std::find(serverfds.begin(), serverfds.end(), currentFD) != serverfds.end()) {
                    sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
                    int clientSocket = accept(currentFD, (struct sockaddr *)&clientAddr, &clientLen);
                    if (clientSocket == -1) {
                        ++i;
                        continue;
                    }
                    fds.push_back((pollfd) {clientSocket, POLLIN | POLLOUT, 0});

                    std::vector<Server*>::iterator serverIt = getServerFromSocket(this->servers, currentFD);
                    if (serverIt == this->servers.end())
                        throw MessageException(std::string("No server found for socket"));
                    requestServer.insert(std::make_pair(clientSocket, *serverIt));
                } else if (fds[i].revents & POLLOUT) { 
                    std::string reqbuff = recieveData(currentFD);
                    if (!reqbuff.empty()) {
						Request req(reqbuff, currentFD);
						Response res(currentFD, req.getHeader().getHttpVer());
						std::map<int, Server*>::iterator servIt = requestServer.find(currentFD);
						if (servIt == requestServer.end())
							throw MessageException(std::string("Mixup in server sockets"));
						servIt->second->dispatchRequest(req, res);
					}
                    close(currentFD);
                    requestServer.erase(currentFD);
                    fds[i] = fds.back();
                    fds.pop_back();
                    continue;
                }
            }
            ++i;
        }
    }
}