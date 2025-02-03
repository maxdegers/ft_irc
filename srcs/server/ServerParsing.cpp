#include "Server.hpp"
#include "Define.hpp"
#include <string>
#include <sstream>
#include "Channel.hpp"

// void Server::JOIN(std::string args)
// {
// 	//JOIN #channel1,#channel2,#channel3 key1,key2,key3
// 	std::string::size_type
// }


// void Server::TOPIC(const std::string &str, Client *client)
// {
//     bool channelExist = false;

//     if (str.empty())
//         client->sendError(client->fd(), ERR_NEEDMOREPARAMS("TOPIC"));

//     std::istringstream iss(str);
//     std::string channel, topic;

// 	if (!(iss >> channel)) {
// 		return client->sendError(client->fd(), ERR_NEEDMOREPARAMS("TOPIC"));
// 	}

//     if (channel[0] != '#')
//         client->sendError(client->fd(), ERR_NOSUCHCHANNEL(channel));
//     for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
//     {
//        if ((*it).getChannelName() == channel)
//             channelExist = true;
//     }
//     if (!channelExist)
//         client->sendError(client->fd(), ERR_NOSUCHCHANNEL(channel));
    
//     if (str.size() == channel.size())
//     {

//     }
// }

void PRIVMSG(const std::string &str, Client *client)
{
    if (str.empty())
        client->sendError(client->fd(), ERR_NEEDMOREPARAMS("PRIVMSG"));

    std::istringstream iss(str);
    std::string destination, message, channel;

	if (!(iss >> destination >> message))
		return client->sendError(client->fd(), ERR_NEEDMOREPARAMS("PRIVMSG"));

    if (message[0] != ':' || message.size() < 2)
        return client->sendError(client->fd(), ERR_NOTEXTTOSEND(client->nickname()));

    if ((destination.size() > 2) && (destination[0] == '%' && destination[1] == '#') )
    {
        channel = channel.substr(2);
        if (channel.empty())
            return client->sendError(client->fd(), ERR_NORECIPIENT(client->nickname(), "PRIVMSG"));
        //check if channel exist

        //call sendToChannel


        
    } else if ((destination.size() > 3) && (destination[0] == '@' && destination[1] == '%' && destination[2] == '#'))
    {
        channel = channel.substr(3);
        if (channel.empty())
            return client->sendError(client->fd(), ERR_NORECIPIENT(client->nickname(), "PRIVMSG"));
        //check if channel exist
        //call sendToChannel

    }
    else
    {
        if ()//check if user exist)
        {
            //call sendToUser
        }
        else
            client->sendError(client->fd(), ERR_NOSUCHNICK(client->nickname(), destination));   
    }
}