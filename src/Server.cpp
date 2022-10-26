#include "Server.hpp"

Server::Server()
{
	this->commands["NICK"]		= &Server::cmd_nick;
	this->commands["USER"]		= &Server::cmd_user;
	this->commands["PING"]		= &Server::cmd_ping;
	this->commands["PRIVMSG"]	= &Server::cmd_privmsg;
	this->commands["NOTICE"]	= &Server::cmd_notice;
	this->commands["JOIN"]		= &Server::cmd_join;
	this->commands["PART"]		= &Server::cmd_part;
	this->commands["LEAVE"]		= &Server::cmd_part;
	this->commands["QUIT"]		= &Server::cmd_quit;
	this->commands["KICK"]		= &Server::cmd_kick;
	this->commands["KILL"]		= &Server::cmd_kill;
	this->commands["kill"]		= &Server::cmd_kill; // 'irssi' send in lower case.
	this->commands["OPER"]		= &Server::cmd_oper;
	this->commands["PASS"]		= &Server::cmd_pass;

	FD_ZERO(&this->readfds);
	FD_ZERO(&this->activefds);

	this->fd_max = 0;
}

Server::~Server()
{
	close(this->sockfd);

	std::list< User * >::const_iterator	it;
	std::list< User * >::const_iterator	ite = this->users.end();

	for (it = this->users.begin(); it != ite; it++)
		delete (*it);

	std::map< std::string, Channel * >::iterator	it_channels;
	for (it_channels = this->channels.begin(); it_channels != this->channels.end(); it_channels++)
		delete ((*it_channels).second);
}

/// @brief Create a server.
/// @param host Host ip address.
/// @param port Port to listen.
/// @return The socket file descriptor.

void	Server::createServer(std::string host, int port, std::string password)
{
	if (port < 1024 || port > 49151)
	{
		std::cerr << "Invalid port." << std::endl;
		exit(-1);
	}

	this->host 	= host;
	this->port	= port;
	this->pass	= password;

	struct sockaddr_in	options;

	int	sockfd;
	int	opt 	= 1;

	if ((sockfd	= socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cerr << "Error when creating socket." << std::endl;
		exit(-1);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Error when setting socket options." << std::endl;
		close(sockfd);
		exit(-1);
	}

	options.sin_family		= AF_INET;
	options.sin_addr.s_addr = INADDR_ANY;
	options.sin_port		= htons(this->port);

	inet_aton(this->host.c_str(), (struct in_addr *) &options.sin_addr.s_addr);

	if (bind(sockfd, (struct sockaddr *) &options, sizeof(options)) == -1)
	{
		std::cerr << "Error when binding socket." << std::endl;
		close(sockfd);
		exit(-1);
	}

	if (listen(sockfd, 128) == -1)
	{
		std::cerr << "Error: " << std::strerror(errno) << std::endl;
		close(sockfd);
		exit(-1);
	}

	/**
	 * @brief Mode non bloquant
	 * L'exécution du programme continue même si il n'y a pas de nouvel utilisateur.
	 */
	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	this->sockfd = sockfd;
	this->fd_max = sockfd;

	FD_SET(sockfd, &this->activefds);

}

/// @brief Wait for a client connection.
/// @return Client socket file descriptor.

int		Server::waitClient()
{
	int					client_socket;
	struct	sockaddr_in	client_address;
	unsigned int		addr_len; 
	
	addr_len = sizeof(client_address);

	client_socket = accept(this->sockfd, (struct sockaddr *) &client_address, &addr_len);

	return (client_socket);
}

/// @brief Add a client to the list of users.
/// @param client_socket User socket file descriptor.

void	Server::addClient(int client_socket)
{
	this->fd_max = client_socket > this->fd_max ? client_socket : this->fd_max;

	FD_SET(client_socket, &(this->activefds));

	this->users.push_back(new User(client_socket));
}

/// @brief Listen the clients.
/// Receive a client's command and execute it.

void	Server::listenClients()
{
	std::list< User * >::iterator	it;

	char 	buffer[512];
	int		len = -1;

	this->readfds = this->activefds;

	if (select(this->fd_max + 1, &this->readfds, NULL, NULL, NULL) == -1)
		return ;

	for (it = this->users.begin(); it != this->users.end(); it++)
	{
		if ((*it)->isDisconnected())
		{
			FD_CLR((*it)->getSocket(), &this->activefds);
			close((*it)->getSocket());
			std::cout << (*it)->getNickname() << " left!" << std::endl;
			delete (*it);
			it = this->users.erase(it);
			break;
		}
		
		{
			std::list< User * >::iterator it;

			for (it = this->users.begin(); it != this->users.end(); it++)
			{
				std::cout << "user: " <<(*it)->getNickname() << std::endl;
			}
			std::cout << "--------------------------" << std::endl;
		}

		int	client_fd = (*it)->getSocket();
	
		if (FD_ISSET(client_fd, &this->readfds))
		{
			len = -1;

			memset(buffer, 0, 512);

			len = recv(client_fd, buffer, 512, MSG_DONTWAIT);

			if (len == 0)
				this->cmd_quit((*it), ":Lost connection");

			else if (len > 0)
				(*it)->bufferCommand(std::string(buffer), *this);
		}
	}
}


/// @brief Checks if a channel does exist.
/// @param channel 
/// @return A pointer to the channel, nullptr if the channel doesn't exist.

Channel *	Server::channelExists(std::string channel)
{
	Channel *	channel_ptr;
	try
	{
		channel_ptr = this->channels.at(channel);
	}
	catch(const std::out_of_range & e)
	{
		return (nullptr);
	}
	return (channel_ptr);
}

/// @brief Get the file descriptor of the server socket.
/// @return A file descriptor.

int		Server::getSockFd() const
{
	return (this->sockfd);
}

/// @brief Get the host IP address.
/// @return An IP address.

std::string	Server::getHost() const
{
	return (this->host);
}


/// @brief Get a pointer to a user.
/// @param nickname
/// @return A pointer to the user, nullptr if the user doesn't exist.

User *		Server::getUser(std::string nickname)
{
	std::list< User * >::iterator it;

	for (it = this->users.begin(); it != this->users.end(); it++)
	{
		if ((*it)->getNickname() == nickname)
			return ((*it));
	}
	return nullptr;
}