#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

# include <iostream>
# include <cstring>
# include <arpa/inet.h>

class Client
{
public:
/* Constructors */
	Client();
	Client(const Client &src);
	~Client();

/* Operators */
	Client &operator=(const Client &src);

/* Getters */
	int			fd() const;
	std::string	ip() const;
	std::string	status() const;

/* Setters */
	void		setFD(int fd);
	void		setIP(const std::string &ip);
	void		setStatus(const std::string &ip);

/* Methods */

	void		sendError(const int fd, const std::string &error);
	
private:
	int			_fd;
	std::string	_ip;
	std::string	_status;
	
};

#endif //CLIENT_HPP
