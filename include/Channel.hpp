#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <list>

# include "User.hpp"
class User;

class Channel
{
	public:
		Channel();
		Channel(std::string name);
		~Channel();

		std::string			getName();
		std::list< User * >	getConnectedUsers();
		User *				getUser(std::string nickname);
		std::list< User * > getOperators();
		std::string			userList();
		std::string			operatorsList();

		bool				userAlreadyConnected(std::string nickname);
		bool				userIsOperator(std::string nickname);
		void				addUser(User * user);
		void				removeUser(User * user);
	
		void				addUserToOperators(User * user);
		void				removeUserFromOperators(User * user);
	private:
		std::string 		name;
		std::list< User * > connected_users;
		std::list< User * > operators;

};

#endif