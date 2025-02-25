/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 18:04:46 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/25 18:10:44 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <exception>
#include <Config/ParseConfig.hpp>
#include <Http.hpp>

int	handleServers(Config &conf, Http &serv) {
	
}

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Missing config file argument" << std::endl;
		return (1);
	} else if (argc > 2) {
		std::cerr << "Unwanted extra arguments" << std::endl;
		return (1);
	}
	try {
		Config		config;
		ParseConfig	parser(argv[1], config);
		parser.parse();

		Http		http;
		http.start();
	} catch (std::exception &e) {
		std::cout << "Error: " << e.what() << std::endl;
		return (2);
	}
	return (0)
}