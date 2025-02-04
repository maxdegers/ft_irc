#include "Server.hpp"
#include "Define.hpp"
#include <string>
#include <sstream>
#include "Channel.hpp"
#include "Client.hpp"
#include <string>

#include <sstream>

// void Server::JOIN(std::string args, Client *client)
// {
// 	std::string::size_type spaceIndex = args.find(' ');
// 	if (spaceIndex == std::string::npos || args.substr(spaceIndex + 1, args.size()).find(' ') != std::string::npos)
// 		return ; //TODO renvoyer une erreur
//
// 	std::string channels = args.substr(0, spaceIndex);
// 	std::string keys = args.substr(spaceIndex + 1, args.size());
//
// 	std::vector<std::string> channelList = split(channels, ',');
// 	std::vector<std::string> keyList = split(channels, ',');
//
// 	std::vector<std::string>::iterator channel = channelList.begin();
// 	std::vector<std::string>::iterator key = keyList.begin();
// 	while (channel != channelList.end())
// 	{
// 		if (channel->at(0) != '#')
// 			return ; //TODO TEJ
// 		Channel *chan = findChannel(*channel);
// 		if (chan)
// 		{
// 			chan->tryToJoin(client, *key);
// 		}
// 		else
// 		{
// 			Channel newChannel = Channel(client, *channel, "") // TODO rajouter l'IP du server
// 			_channels.push_back(newChannel);
// 		}
// 		++key;
// 		++channel;
// 	}
// }

void Server::TOPIC(const std::string &str, Client *client)
{
	Channel							*chan;

    if (str.empty())
        client->sendError(client->fd(), ERR_NEEDMOREPARAMS("TOPIC"));

    std::istringstream iss(str);
    std::string channel, topic;

	if (!(iss >> channel))
		return client->sendError(client->fd(), ERR_NEEDMOREPARAMS("TOPIC"));
    if (channel[0] != '#')
        client->sendError(client->fd(), ERR_NOSUCHCHANNEL(channel));
    chan = findChannel(channel);
    if (!chan)
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

// void PRIVMSG(const std::string &str, Client *client)
// {
//     if (str.empty())
//         client->sendError(client->fd(), ERR_NEEDMOREPARAMS("PRIVMSG"));
//
//     std::istringstream iss(str);
//     std::string destination, message, channel;
//
// 	if (!(iss >> destination >> message))
// 		return client->sendError(client->fd(), ERR_NEEDMOREPARAMS("PRIVMSG"));
//
//     if (message[0] != ':' || message.size() < 2)
//         return client->sendError(client->fd(), ERR_NOTEXTTOSEND(client->nickname()));
//
//     if ((destination.size() > 2) && (destination[0] == '%' && destination[1] == '#') )
//     {
//         channel = channel.substr(2);
//         if (channel.empty())
//             return client->sendError(client->fd(), ERR_NORECIPIENT(client->nickname(), "PRIVMSG"));
//         //check if channel exist
//
//         //call sendToChannel
//
//
//
//     } else if ((destination.size() > 3) && (destination[0] == '@' && destination[1] == '%' && destination[2] == '#'))
//     {
//         channel = channel.substr(3);
//         if (channel.empty())
//             return client->sendError(client->fd(), ERR_NORECIPIENT(client->nickname(), "PRIVMSG"));
//         //check if channel exist
//         //call sendToChannel
//
//     }
//     else
//     {
//         if ()//check if user exist)
//         {
//             //call sendToUser
//         }
//         else
//             client->sendError(client->fd(), ERR_NOSUCHNICK(client->nickname(), destination));
//     }
// }