/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndexHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@stduent.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 14:36:31 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/25 16:24:28 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndexHandler.hpp"
#include "dirent.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "fstream"

AutoIndexHandler::AutoIndexHandler(std::string urlPath, std::string rootPath): urlPath(urlPath), rootPath(rootPath) {}

AutoIndexHandler::~AutoIndexHandler() {}

std::string AutoIndexHandler::getUriPath() {
	return this->urlPath;
}

static bool	hasDirTraversal(std::string path) {
	size_t	first = 0;
	for (; first < path.length();) {
		size_t	second = path.find('/', first + 1);
		std::string	temp = path.substr(first + 1, second - first - 1);
		if (temp == "..")
			return true;
		first = second;
	}
	return false;
}

void AutoIndexHandler::execute(Request& req, Response& res) {
	if (hasDirTraversal(req.getHeader().getRessource())) {
		res.status(403).sendText("Forbidden Request");
		return;
	}
	std::string path = req.getHeader().getRessource().replace(0, this->urlPath.size(), this->rootPath + "/");
	struct stat s;
	if (stat(path.c_str(), &s) != 0) {
		res.status(404).sendText("Couldnt find File");
	}
	if (s.st_mode & S_IFREG) {
		res.sendFile(path);
		return ;
	} else if (s.st_mode & S_IFDIR) {
		std::string ret = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n	<meta charset=\"UTF-8\">\n	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n	<title>Document</title>\n</head>\n<body>\n";
		ret += "	<h1>AutoIndex for: " + req.getHeader().getRessource() + "</h1>\n";
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(path.c_str())) == NULL) {
			res.status(500).sendText("Internal Error");
			return ;
		}
		ret += "	<ul>\n";
		while ((ent = readdir(dir)) != NULL) {
			if (this->urlPath == req.getHeader().getRessource() && std::string(ent->d_name) == "..")
				continue ; 
			if (std::string(ent->d_name) == ".")
				continue ; 
			std::string	filename = ent->d_name;
			if (ent->d_type == DT_DIR)
				filename += "/";
			ret += "		<li><a href=\"" + req.getHeader().getRessource() + "/" + ent->d_name + "\">" + filename + "</a></li>\n";
		}
		ret += "	</ul>\n</body>\n</html>";
		res.type("text/html");
		res.sendText(ret);
	}
	res.status(500).sendText("Server encounted an error");
}
