#include <csignal>
#include <iostream>
#include "Server.hpp"
#include "Log.hpp"

int main(int ac, char **av)
{
	Server	server;

	try
	{
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		server.initArguments(ac, av);
		server.init();
		server.run();
	}
	catch (Server::ArgumentsErrorException &e)
	{
		Log::error(e.what());
		Log::info("Usage: ./ircserv <PORT> <PASSWORD>");
	}
	catch (std::runtime_error &e)
	{
		Log::error(e.what());
	}
}