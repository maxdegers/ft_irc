#pragma once

#define DEBUG_MODE false		//Change to true to chow debug messages

#define ERR_UNKNOWNCOMMAND(client, command) (":irc_server 421 " + (client) + " " + (command) + " :Unknown command\r\n")

//Pass
#define ERR_NEEDMOREPARAMS(command) (":irc_server 461 " + (std::string(command)) + " :Not enough parameters\r\n")
#define ERR_NEEDLESSPARAMS(command) (":irc_server 461 " + (std::string(command)) + " :Too much parameters\r\n")
#define ERR_ALREADYREGISTRED(target) ":irc_server 462 " + (target) + ":You may not reregister\r\n"
#define ERR_NOTREGISTRED(target) ":irc_server 451 " + (target) + " :Register first.\r\n"
#define ERR_PWNOTCHECK ":irc_server 808 :You must send PASS before any command\r\n"
#define ERR_PASSWDMISMATCH ":irc_server 464 :Password incorrect\r\n"
#define RPL_WELCOME(client, nick) (":irc_server 001 " + (client) + " :Welcome to the Internet Relay Chat Server, " + (nick) + " \r\n" )

//Part
#define RPL_PART(nick, channel, msg) (":" + (nick) + " PART " + (channel) + " " + (msg) + "\r\n")

//Privmsg
#define ERR_NORECIPIENT(client, command) (":irc_server 411 " + (client) + " " + (command) + " :No recipient given\r\n") //ckech IF THIS IS THE RIGHT COMMAND
#define ERR_NOSUCHNICK(client , nick) (":irc_server 401 " + (client) + " " + (nick) + " :No such nick\r\n")
#define ERR_NOTEXTTOSEND(client) (":irc_server 412 " + (client) + " :No text to send\r\n")
#define RPL_PRIVMSG(client, nick, msg) (":" + (client) + " PRIVMSG " + (nick) + " " + (msg) + "\r\n")

//Nickname
#define ERR_NONICKNAMEGIVEN ":irc_server 431 :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick) (":irc_server 432 " + (nick) + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(nick) (":irc_server 433 * " + (nick) + " :Nickname is already in use\r\n")
#define RPL_NICK(old, new_nick) (":" + (old) + " NICK " + (new_nick) + "\r\n")
#define RPL_PRENICK(prefix, new_nick) (":" + (prefix) + " NICK " + (new_nick) + "\r\n")

//Kick
#define ERR_USERNOTINCHANNEL(nick, channel, target) (":irc_server 441 " + (nick) + " " + (channel) + " " + (target) + " :They aren't on that channel\r\n")
#define RPL_KICKED(client, channel, kicked) (":" + (client) + " KICK " + (channel) + " " + (kicked) + "\r\n")

//Who
#define RPL_WHOREPLY(client, channel, username, host, nick, status, hopcount, realname) (":irc_server 352 " + (client) + " " + (channel) + " " + (username) + " " + (host) + " irc_server " + (nick) + " " + (status) + " :" + (hopcount) + " " + (realname) + "\r\n")
#define RPL_ENDOFWHO(nick, channel) (":irc_server 315 " + (nick) + " " + (channel) + " :End of WHO list\r\n")

//Mode
#define ERR_UNKNOWNMODE(nick, channel, c) (":irc_server 472 "+ (nick) + " " + (channel) + " " + (c) + " :is unknown mode char to me\r\n")
#define ERR_NEEDMODPARAMS(nick, channel, command) (":irc_server 461 " + (nick) + " " + (channel) + " " + (std::string(command)) + " :Not enough parameters\r\n")
#define RPL_CHANNELMODEIS(nick, channel, mode) (":irc_server 324 " + (nick) + " " + (channel) + " " + (mode) + "\r\n")
#define RPL_MODE(channel, nick, message) (":" + (nick) + " MODE " + (channel) + (message) + " \r\n")
#define RPL_OMODE(channel, nick) (":irc_server MODE " + (channel) + " +o " + (nick) + " \r\n")
#define RPL_ORMODE(channel, nick) (":irc_server MODE " + (channel) + " -o " + (nick) + " \r\n")
#define RPL_LMODE(channel, nick, message, number) (":" + (nick) + " MODE " + (channel) + (message) + (number) + "\r\n")

//Invite
#define RPL_INVITING(client, nick, channel) (":irc_server 341 " + (client) + " " + (nick) + " " + (channel) + "\r\n")
#define RPL_INVITED(client, nick, channel) (":" + (client) + " INVITE " + (nick) + " " + (channel) + " " + "\r\n")
#define ERR_NOSUCHCHANNEL(channel) (":irc_server 403 " + (channel) + " :No such channel\r\n")
#define ERR_NOTONCHANNEL(channel) (":irc_server 442 " + (channel) + " :You're not on that channel\r\n")
#define ERR_USERONCHANNEL(nick, target, channel) (":irc_server 443 " + (target) + " " + (nick) + " " + (channel) + " :is already on channel\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel) (":irc_server 482 " + (client) + " " + (channel) + " :You're not channel operator\r\n")
#define ERR_INVITEONLYCHAN(client, channel) (":irc_server 473 " + (client) + " " + (channel) + " :Cannot join channel (+i)\r\n")

//Topic
#define RPL_NOTOPIC(client, channel) (":irc_server 331 " + (client) + " " + (channel) + " :No topic is set\r\n")
#define RPL_TOPIC(client, channel, topic) (":irc_server 332 " + (client) + " " + (channel) + " :" + (topic) + "\r\n")
#define RPL_TOPICWHOTIME(client, channel, nick, setat) (":irc_server 333 " + (client) + " " + (channel) + " " + (nick) + " " + (setat) + "\r\n")
#define RPL_CHANGETOPIC(client, channel, topic) (":" + (client) + " TOPIC " + (channel) + " " + (topic) + "\r\n")
#define ERR_BADCHANMASK(channel) (":irc_server 476 " + (channel) + " :Bad Channel Mask\r\n")

//Join
#define	RPL_JOIN(nick, channel) (":" + (nick) + " JOIN " + (channel) + "\r\n")
#define RPL_NAMREPLY(client, channel, prenick, nick) (":irc_server 353 " + (client) + " = " + (channel) + " :" + (prenick) + (nick) + "\r\n")
#define ERR_TOOMANYCHANNELS(client, channel) (":irc_server 405 " + (client) + " " + (channel) + " :You have joined too many channels\r\n")
#define ERR_BADCHANNELKEY(client, channel) (":irc_server 475 " + (client) + " " + (channel) + " :Cannot join channel (+k)\r\n")
#define RPL_ENDOFNAMES(nick, channel) (":irc_server 366 " + (nick) + " " + (channel) + " :End of /NAMES list\r\n")
#define ERR_CHANNELISFULL(client, channel) (":irc_server 471 " + (client) + " " + (channel) + " :Cannot join channel (+l)" + "\r\n")