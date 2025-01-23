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
	std::string	incompleteMessage() const;

/* Setters */
	void		setFD(int fd);
	void		setIP(std::string ip);
	void		setIncompleteMessage(std::string msg);

/* Methods */
	
private:
	int			_fd;
	std::string	_ip;
	std::string	_incompleteMessage;
};

#endif //CLIENT_HPP
