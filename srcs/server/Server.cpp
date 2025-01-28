#include "Server.hpp"
#include "Log.hpp"
#include "Define.hpp"

#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <sstream>

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
					readData(_fds[i].fd); // executing the commands straight from in there
			}
		}
	}
	Log::info("Server is down.");
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
		else if (command == "NICK")
			client->NICK(args);
		else if (command == "QUIT")
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
