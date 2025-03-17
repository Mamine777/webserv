
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 16:33:48 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/13 12:06:03 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/cgi.h"
#include "../inc/server.h"

bool    hasExt(const std::string &filename, const std::string &ext)
{
    size_t pos = filename.find('.', filename.find_last_of('/') + 1);
    
    if (pos == std::string::npos)
        return false;
    std::string exetension = filename.substr(pos + 1, filename.find('?', pos + 1) - pos - 1);
	if (exetension == ext)
        return true;
    return false;
}

void	error(const char *str) {std::cerr << str << std::endl;}

bool dup(int old_fd, int new_fd)
{
    if (dup2(old_fd, new_fd) == -1 || close(old_fd) == -1)
        return false;
    return true;
}

bool    findExtension(LocConfig *location, std::string fileType)
{
    for (size_t i = 0; i < location->cgi_extensions.size(); i++)
    {
        if (fileType == location->cgi_extensions[i])
            return true;
    }
    return false;
}

std::string do_stuff(std::string path, LocConfig *location)
{
    std::string newPath = path;
    size_t pos = path.find(location->path);
	//fix here for when cgi handles base location ("/")
    if (pos != std::string::npos)
        newPath.replace(pos, location->path.length(), location->cgi_pass + "/"); 
    return newPath;
}

//TODO: fix errors in pipex order
std::string cgi::executeCgi(std::string pathOr, const std::string &query, LocConfig *location, Request req)
{
    int fd[2], pid;
    std::string output;

    std::string path = do_stuff(pathOr, location);

    std::map<std::string, std::string> envMap;
    envMap["REQUEST_METHOD"] = req.getMethod();
    envMap["QUERY_STRING"] = query;
    envMap["SCRIPT_NAME"] = path;
    envMap["SCRIPT_FILENAME"] = path;
    envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
    envMap["REDIRECT_STATUS"] = "200";

    std::vector<char*> envp;
    for (std::map<std::string, std::string>::iterator it = envMap.begin(); it != envMap.end(); ++it) {
        envp.push_back(strdup((it->first + "=" + it->second).c_str()));
    }
    envp.push_back(NULL);

    if (pipe(fd) == -1) {
        error("pipe failed !\n 500 Internal Server Error");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        error("fork failed !");
        exit(1);
    }

    if (pid == 0) {
        close(fd[0]);
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            error("dup2 failed");
            exit(1);
        }
        close(fd[1]);

        std::string interpreter;
        if (hasExt(path, "py") && findExtension(location, ".py"))
            interpreter = "/usr/bin/python3";
        else if (hasExt(path, "php") && findExtension(location, ".php"))
            interpreter = "/usr/bin/php";
        else if (hasExt(path, "js") && findExtension(location, ".js"))
            interpreter = "/home/boyflo06/.nvm/versions/node/v23.8.0/bin/node";
        else {
            error("not allowed type of execution file");
            exit(1);
        }

        std::vector<char*> argv;
		size_t pointpos = path.find('.', path.find_last_of('/') + 1);
		path = path.substr(0, path.find('?', pointpos != std::string::npos ? pointpos : 0));
        argv.push_back(const_cast<char*>(interpreter.c_str()));
        argv.push_back(const_cast<char*>(path.c_str()));
        argv.push_back(NULL);

        execve(argv[0], argv.data(), envp.data());

        error("exec failed");
        exit(1);
    } 
    else {
        close(fd[1]);
        char buffer[4096];
        ssize_t bytes;
        
        while ((bytes = read(fd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes] = '\0';
            output += buffer;
        }
        close(fd[0]);
        waitpid(pid, NULL, 0);
    }
    return output;
}
