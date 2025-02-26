#include "Config.hpp"

void Config::printConfig() const
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        std::cout << "Serveur #" << i + 1 << " :" << std::endl;
        std::cout << "  - Port : " << servers[i].port << std::endl;
        std::cout << "  - Host : " << servers[i].host << std::endl;
        std::cout << "  - Server Name : " << servers[i].server_name << std::endl;
        std::cout << "  - Client Max Body Size : " << servers[i].client_max_body_size << std::endl;

        for (std::map<int, std::string>::const_iterator it = servers[i].error_pages.begin();
             it != servers[i].error_pages.end(); ++it)
        {
            std::cout << "  - Page d'erreur " << it->first << " -> " << it->second << std::endl;
        }

        for (std::vector<LocConfig>::const_iterator it = servers[i].locations.begin();
             it != servers[i].locations.end(); ++it)
        {
            std::cout << "  Location : " << it->path << std::endl;
            std::cout << "    - Root : " << it->root << std::endl;
            std::cout << "    - Index : " << it->index << std::endl;

            std::cout << "    - Allowed Methods : ";
            for (size_t j = 0; j < it->allowed_methods.size(); j++)
            {
                std::cout << it->allowed_methods[j];
                if (j < it->allowed_methods.size() - 1)
                    std::cout << ", ";
            }
            std::cout << std::endl;

            std::cout << "    - Upload Store : " << it->upload_store << std::endl;
            std::cout << "    - CGI Pass : " << it->cgi_pass << std::endl;

            std::cout << "    - CGI Extensions : ";
            for (size_t j = 0; j < it->cgi_extensions.size(); j++)
            {
                std::cout << it->cgi_extensions[j];
                if (j < it->cgi_extensions.size() - 1)
                    std::cout << ", ";
            }
            std::cout << std::endl;
        }
    }
}

void Config::ErrorsConfig()
{
    std::vector<int> usedPorts;

    for (size_t i = 0; i < servers.size(); i++)
    {
        ServerConfig &server = servers[i];

        //  Vérifier si le port est valide
        if (server.port < 1 || server.port > 65535)
        {
            std::ostringstream ss;
            ss << server.port;
            throw std::runtime_error("Error: Invalid port " + ss.str() + " must be between 1 and 65535");
        }

        bool isDuplicate = false;
        for (size_t j = 0; j < usedPorts.size(); j++)
        {
            if (usedPorts[j] == server.port)
            {
                isDuplicate = true;
                break;
            }
        }
        if (isDuplicate)
        {
            std::ostringstream ss;
            ss << server.port;
            throw std::runtime_error("Error: Port " + ss.str() + " defined multiple times.");
        }
        usedPorts.push_back(server.port);

        //     // Vérifier l'existence du fichier d'erreur (corrigé `error_page`)
        // if (!server.error_pages.empty()) {
        //     for (std::map<int, std::string>::iterator it = server.error_pages.begin(); it != server.error_pages.end(); ++it) {
        //         if (access(it->second.c_str(), F_OK) == -1) {
        //             std::ostringstream ss;
        //             ss << "Erreur: Fichier d'erreur non trouvé pour le code " << it->first << ": " << it->second;
        //             throw std::runtime_error(ss.str());
        //         }
        //     }
        // }
    //verifier les locations si y a '/'
        bool hasRootLocation = false;
        for (size_t j = 0; j < server.locations.size(); j++)
        {
            if (server.locations[j].path == "/")
            {
                hasRootLocation = true;
                break;
            }
        }
        if (!hasRootLocation)
        {
            throw std::runtime_error("Error : No / location in config file");
        }

        // Vérifier les locations
        for (size_t j = 0; j < server.locations.size(); j++)
        {
            LocConfig &loc = server.locations[j];

            //  Vérifier les méthodes HTTP
            for (size_t k = 0; k < loc.allowed_methods.size(); k++)
            {
                std::string method = loc.allowed_methods[k];

                if (method != "GET" && method != "POST" && method != "DELETE")
                {
                    throw std::runtime_error("Error: Invalid http method : " + method + " in location " + loc.path);
                }
            }

            //  Vérifier la configuration CGI
            if (!loc.cgi_pass.empty() && access(loc.cgi_pass.c_str(), X_OK) == -1)
            {
                throw std::runtime_error("Error: CGI path not executable or existant: " + loc.cgi_pass);
            }
        }
    }
}
