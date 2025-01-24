
#include "Client.hpp"
#include "../channels/Channel.hpp"
#include <sys/socket.h>


bool Client::operator==(const Client &compare)
{
	if (this->_nickname == compare._nickname
		&& this->_ip == compare._ip && this->_fd == compare._fd)
		return true;
	return false;
}

std::string	&Client::getUsername()
{
	return _username;
}

std::string Client::sendMessage(std::string message, Client *receive)
{
	send(receive->_fd, message.c_str(), message.size(), 0);
}

std::string Client::sendMessage(std::string message, Channel *receive)
{
	receive->shareMessage(message, _username);
}