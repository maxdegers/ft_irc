//
// Created by gberthol on 1/20/25.
//

#include "Channel.hpp"
#include "Client.hpp"
#include <sys/socket.h>
#include <algorithm>
#include "Define.hpp"

Channel::Channel(Client *creator, const std::string& channelName, const std::string& serverIP)
{
	_opUsers.push_back(creator);
	_user.push_back(creator);
	_channelName = channelName;
	_inviteOnly = false;
	_topicOpOnly = true;
	_maxUsers = 0;
	_serverIP = serverIP;
}

void	Channel::removeOp(Client *remover, Client *clientToRemove)
{
	for (std::vector<Client *>::iterator i = _opUsers.begin(); i != _opUsers.end(); i++)
	{
		if (*i == clientToRemove && checkUserOP(remover))
			_opUsers.erase(i);
	}
}

void	Channel::shareMessage(const std::string& message, const std::string& username)
{
	for (std::vector<Client *>::iterator i = _user.begin(); i != _user.end(); i++)
	{
		if ((*i)->getUsername() != username)
			send((*i)->fd(), message.c_str(), message.size(), 0);
	}
}

void	Channel::setTopicOP(Client *client, bool newRule)
{
	std::string error;

	if (checkUserOP(client))
		_topicOpOnly = newRule;
	else
	{
		error.assign(ERR_CHANOPRIVSNEEDED(client->getUsername(), _channelName));
		send(client->fd(), error.c_str(), error.size(), 0);
	}
}

void	Channel::tryToJoin(Client *newClient, const std::string& password)
{
	std::string error;

	if (_password.empty() || _password == password)
	{
		if (_inviteOnly && std::find(_invitedUsername.begin(), _invitedUsername.end(), newClient->getUsername()) == _invitedUsername.end())
		{
			error.assign(ERR_INVITEONLYCHAN(newClient->getUsername(), _channelName));
			send(newClient->fd(), error.c_str(), error.size(), 0);
			return ;//ERR_INVITEONLYCHAN 471;
		}
		if (_maxUsers && _user.size() + 1 > _maxUsers)
		{
			error.assign(ERR_CHANNELISFULL(newClient->getUsername(), _channelName));
			send(newClient->fd(), error.c_str(), error.size(), 0);
			return ; //ERR_CHANNELISFULL 473;
		}
		_user.push_back(newClient);
		if (std::find(_invitedUsername.begin(), _invitedUsername.end(), newClient->getUsername()) != _invitedUsername.end())
			_invitedUsername.erase(std::find(_invitedUsername.begin(), _invitedUsername.end(), newClient->getUsername()));
		shareMessage(":" + newClient->getUsername() + " JOIN " + _channelName + "\r\n", "");
	}
	error.assign(ERR_BADCHANNELKEY(newClient->getUsername(), _channelName));
	send(newClient->fd(), error.c_str(), error.size(), 0);
	return ; //ERR_BADCHANNELKEY 475;
}

void	Channel::setMaxUser(Client *client, unsigned long newMax)
{
	std::string error;

	if (checkUserOP(client))
		_maxUsers = newMax;
	else
	{
		error.assign(ERR_CHANOPRIVSNEEDED(client->getUsername(), _channelName));
		send(client->fd(), error.c_str(), error.size(), 0);
	}

}

void	Channel::setPassword(Client *client, const std::string& newPassword)
{
	std::string error;

	if (checkUserOP(client))
		_password = newPassword;
	else
	{
		error.assign(ERR_CHANOPRIVSNEEDED(client->getUsername(), _channelName));
		send(client->fd(), error.c_str(), error.size(), 0);
	}
}

void	Channel::setTopic(Client *clientWhoSetTopic, const std::string& newTopic)
{
	std::string error;

	if (_topicOpOnly && checkUserOP(clientWhoSetTopic))
		_topic = newTopic;
	else if (!_topicOpOnly && checkUser(clientWhoSetTopic))
		_topic = newTopic;
	else if (!checkUser(clientWhoSetTopic))
	{
		error.assign(ERR_NOTONCHANNEL(_channelName));
		send(clientWhoSetTopic->fd(), error.c_str(), error.size(), 0);
	}
	else
	{
		error.assign(ERR_CHANOPRIVSNEEDED(clientWhoSetTopic->getUsername(), _channelName));
		send(clientWhoSetTopic->fd(), error.c_str(), error.size(), 0);
	}
}

void	Channel::setInviteOnly(Client *client, bool newInvite)
{
	std::string error;

	if (checkUserOP(client))
		_inviteOnly = newInvite;
	else
	{
		error.assign(ERR_CHANOPRIVSNEEDED(client->getUsername(), _channelName));
		send(client->fd(), error.c_str(), error.size(), 0);
	}
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
	_serverIP = old._serverIP;
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
	this->_serverIP = old._serverIP;
	return *this;
}

void	Channel::addOp(Client *adder, Client *clientToAdd)
{
	std::string error;

	if (checkUserOP(adder))
		_opUsers.push_back(clientToAdd);
	else
	{
		error.assign(ERR_CHANOPRIVSNEEDED(adder->getUsername(), _channelName));
		send(adder->fd(), error.c_str(), error.size(), 0);
	}
}

void Channel::kickUser(Client *kicker, Client *toKick)
{
	std::string error;

	if (checkUserOP(kicker))
	{
		for (std::vector<Client *>::iterator i = _user.begin(); i != _user.end(); i++)
		{
			if (*i == toKick)
				_user.erase(i);
			shareMessage(RPL_KICKED(kicker->getUsername(), _channelName, toKick->getUsername()), "");
		}
	}
	else
	{
		error.assign(ERR_CHANOPRIVSNEEDED(kicker->getUsername(), _channelName));
		send(kicker->fd(), error.c_str(), error.size(), 0);
	}
}

void Channel::inviteUser(Client *host, Client *guest)
{
	std::string awnser;
	bool		inList = false;

	for (std::vector<std::string >::iterator it = _invitedUsername.begin(); it != _invitedUsername.end(); it++)
	{
		if ((*it) == guest->getUsername())
			inList = true;
	}
	if (inList)
	{
		awnser.assign(ERR_USERONCHANNEL(host->nickname(), guest->nickname(), _channelName));
		send(guest->fd(), awnser.c_str(), awnser.size(), 0);
		return;
	}
	_invitedUsername.push_back(guest->getUsername());
	awnser.assign(RPL_INVITING(guest->getUsername(), guest->nickname(), _channelName));
	send(guest->fd(), awnser.c_str(), awnser.size(), 0);
}

void Channel::displayTopic(Client *client)
{
	if (_topic.empty())
		RPL_NOTOPIC(client->getUsername(), _channelName);
	else
		RPL_TOPIC(client->getUsername(), _channelName, _topic);
}

std::string &Channel::getChannelName()
{
	return _channelName;
}
