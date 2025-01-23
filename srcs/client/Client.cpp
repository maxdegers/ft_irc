/* ***************** */
/*      WebServ      */
/* ***************** */

#include "Client.hpp"

/* Constructors ************************************************************* */
Client::Client() : _fd(-1) {}

Client::Client(const Client &src)
{
	*this = src;
}

Client::~Client() {}

/* Operators **************************************************************** */
Client &Client::operator=(const Client &src)
{
	_fd = src.fd();
	_ip = src.ip();
	return (*this);
}

/* Getters ****************************************************************** */
int Client::fd() const
{
	return (_fd);
}

std::string Client::ip() const
{
	return (_ip);
}

std::string Client::status() const
{
	return (_status);
}

std::string Client::incompleteMessage() const
{
	return (_incompleteMessage);
}

/* Setters ****************************************************************** */
void Client::setFD(int fd)
{
	_fd = fd;
}

void Client::setIP(const std::string &ip)
{
	_ip = ip;
}

void Client::setIncompleteMessage(std::string msg)
{
	_incompleteMessage = msg;
}

void Client::setStatus(const std::string &status)
{
	_status = status;
}

/* Methods ****************************************************************** */

void Client::sendError(const int fd, const std::string &error)
{
	const char *err = error.c_str();
	send(fd, err, strlen(err), 0);
}