#include <csignal>
#include <iostream>
#include <string>
#include "Server.hpp"
#include "Client.hpp"

int main(int ac, char **av, char **env)
{
	(void) ac;
	(void) av;
	(void) env;
	Server	server;

	try
	{
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		server.init(4242);
		server.run();
	}
	catch (std::runtime_error &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
	}
}