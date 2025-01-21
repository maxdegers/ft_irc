#ifndef LOG_HPP
# define LOG_HPP

# include <string>

class Log
{
public:
/* enum */
	enum logLevel
	{
		INFO,
		DEBUG,
		ERROR
	};

/* Methods */
	static void log(logLevel lvl, const std::string &message);
	static void info(const std::string &message);
	static void debug(const std::string &message);
	static void error(const std::string &message);

private:
	static std::string getTimeStamp();
	static std::string levelToString(logLevel lvl);
};

#endif //LOG_HPP
