#include "StaticHandler.hpp"

StaticHandler::StaticHandler(std::string urlPath, std::string rootPath, std::string defhtml): uriPath(urlPath), rootPath(rootPath), defhtml(defhtml) {
}

StaticHandler::~StaticHandler() {}

std::string StaticHandler::getUriPath() {
	return this->uriPath;
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

void StaticHandler::execute(Request& req, Response& res) {
	if (hasDirTraversal(req.getHeader().getRessource())) {
		res.status(403).sendText("Forbidden Request");
		return;
	}

	std::string path = req.getHeader().getRessource().replace(0, this->uriPath.size(), this->rootPath + "/");
	std::string filename = path.substr(path.find_last_of('/') + 1);
	
	if (filename.find('.') != std::string::npos)
		res.sendFile(path);
	else {
		path += "/" + this->defhtml;
		res.sendFile(path);
	}
}
