#include "Server.hpp"
#include "Log.hpp"

#include <iostream>
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <csignal> //-> for signal()
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
	return _port;
}

int Server::socketFD() const
{
	return _socketFD;
}

std::vector<Client> Server::clients() const
{
	return _clients;
}

std::vector<struct ::pollfd> Server::fds() const
{
	return _fds;
}

std::string Server::getPassword()
{
	return _password;
}

/* Methods ****************************************************************** */
void Server::initArguments(int ac, char** av)
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

	int en = 1;
	if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw (std::runtime_error("failed to set option (SO_REUSEADDR) on socket"));
	if (bind(_socketFD, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw(std::runtime_error("failed to bind socket"));
	if (listen(_socketFD, SOMAXCONN) == -1)
		throw(std::runtime_error("failed to listen on server socket"));

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

void Server::acceptClient()
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

	Log::info("Client accepted");
}

void Server::readData(int fd)
{
	Client	*client = findClient(fd);
	char	buff[1024];
	std::string string;
	memset(buff, 0, sizeof(buff));

	if (!client->incompleteMessage().empty())
	{
		string = client->incompleteMessage();
		client->setIncompleteMessage("");
	}
	ssize_t	bytes = recv(fd, buff, sizeof(buff) - 1, 0);
	if (bytes <= 0)
	{
		Log::info("Client disconnected");
		destroy(fd);
		close(fd);
		return ;
	}
	buff[bytes] = '\0';
	Log::info("Data received from client");

	string += buff;
	if (string.find('\n') == std::string::npos)
	{
		client->setIncompleteMessage(string);
		return ;
	}
	std::cout << "FULL MSG: " << string;
	// TODO complete command = string
}

void Server::executeCommand(const std::string &completeCommand, Client *client)
{
	(void) completeCommand;
	(void) client;
}

Client *Server::findClient(int fd)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].fd() == fd)
			return (&_clients[i]);
	}
	return (NULL);
}

void Server::signalHandler(int sig)
{
	if (sig == SIGINT || sig == SIGQUIT)
	{
		_signal = true;
		std::cout << std::endl;
	}
}

void Server::closeFDs()
{
	for (int i = 3; i < 1024; i++)
		close(i);
}

void Server::destroy(int fd)
{
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == fd)
		{
			_fds.erase(_fds.begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].fd() == fd)
		{
			_clients.erase(_clients.begin() + i);
			break;
		}
	}
}

/* Exceptions *************************************************************** */
const char* Server::ArgumentsErrorException::what() const throw()
{
	return ("argument error");
}
