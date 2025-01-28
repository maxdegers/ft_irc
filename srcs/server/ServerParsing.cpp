#include "Server.hpp"
#include "Define.hpp"
#include <string>
#include <sstream>

// void Server::JOIN(std::string args)
// {
// 	//JOIN #channel1,#channel2,#channel3 key1,key2,key3
// 	std::string::size_type
// }


// void Server::TOPIC(const std::string &str, Client *client)
// {

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
//        if (*it)
//     }
    

//     if (str.size() == channel.size())
//     {

//     }
// }