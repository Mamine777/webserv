/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_resp.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 16:30:53 by mokariou          #+#    #+#             */
/*   Updated: 2025/02/23 16:34:35 by mokariou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"
#include "Response.h"

std::string server::getContentType(const std::string &filePath) {
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos == std::string::npos) return "text/plain";

    std::string extension = filePath.substr(dotPos + 1);
    if (extension == "html") return "text/html";
    if (extension == "css") return "text/css";
    if (extension == "js") return "application/javascript";
    if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
    if (extension == "png") return "image/png";
    return "text/plain";
}
