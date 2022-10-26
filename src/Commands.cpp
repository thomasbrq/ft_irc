#include "Server.hpp"

/// @brief Check the server password entered by the user.
/// @param user A pointer to an user.
/// @param content A string, the password

void	Server::cmd_pass(User * user, std::string content)
{
	std::vector< std::string > params = this->splitParameters(content, " ");

	if (params[0] != this->pass)
		this->cmd_quit(user, "invalid password.");
	else
		user->setPasswordEntered();
}

/// @brief Change the user's nickname.
/// @param user A pointer to an user.
/// @param nick A string, the new nickname.

void	Server::cmd_nick(User * user, std::string nick)
{
	if (!user->isPasswordEntered())
		return ;

	std::string	message;
	std::vector< std::string > params = this->splitParameters(nick, " ");
	std::list< User * >::const_iterator	it;
	
	for (it = this->users.begin(); it != this->users.end(); it++)
	{
		if ((*it)->getNickname() == params[0] && user->getNickname() != params[0])
		{
			this->sendMessageRPL(user, ERR_NICKNAMEINUSE, " :Nick already in use");
			params[0] += "_";
			break ;
		}
	}

	message = ":" + this->networkName(user) + " NICK " + params[0] + "\n";

	this->sendMessageToServer(message);
	user->setNickname(params[0]);

	if (user->getUsername().length() > 0 && !user->isRegistered())
	{
		welcomeBurst(user);
		user->setRegistered();
	}
}

/// @brief Change the user's username.
/// @param user A pointer to an user.
/// @param username A string, the new username.

void	Server::cmd_user(User * user, std::string username)
{
	if (!user->isPasswordEntered())
		return ;

	user->setUsername(username.substr(0, username.find(" ")));
	
	if (user->getNickname().length() > 0 && !user->isRegistered())
	{
		welcomeBurst(user);
		user->setRegistered();
	}

}

/// @brief Check the connection between the client and the server.
/// @param user A pointer to an user.
/// @param content A string.

void		Server::cmd_ping(User * user, std::string content)
{
	(void) content;
	std::string	hostname = ":" + this->getHost();
	std::string	msg = hostname + " PONG\n";

	send(user->getSocket(), msg.c_str(), msg.length(), MSG_NOSIGNAL);
}

/// @brief Send a message to an user or to a channel.
/// @param user A pointer to an user.
/// @param receiver A string, the destination and the message.

void		Server::cmd_privmsg(User * user, std::string receiver)
{
	std::string	destination = receiver.substr(0, receiver.find(" "));
	std::string message 	= receiver.substr(receiver.find(":"), receiver.length());

	message = ":" + user->getNickname() + " PRIVMSG " + destination + " " + message + "\n";

	if (destination[0] == '#')
		this->sendMessageToChannel(user, destination, message, false, true);
	else
		this->sendMessageToUser(user, destination, message, true);
}

/// @brief Send a notice message to an user or to a channel.
/// @param user A pointer to an user.
/// @param content A string, the destination and the message.

void		Server::cmd_notice(User * user, std::string content)
{
	std::string	destination = content.substr(0, content.find(" "));
	std::string message 	= content.substr(content.find(":"), content.length());

	message = ":" + user->getNickname() + " NOTICE " + destination + " " + message + "\n";

	if (destination[0] == '#')
		this->sendMessageToChannel(user, destination, message, false, false);
	else
		this->sendMessageToUser(user, destination, message, false);
}

/// @brief Leave the user from a channel.
/// @param user A pointer to an user.
/// @param channel A string, the channel.

void		Server::cmd_part(User * user, std::string channel)
{
	Channel		*channel_ptr;

	std::string	message;
	if (channel.find(" ") == std::string::npos)
	{
		message = user->getNickname();
		channel = channel.substr(0, channel.length());
	}
	else
	{
		message = channel.substr(channel.find(" ") + 1, channel.length());
		channel = channel.substr(0, channel.find(" "));
	}

	channel_ptr = this->channelExists(channel);

	if (!channel_ptr || !channel_ptr->userAlreadyConnected(user->getNickname()))
		return ;

	message = ":" + this->networkName(user) + " PART " + channel + " :" + message + "\n";
	this->sendMessageToChannel(user, channel_ptr->getName(), message, true, true);

	user->removeChannel(this->channels[channel]);
	channel_ptr->removeUser(user);
	channel_ptr->removeUserFromOperators(user);
}

/// @brief Leave the user from the server.
/// @param user A pointer to an user.
/// @param message A string, the leaving message.

void		Server::cmd_quit(User * user, std::string message)
{
	std::map< std::string, Channel * >::iterator	it;
	std::map< std::string, Channel * > connected_channels = user->getChannels();

	for (it = connected_channels.begin(); it != connected_channels.end(); it++)
		this->cmd_part(user, (*it).first + " " + message.substr(0, message.length()));

	user->setDisconnected();
}

/// @brief Joins a channel or create a new one.
/// @brief user A pointer to a user.
/// @param channel A string with the name of the channel to join.

void		Server::cmd_join(User * user, std::string channel)
{
	Channel	*	channel_ptr;

	channel = channel.substr(0, channel.length());

	channel_ptr = this->channelExists(channel);
	if (!channel_ptr)
	{
		channel_ptr = new Channel(channel);
		this->channels[channel] = channel_ptr;
		channel_ptr->addUserToOperators(user);
	}
	
	if (channel_ptr->userAlreadyConnected(user->getNickname()))
		return ;

	channel_ptr->addUser(user);
	user->addChannel(channel_ptr);

	std::string message = ":" + this->networkName(user) + " JOIN " + channel + "\n";

	this->sendMessageToChannel(user, channel, message, true, true);

	this->sendMessageRPL(user, RPL_NAMREPLY, "= " + channel + " :" + channel_ptr->operatorsList() + " " + channel_ptr->userList());
	this->sendMessageRPL(user, RPL_ENDOFNAMES, channel + " :End of NAMES list");
}

