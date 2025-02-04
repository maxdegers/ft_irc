#include "Server.hpp"
#include "Define.hpp"
#include <string>
#include <sstream>
#include "Channel.hpp"
#include "Client.hpp"
#include <string>

// void Server::JOIN(std::string args)
// {
// 	//JOIN #channel1,#channel2,#channel3 key1,key2,key3
// 	std::string::size_type
// }

/*
void Server::TOPIC(const std::string &str, Client *client)
{
    bool							channelExist = false;
	Channel							*chan;

    if (str.empty())
        client->sendError(client->fd(), ERR_NEEDMOREPARAMS("TOPIC"));

    std::istringstream iss(str);
    std::string channel, topic;

	if (!(iss >> channel))
		return client->sendError(client->fd(), ERR_NEEDMOREPARAMS("TOPIC"));
    if (channel[0] != '#')
        client->sendError(client->fd(), ERR_NOSUCHCHANNEL(channel));
    for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
    {
       if ((*it).getChannelName() == channel)
	   {
		   channelExist = true;
		   chan = &(*it);
	   }
    }
    if (!channelExist)
		return client->sendError(client->fd(), ERR_NOSUCHCHANNEL(channel));
	if (!chan->checkUser(client))
		return client->sendError(client->fd(), ERR_NOTONCHANNEL(channel));
    if (str.size() == channel.size())
		chan->displayTopic(client);
	else if (str[channel.size() + 1] == ':')
		chan->setTopic(client, str.substr(channel.size() + 2));
	else
		client->sendError(client->fd(), ERR_NEEDMOREPARAMS("TOPIC"));
}

void Server::INVITE(const std::string &cmd, Client *client)
{
	bool							channelExist = false;
	Channel							*chan;
	std::string						chanName;

	if (cmd.find(' ') == std::string::npos)
		return client->sendError(client->fd(), ERR_NEEDMOREPARAMS("INVITE"));
	chanName = cmd.substr(cmd.find(' ') + 1, (cmd.size() - cmd.find(' ') + 1));
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if ((*it).getChannelName() == chanName)
		{
			channelExist = true;
			chan = &(*it);
		}
	}
	if (!channelExist)
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
}*/

void Server::MODE(const std::string &cmd, Client *client)
{
	std::vector<std::string> all_args;
	std::string cmdCpy = cmd;

	for (size_t pos = cmdCpy.find(' '); pos != std::string::npos; pos = cmdCpy.find(' '))
	{
		all_args.push_back(cmdCpy.substr(0, pos));
		cmdCpy.erase(0, pos + 1);
	}
	all_args.push_back(cmd);
}