//
// Created by gberthol on 1/20/25.
//

#include "Channel.hpp"
#include <sys/types.h>
#include <sys/socket.h>

void	Channel::removeOp(Client *remover, Client *clientToRemove)
{
	for (std::vector<Client *>::iterator i = _opUsers.begin(); i != _opUsers.end(); i++)
	{
		if (*i == clientToRemove && checkUserOP(clientToRemove))
			_opUsers.erase(i);
	}
}

void	Channel::shareMessage(std::string message, std::string username)
{
	for (std::vector<Client *>::iterator i = _user.begin(); i != _user.end(); i++)
	{
		if (*i.getUsername() != username)
			send(i.fd(), message.c_str(), message.size(), 0);
	}
}

void	Channel::setMaxUser(unsigned long newMax)
{
	_maxUsers = newMax;
}

void	Channel::setPassword(std::string newPassword)
{
	_password = newPassword;
}

void	Channel::setTopic(Client *clientWhoSetTopic, std::string newTopic)
{
	if (_topicOpOnly && checkUserOP(clientWhoSetTopic))
		_topic = newTopic;
	else if (!_topicOpOnly && checkUser(clientWhoSetTopic))
		_topic = newTopic;
	else if (!checkUser(clientWhoSetTopic) )
		throw NotInChannelException();
	else
		throw NotOperatorException();
}

void	Channel::setInviteOnly(bool newInvite)
{
	_inviteOnly = newInvite;
}

bool	Channel::checkUserOP(Client *clientToCheck)
{
	for (std::vector<Client *>::iterator i = _opUsers.begin(); i != _opUsers.end(); i++)
	{
		if (*i == clientToCheck)
			return true;
	}
	return false;
}

bool	Channel::checkUser(Client *clientToCheck)
{
	for (std::vector<Client *>::iterator i = _user.begin(); i != _user.end(); i++)
	{
		if (*i == clientToCheck)
			return true;
	}
	return false;
}

const char *Channel::NotOperatorException::what() const throw()
{
	return "[Error] this user is not an operator on the channel";
}

const char *Channel::NotInChannelException::what() const throw()
{
	return "[Error] this user did not join this channel";
}
