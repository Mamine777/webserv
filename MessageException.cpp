#include "MessageException.hpp"

MessageException::MessageException(std::string message): message(message) {}

MessageException::MessageException(char *c_message): message(std::string(c_message)) {}

MessageException::~MessageException() throw() {}

const char *MessageException::what() const throw()
{
	return this->message.c_str();
}