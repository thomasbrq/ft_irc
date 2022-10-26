#include "User.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

User::User()
{
	this->buffer = "";
	this->is_server_op = false;
	this->is_disconnected = false;
	this->is_password_entered = false;
	this->is_registered = false;
}

User::User(int sockfd) : socket(sockfd)
{
	this->buffer = "";
	this->is_server_op = false;
	this->is_disconnected = false;
	this->is_password_entered = false;
	this->is_registered = false;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

User::~User()
{
	close(this->socket);
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/

void					User::addChannel(Channel * channel)
{
	this->channels[channel->getName()] = channel;
}

void					User::removeChannel(Channel * channel)
{
	std::map< std::string, Channel * >::iterator it;

	it = this->channels.find(channel->getName());
	it = this->channels.erase(it);
}

void					User::bufferCommand(std::string buffer, Server &server)
{
	this->buffer += buffer;
	if (this->buffer.find("\r\n") == std::string::npos)
		return ;
	else
	{
		server.splitMessage(this, this->buffer);
		this->buffer.clear();
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string		User::getNickname()
{
	return (this->nickname);
}

std::string		User::getUsername()
{
	return (this->username);
}

int				User::getSocket()
{
	return (this->socket);
}

std::map< std::string, Channel * >	User::getChannels()
{
	return (this->channels);
}

bool			User::getOp()
{
	return (this->is_server_op);
}

bool			User::isDisconnected()
{
	return (this->is_disconnected);
}

bool			User::isRegistered()
{
	return (this->is_registered);
}

bool			User::isPasswordEntered()
{
	return (this->is_password_entered);
}

void			User::setDisconnected()
{
	this->is_disconnected = true;
}

void			User::setNickname(const std::string nickname)
{
	this->nickname = nickname;
}

void			User::setUsername(const std::string username)
{
	this->username = username;
}

void			User::setSocket(int socket)
{
	this->socket = socket;
}

void			User::setOp()
{
	this->is_server_op = true;
}

void			User::setRegistered()
{
	this->is_registered = true;
}

void			User::setPasswordEntered()
{
	this->is_password_entered = true;
}


/* ************************************************************************** */