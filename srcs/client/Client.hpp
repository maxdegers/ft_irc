#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <iostream>
# include <cstring>
# include <arpa/inet.h>
# include "Define.hpp"

#include "Server.hpp"

class Server;

typedef enum status {
	NOT_REGISTERED 		= 0,
	ONGOING_REGISTERING = 1,
	REGISTERED			= 2
}	t_status;

class Client
{
public:
/* Constructors */
	Client(int fd, Server *serv);
	~Client();

/* Getters */
	int			fd() const;
	std::string	ip() const;

/* Setters */
	void		setIP(const std::string &ip);

/* Methods */

	void	PASS(std::string cmd, std::string str);
	// void	NICK(std::string cmd, std::string str);
	// void	USER(std::string cmd, std::string str);

	void	sendError(const int fd, const std::string &error);

	

private:
	
	Server			*_server;
	int				_fd;
	t_status		_status;
	std::string		_password;
	std::string		_ip;
	std::string		_nickname;
	std::string		_username;
	std::string		_realname;
	std::string		_hostname;

	
};

#endif //CLIENT_HPP
