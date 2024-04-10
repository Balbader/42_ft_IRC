#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <Channel.hpp>

#define BUFFER_SIZE 512

#define PASS_FLAG 2
#define USER_FLAG 4
#define NICK_FLAG 8

#define OPER_USER "foo"
#define OPER_PASS "bar"

#define CLI_OPER 0b0001 // 1 in decimal
#define CLI_INV 0b0010 // 2 in decimal
#define CLI_WALLOP 0b0100 // 4 in decimal
#define CLI_NOTICE 0b1000 // 8 in decimal

class Channel;

class Client {
private:
	int	fd;
	int	registration;	
	bool welcome;		
	bool knowPassword;
	bool toDisconnect;
	unsigned int flags;
	std::string	nickname;
	std::string	username;
	std::string	hostname;
	std::string	servername;
	std::string	realname;
	std::string	sendData;
	std::vector<Channel *> channels;

	bool setOperator(bool on);
	bool setInvisible(bool on);
	bool setNotice(bool on);
	bool setWallop(bool on);

public:
	~Client(void);
	Client(void);
	Client(int fd, std::string hostname);

	void setSendData(std::string data);
	void setHostname(std::string name);
	void setServername(std::string name);
	void setUsername(std::string name);
	void setRealname(std::string name);
	void setNickname(std::string name);
	bool setMode(char mode, bool on);
	void setRegistration(int flag);
	void setOp(bool value);
	void setKnowPassword(bool value);
	void setToDisconnect(bool value);
	void setWelcome(bool value);
	void resetSendData(int len);
	void addChannel(Channel &ch);
	void removeChannel(Channel &ch);

	bool isVisible() const;
	int getFd() const;
	int getRegistration() const;
	bool getWelcome() const;
	bool getOp() const;
	bool getToDisconnect() const;
	std::string	getNickname() const;
	std::string	getUsername() const;
	std::string	getHostname() const;
	std::string	getServername() const;
	std::string	getRealname() const;
	std::string	getSendData() const;
	std::vector<Channel *> & getChannels();
	std::string	getModesStr() const;
	std::string getClientPrefix();
};
