/* ***************** */
/*      WebServ      */
/* ***************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

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

/* Setters */
	void		setFD(int fd);
	void		setIP(std::string ip);

/* Methods */
	
private:
	int			_fd;
	std::string	_ip;
};

#endif //CLIENT_HPP
