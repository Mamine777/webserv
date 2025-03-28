
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <sstream>   
#include <algorithm> 
#include <unistd.h>

struct LocConfig 
{
    std::string path;
    std::string root;
    std::string index;
    std::vector<std::string> allowed_methods;
    std::string redirect_url;
    bool directory_listing;
    std::string upload_store;
    std::string cgi_pass;
    std::vector<std::string> cgi_extensions;

};

struct ServerConfig 
{
    int port;
    std::string host;
    std::string server_name;
    size_t client_max_body_size;
    std::map<int, std::string> error_pages;
    std::vector<LocConfig> locations;
};

struct Config 
{
    std::vector<ServerConfig> servers;
    void printConfig() const;
    void ErrorsConfig();
};


#endif
