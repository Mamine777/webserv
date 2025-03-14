/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:01:29 by fghysbre          #+#    #+#             */
/*   Updated: 2025/03/14 14:52:50 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <server.h>

#include <map>
#include <vector>

extern bool runServ;

class Http {
   private:
    std::map<int, std::vector<server *> > socket_server;
    std::map<uint16_t, int> port_socket;
    std::vector<pollfd> fds;

   public:
    enum clientSteps { CS_INIT, CS_READ, CS_WRITE, CS_ERROR };

    Http();
    ~Http();

    void addport(uint16_t port, server *serv);
    void start();
    void errorPage(int code, Response &res, server *server);

    void recieveReq(int fd, Request &req,
                    std::map<int, Http::clientSteps> &steps,
                    std::map<int, Response> &responses, int serverSock);
};
