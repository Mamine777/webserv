#include "Config.hpp"

void Config::printConfig() const {
    for (size_t i = 0; i < servers.size(); i++) {
        std::cout << "Serveur #" << i + 1 << " :" << std::endl;
        std::cout << "  - Port : " << servers[i].port << std::endl;
        std::cout << "  - Host : " << servers[i].host << std::endl;
        std::cout << "  - Server Name : " << servers[i].server_name << std::endl;
        std::cout << "  - Client Max Body Size : " << servers[i].client_max_body_size << std::endl;

      
        for (std::map<int, std::string>::const_iterator it = servers[i].error_pages.begin(); 
             it != servers[i].error_pages.end(); ++it) {
            std::cout << "  - Page d'erreur " << it->first << " -> " << it->second << std::endl;
        }

        for (std::vector<LocConfig>::const_iterator it = servers[i].locations.begin();
             it != servers[i].locations.end(); ++it) {
            std::cout << "  Location : " << it->path << std::endl;
            std::cout << "    - Root : " << it->root << std::endl;
            std::cout << "    - Index : " << it->index << std::endl;
            
           
            std::cout << "    - Allowed Methods : ";
            for (size_t j = 0; j < it->allowed_methods.size(); j++) {
                std::cout << it->allowed_methods[j];
                if (j < it->allowed_methods.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;

            std::cout << "    - Upload Store : " << it->upload_store << std::endl;
            std::cout << "    - CGI Pass : " << it->cgi_pass << std::endl;

            std::cout << "    - CGI Extensions : ";
            for (size_t j = 0; j < it->cgi_extensions.size(); j++) {
                std::cout << it->cgi_extensions[j];
                if (j < it->cgi_extensions.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
        }
    }
}
