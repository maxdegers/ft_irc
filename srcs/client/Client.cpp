#include "Client.hpp"

/* Constructors ************************************************************* */
Client::Client(int fd,Server *serv) : _fd(fd), _status(NOT_REGISTERED), _server(serv) {}

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

/* Setters ****************************************************************** */

void Client::setIP(const std::string &ip)
{
	_ip = ip;
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
	return ;
}
