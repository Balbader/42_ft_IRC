#pragma once
#include "Server.hpp"

# define GREEN "\e[1;32m"
# define RESET "\e[0m"
# define RED "\e[1;91m"
# define CYAN "\e[1;36m"
# define YELLOW "\e[1;33m"
# define PURPLE "\e[1;35m"
# define BLUE "\e[1;34m"

struct Modes {
	bool away;
	bool invisible;
	bool wallops;
	bool restricted;
	bool op;
	bool localOp;
	bool server;
};

std::string fillIt(std::string str, size_t len);
const std::string currentDateTime();

class Client {
public:
    Client();
    Client(int fd);
    Client(const Client & x);
    ~Client();
    Client & operator=(const Client & rhs);

    // Getters
    std::string	getUserName() const;
    std::string	getNickName() const;
    std::string	getFullName() const;
    std::string	getID()	const;
    std::string	getHost() const;
    int	getClientfd() const;
    bool getAuth() const;
    int	getRegistered() const;
    int	getisOperator() const;
    int	getMode(char mode) const;
    std::string	getUserPerfix()	const;
    std::string	getUserInfo() const;
    std::string	getAllChannels() const;
    std::map<std::string, Channel*>	getJoinedChannels()	const;

    // Setters
    void setUserName(std::string UserName);
    void setNickName(std::string NickName);
    void setFullName(std::string FullName);
    void setID(std::string ID);
    void setClientfd(int clientfd);
    void setAuth(int Auth);
    void setRegistered(int Registered);
    void setIsOperator(int isOperator);
    void setMode(int value, char mode);
    void joinChannel( std::string ChannelName, Channel *channel );
    void leaveChannel( std::string ChannelName );
    std::string	leaveAllChannels();

    int	isJoined( std::string ChannelName ) const;

    std::string	JoinedChannels() const;

private:
    int	_clientfd;
    bool _Auth;
    bool _Registered;
    bool _isOperator;
    std::string	_NickName;
    std::string	_UserName;
    std::string	_FullName;
    const std::string _Host;
    std::string	_ID;
    struct sockaddr_storage	_remotaddr;
    socklen_t _addrlen;
    struct Modes _modes;
    std::map<std::string, Channel *> _joinedChannels;
};
