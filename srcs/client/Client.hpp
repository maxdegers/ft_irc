#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>
# include <iostream>
# include "Server.hpp"

class Channel;
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
	Client(int fd, const std::string &ip, Server *serv);
	~Client();

/* Operators */
	bool operator==(const Client &compare);

/* Getters */
	int			fd() const;
	std::string	ip() const;
	std::string	&getUsername();
	t_status	status() const;
	std::string	incompleteMessage() const;
	std::string	nickname() const;

/* Setters */
	void		setStatus(const t_status &status);
	void		setIncompleteMessage(std::string msg);

/* Methods */
	void sendMessage(std::string message, Client *receive);
	void sendMessage(std::string message, Channel *receive);


	void	PASS(const std::string &str);
	// void	NICK(const std::string &str);
	// void	USER(std::string cmd, std::string str);

	void	sendError(const int fd, const std::string &error);

private:
	Server						*_server;
	int							_fd;
	t_status					_status;
	std::string					_ip;
	std::vector<Channel *>		_channels;
	std::string					_nickname;
	std::string					_username;
	std::string					_prefix;
	std::string					_realname;
	std::string					_hostname;
	std::string					_incompleteMessage;
};

#endif //CLIENT_HPP
