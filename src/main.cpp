/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 18:04:46 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/26 00:16:26 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <exception>
#include <Config/ParseConfig.hpp>
#include <Http.hpp>
#include <vector>
#include <algorithm>

int	handleServers(Config &conf, Http &serv) {
	std::vector<ServerConfig>::iterator servIt = conf.servers.begin();
	for (; servIt != conf.servers.end(); ++servIt) {
		Server *tmpserv = new Server((*servIt));
		serv.addServer(tmpserv);
		tmpserv->addPort(tmpserv->getConfig().port);
		std::vector<LocConfig>::iterator	locIt = tmpserv->getConfig().locations.begin();
		for (; locIt != tmpserv->getConfig().locations.end(); ++locIt) {
			if (!(*locIt).cgi_extensions.empty() && !(*locIt).cgi_pass.empty()) {
				//TODO: Handle the cgi scripts
			} else if ((*locIt).directory_listing) {
				tmpserv->serveAutoIndex((*locIt).path, (*locIt).root);
			} else {
				std::cout << "added static serving for:" << std::endl;
				std::cout << (*locIt).path << std::endl << (*locIt).root << std::endl << (*locIt).index << std::endl;
				tmpserv->serveStatic((*locIt).path, (*locIt).root, (*locIt).index);
			}
			if (!(*locIt).upload_store.empty() && std::find((*locIt).allowed_methods.begin(), (*locIt).allowed_methods.end(), "POST") != (*locIt).allowed_methods.end()) {
				//TODO: Handle post method
			} 
		}
	}
	return (1);
}

int main(int argc, char **argv) {
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
		ParseConfig	parser("./defaults/temp.config", config);
		parser.parse();

		Http		http;
		handleServers(config, http);
		http.start();
	} catch (std::exception &e) {
		std::cout << "Error: " << e.what() << std::endl;
		return (2);
	}
	return (0);
}