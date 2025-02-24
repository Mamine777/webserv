/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 13:52:59 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/24 16:50:43 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Http.hpp"

void	homeGet(Request &req, Response &res) {
	(void)req;
	(void)res;
	std::cout << "Sound the alarms, someones in my home" << std::endl;
	res.status(400);
	res.sendFile("index.html");
}

void	nothomeGet(Request &req, Response &res) {
	(void)req;
	(void)res;
	std::cout << "Sound the alarms, no-one is in my home" << std::endl;
	res.sendText("Bienvenu dans pas ma baraque :(\n");
}

void	pingGet(Request &req, Response &res) {
	(void) req;
	res.sendText("pong\n");
}

int main(void) {
	Http	http;
    Server  app;

	app.get("/home", homeGet);
	app.get("/nothome/home", nothomeGet);
	
    app.addPort(8080);
	app.addPort(8081);
	http.addServer(&app);

	Server app2;
	app2.serveStatic("/static", "./test", "index.html");
	app2.addPort(7070);
	app2.addPort(7071);
	http.addServer(&app2);
	
	http.start();
}