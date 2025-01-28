#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include <vector>

struct pollfd;
class Client;

class Server
{
public:
/* Constructors */
	Server();
	Server(const Server &src);
	~Server();

/* Operators */
	Server &operator=(const Server &src);

/* Getters */
	int port() const;
	int socketFD() const;
	std::string getPassword();
	std::vector<Client> clients() const;
	std::vector<struct ::pollfd> fds() const;

/* Methods */
	void			initArguments(int ac, char **av);
	void			init();
	void			run();

	void			acceptClient();
	void			readData(int fd);
	void			executeCommand(const std::string &completeCommand, Client *client);

	Client			*findClient(int fd);
	bool			checkNick(const std::string &nick);

	static void		signalHandler(int sig);

	void			closeFDs();
	void			QUIT(int fd);

/* Exceptions */
	class ArgumentsErrorException : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};

private:
	std::string					_password;
	int							_port;
	int							_socketFD;
	static bool					_signal;
	std::vector<Client>			_clients;
	std::vector<struct pollfd>	_fds;

	void separateCmdArg(const std::string &completeCommand, std::string &command, std::string &args);
	void truncCarriageReturns(std::string &str);

	void JOIN(std::string args);

};

#endif //SERVER_HPP
