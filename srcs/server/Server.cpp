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
		deleteUnusedChannels();
	}
	Log::info("Server is down.");
}

void	Server::executeCommand(const std::string &completeCommand, Client *client)
{
	std::string	command;
	std::string	args;

	separateCmdArg(completeCommand, command, args);

    if ( _commandMap.find(command) != _commandMap.end()) {
        ecmd cmd = _commandMap[command];
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
			case CMD_KICK:
				KICK(args, client);
				break;
			case CMD_INVITE:
				INVITE(args, client);
				break;
			case CMD_TOPIC:
				TOPIC(args, client);
				break;
			case CMD_MODE:
				MODE(args, client);
				break;
			case CMD_JOIN:
				JOIN(args, client);
				break;
			case CMD_QUIT:
				QUIT(client->fd());
				break;
			case CMD_PRIVMSG:
				PRIVMSG(args, client);
				break;
			case CMD_WHO:
				WHO(args, client);
				break;
			default:
				break;
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
