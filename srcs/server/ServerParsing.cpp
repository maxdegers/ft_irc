#include "Server.hpp"
#include "Define.hpp"
#include <string>
#include <sstream>
#include "Channel.hpp"
#include "Client.hpp"
#include <string>
#include "Log.hpp"
#include <sstream>

void Server::JOIN(std::string args, Client *client)
{
	std::string::size_type spaceIndex = args.find(' ');
	// if (spaceIndex == std::string::npos || args.substr(spaceIndex + 1, args.size()).find(' ') != std::string::npos)
	// 	return ; //TODO renvoyer une erreur

	std::string channels = args.substr(0, spaceIndex);
	std::string keys = args.substr(spaceIndex + 1, args.size());

	std::vector<std::string> channelList = split(channels, ',');
	std::vector<std::string> keyList = split(channels, ',');

	std::vector<std::string>::iterator channel = channelList.begin();
	std::vector<std::string>::iterator key = keyList.begin();
	while (channel != channelList.end())
	{
		if (channel->at(0) != '&')
			return ; //TODO TEJ
		Channel *chan = findChannel(*channel);
		if (chan)
		{
			chan->tryToJoin(client, *key);
		}
		else
		{
			Channel newChannel = Channel(client, *channel, _hostname); // TODO rajouter l'IP du server
			_channels.push_back(newChannel);
		}
		++key;
		++channel;
	}
}

void Server::TOPIC(const std::string &str, Client *client)
{
	Channel *chan;

	if (str.empty())
		client->sendError(client->fd(), ERR_NEEDMOREPARAMS("TOPIC"));
	std::istringstream iss(str);
	std::string channel, topic;
	if (!(iss >> channel))
		return client->sendError(client->fd(), ERR_NEEDMOREPARAMS("TOPIC"));
	chan = findChannel(channel);
	if (!chan)
		return client->sendError(client->fd(), ERR_NOSUCHCHANNEL(channel));
	if (!(iss >> topic))
		chan->displayTopic(client, false);
	else
		chan->setTopic(client, topic.substr(1, topic.size() - 1));
}

void Server::PRIVMSG(const std::string &str, Client *client)
{
	if (str.empty())
		client->sendError(client->fd(), ERR_NEEDMOREPARAMS("PRIVMSG"));
	std::istringstream iss(str);
	std::string destination, message, channel;
	if (!(iss >> destination >> message))
		return client->sendError(client->fd(), ERR_NEEDMOREPARAMS("PRIVMSG"));
	if (message[0] != ':' || message.size() < 2)
		return client->sendError(client->fd(), ERR_NOTEXTTOSEND(client->nickname()));
	if ((destination.size() > 1) && (destination[0] == '&') )
	{
		channel = channel.substr(1);
		if (channel.empty())
			return client->sendError(client->fd(), ERR_NORECIPIENT(client->nickname(), "PRIVMSG"));
		Channel *channelDestination = findChannel(destination);
		if (channelDestination)
			channelDestination->shareMessage(message, client->nickname());
		else
			return ; //TODO renvoyer une erreur
	}
	else
	{
		Client  *clientDestination = findClient(destination);
		if (clientDestination)
		{
			// TODO send private msg
		}
		else
			client->sendError(client->fd(), ERR_NOSUCHNICK(client->nickname(), destination));
	}
}

void Server::INVITE(const std::string &cmd, Client *client)
{
	Channel							*chan;
	std::string						chanName;

	if (cmd.find(' ') == std::string::npos)
		return client->sendError(client->fd(), ERR_NEEDMOREPARAMS("INVITE"));
	chanName = cmd.substr(cmd.find(' ') + 1, (cmd.size() - cmd.find(' ') + 1));
	chan = findChannel(chanName);
	if (!chan)
		return client->sendError(client->fd(), ERR_NOSUCHCHANNEL(chanName));
	if (!chan->checkUser(client))
		return client->sendError(client->fd(), ERR_NOTONCHANNEL(chanName));
	if (!chan->checkUserOP(client))
		return client->sendError(client->fd(), ERR_CHANOPRIVSNEEDED(client->getUsername(), chanName));
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if ((*it).nickname() == cmd.substr(0, cmd.find(' ')))
			chan->inviteUser(client, &(*it));
	}
}

void Server::MODE(const std::string &cmd, Client *client)
{
	std::vector<std::string> all_args = split(cmd, ' ');
	Channel							*chan;

	chan = findChannel(all_args.at(0));
	if (!chan)
		return client->sendError(client->fd(), ERR_NOSUCHCHANNEL(all_args.at(0)));
	if (!chan->checkUser(client))
		return client->sendError(client->fd(), ERR_NOTONCHANNEL(all_args.at(0)));
	if (!chan->checkUserOP(client))
		return client->sendError(client->fd(), ERR_CHANOPRIVSNEEDED(client->getUsername(), all_args.at(0)));
	for (std::vector<std::string>::iterator it = all_args.begin() + 1; it != all_args.end(); it++)
	{
		if (((*it).at(0) == '-' || (*it).at(0) == '+') && (*it).size() == 2)
		{
			switch ((*it).at(1))
			{
			case 'i':
				if ((*it).at(0) == '-')
					chan->setInviteOnly(client, false);
				else
					chan->setInviteOnly(client, true);
				break;
			case 't':
				if ((*it).at(0) == '-')
					chan->setTopicOP(client, false);
				else
					chan->setTopicOP(client, true);
				break;
			case 'k':
				break;
			case 'o':
				break;
			case 'l':
				break;
			default:
				break;
			}
		}
	}
}

void Server::KICK(std::string args, Client* client)
{
	std::vector<std::string>	argsList = split(args, ' ');
	Channel						*channel = findChannel(argsList[0]);
	Client						*clientTarget = findClient(argsList[1]);

	if (!channel)
	{
		client->sendMessage(ERR_NOSUCHCHANNEL(argsList[0]), client);
		return ;
	}
	if (!channel->checkUser(client))
	{
		client->sendMessage(ERR_NOTONCHANNEL(argsList[1]), client);
		return ;
	}
	if (!clientTarget || !channel->checkUser(clientTarget))
	{
		client->sendMessage(ERR_USERNOTINCHANNEL(client->nickname(), argsList[0], argsList[1]), client);
		return ;
	}
	if (!channel->checkUserOP(client))
	{
		client->sendMessage(ERR_CHANOPRIVSNEEDED(client->nickname(), argsList[0]), client);
		return ;
	}

	channel->kickUser(client, clientTarget);
}
