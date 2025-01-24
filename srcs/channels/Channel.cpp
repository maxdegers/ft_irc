//
// Created by gberthol on 1/20/25.
//

#include "Channel.hpp"
#include "Client.hpp"
#include <sys/types.h>
#include <sys/socket.h>

void	Channel::removeOp(Client *remover, Client *clientToRemove)
{
	for (std::vector<Client *>::iterator i = _opUsers.begin(); i != _opUsers.end(); i++)
	{
		if (*i == clientToRemove && checkUserOP(remover))
			_opUsers.erase(i);
	}
}

void	Channel::shareMessage(std::string message, std::string username)
{
	for (std::vector<Client *>::iterator i = _user.begin(); i != _user.end(); i++)
	{
		if ((*i)->getUsername() != username)
			send((*i)->fd(), message.c_str(), message.size(), 0);
	}
}

void	Channel::setMaxUser(Client *client, unsigned long newMax)
{
	if (checkUserOP(client))
		_maxUsers = newMax;
	else
		throw NotOperatorException();
}

void	Channel::setPassword(Client *client, std::string newPassword)
{
	if (checkUserOP(client))
		_password = newPassword;
	else
		throw NotOperatorException();
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

void	Channel::setInviteOnly(Client *client, bool newInvite)
{
	if (checkUserOP(client))
		_inviteOnly = newInvite;
	else
		throw NotOperatorException();
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
