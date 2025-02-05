#include "Client.hpp"
#include <cstring>
#include <arpa/inet.h>
#include <sstream>
#include "Define.hpp"
#include "../channels/Channel.hpp"
#include "Log.hpp"
#include <sys/socket.h>

/* Constructors ************************************************************* */
Client::Client(int fd, const std::string &ip, Server *serv) : 	_server(serv), _fd(fd), _status(NOT_REGISTERED), _ip(ip)
{
}

Client::~Client()
{
	Log::debug("Client object deleted");
}

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

std::string &Client::getPrefix()
{
	return _prefix;
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
	std::string error;

	if (this->_status > NOT_REGISTERED)
		error = ERR_ALREADYREGISTRED(this->_nickname);

	if (str.empty() && error.empty())
		error = ERR_NEEDMOREPARAMS("PASS");

	if (str != this->_server->getPassword() && error.empty())
		error = ERR_PASSWDMISMATCH;

	if (!error.empty())
	{
		this->sendError(_fd, error);
		return ;
	}

	this->_status = ONGOING_REGISTERING;
}

void Client::NICK(const std::string &str) {
	if (status() == NOT_REGISTERED) {
		return sendError(_fd, ERR_PWNOTCHECK);
	}

	if (str.empty()) {
		return sendError(_fd, ERR_NONICKNAMEGIVEN);
	}

	std::string invalidChars = "=#&:";
	for (size_t i = 0; i < str.size(); ++i) {
		if (invalidChars.find(str[i]) != std::string::npos || iswspace(str[i]) || (i == 0 && isdigit(str[i]))) {
			return sendError(_fd, ERR_ERRONEUSNICKNAME(str));
		}
	}

	if (_server->checkNick(str)) {
		return sendError(_fd, ERR_NICKNAMEINUSE(str));
	}

	if (!_nickname.empty())
	{
		for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			sendMessage(RPL_PRENICK(_prefix, str), *it); // Message de pré-changement
			sendMessage(RPL_NICK(_nickname, str), *it); // Notification du changement
		}
	}

	_nickname = str;

	_prefix = _nickname + "!" + _username + "@" + _hostname;

	if (!_username.empty()) {
		_status = REGISTERED;
		sendMessage(RPL_WELCOME(_nickname, _nickname), this);
		Log::info(_nickname + " is now registered");
		return ;
	}

}


void Client::USER(const std::string &str) {
	if (status() == NOT_REGISTERED) {
		return sendError(_fd, ERR_PWNOTCHECK);
	}
	
	if (this->status() == REGISTERED) {
		return sendError(_fd, ERR_ALREADYREGISTRED(this->_username));
	}

	if (str.empty() || str.find(' ') == std::string::npos) {
		return sendError(_fd, ERR_NEEDMOREPARAMS("USER"));
	}

	std::istringstream iss(str);
	std::string username, mode, unused, realname;

	if (!(iss >> username >> mode >> unused)) {
		return sendError(_fd, ERR_NEEDMOREPARAMS("USER"));
	}

	std::getline(iss, realname);
	if (realname.empty() || realname[1] != ':') {
		return sendError(_fd, ERR_NEEDMOREPARAMS("USER"));
	}

	realname = realname.substr(2);
	if (username.empty() || mode != "0" || unused != "*") {
		return sendError(_fd, ERR_NEEDMOREPARAMS("USER"));
	}

	this->_username = username;
	this->_realname = realname;

	if (!this->_nickname.empty()) {
		this->_prefix = this->_nickname + "!" + this->_username + "@" + this->_hostname;
		this->_status = REGISTERED;
		sendMessage(RPL_WELCOME(this->_nickname, this->_nickname), this);
		Log::info(_nickname + " is now registered");
		return ;

	}
}

void Client::addChannel(Channel *channel)
{
	_channels.push_back(channel);
}

void Client::removeChannel(Channel *channel)
{
	for (std::vector<Channel *>::iterator it = _channels.begin(); it < _channels.end(); it++)
	{
		if ((*it) == channel)
		{
			_channels.erase(it);
			return ;
		}
	}
}

void Client::removeChannels()
{
	for (std::vector<Channel *>::iterator it = _channels.begin(); it < _channels.end(); it++)
		(*it)->removeUser(this);
}
