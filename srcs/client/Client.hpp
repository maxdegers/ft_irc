/* ***************** */
/*      WebServ      */
/* ***************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

class Channel;

class Client
{
public:
/* Constructors */
	Client();
	Client(const Client &src);
	~Client();

/* Operators */
	bool operator==(const Client &compare);
	Client &operator=(const Client &src);

/* Getters */
	int			fd() const;
	std::string	ip() const;
	std::string	&getUsername();

/* Setters */
	void		setFD(int fd);
	void		setIP(std::string ip);

/* Methods */
	std::string sendMessage(std::string message, Client *receive);
	std::string sendMessage(std::string message, Channel *receive);

private:
	int			_fd;
	std::string	_ip;
	std::vector<Channel *>		_channels;
	std::string					_nickname;
	std::string					_username;
};

#endif //CLIENT_HPP

// TODO Operator to compare 2 clients
/*

*/
