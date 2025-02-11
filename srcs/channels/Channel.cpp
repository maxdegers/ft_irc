//
// Created by gberthol on 1/20/25.
//

#include "Channel.hpp"
#include "Client.hpp"
#include <sys/socket.h>
#include <algorithm>
#include "Define.hpp"
#include "Log.hpp"

Channel::Channel(Client *creator, const std::string& channelName, const std::string& serverIP)
{
	_opUsers.push_back(creator);
	_user.push_back(creator);
	_channelName = channelName;
	_inviteOnly = false;
	_topicOpOnly = true;
	_maxUsers = 0;
	_serverIP = serverIP;
	shareMessage(RPL_JOIN(creator->nickname(), _channelName), "");
	Log::info("User " + creator->getUsername() + " created channel " + _channelName);
	Log::info("User " + creator->getUsername() + " joined channel " + _channelName);
	shareMessage(RPL_TOPIC(creator->nickname(), _channelName, _topic), "");
	shareMessage(RPL_NAMREPLY(creator->nickname(), channelName, std::string("@"), creator->nickname()), "");
	shareMessage(RPL_ENDOFNAMES(creator->nickname(), channelName), "");
}

Channel::Channel(Client *creator, const std::string& channelName, const std::string& serverIP, const std::string &password)
{
	_opUsers.push_back(creator);
	_user.push_back(creator);
	_channelName = channelName;
	_inviteOnly = false;
	_topicOpOnly = true;
	_maxUsers = 0;
	_serverIP = serverIP;
	_password = password;
	shareMessage(RPL_JOIN(creator->nickname(), _channelName), "");
	Log::info("User " + creator->getUsername() + " created channel " + _channelName);
	Log::info("User " + creator->getUsername() + " joined channel " + _channelName);
	shareMessage(RPL_TOPIC(creator->nickname(), _channelName, _topic), "");
	shareMessage(RPL_NAMREPLY(creator->nickname(), channelName, std::string("@"), creator->nickname()), "");
	shareMessage(RPL_ENDOFNAMES(creator->nickname(), channelName), "");
}

void	Channel::removeOp(Client *remover, Client *clientToRemove)
{
	for (std::vector<Client *>::iterator i = _opUsers.begin(); i != _opUsers.end(); i++)
	{
		if (*i == clientToRemove && checkUserOP(remover))
			_opUsers.erase(i);
	}
}

void	Channel::shareMessage(const std::string& message, const std::string& nick)
{
	for (std::vector<Client *>::iterator i = _user.begin(); !_user.empty() && i < _user.end(); i++)
	{
		std::cout << (*i)->nickname() << "'" << (*i)->fd() << "'" <<  std::endl;
		if ((*i) && (*i)->nickname() != nick)
		{
			send((*i)->fd(), message.c_str(), message.size(), 0);
			Log::debug("send message: '" + message + "' to " + (*i)->getUsername());
		}
	}
}

void	Channel::setTopicOP(Client *client, bool newRule)
{
	std::string error;

	if (checkUserOP(client))
	{
		_topicOpOnly = newRule;
		shareMessage(RPL_MODE(_channelName, client->nickname(), "+t"), "");
	}
	else
	{
		error.assign(ERR_CHANOPRIVSNEEDED(client->getUsername(), _channelName));
		send(client->fd(), error.c_str(), error.size(), 0);
	}
}

void	Channel::tryToJoin(Client *newClient, const std::string& password)
{
	std::string error;

	if (checkUser(newClient))
	{
		error.assign(ERR_USERONCHANNEL(newClient->nickname(), newClient->nickname(), _channelName));
		send(newClient->fd(), error.c_str(), error.size(), 0);
		return ;
	}
	if (_password.empty() || _password == password)
	{
		if (_inviteOnly && std::find(_invitedUsername.begin(), _invitedUsername.end(), newClient->getUsername()) == _invitedUsername.end())
		{
			error.assign(ERR_INVITEONLYCHAN(newClient->getUsername(), _channelName));
			send(newClient->fd(), error.c_str(), error.size(), 0);
			return ;
		}
		if (_maxUsers && _user.size() + 1 > _maxUsers)
		{
			error.assign(ERR_CHANNELISFULL(newClient->getUsername(), _channelName));
			send(newClient->fd(), error.c_str(), error.size(), 0);
			return ;
		}
		_user.push_back(newClient);
		newClient->addChannel(this);
		if (std::find(_invitedUsername.begin(), _invitedUsername.end(), newClient->getUsername()) != _invitedUsername.end())
			_invitedUsername.erase(std::find(_invitedUsername.begin(), _invitedUsername.end(), newClient->getUsername()));
		Log::info("User " + newClient->nickname() + " joined channel " + _channelName);
		shareMessage(":" + newClient->nickname() + " JOIN " + _channelName + "\r\n", "");
		error.assign(RPL_TOPIC(newClient->nickname(), _channelName, _topic));
		send(newClient->fd(), error.c_str(), error.size(), 0);
		for (std::vector<Client *>::iterator it = _user.begin(); it < _user.end(); it++)
		{
			if (checkUserOP(*it))
				error.assign(RPL_NAMREPLY(newClient->nickname(), _channelName, "@", (*it)->nickname()));
			else
				error.assign(RPL_NAMREPLY(newClient->nickname(), _channelName, "", (*it)->nickname()));
			send(newClient->fd(), error.c_str(), error.size(), 0);
		}
		error.assign(RPL_ENDOFNAMES(newClient->nickname(), _channelName));
		send(newClient->fd(), error.c_str(), error.size(), 0);
		return ;
	}
	error.assign(ERR_BADCHANNELKEY(newClient->getUsername(), _channelName));
	send(newClient->fd(), error.c_str(), error.size(), 0);
	return ; //ERR_BADCHANNELKEY 475;
}

