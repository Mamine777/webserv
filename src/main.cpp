/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 14:53:15 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/02 21:42:48 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.h>
#include <Http.h>
#include <signal.h>

bool runServ = true;

int	handleServers(Config &conf, Http &serv) {
	std::vector<ServerConfig>::iterator servIt = conf.servers.begin();
	for (; servIt != conf.servers.end(); ++servIt) {
		server *tmpserv = new server((*servIt));
		serv.addport((*servIt).port, tmpserv);
	}
	return (1);
}

void	sighandler(int) {
	std::cout << "\33[2K\rCtrl + C called, exiting..." << std::endl;
	runServ = false;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Missing config file argument" << std::endl;
		return (1);
	} else if (argc > 2) {
		std::cerr << "Unwanted extra arguments" << std::endl;
		return (1);
	}
	signal(SIGINT, sighandler);
	try {
		Config		config;
		ParseConfig	parser(argv[1], config);
		parser.parse();
		config.ErrorsConfig();

		Http		http;
		handleServers(config, http);
		http.start();
	} catch (std::exception &e) {
		if (runServ == true) {
			std::cout << "Error: " << e.what() << std::endl;
			return (2);
		}
	}
	return (0);
}