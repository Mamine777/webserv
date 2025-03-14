/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 18:55:28 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/14 19:21:40 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"

#include <dirent.h>

#include <fstream>
#include <string>

#include "../inc/Response.h"
#include "../inc/cgi.h"
#include "sys/stat.h"
#include "sys/types.h"

server::server(ServerConfig &config) : _config(config) {}

server::~server() {}

bool findout(std::string Method, std::vector<std::string> allowed_method) {
    for (size_t i = 0; i < allowed_method.size(); i++)
        if (Method == allowed_method[i]) return true;
    return false;
}

bool request_checking(std::string path) {
    if (path.find("src") != std::string::npos ||
        path.find("inc") != std::string::npos ||
        path.find("Makefile") != std::string::npos)
        return false;
    return true;
}
bool hasDirTraversal(std::string path) {
    size_t first = 0;
    for (; first < path.length();) {
        size_t second = path.find('/', first + 1);
        if (first + 1 > path.size())
            return (false);
        std::string temp = path.substr(first + 1, second - first - 1);
        if (temp == "..") return true;
        first = second;
    }
    return false;
}

void handleMethod(LocConfig *location, Response &response, Request &req,
                  cgi &CGI, server *thisPtr) {
    if (hasDirTraversal(req.getPath())) {
        thisPtr->errorPage(403, response);
        return;
    }
    if (req.getMethod() == "GET" && findout("GET", location->allowed_methods)) {
        /* std::string filePath = "." + req.getPath();
        if (!location->cgi_pass.empty() && req.getPath().find(location->path) ==
        0) { std::string cgiOutput = CGI.executeCgi(location->cgi_pass, "");
        response.setStatus(200);
        response.setBody(cgiOutput);
        }
        else if (filePath == "./")
        {
                filePath += location->index;
                response.setBodyFromFile(filePath);
                if (response.getStatus() == 404)
                        response.setBody("404 Not Found");
        }
        else {
                response.setBodyFromFile(filePath);
                if (response.getStatus() == 404)
                        response.setBody("404 Not Found");
                else
                        response.setStatus(200);
        } */
        if (!location->cgi_pass.empty()) {
            (void)CGI;  // mute compilation error
            // TODO: Do something better than whats above, next time if you dont
            // know just ask
        } else if (location->directory_listing) {
            std::string path = req.getPath().replace(0, location->path.size(),
                                                     location->root + "/");
            struct stat s;
            if (stat(path.c_str(), &s) != 0) {
                thisPtr->errorPage(404, response);
                return ;
            }
            if (s.st_mode & S_IFREG) {
                if (access(path.c_str(), R_OK)) {
                    thisPtr->errorPage(403, response);
                    return ;
                }
                response.setBodyFromFile(path);
                if (path.find_last_of("/") + 1 <= path.size())
                    response.setType(path.substr(path.find_last_of("/") + 1));
                else
                    response.setType(path);
                return;
            } else if (s.st_mode & S_IFDIR) {
                std::string ret =
                    "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n	<meta "
                    "charset=\"UTF-8\">\n	<meta name=\"viewport\" "
                    "content=\"width=device-width, "
                    "initial-scale=1.0\">\n	"
                    "<title>Document</title>\n</head>\n<body>\n";
                ret +=
                    "	<h1>AutoIndex for: " + req.getPath() + "</h1>\n";
                DIR *dir;
                struct dirent *ent;
                std::vector<std::string>    dirs;
                std::vector<std::string>    files;
                if ((dir = opendir(path.c_str())) == NULL) {
                    thisPtr->errorPage(500, response);
                    closedir(dir);
                    return;
                }
                ret += "	<ul>\n";
                while ((ent = readdir(dir)) != NULL) {
                    if (std::string(ent->d_name) == ".." || std::string(ent->d_name) == ".") continue ;
                    if (ent->d_type == DT_DIR)
                        dirs.push_back(std::string(ent->d_name));
                    else
                        files.push_back(std::string(ent->d_name));
                }
                std::sort(dirs.begin(), dirs.end());
                std::sort(files.begin(), files.end());
                if (location->path != req.getPath())
                ret += "		<li><a href=\"" + req.getPath() + "/..\">" + "../" + "</a></li>\n";
                for (std::vector<std::string>::iterator it = dirs.begin(); it != dirs.end(); ++it)
                    ret += "		<li><a href=\"" + req.getPath() + "/" + *it + "\">" + *it + "/</a></li>\n";
                for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
                    ret += "		<li><a href=\"" + req.getPath() + "/" + *it + "\">" + *it + "</a></li>\n";
                /* while ((ent = readdir(dir)) != NULL) {
                    if (location->path == req.getPath() &&
                        std::string(ent->d_name) == "..")
                        continue;
                    if (std::string(ent->d_name) == ".") continue;
                    std::string filename = ent->d_name;
                    if (ent->d_type == DT_DIR) filename += "/";
                    ret += "		<li><a href=\"" + req.getPath() + "/" +
                           ent->d_name + "\">" + filename + "</a></li>\n";
                } */
                ret += "	</ul>\n</body>\n</html>";
                closedir(dir);
                response.setType("text/html");
                response.setBody(ret);
            }
        } else if (!location->directory_listing && !location->root.empty()) {
            std::string path = req.getPath().replace(0, location->path.size(),
                                                     location->root + "/");
            std::string filename = path;
            if (path.find_last_of("/") <= path.size())
                filename = path.substr(path.find_last_of("/"));
                
            
            if (filename.find('.') == std::string::npos)
                path += "/" + location->index;
            struct stat s;
            if (stat(path.c_str(), &s)) {
                thisPtr->errorPage(404, response);
                return ;
            }
            if (access(path.c_str(), R_OK)) {
                thisPtr->errorPage(403, response);
                return ;
            }
            response.setBodyFromFile(path);
            if (path.find_last_of('/') + 1 <= path.size())
                response.setType(path.substr(path.find_last_of('/') + 1));
            else
                response.setType(path);
        } else if (!location->redirect_url.empty()) {
            response.setHeader("Connection", "close");
            response.setHeader("Location", location->redirect_url);
            response.setStatus(301);
        }
    } else if (req.getMethod() == "POST" &&
               findout("POST", location->allowed_methods)) {
        std::string path = req.getPath().replace(0, location->path.size(),
                                                 location->upload_store + "/");
        std::ofstream file(path.c_str());
        if (file.fail()) {
            thisPtr->errorPage(500, response);
            return;
        }
        file << req.getBody();
        if (!location->redirect_url.empty()) {
            response.setHeader("Connection", "close");
            response.setHeader("Location", location->redirect_url);
            response.setStatus(301);
        } else {
            response.setHeader("Connection", "close");
            response.setStatus(200);
        }
    } else if (req.getMethod() == "DELETE" &&
               findout("DELETE", location->allowed_methods)) {
        std::string filePath = "." + req.getPath();
        if (!request_checking(req.getPath())) {
            thisPtr->errorPage(403, response);
        } else {
            if (remove(filePath.c_str()) == 0) {
                response.setStatus(200);
                response.setBody("File deleted successfully");
            }
        }
    } else {
        thisPtr->errorPage(405, response);
    }
}

