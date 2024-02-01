#ifndef USER_HPP
#define USER_HPP

#include "class/Channel.hpp"

#include <iostream>
#include <map>
#include <netinet/in.h> // sockaddr_in
#include <string>
#include <sys/types.h> // socket, bind, listen, recv, send
#include <sys/socket.h> // socket, bind, listen, recv, send

class Channel;

#ifndef ALIVETIME
# define ALIVETIM 0
#endif

#ifndef TIMEOUT
# define TIMEOUT 1
#endif

class User	{
public:
	User(sockaddr_in const &addr = sockaddr_in(), int sockfd = -1);
	virtual ~User();

	User(const User	 &);

	void addChannel(Channel &);
	void delChannel(std::string const&);
	void updateLastActivity();

	bool init(int const&, sockaddr_in const&);

	// Getters
	sockaddr_in const& getAddr() const;
	int const& getSocket()const;
	std::string const& getNickname() const;
	std::string const& getUsername() const;
	std::string const& getHostname() const;
	std::string const& getServname() const;
	std::string const& getRealname() const;
	std::string const& getPassword() const;
	std::string const& getAwayMsg() const;
	std::string const& getModes() const;
	std::string const& getMask() const;
	std::string const& getMsg() const;

	bool const& getIsRegistered() const;
	bool const&getWaitingForPong() const;

	time_t const& getLastActivity() const;

	std::map<std::string const, Channel *const> const& getLookupChannel() const;

	static std::string const& getAvailableModes();
	static std::string const& getAvailableNicknameChars();

	// Setters
	void setSocket(int const);
	void setAddr(sockaddr_in const&);
	void setNickname(std::string const&);
	void setUsername(std::string const&);
	void setHostname(std::string const&);
	void setServname(std::string const&);
	void setRealname(std::string const&);
	void setPassword(std::string const&);
	void setAwayMsg(std::string const&);
	void setModes(std::string const&);
	void setMasks(std::string const&);
	void setMsg(std::string const&);
	void setIsRegistered(bool const);
	void setWaitingForPong(bool const);

	// Deprecated: need to remove
	void setChannels(std::map<std::string const, Chanel*> const &);


private:
	
};

#endif // !USER_HPP
