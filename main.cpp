#include "Server.hpp"

bool	server_interrupted = false;

void	handleSignal(int signum)
{
	if (signum == SIGINT)
		server_interrupted = true;
}

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "./ircserv <port> <password>" << std::endl;
		return (-1);
	}

	Server	irc;

	irc.createServer("127.0.0.1", atoi(argv[1]), std::string(argv[2]));

	int	client_socket;
	while (42)
	{
		signal(SIGINT, &handleSignal);
		if (server_interrupted == true)
			break ;
		client_socket = 0;
		if ((client_socket = irc.waitClient()) != -1)
			irc.addClient(client_socket);
		irc.listenClients();
	}
	return (0);
}