void server::dispatchRequest(Request &req, Response &res) {
    std::string longest = "";
    LocConfig *loc;
    /* for (size_t i = 0; i < this->_config.locations.size(); i++)
    {
            if (req.getPath().substr(0, this->_config.locations[i].path.size())
    == this->_config.locations[i].path)
            {
                    if (this->_config.locations[i].path.size() > longest.size())
    { loc = &this->_config.locations[i]; longest =
    this->_config.locations[i].path;
                    }
            }
    } */
    std::vector<LocConfig>::iterator locIt = this->_config.locations.begin();
    for (; locIt != this->_config.locations.end(); ++locIt) {
        if ((!req.getPath().compare(0, locIt->path.size(), locIt->path) &&
             (locIt->path.size() == req.getPath().size() ||
              req.getPath()[locIt->path.size()] == '/') &&
             locIt->path.size() > longest.size()) ||
            (locIt->path == "/" && longest.empty())) {
            loc = &(*locIt);
            longest = loc->path;
        }
    }
    if (!loc) {
        this->errorPage(404, res);
        return;
    }

    cgi CGI;
    if (req.getMethod() == "GET" || req.getMethod() == "POST" ||
        req.getMethod() == "DELETE")
        handleMethod(loc, res, req, CGI, this);
    else {
        this->errorPage(405, res);
        return;
    }
    res.setRetVal(res.toString());
}

ServerConfig &server::getConfig() { return this->_config; }

void server::errorPage(int code, Response &res) {
    std::string filePath;
    if (this->_config.error_pages.find(code) != this->_config.error_pages.end())
        filePath = this->_config.error_pages[code];
    else if (code == 404)
        filePath = "errors/404.html";
	else
		filePath = "errors/template.html";

    std::string fileStr;
    if (filePath == "errors/template.html") {
        std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
        std::stringstream ss;

        if (file.fail()) {
            std::cerr << "Failed to open template error page" << std::endl;
            return;
        }
        ss << file.rdbuf();
        file.close();
        fileStr = ss.str();

        std::stringstream   codess;
        codess << code;
        std::string strCode = codess.str();
        fileStr.replace(fileStr.find("{{.code}}"), 9, strCode);
        fileStr.replace(fileStr.find("{{.message}}"), 12,
                        Response::getStatusMessage(code));
    } else {
        std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);

        if (file.fail()) {
            std::cerr << "Failed to open " << code << " error page"
                      << std::endl;
            return;
        }
        std::stringstream ss;
        ss << file.rdbuf();
        file.close();
        fileStr = ss.str();
    }

    res.setBody(fileStr);
    res.setStatus(code);
    res.setType("text/html");
    std::stringstream lengthss;
    lengthss << fileStr.size();
    res.setHeader("Content-Length", lengthss.str());
    res.setHeader("Connection", "close");
    res.setRetVal(res.toString());
}
