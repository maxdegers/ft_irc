#include "Server.hpp"

#include <sstream>

std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> result;
	std::string token;
	std::istringstream tokenStream(str);

	while (std::getline(tokenStream, token, delimiter)) {
		result.push_back(token);
	}

	return result;
}

void Server::JOIN(std::string args, Client *client)
{
	std::string::size_type spaceIndex = args.find(' ');
	if (spaceIndex == std::string::npos || args.substr(spaceIndex + 1, args.size()).find(' ') != std::string::npos)
		return ; //TODO renvoyer une erreur

	std::string channels = args.substr(0, spaceIndex);
	std::string keys = args.substr(spaceIndex + 1, args.size());

	std::vector<std::string> channelList = split(channels, ',');
	std::vector<std::string> keyList = split(channels, ',');

	std::vector<std::string>::iterator channel = channelList.begin();
	std::vector<std::string>::iterator key = keyList.begin();
	while (channel != channelList.end())
	{
		if (channel->at(0) != '#')
			return ; //TODO TEJ
		Channel *chan = findChannel(*channel);
		if (chan)
		{
			chan->tryToJoin(client, *key);
		}
		else
		{
			Channel newChannel = Channel(client, *channel, "") // TODO rajouter l'IP du server
			_channels.push_back(newChannel);
		}
		++key;
		++channel;
	}
}
