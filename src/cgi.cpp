/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 16:33:48 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/08 16:47:24 by mokariou         ###   ########.fr       */
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
std::string cgi::executeCgi(std::string path, const std::string &query, LocConfig *location)
{
    int fd[2], pid;
    std::string output;
    char *temp1;

    if (pipe(fd) == -1) { error("pipe failed !\n 500 Internal Server Error"), exit(1); }
    pid = fork();
    if (pid < 0) { error("fork failed !"), exit(1); }

    if (pid == 0) {
        close(fd[0]);
        setenv("REQUEST_METHOD", "GET", 1);
        setenv("QUERY_STRING", query.c_str(), 1);
        
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
        free(temp1);
        free(temp);
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
    }
    return output;
}
