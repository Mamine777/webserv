#include "Config/ParseConfig.hpp"

ParseConfig::ParseConfig(std::string filename, Config &config) : _filename(filename), config(config) {}

ParseConfig::~ParseConfig() {}

size_t ParseConfig::parseSize(const std::string &sizeStr)
{
    if (sizeStr.empty()) return 0;

    size_t num = 0;
    size_t i = 0;
    
    while (i < sizeStr.length() && std::isdigit(sizeStr[i])) {
        num = num * 10 + (sizeStr[i] - '0');
        i++;
    }

    if (i < sizeStr.length()) {
        char unit = std::toupper(sizeStr[i]);
        switch (unit) {
            case 'K': return num * 1024;
            case 'M': return num * 1024 * 1024;
            case 'G': return num * 1024 * 1024 * 1024;
            default: throw std::runtime_error("Invalid unit for client_max_body_size");
        }
    }else{
        if (num < 499)
            return 0;
    }
    return num;
}

void ParseConfig::parse() {
    std::ifstream file(_filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file");
    }

    ServerConfig current_server;
    current_server.port = 0;
    current_server.client_max_body_size = 0;
    LocConfig current_loc;
    current_loc.directory_listing = false;
    std::string line;
    bool in_serv = false;
    bool in_loc = false;

    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key.empty() || key[0] == '#') continue;

        if (key == "server") {
            if (in_serv && !in_loc) {
                config.servers.push_back(current_server);
                current_server = ServerConfig();
                current_server.port = 0;
                current_server.client_max_body_size = 0;
            }
            in_serv = true;
            in_loc = false;
        }
        else if (key == "location") {
            if (in_loc) {
                current_server.locations.push_back(current_loc);
                current_loc = LocConfig();
            }
            in_loc = true;
            iss >> current_loc.path;
        }
        else if (key == "}") {
            if (in_loc) {
                current_server.locations.push_back(current_loc);
                current_loc = LocConfig();
                current_loc.directory_listing = false;
                in_loc = false;
            } else if (in_serv) {
                config.servers.push_back(current_server);
                current_server = ServerConfig();
                current_server.port = 0;
                current_server.client_max_body_size = 0;
                in_serv = false;
            }
        }
        else if (in_serv && !in_loc) {
            if (key == "listen") {
                iss >> current_server.port;
            } else if (key == "host") {
                iss >> current_server.host;
                if (!current_server.host.empty() && current_server.host[current_server.host.length() - 1] == ';') 
                    current_server.host.erase(current_server.host.length() - 1);
            } else if (key == "server_name") {
                iss >> current_server.server_name;
                if (!current_server.server_name.empty() && current_server.server_name[current_server.server_name.length() - 1] == ';') 
                    current_server.server_name.erase(current_server.server_name.length() - 1);
            } else if (key == "error_page") {
                int code;
                std::string path;
                iss >> code >> path;
                if (!path.empty() && path[path.length() - 1] == ';') 
                    path.erase(path.length() - 1);
                current_server.error_pages[code] = path;
            } else if (key == "client_max_body_size") {
                std::string size;
                iss >> size;
                if (!size.empty() && size[size.length() - 1] == ';') 
                    size.erase(size.length() - 1);
                current_server.client_max_body_size = parseSize(size);
            }
        }
        else if (in_loc) {
            if (key == "root") {
                iss >> current_loc.root;
                if (!current_loc.root.empty() && current_loc.root[current_loc.root.length() - 1] == ';') 
                    current_loc.root.erase(current_loc.root.length() - 1);
            } else if (key == "index") {
                iss >> current_loc.index;
                if (!current_loc.index.empty() && current_loc.index[current_loc.index.length() - 1] == ';') 
                    current_loc.index.erase(current_loc.index.length() - 1);
            } else if (key == "allow_methods") {
                std::string method;
                while (iss >> method) {
                    if (!method.empty() && method[method.length() - 1] == ';') 
                        method.erase(method.length() - 1);
                    if (method == "GET" || method == "POST" || method == "DELETE") {
                        current_loc.allowed_methods.push_back(method);
                    }
                }
            } else if (key == "redirect") {
                iss >> current_loc.redirect_url;
                if (!current_loc.redirect_url.empty() && current_loc.redirect_url[current_loc.redirect_url.length() - 1] == ';') 
                    current_loc.redirect_url.erase(current_loc.redirect_url.length() - 1);
            } else if (key == "directory_listing") {
                std::string value;
                iss >> value;
                if(value == "on" || value == "on;")
                {
                    current_loc.directory_listing = true;
                }else 
                {
                    current_loc.directory_listing = false;
                };
                
            } else if (key == "upload_store") {
                iss >> current_loc.upload_store;
                if (!current_loc.upload_store.empty() && current_loc.upload_store[current_loc.upload_store.length() - 1] == ';') 
                    current_loc.upload_store.erase(current_loc.upload_store.length() - 1);
            } else if (key == "cgi_pass") {
                iss >> current_loc.cgi_pass;
                if (!current_loc.cgi_pass.empty() && current_loc.cgi_pass[current_loc.cgi_pass.length() - 1] == ';') 
                    current_loc.cgi_pass.erase(current_loc.cgi_pass.length() - 1);
            } else if (key == "cgi_extensions") {
                std::string ext;
                while (iss >> ext) {
                    if (!ext.empty() && ext[ext.length() - 1] == ';') 
                        ext.erase(ext.length() - 1);
                    if (!ext.empty() && ext[0] == '.') {
                        current_loc.cgi_extensions.push_back(ext);
                    }
                }
            }
        }
    }
    
    if (in_loc) {
        current_server.locations.push_back(current_loc);
    }
    if (in_serv) {
        config.servers.push_back(current_server);
    }
}
