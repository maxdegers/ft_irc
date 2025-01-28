#include "Server.hpp"
#include "Log.hpp"
#include "Define.hpp"

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <csignal>
#include <cstring>
#include <sstream>

/* Statics ****************************************************************** */
bool	Server::_signal = false;

/* Constructors ************************************************************* */
Server::Server() : _port(-1), _socketFD(-1) {}

Server::Server(const Server &src)
{
	*this = src;
}

Server::~Server()
{
	closeFDs();
}

/* Operators **************************************************************** */
Server &Server::operator=(const Server &src)
{
	_port = src.port();
	_socketFD = src.socketFD();
	_clients = src.clients();
	_fds = src.fds();

	return (*this);
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

std::vector<Client> Server::clients() const
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

/* Private Methods ********************************************************** */
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
	args = args.substr(0, args.size() - 1);
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

/* Methods ****************************************************************** */
void	Server::initArguments(int ac, char** av)
{
	if (ac != 3)
		throw (ArgumentsErrorException());

	int					port;
	std::stringstream	ss(av[1]);

	ss >> port;
	if (!ss.eof() || ss.fail())
		throw (ArgumentsErrorException());

	if (port < 1024 || port > 65535)
		throw (ArgumentsErrorException());

	_port = port;
	_password = av[2];
}

void	Server::init()
{
	struct sockaddr_in addr;
	struct pollfd newPoll;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;

	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFD < 0)
		throw (std::runtime_error("failed to create socket"));
	Log::debug("Server socket created");

	int en = 1;
	if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw (std::runtime_error("failed to set option (SO_REUSEADDR) on socket"));
	Log::debug("Server options setup");
	if (bind(_socketFD, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw(std::runtime_error("failed to bind socket"));
	Log::debug("Server socket bound on port");
	if (listen(_socketFD, SOMAXCONN) == -1)
		throw(std::runtime_error("failed to listen on server socket"));
	Log::debug("Server listening");

	newPoll.fd = _socketFD;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	_fds.push_back(newPoll);

	Log::info("Server is up !");
}

void	Server::run()
{
	while (_signal == false)
	{
		if ((poll(&_fds[0], _fds.size(), -1) == -1) && _signal == false)
			throw (std::runtime_error("poll() error"));

		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _socketFD)
					acceptClient();
				else
					readData(_fds[i].fd);
			}
		}
	}
	Log::info("Server is down.");
}

void	Server::acceptClient()
{
	struct sockaddr_in	clientAddr;
	struct pollfd		newPoll;
	socklen_t			len = sizeof(clientAddr);

	int incofd = accept(_socketFD, (sockaddr *)&(clientAddr), &len);
	if (incofd < 0)
	{
		Log::error("Client was not accepted");
		return ;
	}
	newPoll.fd = incofd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;

	Client client(incofd, inet_ntoa(clientAddr.sin_addr), this);
	_clients.push_back(client);
	_fds.push_back(newPoll);

	Log::debug("Client accepted to socket");
}

void	Server::readData(int fd)
{
	Client		*client = findClient(fd);
	char		buff[1024];
	std::string string;

	memset(buff, 0, sizeof(buff));

	// Retrieve incomplete message from the client
	if (!client->incompleteMessage().empty())
	{
		string = client->incompleteMessage();
		client->setIncompleteMessage("");
	}

	// Receive data from the client
	ssize_t	bytes = recv(fd, buff, sizeof(buff) - 1, 0);
	if (bytes <= 0)
	{
		QUIT(fd);
		return;
	}
	buff[bytes] = '\0';
	Log::debug("Data received from client");

	// Append new data to the string
	string += buff;

	// Process the string line by line
	std::istringstream	stream(string);
	std::string			line;
	bool				hasIncompleteLine = true;

	while (std::getline(stream, line))
	{
		if (stream.eof() && string.at(string.size() - 1) != '\n')
		{
			client->setIncompleteMessage(line);
			hasIncompleteLine = false;
		}
		else
		{
			truncCarriageReturns(line);
			executeCommand(line, client);
		}
	}
	if (hasIncompleteLine && string.at(string.size() - 1) != '\n')
		client->setIncompleteMessage(line);
}

void	Server::executeCommand(const std::string &completeCommand, Client *client)
{
	std::string	command;
	std::string	args;

	separateCmdArg(completeCommand, command, args);
	if (client->status() == NOT_REGISTERED)
	{
		if (command == "CAP")
			client->sendError(client->fd(), ERR_UNKNOWNCOMMAND(std::string(""), command));
		else if (command == "PASS")
			client->PASS(args);
		else if (command == "QUIT")
			QUIT(client->fd());
		else
			client->sendError(client->fd(), ERR_PWNOTCHECK);
	}
	else if (client->status() == ONGOING_REGISTERING)
	{
		if (command == "USER")
			client->USER(args);
		if (command == "NICK")
			client->NICK(args);
		if (command == "QUIT")
			QUIT(client->fd());
		else
			return ; //TODO voir si il faut renvoyer un truc
	}
	else // client->status == REGISTERED
	{
		if (command == "QUIT")
			QUIT(client->fd());
	}
}

Client	*Server::findClient(int fd)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].fd() == fd)
			return (&_clients[i]);
	}
	return (NULL);
}

bool	Server::checkNick(const std::string& nick)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].nickname() == nick)
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

void	Server::QUIT(int fd)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it < _clients.end(); ++it)
	{
		if (it->fd() == fd)
		{
			//TODO broadcast the fact that the client disconnected to other clients
			_clients.erase(it);
			break ;
		}
	}
	for (std::vector<struct pollfd>::iterator it = _fds.begin(); it < _fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_fds.erase(it);
			break ;
		}
	}
	close(fd);
	Log::info("Client disconnected");
}

/* Exceptions *************************************************************** */
const char* Server::ArgumentsErrorException::what() const throw()
{
	return ("argument error");
}
