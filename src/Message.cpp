#include "Server.hpp"


/// @brief Sends a message to a specific user.
/// @param userFrom 
/// @param userTo 
/// @param message 
/// @param send_errors true if the command is privmsg, false otherwise.

void		Server::sendMessageToUser(User * userFrom, std::string userTo, std::string message, bool send_errors)
{
    std::list< User * >::iterator it;

    for (it = this->users.begin(); it != this->users.end(); it++)
    {
        if ((*it)->getNickname() == userTo)
        {
			send((*it)->getSocket(), message.c_str(), message.length(), MSG_NOSIGNAL);
            return ;
        }
    }
	if (!send_errors)
		this->sendMessageRPL(userFrom, ERR_NOSUCHNICK, userTo + " :no user with this nickname.");
}

/// @brief Sends a message to all the users in a channel.
/// @param userFrom 
/// @param channel 
/// @param message 
/// @param send_to_everyone if true, sends the message to the sender as well.
/// @param send_errors true if the command is privmsg, false otherwise.

void		Server::sendMessageToChannel(User * userFrom, std::string channel, std::string message, bool send_to_everyone, bool send_errors)
{
	Channel *						channel_ptr;
	std::list< User * > 			connected_users;
	
	channel_ptr = this->channelExists(channel);
	if (!channel_ptr)
	{
		if (!send_errors)
			this->sendMessageRPL(userFrom, ERR_NOSUCHCHANNEL, channel + " :This channel doesn't exists.");
		return ;
	}

	if (!channel_ptr->userAlreadyConnected(userFrom->getNickname()))
	{
		if (!send_errors)
			this->sendMessageRPL(userFrom, ERR_CANNOTSENDTOCHAN, channel + " :You are not connected to this channel.");
		return ;
	}
	
	connected_users = channel_ptr->getConnectedUsers();

	std::list< User * >::iterator 	it;
	for(it = connected_users.begin(); it != connected_users.end(); it++)
	{
		if (userFrom->getNickname() == (*it)->getNickname() && !send_to_everyone)
			continue ;
		send((*it)->getSocket(), message.c_str(), message.length(), MSG_NOSIGNAL);
	}
}

void		Server::sendMessageToServer(std::string message)
{
	std::list< User * >::iterator it;

	for (it = this->users.begin(); it != this->users.end(); it++)
		send((*it)->getSocket(), message.c_str(), message.length(), MSG_NOSIGNAL);
}