#include "Server.hpp"
#include "Log.hpp"
#include <iostream>
#include <sstream>
#include <csignal>
#include <poll.h>

/* Statics ****************************************************************** */
bool	Server::_signal = false;

/* Constructors ************************************************************* */
Server::Server() : _hostname("irc_server"), _port(-1), _socketFD(-1) {
	_commandMap.insert(std::make_pair("PASS", CMD_PASS));
	_commandMap.insert(std::make_pair("USER", CMD_USER));
	_commandMap.insert(std::make_pair("NICK", CMD_NICK));
	_commandMap.insert(std::make_pair("KICK", CMD_KICK));
	_commandMap.insert(std::make_pair("INVITE", CMD_INVITE));
	_commandMap.insert(std::make_pair("TOPIC", CMD_TOPIC));
	_commandMap.insert(std::make_pair("MODE", CMD_MODE));
	_commandMap.insert(std::make_pair("JOIN", CMD_JOIN));
	_commandMap.insert(std::make_pair("QUIT", CMD_QUIT));
	_commandMap.insert(std::make_pair("PRIVMSG", CMD_PRIVMSG));
	_commandMap.insert(std::make_pair("WHO", CMD_WHO));
}

Server::Server(const Server &src)
{
	*this = src;
}

Server::~Server()
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		delete *it;
	}
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		delete *it;
	}
	_clients.clear();
	_channels.clear();
	closeFDs();
}

/* Getters ****************************************************************** */
int Server::port() const
{
	return (_port);
}

int Server::socketFD() const
{
	return (_socketFD);
}

std::vector<Client *> Server::clients() const
{
	return (_clients);
}

std::vector<struct ::pollfd> Server::fds() const
{
	return (_fds);
}

std::string Server::getPassword()
{
	return (_password);
}

/* Utils ******************************************************************** */
void	Server::separateCmdArg(const std::string &completeCommand, std::string &command, std::string &args)
{
	std::string::size_type firstSpace = completeCommand.find(' ');
	std::string::size_type size = completeCommand.size();
	if (firstSpace == std::string::npos || firstSpace == size - 1)
	{
		command = completeCommand.substr(0, size - 1);
		return ;
	}
	command = completeCommand.substr(0, firstSpace);
	args = completeCommand.substr(firstSpace + 1, size);
	args = args.substr(0, args.size());
}

void	Server::truncCarriageReturns(std::string &str)
{
	std::string::iterator it = str.begin();

	while (it != str.end())
	{
		if (*it == '\r')
			it = str.erase(it);
		else
			++it;
	}
}

std::vector<std::string> Server::split(const std::string& str, char delimiter)
{
	std::vector<std::string> result;
	std::string token;
	std::istringstream tokenStream(str);

	while (std::getline(tokenStream, token, delimiter))
		result.push_back(token);

	return result;
}

/* Utils ******************************************************************** */
Client	*Server::findClient(int fd)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->fd() == fd)
			return (_clients[i]);
	}
	return (NULL);
}

Client	*Server::findClient(std::string name)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->nickname() == name)
			return (_clients[i]);
	}
	return (NULL);
}

Channel	*Server::findChannel(const std::string &name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getChannelName() == name)
			return (_channels[i]);
	}
	return (NULL);
}

bool	Server::checkNick(const std::string& nick)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->nickname() == nick)
		{
			return (true);
		}
	}
	return (false);
}

void	Server::signalHandler(int sig)
{
	if (sig == SIGINT || sig == SIGQUIT)
	{
		_signal = true;
		std::cout << std::endl;
	}
}

void	Server::closeFDs()
{
	for (int i = 3; i < 1024; i++)
		close(i);
}

void	Server::deleteUnusedChannels()
{
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end();)
	{
		if ((*it)->getUserAmount() == 0)
		{
			Log::info("Channel [" + (*it)->getChannelName() + "] deleted");
			delete *it;
			_channels.erase(it);
		}
		else
			++it;
	}
}

/* Exceptions *************************************************************** */
const char* Server::ArgumentsErrorException::what() const throw()
{
	return ("argument error");
}