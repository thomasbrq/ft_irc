#ifndef SERVER_HPP
# define SERVER_HPP

# include "User.hpp"

# include "Channel.hpp"

# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <iostream>
# include <errno.h>
# include <fcntl.h>

#include <sys/select.h>

# include <list>
# include <vector>
# include <map>
# include <utility>

# include <fstream>
# include <string>

#include <signal.h>

# define RPL_WELCOME			"001"
# define RPL_YOURHOST			"002"
# define RPL_CREATED			"003"
# define RPL_MYINFO				"004"
# define RPL_MOTD				"372"
# define RPL_MOTDSTART			"375"
# define RPL_ENDOFMOTD			"376"
# define RPL_TOPIC				"332"
# define RPL_NAMREPLY			"353"
# define RPL_ENDOFNAMES			"366"
# define RPL_YOUREOPER			"381"

# define ERR_NOSUCHNICK			"401"
# define ERR_NOSUCHCHANNEL		"403"
# define ERR_CANNOTSENDTOCHAN	"404"
# define ERR_NOMOTD				"422"
# define ERR_NICKNAMEINUSE		"433"

# define ERR_NEEDMOREPARAMS		"461"
# define ERR_USERNOTINCHANNEL 	"441"
# define ERR_NOTONCHANNEL		"442"
# define ERR_CHANOPRIVSNEEDED	"482"

# define ERR_NOPRIVILEGES		"481"

# define ERR_PASSWDMISMATCH		"464"

# define MSG_NOSIGNAL			0x2000

# define OP_PASS				"password"

class User;

class Server
{
	public:
		Server();
		~Server();

		void						createServer(std::string host, int port, std::string password);
		int							waitClient();

		void						addClient(int sockfd);
		void						listenClients();

		int							getSockFd() const;
		std::string					getHost() const;
		User *						getUser(std::string nickname);

		void						executeCommand(User * user, std::string message);
		void						splitMessage(User * user, std::string message);
		std::vector< std::string >	splitParameters(std::string content, std::string delimiter);
		void						sendMessageRPL(User * user, std::string rpl_code, std::string message);
		void						sendMessageToUser(User * userFrom, std::string userTo, std::string message, bool send_errors);
		void						sendMessageToChannel(User * userFrom, std::string channel, std::string message, bool send_to_everyone, bool send_errors);
		void						sendMessageToServer(std::string message);

		void						welcomeBurst(User * user);
		void						sendMOTD(User * user);
		std::string					networkName(User * user);
		void						cmd_nick(User * user, std::string nick);
		void						cmd_user(User * user, std::string username);
		void						cmd_ping(User * user, std::string content);
		void						cmd_join(User * user, std::string channel);
		void						cmd_part(User * user, std::string channel);
		void						cmd_privmsg(User * user, std::string receiver);
		void						cmd_quit(User * user, std::string message);
		void						cmd_kick(User * user, std::string content);
		void						cmd_oper(User * user, std::string content);
		void						cmd_kill(User * user, std::string content);
		void						cmd_pass(User * user, std::string password);
		void						cmd_notice(User * user, std::string content);

		bool						userAlreadyJoined(User * user, std::string channel);
		Channel *					channelExists(std::string channel);

	private:
		int					sockfd;
		std::string			host;
		int					port;
		int					fd_max;

		std::string			pass;

		struct fd_set		readfds;
		struct fd_set		activefds;

		std::list< User * >	users;
		std::map< std::string, Channel * >	channels;
		std::map< std::string, void	(Server::*)(User *, std::string) >	commands;

};

#endif