void	Channel::setMaxUser(Client *client, unsigned long newMax)
{
	std::string error;

	if (checkUserOP(client))
	{
		_maxUsers = newMax;
		shareMessage(RPL_CHANNELMODEIS(client->nickname(), _channelName, "+l"), "");
	}
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
	{
		_topic = newTopic;
		displayTopic(clientWhoSetTopic, true);
	}
	else if (!_topicOpOnly && checkUser(clientWhoSetTopic))
	{
		_topic = newTopic;
		displayTopic(clientWhoSetTopic, true);
	}
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
	{
		_inviteOnly = newInvite;
		if (_inviteOnly)
			shareMessage(RPL_CHANNELMODEIS(client->nickname(), _channelName, "+i"), "");
		else
			shareMessage(RPL_CHANNELMODEIS(client->nickname(), _channelName, "-i"), "");
	}
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

	Log::debug(adder->nickname() + " try to add " + clientToAdd->nickname() + "at the operator list");
	if (checkUserOP(adder))
	{
		_opUsers.push_back(clientToAdd);
		shareMessage(RPL_OMODE(_channelName, clientToAdd->nickname()), "");
	}
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
		for (std::vector<Client *>::iterator i = _user.begin(); i < _user.end(); i++)
		{
			if (*i == toKick)
			{
				shareMessage(RPL_KICKED(kicker->nickname(), _channelName, toKick->nickname()), "");
				_user.erase(i);
				toKick->removeChannel(this);
			}
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
	send(host->fd(), awnser.c_str(), awnser.size(), 0);
	awnser.assign(RPL_INVITED(guest->getUsername(), guest->nickname(), _channelName));
	send(guest->fd(), awnser.c_str(), awnser.size(), 0);
}

void Channel::displayTopic(Client *client, bool toAll)
{
	std::string msg;

	if (_topic.empty())
		msg.assign(RPL_NOTOPIC(client->getUsername(), _channelName));
	else
		msg.assign(RPL_TOPIC(client->getUsername(), _channelName, _topic));
	if (toAll)
		shareMessage(msg, "");
	else
		send(client->fd(), msg.c_str(), msg.size(), 0);
}

std::string &Channel::getChannelName()
{
	return _channelName;
}

void Channel::listUsers(Client *client)
{
	std::string msg;

	for (std::vector<Client *>::iterator it = _user.begin(); it != _user.end(); ++it)
	{
		if (checkUserOP(*it))
			msg.assign(RPL_WHOREPLY(client->nickname(), _channelName, (*it)->getUsername(), (*it)->ip(), (*it)->nickname(), "@", "0", (*it)->realname()));
		else
			msg.assign(RPL_WHOREPLY(client->nickname(), _channelName, (*it)->getUsername(), (*it)->ip(), (*it)->nickname(), "", "0", (*it)->realname()));
		send(client->fd(), msg.c_str(), msg.size(), 0);
	}
	msg.assign(RPL_ENDOFWHO(client->nickname(), _channelName));
	send(client->fd(), msg.c_str(), msg.size(), 0);
}

// FONCTION A NE PAS SE SERVIR POUR UN CLIENT TOUJOURS CONNECTE
void Channel::removeUser(const std::string &name)
{
	std::cout << _user.size() << std::endl;

	for (std::vector<Client *>::iterator i = _user.begin(); _user.empty() || i < _user.end(); i++)
	{
		if ((*i)->nickname() == name)
		{
			Log::info("client: " + name + "is now disconnected");
			shareMessage(RPL_KICKED(name, _channelName, name), "");
			_user.erase(i);
			return;
		}
	}
}

size_t Channel::getUserAmout()
{
	return _user.size();
}
