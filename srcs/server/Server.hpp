#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include <vector>

struct pollfd;

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
	std::vector<Client> clients() const;
	//TODO faire une methode de recherche de client par son fd (voir fonction destroy)
	std::vector<struct ::pollfd> fds() const;

/* Methods */
	void	init(int port);
	void	run();

	void	acceptClient();
	void	readData(int fd);

	static void	signalHandler(int sig);

	void	closeFDs();
	void	destroy(int fd);

private:
	int							_port;
	int							_socketFD;
	static bool					_signal;
	std::vector<Client>			_clients;
	std::vector<struct pollfd>	_fds;
};

#endif //SERVER_HPP
