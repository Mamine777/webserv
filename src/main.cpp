/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 14:53:15 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/11 16:18:51 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <server.h>
#include <Http.h>


int	handleServers(Config &conf, Http &serv) {
	std::vector<ServerConfig>::iterator servIt = conf.servers.begin();
	for (; servIt != conf.servers.end(); ++servIt) {
		server *tmpserv = new server((*servIt));
		serv.addServer(tmpserv);
		tmpserv->setupServer((*servIt).port);
	}
	return (1);
}

int main(int argc, char **argv, char **envp) {
	if (argc < 2) {
		std::cerr << "Missing config file argument" << std::endl;
		return (1);
	} else if (argc > 2) {
		std::cerr << "Unwanted extra arguments" << std::endl;
		return (1);
	}
	(void)argv;
	try {
		Config		config;
		ParseConfig	parser(argv[1], config);
		parser.parse();
		config.ErrorsConfig();

		Http		http;
		handleServers(config, http);
		http.start();
	} catch (std::exception &e) {
		std::cout << "Error: " << e.what() << std::endl;
		return (2);
	}
	return (0);
}