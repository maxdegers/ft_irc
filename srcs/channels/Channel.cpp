//
// Created by gberthol on 1/20/25.
//

#include "Channel.hpp"
#include "Client.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <algorithm>


Channel::Channel(Client *creator, std::string channelName)
{
	_opUsers.push_back(creator);
	_user.push_back(creator);
	_channelName = channelName;
	_inviteOnly = false;
	_topicOpOnly = true;
	_maxUsers = 0;
}

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

void	Channel::tryToJoin(Client *newClient, const std::string& password)
{
	if (_password.empty() || _password == password)
	{
		if (_inviteOnly && std::find(_invitedUsername.begin(), _invitedUsername.end(), newClient->getUsername()) == _invitedUsername.end())
		{
			return ;//ERR_INVITEONLYCHAN 471;
		}
		if (_maxUsers && _user.size() + 1 > _maxUsers)
		{
			return ; //ERR_CHANNELISFULL 473;
		}
		_user.push_back(newClient);
		//message of join;
	}
	return ; //ERR_BADCHANNELKEY (475);
}

void	Channel::setMaxUser(unsigned long newMax)
{
	_maxUsers = newMax;
}

void	Channel::setPassword(const std::string& newPassword)
{
	_password = newPassword;
}

void	Channel::setTopic(Client *clientWhoSetTopic, const std::string& newTopic)
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

Channel::~Channel()
{

}

Channel::Channel(const Channel &old)
{
	_user = old._user;
	_opUsers = old._opUsers;
	_inviteOnly = old._inviteOnly;
	_topicOpOnly = old._topicOpOnly;
	_maxUsers = old._maxUsers;
	_password = old._password;
	_topic = old._topic;
	_invitedUsername = old._invitedUsername;
	_channelName = old._channelName;
}

Channel &Channel::operator=(const Channel &old)
{
	this->_user = old._user;
	this->_opUsers = old._opUsers;
	this->_inviteOnly = old._inviteOnly;
	this->_topicOpOnly = old._topicOpOnly;
	this->_maxUsers = old._maxUsers;
	this->_password = old._password;
	this->_topic = old._topic;
	this->_invitedUsername = old._invitedUsername;
	this->_channelName = old._channelName;
	return *this;
}

const char *Channel::NotOperatorException::what() const throw()
{
	return "[Error] this user is not an operator on the channel";
}

const char *Channel::NotInChannelException::what() const throw()
{
	return "[Error] this user did not join this channel";
}

void	Channel::addOp(Client *adder, Client *clientToAdd)
{
	if (checkUserOP(adder))
		_opUsers.push_back(clientToAdd);
}

void Channel::kickUser(Client *kicker, Client *toKick)
{
	if (checkUserOP(kicker))
	{
		for (std::vector<Client *>::iterator i = _user.begin(); i != _user.end(); i++)
		{
			if (*i == toKick)
				_user.erase(i);
		}
	}
}

void Channel::inviteUser(Client *host, Client *guest)
{
	if (checkUser(host))
	{
		_invitedUsername.push_back(guest->getUsername());
	}
}
