#include "Request.hpp"

Request::Request(std::string req): head(ReqHeader(req)) {
}

Request::~Request() {
}

ReqHeader &Request::getHeader()
{
	return (this->head);
}