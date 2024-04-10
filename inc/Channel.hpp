#pragma once

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#include <utils.hpp>
#include <Client.hpp>
#include <Logger.hpp>

#define CHANNEL_PREFIX "&#!+"
#define USER_CHANNEL_LIMIT 10
#define USER_OPERATOR 0b1000
#define USER_SPEAKER 0b10000

class Client;

class Channel {
private:
	Logger LOGGER;
	Client *creator;
	std::string	name;
	std::string	topic;
	std::string	host;
	std::string	password;
	std::set<char> modes;
	unsigned int userLimit;
	bool initialized;
	std::map<Client *, unsigned int> clients;
	std::set<std::string> invited;

public:
	~Channel(void);
	Channel(void);

	std::string	getName() const;
	std::string	getTopic() const;
	Client &getCreator();
	unsigned int getUserLimit() const;
	std::set<char> &getMode();
	std::map<Client *, unsigned int> &getClients();
	std::map<Client *, unsigned int>::iterator getClientByNick(std::string clientNickname);
	std::string getStrModes();

	bool isInitialized();
	bool evalPassword(std::string psw);
	void setPassword(std::string password);
	void setTopic(std::string topic);
	void setUserLimit(unsigned int limit);
	bool isInvited(std::string nickname);
	void addInvite(std::string nickname);
	void removeInvited(std::string nickname);
	void asureOperator();

	void addClient(Client &client);
	void removeClient(Client &client);
	void removePassword();
	bool setOperator(std::string clientNickname, bool newValue);
	void setSpeaker(std::string clientNickname, bool newValue);
	void broadcast(Client &sender, std::string message, bool toSend);
	bool toggleMode(char mode, bool on);

	void initialize(std::string name, std::string password, Client &op);
	void initialize(std::string name, Client &op);
};
