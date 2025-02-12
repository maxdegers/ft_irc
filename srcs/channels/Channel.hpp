#pragma once

#include <iostream>

#include <vector>

class Client;

class Channel
{
	public:
		Channel(Client *creator, const std::string& channelName, const std::string& serverIP);
		Channel(Client *creator, const std::string& channelName, const std::string& serverIP, const std::string &password);

		Channel(const Channel &old);
		~Channel();
		Channel &operator=(const Channel &old);

		void	shareMessage(const std::string& message, const std::string& username);

		void	inviteUser(Client *host, Client *guest);

		std::string	getChannelName() const;

		void	removeOp(Client *remover, Client *clientToRemove, bool sendMsg);
		void	kickUser(Client *kicker, Client *toKick);
		size_t	getUserAmount();

		void	tryToJoin(Client *newClient, const std::string& password);
		void	addOp(Client *adder, Client *clientToAdd);

		void	setMaxUser(Client *client, unsigned long newMax);
		void	setPassword(Client *client, const std::string& newPassword);
		void	setTopic(Client *client, const std::string& newTopic);
		void	setInviteOnly(Client *client, bool newInvite);
		void	setTopicOP(Client *client, bool newRule);

		bool	checkUserOP(Client *clientToCheck);
		bool	checkUser(Client *clientToCheck);

		void	displayTopic(Client *client, bool toAll);

		void	listUsers(Client *client);

		std::string &getChannelName();

		void removeUser(const std::string &name);

	private:
		std::vector<Client *>		_user;
		std::vector<Client *>		_opUsers;
		bool						_inviteOnly;
		bool						_topicOpOnly;
		unsigned long				_maxUsers;
		std::string					_password;
		std::string					_topic;
		std::vector<std::string>	_invitedUsername;
		std::string					_channelName;
		std::string 				_serverIP;
};
