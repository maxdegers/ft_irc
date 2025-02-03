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

	std::map<std::string, ecmd>::iterator it = _commandMap.find(command);
    if (it != _commandMap.end()) {
        ecmd cmd = it->second;
		switch (cmd)
		{
			case CMD_PASS:
				client->PASS(args);
				break;
			case CMD_NICK:
				client->NICK(args);
				break;
			case CMD_USER:
				client->USER(args);
				break;
			if (client->status() == REGISTERED)
			{
			case CMD_KICK:
				break;
			case CMD_INVITE:
				break;
			case CMD_TOPIC:
				// TOPIC(args, client);
				break;
			case CMD_MODE:
				break;
			case CMD_JOIN:
				break;
			case CMD_QUIT:
				break;
			case CMD_PRIVMSG:
				// PRIVMSG(args, client);
				break;
			default:
				break;
			}
		}
    } else {
		static std::string commands[] = {"CAP", "AUTHENTICATE", "PING", "PONG", "OPER", "ERROR", "NAMES", "MODT", \
		"VERSION", "ADMIN", "CONNECT", "LUSERS", "TIME", "STATS", "HELP", "INFO", "NOTICE"};
		for (int i = 0; i < 17; ++i) {
			if (!command.compare(0, commands[i].size(), commands[i]))
				return client->sendMessage(ERR_UNKNOWNCOMMAND(client->getPrefix(), commands[i]), client), (void)0;
		}
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
