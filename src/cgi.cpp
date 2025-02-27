/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 16:33:48 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/25 15:07:06 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/cgi.h"
#include "../inc/server.h"


bool    cgi::isCGIrequest(const std::string& path)
{
	if (path.find("/home") != std::string::npos)
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

std::string cgi::executeCgi(std::string path, const std::string &query)
{
    int fd[2], pid;

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
        char *argv[] = {temp, NULL};
        execve(argv[0], argv, NULL);
        free(temp);
        error("execution failed"), exit(1);
    } 
    else {
        close(fd[1]);
        char buffer[4096];
        std::string output;
        ssize_t bytes;
        
        while ((bytes = read(fd[0], buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[bytes] = '\0';
            output += buffer;
        }
        close(fd[0]);
        waitpid(pid, NULL, 0);
        return output;
    }
}
