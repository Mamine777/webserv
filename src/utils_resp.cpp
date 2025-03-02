/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_resp.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 16:30:53 by mokariou          #+#    #+#             */
/*   Updated: 2025/03/02 12:36:49 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/server.h"
#include "server.h"

std::string server::getContentType(const std::string &filePath) {
    size_t dotPos = filePath.find_last_of('.');

    //std::cout << "===================>" << filePath << std::endl;
    if (dotPos == std::string::npos) return "text/plain";

    std::string extension = filePath.substr(dotPos + 1);
    if (extension == "html") return "text/html";
    if (extension == "css") return "text/css";
    if (extension == "js") return "application/javascript";
    if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
    if (extension == "png") return "image/png";
    return "text/plain";
}


