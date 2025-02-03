#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Channel.hpp"
# include <map>
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
	Channel			*findChannel(const std::string &name);
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
	std::vector<Channel>		_channels;

	typedef enum Command {
    	CMD_KICK,
   	 	CMD_INVITE,
    	CMD_TOPIC,
    	CMD_MODE,
    	CMD_JOIN,
    	CMD_QUIT,
    	CMD_PRIVMSG,
    	CMD_PASS,
    	CMD_NICK,
    	CMD_USER,
    	CMD_UNKNOWN // Pour les commandes non reconnues
	} 			ecmd;
	std::map<std::string, ecmd> _commandMap;


	void separateCmdArg(const std::string &completeCommand, std::string &command, std::string &args);
	void truncCarriageReturns(std::string &str);
	std::vector<std::string> split(const std::string &str, char delimiter);

	void JOIN(std::string args, Client *client);
	void			TOPIC(const std::string &str, Client *client);
	// void			PRIVMSG(const std::string &str, Client *client);

};

#endif //SERVER_HPP