/// @brief Kick an user from a channel.
/// @brief user A pointer to a user.
/// @param content A string, a channel, a nickname, the reason. 

void		Server::cmd_kick(User * user, std::string content)
{
	std::vector< std::string >	params = this->splitParameters(content, " ");

	if (params.size() < 2)
		return (this->sendMessageRPL(user, ERR_NEEDMOREPARAMS, " :Not enough params."));

	Channel * channel_ptr = this->channelExists(params[0]);
	if (channel_ptr == nullptr)
		return (this->sendMessageRPL(user, ERR_NOSUCHCHANNEL, " :No such channel."));
	
	if (!channel_ptr->userAlreadyConnected(user->getNickname()))
		return (this->sendMessageRPL(user, ERR_NOTONCHANNEL, " :User not on the channel."));
	
	if (!channel_ptr->userIsOperator(user->getNickname()))
		return (this->sendMessageRPL(user, ERR_CHANOPRIVSNEEDED, " :You are not op."));
	
	User * target = channel_ptr->getUser(params[1]);

	if (!target)
		return (this->sendMessageRPL(user, ERR_USERNOTINCHANNEL, params[1] + " " + params[0] + " :This user is not on the channel."));

	std::string message = ":" + this->networkName(user) + " KICK " + params[0] + " " + params[1] + " " + params[2] + "\n";
	this->sendMessageToChannel(user, channel_ptr->getName(), message, true, true);

	target->removeChannel(this->channels[params[0]]);
	channel_ptr->removeUser(target);
	channel_ptr->removeUserFromOperators(target);
}

/// @brief Make an user the server operator.
/// @brief user A pointer to a user.
/// @param content A string, the user, the password.

void						Server::cmd_oper(User * user, std::string content)
{
	std::vector< std::string > params = this->splitParameters(content, " ");

	if (params.size() < 2)
		return (this->sendMessageRPL(user, ERR_NEEDMOREPARAMS, " :Not enough params."));

	if (params[1] != OP_PASS)
		return (this->sendMessageRPL(user, ERR_PASSWDMISMATCH, " :Invalid password."));

	User * target = this->getUser(params[0]);

	if (!target)
		return (this->sendMessageRPL(user, ERR_NOSUCHNICK, " :User does not exist."));

	target->setOp();

	this->sendMessageRPL(target, RPL_YOUREOPER, " :You are now a server operator.");
}

/// @brief Leave an user from the server.
/// @brief user A pointer to a user.
/// @param content A string, the user, the reason.

void						Server::cmd_kill(User * user, std::string content)
{
	std::vector< std::string > params = this->splitParameters(content, " ");

	if (params.size() < 2)
		return (this->sendMessageRPL(user, ERR_NEEDMOREPARAMS, " :Not enough params."));

	if (!user->getOp())
		return (this->sendMessageRPL(user, ERR_NOPRIVILEGES, " :You are not op."));

	User * target = this->getUser(params[0]);

	if (!target)
		return (this->sendMessageRPL(user, ERR_NOSUCHNICK, ":User does not exist"));

	this->cmd_quit(target, "KILL " + params[1]);
}


/// @brief Get the network name and address.
/// @param user A pointer to an user.
/// @return A string that contains the network name.

std::string	Server::networkName(User * user)
{
	return (user->getNickname() + "!" + user->getUsername() + "@" + this->getHost());
}

/// @brief Send a message to a client.
/// @param user A pointer to an user.
/// @param rpl_code The IRC RPL code.
/// @param message A message.

void	Server::sendMessageRPL(User * user, std::string rpl_code, std::string message)
{
	std::string	hostname = ":" + this->getHost();
	std::string	rpl = hostname + " " + rpl_code + " " + user->getNickname() + " " + message + "\n";

	send(user->getSocket(), rpl.c_str(), rpl.length(), MSG_NOSIGNAL);
}

/// @brief Send the welcome messages to an user.
/// @param user A pointer to an user.

void		Server::welcomeBurst(User * user)
{
	// Introduces users to the network
	this->sendMessageRPL(user, RPL_WELCOME, ":Welcome " + networkName(user));

	// Tells users about the server they’re connecting to
	this->sendMessageRPL(user, RPL_YOURHOST, ":Your host is " + this->getHost());

	// Tells users when the server was started
	this->sendMessageRPL(user, RPL_CREATED, ":This server was created by meu and erndael");
	
	// Gives users mode letter information
	this->sendMessageRPL(user, RPL_MYINFO, ":Server version: 1.0");

	// Shows users the server’s Message of the Day
	this->sendMOTD(user);
}

/// @brief Send the MOTD to a new user.
/// @param user A pointer to an user.

void		Server::sendMOTD(User * user)
{
	std::ifstream ifs ("MOTD");
	if (!ifs.is_open())
	{
		this->sendMessageRPL(user, ERR_NOMOTD, ":MOTD File is missing");
		return ;
	}

	this->sendMessageRPL(user, RPL_MOTDSTART, ":Welcome to our IRC server!");

	std::string line;
	while (std::getline(ifs, line))
		this->sendMessageRPL(user, RPL_MOTD, line);

	ifs.close();
	
	this->sendMessageRPL(user, RPL_ENDOFMOTD, ":");
}