/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 13:52:59 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/17 23:15:09 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include "MessageException.hpp"

 void reapZombies(int) {
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

void startServer(int serverSocket) {
    signal(SIGCHLD, reapZombies);

    int i = 0;
    while (1) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            perror("accept failed");
            continue;
        }

        std::cout << "Caught connection request [" << i << "]" << std::endl;
        pid_t pid = fork();
        if (pid == 0) { // Child process
            char buffer[1024] = {};
            recv(clientSocket, buffer, sizeof(buffer), 0);
            std::cout << "client[" << i << "]:" << std::endl << buffer << std::endl;

            // Proper HTTP Response
            std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 13\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Hello Traveler";

            send(clientSocket, response.c_str(), response.length(), 0);
            close(clientSocket);
            exit(0);
        }
        close(clientSocket);
        i++;
    }
}

int main(void) {
	throw MessageException("I dont like this >:(");
	
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket failed");
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind failed");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {
        perror("listen failed");
        close(serverSocket);
        return 1;
    }

    startServer(serverSocket);
    close(serverSocket);

}

Server app;
app.get("/home", [](Request req, Response res) {
	res.send("hello world")
}) */

#include "Server.hpp"

int main(void) {
    Server  app;
    app.listen(8080);
}