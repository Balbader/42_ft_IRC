#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"
#include "Config.hpp"
#include "User.hpp"
#include "colors.hpp"

#include <ctime> // time
#include <fcntl.h>
#include <iostream>
#include <list>
#include <map>
#include <netinet/in.h> // sockaddr_in
#include <poll.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 4096
#endif

extern bool g_interrupted;

class Server {
public:
	Server();
	virtual ~Server();

	void stop();

	bool init(std::string const&);
	bool run();
	bool start(uint16_t const);

/*
 * optional cmds:
 * ping
 * nick
 * names
 * time
 * ban
 * kickban
*/

private:
	typedef bool (Server::*t_fct)(User &user, std::string const &);

	// Attributes
	int _state;
	int _socket;
	Config _config;
	std::string _creationTime;
	std::vector<pollfd> _pollfds;
	std::list<User> _users;
	std::map<std::string const, t_fct const> _lookupCmds;
	std::map<std::string const, User *const> _lookupUsers;
	std::map<std::string const, Channel> _lookupChannels;
	std::map<uint const, std::string const> _lookupLogMsgTypes;
	std::list<std::string> _banlist;
	static std::pair<std::string const, t_fct const> const _arrayCmds[];
	static std::pair<uint const, char const *const> const _arrayLogMsgTypes[];

	// Methods
	void logMsg(uint const type, std::string const&);
	void joinSend(User &, Channel &, std::string const&);
	void partSend(User &, std::string &, std::sting &);
	void addToBanlist(User const &);

	bool DIE(User &, std::string const &);
	bool JOIN(User &, std::string const &);
	bool KICK(User &, std::string const &);
	bool KILL(User &, std::string const &);
	bool MODE(User &, std::string const &);
	bool MOTD(User &, std::string const &);
	bool NICK(User &, std::string const &);
	bool OPER(User &, std::string const &);
	bool PART(User &, std::string const &);
	bool PASS(User &, std::string const &);
	bool PING(User &, std::string const &);
	bool PRIVMSG(User &, std::string const &);
	bool QUIT(User &, std::string const &);
	bool USER(User &, std::string const &);
	bool WHOIS(User &, std::string const &);
	bool DIE(User &, std::string const &);

	bool checkStillActive(User &);
	bool checkPONG(User &, std::string const &);
	bool judge(User &);
	bool replyPush(User &, std::string const &);
	bool replySend(User &);
	bool welcome();

	static std::string toString(int const);

	enum e_state {
		STOPPED,
		RUNNING
	};

	enum e_logMsg {
		ERROR,
		INTERNAL,
		RECEIVED,
		SENT
	};

	enum e_rplNo {
		RPL_WELCOME = 001,
		RPL_YOURHOST = 002,
		RPL_CREATED = 003,
		RPL_MYINFO = 004,

		RPL_UMODEIS = 221,
		RPL_AWAY = 301,
		RPL_WHOISREGNICK = 307,
		RPL_WHOISUSER = 311,
		RPL_WHOISOPERATOR = 313,
		RPL_ENDOFWHOIS = 318,
		RPL_CHANNELMODEIS = 324,
		RPL_TOPIC = 332,
		RPL_WHOISMODES = 379,
		RPL_YOUROPER = 381,

		ERR_NOSUCHNICK = 401,
		ERR_NOSUCHANNEL = 403,
		ERR_CANTSENDTOCHAN = 404,
		ERR_NORECIPIENT = 411,
		ERR_NOEXTTOSEND = 412,
		ERR_NONICKNAMEGIVEN = 431,
		ERR_ERRORUSENICKNAME = 432,
		ERR_NICKNAMEINUSE = 433,
		ERR_NEEDMOREPARAMS = 461,
		ERR_ALREADYREGISTERED = 462,
		ERR_PASSWDMISMATCH = 464,
		ERR_UNKNOWNMODE = 472,
		ERR_NOPRIVILEGES = 481,
		ERR_UMODEUNKNOWNFLAG = 501,
		ERR_USERDONTMATCH = 502
	}
	
};

#endif // !SERVER_HPP
