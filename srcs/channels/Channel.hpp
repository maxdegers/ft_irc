//
// Created by gberthol on 1/20/25.
//

#pragma once

#include <iostream>

#include <vector>

class Client;

class Channel
{
	public:
		Channel(Client *creator, const std::string& channelName, const std::string& serverIP);
		Channel(const Channel &old);
		~Channel();
		Channel &operator=(const Channel &old);

		void	shareMessage(const std::string& message, const std::string& username);

		void	inviteUser(Client *host, Client *guest);

		void	removeOp(Client *remover, Client *clientToRemove);
		void	kickUser(Client *kicker, Client *toKick);

		void	tryToJoin(Client *newClient, const std::string& password);
		void	addOp(Client *adder, Client *clientToAdd);

		void	setMaxUser(Client *client, unsigned long newMax);
		void	setPassword(Client *client, const std::string& newPassword);
		void	setTopic(Client *client, const std::string& newTopic);
		void	setInviteOnly(Client *client, bool newInvite);

		bool	checkUserOP(Client *clientToCheck);
		bool	checkUser(Client *clientToCheck);

		std::string &getChannelName();

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
