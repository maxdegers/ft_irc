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
	std::vector<std::string> tab = split(args, ' ');
	std::string allowedChars = "0123456789abcdefghijklmnopqrstuvwxyz_-";
	if (tab.size() <= 0)
	{
		client->sendMessage(ERR_NEEDMOREPARAMS("JOIN"), client);
		return ;
	}
	if (tab.size() > 2)
	{
		client->sendMessage(ERR_NEEDLESSPARAMS("JOIN"), client);
		return ;
	}
	if (tab.size() == 1 && tab[0] == "0")
	{
		client->removeChannels();
		return ;
	}
	Channel *channel = findChannel(tab[0]);
	if (channel && tab.size() == 1)
		channel->tryToJoin(client, "");
	else if (channel && tab.size() == 2)
		channel->tryToJoin(client, tab[1]);
	else if (!channel)
	{
		if (tab[0][0] != '&')
		{
			client->sendMessage(ERR_NOSUCHCHANNEL(tab[0]), client);
			return ;
		}
		for (std::string::iterator i = tab[0].begin(); i != tab[0].end(); ++i)
		{
			if (!allowedChars.find(*i))
			{
				client->sendMessage(ERR_BADCHANMASK(tab[0]), client);
				return ;
			}
		}
		if (tab.size() == 1)
			_channels.push_back(Channel(client, tab[0], _hostname));
		else
			_channels.push_back(Channel(client, tab[0], _hostname, tab[1]));
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
	std::string destination, message;
	if (!(iss >> destination >> message))
		return client->sendError(client->fd(), ERR_NEEDMOREPARAMS("PRIVMSG"));

	if (std::string::npos == str.find(':'))
		return (client->sendError(client->fd(), ERR_NOTEXTTOSEND(client->nickname())));

	std::string msg = str.substr(str.find(':'), str.size());
	if ((destination.size() > 1) && (destination[0] == '&') )
	{
		Channel *channelDestination = findChannel(destination);
		if (channelDestination)
		{
			client->sendMessage(RPL_PRIVMSG(client->nickname(), channelDestination->getChannelName(), msg), channelDestination);
		}
		else
			return client->sendError(client->fd(), ERR_NOSUCHCHANNEL(destination));
	}
	else
	{
		Client  *clientDestination = findClient(destination);
		if (clientDestination)
		{
			client->sendMessage(RPL_PRIVMSG(client->nickname(), clientDestination->nickname(), msg), clientDestination);
		}
		else
			client->sendError(client->fd(), ERR_NOSUCHNICK(client->nickname(), destination));
	}
}

void Server::INVITE(const std::string &cmd, Client *client)
{
	Channel			*chan;
	std::string		chanName;

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
	std::vector<std::string>	all_args = split(cmd, ' ');
	Channel						*chan;
	bool						isPlus;
	size_t						maxUser = 0;

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
				if ((*it).at(0) == '-')
				{
					chan->setPassword(client, "");
				}
				else
				{
					it++;
					if (it >= all_args.end())
						return client->sendError(client->fd(), ERR_NEEDMODPARAMS(client->nickname(), chan->getChannelName(), "+k"));
					chan->setPassword(client, *it);
				}
				break;
			case 'o':
				isPlus = ((*it).at(0) == '-');
				it++;
				if (it >= all_args.end())
					return client->sendError(client->fd(), ERR_NEEDMODPARAMS(client->nickname(), chan->getChannelName(), "+k"));
				if (findClient(*it))
				{
					if (!isPlus)
						chan->removeOp(client, findClient(*it));
					else
						chan->addOp(client, findClient(*it));
				}
				else
					return client->sendError(client->fd(), ERR_NOSUCHNICK(client->nickname(), *it));
				break;
			case 'l':
				if ((*it).at(0) == '+')
				{
					it++;
					if (it >= all_args.end())
						return client->sendError(client->fd(), ERR_NEEDMODPARAMS(client->nickname(), chan->getChannelName(), "+k"));
					std::stringstream sstream(*it);
					sstream >> maxUser;
					chan->setMaxUser(client, maxUser);
				}
				else
					chan->setMaxUser(client, 0);
				break;
			default:
				return client->sendError(client->fd(), ERR_UNKNOWNMODE(client->nickname(), chan->getChannelName(), *it));
				break;
			}
		}
		else
			return client->sendError(client->fd(), ERR_UNKNOWNMODE(client->nickname(), chan->getChannelName(), *it));
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

void Server::WHO(const std::string& args, Client *client)
{
	std::vector<std::string> tab = split(args, ' ');
	if (tab.size() < 1)
		client->sendMessage(ERR_NEEDMOREPARAMS("WHO"), client);
	else if (tab.size() > 1)
		client->sendMessage(ERR_NEEDLESSPARAMS("WHO"), client);
	else
	{
		Channel *channel = findChannel(tab[0]);
		channel->listUsers(client);
	}
}
