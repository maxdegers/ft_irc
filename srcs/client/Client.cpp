#include "Client.hpp"
#include <cstring>
#include <arpa/inet.h>
#include "Define.hpp"

/* Constructors ************************************************************* */
Client::Client(int fd, const std::string &ip, Server *serv) : _server(serv), _fd(fd), _status(NOT_REGISTERED), _ip(ip) {}

Client::~Client() {}

/* Getters ****************************************************************** */
int Client::fd() const
{
	return (_fd);
}

std::string Client::ip() const
{
	return (_ip);
}

t_status Client::status() const
{
	return (_status);
}

std::string Client::incompleteMessage() const
{
	return (_incompleteMessage);
}

/* Setters ****************************************************************** */
void Client::setIncompleteMessage(std::string msg)
{
	_incompleteMessage = msg;
}

void Client::setStatus(const t_status &status)
{
	_status = status;
}

/* Methods ****************************************************************** */
void Client::sendError(const int fd, const std::string &error)
{
	const char *err = error.c_str();
	send(fd, err, strlen(err), 0);
}


void	Client::PASS(std::string cmd ,std::string str)
{
	std::string error;


	if (this->_status > NOT_REGISTERED)
		error = ERR_ALREADYREGISTRED(this->_nickname);

	if (this->_password.empty() || error.empty())
		error = ERR_NEEDMOREPARAMS(cmd);

	if (str.compare(this->_server->getPassword()) || error.empty())
		error = ERR_PASSWDMISMATCH;

	if (!error.empty())
		this->sendError(_fd, error);

	this->_status = ONGOING_REGISTERING;
}
