/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 14:53:15 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/25 13:22:09 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"


int main(int ac, char **av)
{
	if (ac != 2) {std::cout << "not the right arguments !" << std::endl; return -1;}
	try {
		Config config;
        ParseConfig parser(av[1], config);
		parser.parse();
		config.printConfig();
		std::cout << "==========================================>"<< config.servers. << std::endl;

		exit(1);
		server	server(config, parser);
		server.start();
	} catch (const std::exception &e){
		std::cout << "Error: " << e.what() << std::endl; return -1;	
	}
	return 0;
}