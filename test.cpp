/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 13:52:59 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/20 16:58:30 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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

int main(void) {
    Server  app;

	app.get("/home", homeGet);
	app.get("/nothome/home", nothomeGet);
	
    app.listen(8080);
}