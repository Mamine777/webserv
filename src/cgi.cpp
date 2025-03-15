
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
    size_t pos = filename.rfind('.');
    
    if (pos == std::string::npos)
        return false;
    std::string exetension = filename.substr(pos + 1);
    if (exetension == ext)
        return true;
    return false;
}

void	error(const char *str) {std::cout << str << std::endl;}

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
        newPath.replace(pos, location->path.length(), location->cgi_pass); 
    return newPath;
}

//TODO: fix errors in pipex order
std::string cgi::executeCgi(std::string pathOr, const std::string &query, LocConfig *location, Request req)
{
    int fd[2], pid;
    std::string output;
    char *temp1;


    std::string path = do_stuff(pathOr, location);

    std::map<std::string, std::string> envMap;
        envMap["REQUEST_METHOD"] = req.getMethod();
        envMap["QUERY_STRING"] = query;
        envMap["SCRIPT_NAME"] = path;
        envMap["SCRIPT_FILENAME"] = path;
        envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
        envMap["REDIRECT_STATUS"] = "200";

    char **envp = new char*[envMap.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = envMap.begin(); it != envMap.end(); ++it) {
        std::string envString = it->first + "=" + it->second;
        envp[i] = strdup(envString.c_str());
        i++;
    }
    envp[i] = NULL;
    if (pipe(fd) == -1) { error("pipe failed !\n 500 Internal Server Error"), exit(1); }
    pid = fork();
    if (pid < 0) { error("fork failed !"), exit(1); }

    if (pid == 0) {
        close(fd[0]);
        if (!dup(fd[1], STDOUT_FILENO)) {
            error("dup failed"), exit(1);
        }

        char *temp = strdup(path.c_str());
        if (hasExt(path, "py") && findExtension(location, ".py"))
            temp1 = strdup("/usr/bin/python3");
        else if (hasExt(path, "php") && findExtension(location, ".php"))
            temp1 = strdup("/usr/bin/php");
        else if (hasExt(path, "js") && findExtension(location, ".js"))
            temp1 = strdup("/usr/bin/node");
        else
            error("not allowed type of execution file"), free(temp), exit(1);
        char *argv[] = {temp1, temp,NULL};
        char *envp[] = {NULL};
        execve(argv[0], argv, envp);
		//FIXME: change these pointers for strings cuz they will leak if execve runs
        free(temp1);
        free(temp);
        for (int j = 0; envp[j]; j++) {
            free(envp[j]);
        }
       // delete[] envp;
	   //FIXME: exit in case execve doesnt work
    } 
    else {
        close(fd[1]);
        char buffer[4096];
        ssize_t bytes;
        
        while ((bytes = read(fd[0], buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[bytes] = '\0';
            output += buffer;
        }
        close(fd[0]);
        waitpid(pid, NULL, 0);
        for (int j = 0; envp[j]; j++) {
            free(envp[j]);
        }
        delete[] envp;
    }
    return output;
}
