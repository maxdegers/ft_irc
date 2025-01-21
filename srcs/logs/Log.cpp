#include "Log.hpp"
#include "colors.h"
#include <iostream>
#include <ctime>

/* Methods ****************************************************************** */
void Log::log(logLevel lvl, const std::string &message)
{
	std::cout
		<< B_WHITE << "["
		<< MAGENTA << getTimeStamp()
		<< B_WHITE << "]"
		<< RESET << "-"
		<< B_WHITE << "["
		<< levelToString(lvl)
		<< B_WHITE << "] "
		<< RESET << message
		<< std::endl;
}

void Log::info(const std::string &message)
{
	log(INFO, message);
}

void Log::debug(const std::string &message)
{
	log(DEBUG, message);
}

void Log::error(const std::string &message)
{
	log(ERROR, message);
}

std::string Log::getTimeStamp()
{
	char buffer[80];
	std::time_t now = std::time(0);
	std::tm *tm_info = std::localtime(&now);
	std::strftime(buffer, sizeof(buffer), "%H:%M:%S", tm_info);
	return (std::string(buffer));
}

std::string Log::levelToString(const logLevel lvl)
{
	switch (lvl)
	{
		case INFO:
			return B_GREEN"INFO";
		case DEBUG:
			return B_BRIGHT_YELLOW"DEBUG";
		case ERROR:
			return B_BRIGHT_RED"ERROR";
		default:
			return B_WHITE"UNKNOWN";
	}
}