#ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include <map>

# include "Channel.hpp"
class Channel;

#include "Server.hpp"
class Server;

class User
{

	public:
		User();
		User(int sockfd);
		~User();

		std::string 						getNickname();
		std::string 						getUsername();
		int 								getSocket();
		std::map< std::string, Channel * >	getChannels();
		bool								getOp();
		bool								isDisconnected();
		bool								isRegistered();
		bool								isPasswordEntered();

		void					setNickname(const std::string nickname);
		void					setUsername(const std::string username);
		void					setSocket(int socket);
		void					setOp();
		void					setDisconnected();
		void					setRegistered();
		void					setPasswordEntered();

		void					bufferCommand(std::string buffer, Server & server);

		void					addChannel(Channel * channel);
		void					removeChannel(Channel * channel);

	private:
		int						socket;
		std::string 			nickname;
		std::string 			username;
		std::string				buffer;

		bool					is_server_op;
		bool					is_disconnected;
		bool					is_registered;
		bool					is_password_entered;

		std::map< std::string, Channel * >	channels;

};

#endif /* ************************************************************ USER_H */