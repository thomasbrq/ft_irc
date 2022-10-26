#include "Server.hpp"

/// @brief Execute a command.
/// @param user A pointer to an user.
/// @param message A string.

void    Server::executeCommand(User * user, std::string message)
{
    size_t delimiter_pos;
    
    if ((delimiter_pos = message.find(" ")) == std::string::npos)
        return ;

    std::string command = message.substr(0, delimiter_pos);
    std::string content = message.substr(delimiter_pos + 1, message.length());

    if (user->isRegistered() 
        || command == "PASS"
        || command == "NICK"
        || command == "USER")
    {
        try
        {
            std::cout << "Try to execute: " << command << std::endl;
            (this->*commands.at(command))(user, content);
        }
        catch (std::out_of_range &e)
        {
            std::cerr << "Unknown command: " << command << std::endl;
        }
    }
}

/// @brief Split the received message into several commands.
/// @param user A pointer to an user.
/// @param message A string.

void		Server::splitMessage(User * user, std::string message)
{
    size_t      delimiter_pos;
    std::string line;

    while ((delimiter_pos = message.find("\r\n")) != std::string::npos)
    {
        line = message.substr(0, delimiter_pos);
        std::cout << line << std::endl;

        message.erase(0, delimiter_pos + 2);
        this->executeCommand(user, line);
    }
}

/// @brief Split a command's parameters using a specific delimiter.
/// @param content 
/// @param delimiter 
/// @return a vector containing each parameters as a string
std::vector< std::string >	Server::splitParameters(std::string content, std::string delimiter)
{
    std::vector< std::string >  words;
    std::string                 word;

    size_t                      pos = 0;

    while ((pos = content.find(delimiter)) != std::string::npos) {
        if (content.front() == ':')
            break;
        word = content.substr(0, pos);
        words.push_back(word);
        content.erase(0, pos + delimiter.length());
    }

    words.push_back(content);

    return words;
}