#include "Channel.hpp"

Channel::Channel()
{
}

Channel::Channel(std::string name) : name(name)
{
}

Channel::~Channel()
{
}

std::string         Channel::getName()
{
    return this->name;
}

std::list< User * >	Channel::getConnectedUsers()
{
	return (this->connected_users);
}

User *				Channel::getUser(std::string nickname)
{
	std::list< User * >::iterator	it;

	for (it = this->connected_users.begin(); it != this->connected_users.end(); it++)
		if ((*it)->getNickname() == nickname)
			return *it;
	return nullptr;
}

/// @brief Get users connected to a channel as a string.
/// @return A string with the names of all connected users.

std::string			Channel::userList()
{
	std::string user_list = "";

	std::list< User * >::iterator it;

	for (it = this->connected_users.begin(); it != this->connected_users.end(); it++)
		user_list += (*it)->getNickname() + " ";
	
	user_list = user_list.substr(0, user_list.length() - 1);

	return user_list;
}

/// @brief Get operators connected to a channel as a string.
/// @return A string with the names of all connected operators.
std::string			Channel::operatorsList()
{
	std::string	operators_list = "";

	std::list< User * >::iterator	it;

	for (it = this->operators.begin(); it != this->operators.end(); it++)
		operators_list += "@" + (*it)->getNickname() + " ";
	
	operators_list = operators_list.substr(0, operators_list.length() - 1);

	return operators_list;
}

/// @brief  Check if a user is connected to a channel.
/// @param nickname of the user.
/// @return true if the user is connected, false otherwise.
bool				Channel::userAlreadyConnected(std::string nickname)
{
	std::list< User * >::iterator	it;
	for (it = this->connected_users.begin(); it != this->connected_users.end(); it++)
		if ((*it)->getNickname() == nickname)
			return (true);
	return (false);
}

/// @brief Check if a user is operator.
/// @param nickname of the user.
/// @return true if the user is operator, false otherwise.
bool				Channel::userIsOperator(std::string nickname)
{
	std::list< User * >::iterator	it;
	for (it = this->operators.begin(); it != this->operators.end(); it++)
		if ((*it)->getNickname() == nickname)
			return (true);
	return (false);
}

void                Channel::addUser(User * user)
{
    this->connected_users.push_back(user);
}

void                Channel::removeUser(User * user)
{
    std::list< User * >::iterator	it;

	for (it = this->connected_users.begin(); it != this->connected_users.end(); it++)
	{
		if ((*it)->getNickname() == user->getNickname())
		{
			it = this->connected_users.erase(it);
			return ;
		}
	}
}

void				Channel::addUserToOperators(User * user)
{
	this->operators.push_back(user);
}

void				Channel::removeUserFromOperators(User * user)
{
	std::list< User * >::iterator	it;

	for (it = this->operators.begin(); it != this->operators.end(); it++)
	{
		if ((*it)->getNickname() == user->getNickname())
			it = this->operators.erase(it);	
	}
}
