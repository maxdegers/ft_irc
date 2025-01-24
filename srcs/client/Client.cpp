#include "Client.hpp"
#include <cstring>
#include <arpa/inet.h>
#include "Define.hpp"
#include "../channels/Channel.hpp"
#include <sys/socket.h>

/* Constructors ************************************************************* */
Client::Client(int fd, const std::string &ip, Server *serv) : _server(serv), _fd(fd), _status(NOT_REGISTERED), _ip(ip) {}

Client::~Client() {}

/* Operators **************************************************************** */
bool Client::operator==(const Client &compare)
{
	if (this->_nickname == compare._nickname && this->_username == compare._username
		&& this->_ip == compare._ip && this->_fd == compare._fd)
		return true;
	return false;
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

t_status Client::status() const
{
	return (_status);
}

std::string Client::nickname() const
{
	return (_nickname);
}

std::string	&Client::getUsername()
{
	return _username;
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

void Client::sendMessage(std::string message, Client *receive)
{
	send(receive->_fd, message.c_str(), message.size(), 0);
}

void Client::sendMessage(std::string message, Channel *receive)
{
	receive->shareMessage(message, _username);
}

void	Client::PASS(const std::string &str)
{
	std::cout <<"'" + str + "'" << std::endl;
	std::string error;

	if (this->_status > NOT_REGISTERED)
		error = ERR_ALREADYREGISTRED(this->_nickname);

	if (str.empty() && error.empty())
		error = ERR_NEEDMOREPARAMS("PASS");

	if (str.compare(this->_server->getPassword()) && error.empty())
		error = ERR_PASSWDMISMATCH;

	if (!error.empty())
	{
		this->sendError(_fd, error);
		return ;
	}

	this->_status = ONGOING_REGISTERING;
}

// void	Client::NICK(const std::string &str)
// {
// 	if (status() == NOT_REGISTERED)
// 		return (sendError(_fd, ERR_PWNOTCHECK));
// 	if (str.empty())
// 		return (sendError(_fd, ERR_NONICKNAMEGIVEN));

// 	std::string charset = "=#&:";


// 	for (int i = 0; str[i]; ++i) {
// 		if (std::string::npos != charset.find(str[i]) || iswspace(str[i]) || (i == 0 && isdigit(str[i]))) //TODO to endestend
// 			return (sendError(_fd, ERR_ERRONEUSNICKNAME(str)));
// 	}
// 	// if ("bot" == str)
// 	// 	return (sendError(_fd, ERR_NICKNAMEINUSE(str))); //TODO ADD IF BONUS


// 	if (_server->checkNick(str))
// 		return (sendError(_fd, ERR_NICKNAMEINUSE(str)));

// 	if (!_nickname.empty()) {
// 		sendMessage(RPL_PRENICK(_prefix, str)); //TODO SandMessage to server
// 		sendMessage(RPL_NICK(_nickname, str)); //TODO SandMessage to server
// 		_nickname = str;
// 		_prefix = _nickname + "!" + _username + "@" + _hostname; //TODO to endestend prefix
// 		return ;
// 	}

// 	_nickname = str;

// 	if (!_username.empty()) {
// 		_prefix = _nickname + "!" + _username + "@" + _hostname;
// 		_status = REGISTERED;
// 		return (sendError(_fd, RPL_WELCOME(_nickname, _nickname)));
// 	}
// }

