#pragma once

#define DEBUG_MODE false		//Change to true to chow debug messages

#define ERR_UNKNOWNCOMMAND(client, command) (":irc_server 421 " + (client) + " " + (command) + " :Unknown command\n")

//Pass
#define ERR_NEEDMOREPARAMS(command) (":irc_server 461 " + (std::string(command)) + " :Not enough parameters\n")
#define ERR_ALREADYREGISTRED(target) ":irc_server 462 " + (target) + ":You may not reregister\n"
#define ERR_NOTREGISTRED(target) ":irc_server 451 " + (target) + " :Register first.\n"
#define ERR_PWNOTCHECK ":irc_server 808 :You must send PASS before any command\n"
#define ERR_PASSWDMISMATCH ":irc_server 464 :Password incorrect\n"
#define RPL_WELCOME(client, nick) (":irc_server 001 " + (client) + " :Welcome to the Internet Relay Chat Server, " + (nick) + " \n" )

//Part
#define RPL_PART(nick, channel, msg) (":" + (nick) + " PART " + (channel) + " " + (msg) + "\n")

//Privmsg
#define ERR_NOSUCHNICK(client , nick) (":irc_server 401 " + (client) + " " + (nick) + " :No such nick\n")
#define ERR_NOTEXTTOSEND(client) (":irc_server 412 " + (client) + " :No text to send\n")
#define RPL_PRIVMSG(client, nick, msg) (":" + (client) + " PRIVMSG " + (nick) + " " + (msg) + "\n")

//Nickname
#define ERR_NONICKNAMEGIVEN ":irc_server 431 :No nickname given\n"
#define ERR_ERRONEUSNICKNAME(nick) (":irc_server 432 " + (nick) + " :Erroneus nickname\n")
#define ERR_NICKNAMEINUSE(nick) (":irc_server 433 * " + (nick) + " :Nickname is already in use\n")
#define RPL_NICK(old, new_nick) (":" + (old) + " NICK " + (new_nick) + "\n")
#define RPL_PRENICK(prefix, new_nick) (":" + (prefix) + " NICK " + (new_nick) + "\n")

//Kick
#define ERR_USERNOTINCHANNEL(nick, channel, target) (":irc_server 441 " + (nick) + " " + (channel) + " " + (target) + " :They aren't on that channel\n")
#define RPL_KICKED(client, channel, kicked) (":" + (client) + " KICK " + (channel) + " " + (kicked) + "\n")

//Who
#define RPL_WHOREPLY(who) (":irc_server 352 " + (who) + "\n")
#define RPL_ENDOFWHO(nick, channel) (":irc_server 315 " + (nick) + " " + (channel) + " :End of WHO list\n")

//Mode
#define ERR_UNKNOWNMODE(nick, channel, c) (":irc_server 472 "+ (nick) + " " + (channel) + " " + (c) + " :is unknown mode char to me\r\n")
#define ERR_NEEDMODPARAMS(nick, channel, command) (":irc_server 461 " + (nick) + " " + (channel) + " " + (std::string(command)) + " :Not enough parameters\n")
#define RPL_CHANNELMODEIS(nick, channel, mode) (":irc_server 324 " + (nick) + " " + (channel) + " " + (mode) + "\n")
#define RPL_MODE(channel, nick, message) (":" + (nick) + " MODE " + (channel) + (message))
#define RPL_OMODE(channel, nick, message, nick2) (":" + (nick) + " MODE " + (channel) + (message) + (nick2) + " \n")
#define RPL_LMODE(channel, nick, message, number) (":" + (nick) + " MODE " + (channel) + (message) + (number) + "\n")

//Invite
#define RPL_INVITING(client, nick, channel) (":irc_server 341 " + (client) + " " + (nick) + " " + (channel) + "\n")
#define RPL_INVITED(client, nick, channel) (":" + (client) + " INVITE " + (nick) + " " + (channel) + " " + "\n")
#define ERR_NOSUCHCHANNEL(channel) (":irc_server 403 " + (channel) + " :No such channel\n")
#define ERR_NOTONCHANNEL(channel) (":irc_server 442 " + (channel) + " :You're not on that channel\n")
#define ERR_USERONCHANNEL(nick, target, channel) (":irc_server 443 " + (target) + " " + (nick) + " " + (channel) + " :is already on channel\n")
#define ERR_CHANOPRIVSNEEDED(client, channel) (":irc_server 482 " + (client) + " " + (channel) + " :You're not channel operator\n")
#define ERR_INVITEONLYCHAN(client, channel) (":irc_server 473 " + (client) + " " + (channel) + " :Cannot join channel (+i)\n")

//Topic
#define RPL_NOTOPIC(client, channel) (":irc_server 331 " + (client) + " " + (channel) + " :No topic is set\n")
#define RPL_TOPIC(client, channel, topic) (":irc_server 332 " + (client) + " " + (channel) + " :" + (topic) + "\n")
#define RPL_TOPICWHOTIME(client, channel, nick, setat) (":irc_server 333 " + (client) + " " + (channel) + " " + (nick) + " " + (setat) + "\n")
#define RPL_CHANGETOPIC(client, channel, topic) (":" + (client) + " TOPIC " + (channel) + " " + (topic) + "\n")
#define ERR_BADCHANMASK(channel) (":irc_server 476 " + (channel) + " :Bad Channel Mask\n")

//Join
#define RPL_JOIN(nick, channel) (":" + (nick) + " JOIN " + (channel) + "\n")
#define RPL_NAMREPLY(client, channel, prenick) (":irc_server 353 " + (client) + " = " + (channel) + " :" + (prenick) + "\n")
#define ERR_TOOMANYCHANNELS(client, channel) (":irc_server 405 " + (client) + " " + (channel) + " :You have joined too many channels\n")
#define ERR_BADCHANNELKEY(client, channel) (":irc_server 475 " + (client) + " " + (channel) + " :Cannot join channel (+k)\n")
#define RPL_ENDOFNAMES(nick, channel) (":irc_server 366 " + (nick) + " " + (channel) + " :End of /NAMES list\n")
#define ERR_CHANNELISFULL(client, channel) (":irc_server 471 " + (client) + " " + (channel) + " :Cannot join channel (+l)" + "\r\n")