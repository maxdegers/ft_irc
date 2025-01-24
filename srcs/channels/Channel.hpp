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
		Channel(Client *creator, std::string channelName);
		Channel(const Channel &old);
		~Channel();
		Channel &operator=(const Channel &old);

		void	shareMessage(std::string message, std::string username);

		void	inviteUser(Client *host, Client *guest);

		void	removeOp(Client *remover, Client *clientToRemove);
		void	kickUser(Client *kicker, Client *toKick);

		void	tryToJoin(Client *newClient, const std::string& password);
		void	addOp(Client *adder, Client *clientToAdd);

		void	setMaxUser(unsigned long newMax);
		void	setPassword(const std::string& newPassword);
		void	setTopic(Client *clientWhoSetTopic, const std::string& newTopic);
		void	setInviteOnly(bool newInvite);

		bool	checkUserOP(Client *clientToCheck);
		bool	checkUser(Client *clientToCheck);

		class NotOperatorException : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

		class NotInChannelException : public std::exception
		{
		public:
			virtual const char *what() const throw();
		};

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
};
