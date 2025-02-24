/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fghysbre <fghysbre@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 16:31:48 by fghysbre          #+#    #+#             */
/*   Updated: 2025/02/24 22:39:49 by fghysbre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

#include <sys/socket.h>
#include <sys/types.h>

#include <fstream>
#include <sstream>

Response::Response(int clientSocket, std::string httpVer) : clientSocket(clientSocket), head(ResHeader(httpVer)) {
}

Response::~Response() {
}

int Response::getClientSock() {
	return this->clientSocket;
}

void Response::append(std::string field) {
	this->append(field, "");
}

void Response::append(std::string field, std::string value) {
	this->head.getParams().insert(std::pair<std::string, std::string>(field, value));
}

void Response::attachment() {
	this->head.setParam("Content-Disposition", "attachment");
}

void Response::attachment(std::string path) {
	std::string filename = path.substr(path.find_last_of('/') + 1);
	this->head.setParam("Content-Disposition", "attachment; filename=\"" + filename + "\"");
	if (filename.find_last_of('.') != std::string::npos)
		this->type(filename.substr(filename.find_last_of('.') + 1));
}

void Response::cookie(std::string name, std::string value) {
	this->head.setCookie(name, value);
}

Response &Response::status(unsigned int status) {
	this->head.setStatus(status);
	return (*this);
}

void Response::type(std::string type) {
	if (type.find('/') != std::string::npos) {
		this->head.setParam("Content-Type", type);
		return;
	}
	size_t pointpos = type.find_last_of('.');
	if (pointpos == std::string::npos) {		
		this->head.setParam("Content-Type", "text/plain");
		return;
	}
	std::string fileExt = type.substr(pointpos + 1);
	// https://developer.mozilla.org/en-US/docs/Web/HTTP/MIME_types/Common_types
	if (fileExt == "aac")
		this->head.setParam("Content-Type", "audio/aac");
	else if (fileExt == "abw")
		this->head.setParam("Content-Type", "application/x-abiword");
	else if (fileExt == "apng")
		this->head.setParam("Content-Type", "image/apng");
	else if (fileExt == "arc")
		this->head.setParam("Content-Type", "application/x-freearc");
	else if (fileExt == "avif")
		this->head.setParam("Content-Type", "image/avif");
	else if (fileExt == "avi")
		this->head.setParam("Content-Type", "video/x-msvideo");
	else if (fileExt == "azw")
		this->head.setParam("Content-Type", "application/vnd.amazon.ebook");
	else if (fileExt == "bin")
		this->head.setParam("Content-Type", "application/octet-stream");
	else if (fileExt == "bmp")
		this->head.setParam("Content-Type", "image/bmp");
	else if (fileExt == "bz")
		this->head.setParam("Content-Type", "application/x-bzip");
	else if (fileExt == "bz2")
		this->head.setParam("Content-Type", "application/x-bzip2");
	else if (fileExt == "cda")
		this->head.setParam("Content-Type", "application/x-cdf");
	else if (fileExt == "csh")
		this->head.setParam("Content-Type", "application/x-csh");
	else if (fileExt == "css")
		this->head.setParam("Content-Type", "text/css");
	else if (fileExt == "csv")
		this->head.setParam("Content-Type", "text/csv");
	else if (fileExt == "doc")
		this->head.setParam("Content-Type", "application/msword");
	else if (fileExt == "docx")
		this->head.setParam("Content-Type", "application/vnd.openxmlformats-officedocument.wordprocessingml.document");
	else if (fileExt == "eot")
		this->head.setParam("Content-Type", "application/vnd.ms-fontobject");
	else if (fileExt == "epub")
		this->head.setParam("Content-Type", "application/epub+zip");
	else if (fileExt == "gz")
		this->head.setParam("Content-Type", "application/x-gzip");
	else if (fileExt == "gif")
		this->head.setParam("Content-Type", "image/gif");
	else if (fileExt == "htm" || fileExt == "html")
		this->head.setParam("Content-Type", "text/html");
	else if (fileExt == "ico")
		this->head.setParam("Content-Type", "image/vnd.microsoft.icon");
	else if (fileExt == "ics")
		this->head.setParam("Content-Type", "text/calendar");
	else if (fileExt == "jar")
		this->head.setParam("Content-Type", "application/java-archive");
	else if (fileExt == "jpeg" || fileExt == "jpg")
		this->head.setParam("Content-Type", "image/jpeg");
	else if (fileExt == "js")
		this->head.setParam("Content-Type", "text/javascript");
	else if (fileExt == "json")
		this->head.setParam("Content-Type", "application/json");
	else if (fileExt == "jsonld")
		this->head.setParam("Content-Type", "application/ld+json");
	else if (fileExt == "mid" || fileExt == "midi")
		this->head.setParam("Content-Type", "audio/x-midi");
	else if (fileExt == "mjs")
		this->head.setParam("Content-Type", "text/javascript");
	else if (fileExt == "mp3")
		this->head.setParam("Content-Type", "audio/mpeg");
	else if (fileExt == "mp4")
		this->head.setParam("Content-Type", "video/mp4");
	else if (fileExt == "mpeg")
		this->head.setParam("Content-Type", "video/mpeg");
	else if (fileExt == "mpkg")
		this->head.setParam("Content-Type", "application/vnd.apple.installer+xml");
	else if (fileExt == "odp")
		this->head.setParam("Content-Type", "application/vnd.oasis.opendocument.presentation");
	else if (fileExt == "ods")
		this->head.setParam("Content-Type", "application/vnd.oasis.opendocument.spreadsheet");
	else if (fileExt == "odt")
		this->head.setParam("Content-Type", "application/vnd.oasis.opendocument.text");
	else if (fileExt == "oga")
		this->head.setParam("Content-Type", "audio/ogg");
	else if (fileExt == "ogv")
		this->head.setParam("Content-Type", "video/ogg");
	else if (fileExt == "ogx")
		this->head.setParam("Content-Type", "application/ogg");
	else if (fileExt == "opus")
		this->head.setParam("Content-Type", "audio/ogg");
	else if (fileExt == "otf")
		this->head.setParam("Content-Type", "font/otf");
	else if (fileExt == "png")
		this->head.setParam("Content-Type", "image/png");
	else if (fileExt == "pdf")
		this->head.setParam("Content-Type", "application/pdf");
	else if (fileExt == "php")
		this->head.setParam("Content-Type", "application/x-httpd-php");
	else if (fileExt == "ppt")
		this->head.setParam("Content-Type", "application/vnd.ms-powerpoint");
	else if (fileExt == "pptx")
		this->head.setParam("Content-Type", "application/vnd.openxmlformats-officedocument.presentationml.presentation");
	else if (fileExt == "rar")
		this->head.setParam("Content-Type", "application/vnd.rar");
	else if (fileExt == "rtf")
		this->head.setParam("Content-Type", "application/rtf");
	else if (fileExt == "sh")
		this->head.setParam("Content-Type", "application/x-sh");
	else if (fileExt == "svg")
		this->head.setParam("Content-Type", "image/svg+xml");
	else if (fileExt == "tar")
		this->head.setParam("Content-Type", "application/x-tar");
	else if (fileExt == "tif" || fileExt == "tiff")
		this->head.setParam("Content-Type", "image/tiff");
	else if (fileExt == "ts")
		this->head.setParam("Content-Type", "video/mp2t");
	else if (fileExt == "ttf")
		this->head.setParam("Content-Type", "font/ttf");
	else if (fileExt == "txt")
		this->head.setParam("Content-Type", "text/plain");
	else if (fileExt == "vsd")
		this->head.setParam("Content-Type", "application/vnd.visio");
	else if (fileExt == "wav")
		this->head.setParam("Content-Type", "audio/wav");
	else if (fileExt == "weba")
		this->head.setParam("Content-Type", "audio/webm");
	else if (fileExt == "webm")
		this->head.setParam("Content-Type", "video/webm");
	else if (fileExt == "webp")
		this->head.setParam("Content-Type", "image/webp");
	else if (fileExt == "woff")
		this->head.setParam("Content-Type", "font/woff");
	else if (fileExt == "woff2")
		this->head.setParam("Content-Type", "font/woff2");
	else if (fileExt == "xhtml")
		this->head.setParam("Content-Type", "application/xhtml+xml");
	else if (fileExt == "xls")
		this->head.setParam("Content-Type", "application/vnd.ms-excel");
	else if (fileExt == "xlsx")
		this->head.setParam("Content-Type", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
	else if (fileExt == "xml")
		this->head.setParam("Content-Type", "application/xml");
	else if (fileExt == "xul")
		this->head.setParam("Content-Type", "application/vnd.mozilla.xul+xml");
	else if (fileExt == "zip")
		this->head.setParam("Content-Type", "application/zip");
	else if (fileExt == "3gp")
		this->head.setParam("Content-Type", "video/3gpp");
	else if (fileExt == "3g2")
		this->head.setParam("Content-Type", "video/3gpp2");
	else if (fileExt == "7z")
		this->head.setParam("Content-Type", "application/x-7z-compressed");
	else
		this->head.setParam("Content-Type", "text/plain");
}

void Response::sendText(std::string str) {
	std::stringstream ss;
	ss << str.length();
	this->head.setParam("Content-Type", "text/plain");
	this->head.setParam("Content-Length", ss.str());
	this->head.setParam("Connection", "close");
	std::string ret = this->head.toString();
	ret += str;
	send(this->clientSocket, ret.c_str(), ret.size(), 0);
}

void Response::sendFile(std::string path) {
	std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
	if (file.fail()) {
		std::cerr << "Failed to open file " << path << std::endl;
		this->status(404).sendText("File not found");
		return;
	}

	std::stringstream ss;
	ss << file.tellg();
	file.seekg(0, std::ios::beg);
	std::string filename = path;
	if (path.find('/') != std::string::npos)
		filename = filename.substr(path.find_last_of('/') + 1);
	this->type(filename);
	std::cout << "type = " << this->head.getField("Content-Type") << std::endl;
	this->head.setParam("Content-Length", ss.str());
	this->head.setParam("Connection", "close");
	std::string ret = this->head.toString();
	if (send(this->clientSocket, ret.c_str(), ret.size(), 0) == -1) {
		std::cerr << "Failed to send header of file " << path << std::endl;
		file.close();
		return;
	}

	char buffer[4098];
	while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
		ssize_t bytes_sent = send(this->clientSocket, buffer, file.gcount(), 0);
		std::cout << "file.gcount() = " << file.gcount() << std::endl;
		if (bytes_sent == -1) {
			std::cerr << "Failed to send file" << path << std::endl;
			file.close();
			return;
		}
	}
	file.close();
}
