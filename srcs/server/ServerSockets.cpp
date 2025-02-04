#include "Server.hpp"
#include "Log.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sstream>
#include <cstring>

/* Sockets ****************************************************************** */
void	Server::init()
{
	struct sockaddr_in addr;
	socklen_t	addrLen = sizeof(addr);
	struct pollfd newPoll;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;

	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFD < 0)
		throw (std::runtime_error("failed to create socket"));
	Log::debug("Server socket created");

	int en = 1;
	if (getsockname(_socketFD, (struct sockaddr *) &addr, &addrLen) == -1)
		throw (std::runtime_error("failed to retrieve hostname"));
	Log::debug("Retrieved hostname");

	if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw (std::runtime_error("failed to set option (SO_REUSEADDR) on socket"));
	Log::debug("Server options setup");

	if (bind(_socketFD, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw(std::runtime_error("failed to bind socket"));
	Log::debug("Server socket bound on port");

	if (listen(_socketFD, SOMAXCONN) == -1)
		throw(std::runtime_error("failed to listen on server socket"));
	Log::debug("Server listening");

	std::cout << _hostname << std::endl;
	newPoll.fd = _socketFD;
	newPoll.events = POLLIN;
	newPoll.revents = 0;
	_fds.push_back(newPoll);

	Log::info("Server is up !");
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
		return ;
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

