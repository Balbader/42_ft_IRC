#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <csignal>

#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include <Client.hpp>
#include <Channel.hpp>
#include <Logger.hpp>
#include <utils.hpp>

#include <fcntl.h>

#define LOSTCONNECTION 0
#define QUITED 1
#define KICKED 2
#define MAX_CLIENTS 5
#define FORBIDDEN_CHARS " ,*?!@$.#&:\r\n\0\a+"

class Server {
private:
	Logger LOGGER;
	static bool	online;
	std::string	creationDatetime;
	std::string	passwd;
	int	port;
	int	socket;
	std::map<int, Client> clients;
	std::map<std::string, Channel> channels;
	std::vector<pollfd> pollFds;
	struct sockaddr_in address;
  bool maxClientsFlag;

	bool isPortInvalid(int port);
	bool isPasswdInvalid(std::string passwd);
	void setupSocket();
	
	/* server input/output */
	std::string receiveDataFromSocket(int fd);
	void sendDataThroughSocket(Client &client);

	/* server event handler */
	bool handleServerEvents();
	void handleClientEvents();
	void handleDisconnectionEvents();
	void handleEmptyChannelEvents();

	/* server connection manager */
	bool addNewClient();
	void unexpectedDisconnectHandling(int fd);
	void ejectClient(int clientFd, int reason);
	void removeChannel(std::string name);

	/* server command executor */
	void executeCommands(Client &client, std::vector<Command> &commands);
	void executeCommand(Client &client, Command &command);

	/* server channel manager */
	std::map<std::string, Channel>::iterator getChannelByName(std::string channelName);
	void removeClientFromChannel(Client &client, Channel &channel, std::string message);
	void broadcastMessage(int fd, std::string message);

	/* commands */
	void pass(Client &client, Command &command);
	void user(Client &client, Command &command);
	void nick(Client &client, Command &command);
	void quit(Client &client, Command &command);
	void oper(Client &client, Command &command);
	void ping(Client &client, Command &command);
	void join(Client &client, Command &command);
	void who(Client &client, Command &command);
	void whois(Client &client, Command &command);
	void mode(Client &client, Command &command);
	void privmsg(Client &client, Command &command);
	void topic(Client &client, Command &command);
	void kick(Client &client, Command &command);
	void invite(Client &client, Command &command);
	void notice(Client &client, Command &command);
	void part(Client &client, Command &command);
	void channelMode(Client &client, Command &command);
	void userMode(Client &client, Command &command);
	void successfulJoin(Client &client, Channel &channel);
	bool validNickname(std::string nickname);
	bool nicknameAlreadyExists(std::string nickname);
	bool validChannelName(std::string name);

	/* replies */
	std::string motd(Client &client);
	std::string welcome(Client &client);
	std::string changednickname(Client &client, std::string nickname); 
	std::string needmoreparams(Client &client, std::string command);
	std::string alreadyregistered(Client &client);
	std::string passwdmismatch(Client &client);
	std::string nonicknamegiven(Client &client);
	std::string erroneusnickname(Client &client, std::string nickname);
	std::string nicknameinuse(Client &client, std::string nickname);
	std::string youreoper(Client &client);
	std::string nooperhost(Client &client);
	std::string unknowncommand(Client &client, std::string command);
	std::string nosuchchannel(Client &client, std::string name);
	std::string topic(Client &client, Channel &channel);
	std::string notopic(Client &client, Channel &channel);
	std::string namreply(Client &client, Channel &channel, bool found);
	std::string whoreply(Client &client, Channel &channel);
	std::string whoisreply(Client &client);
	std::string nosuchserver(Client &client, std::string name);
	std::string unknownmode(Client &client, char c);
	std::string badchannelkey(Client &client, std::string channel);
	std::string usersdontmatch(Client &client);
	std::string unknownmodeflag(Client &client);
	std::string notonchannel(Client &client, std::string name);
	std::string norecipient(Client &client, std::string name);
	std::string	notexttosend(Client &client);
	std::string nosuchnick(Client &client, std::string name);
	std::string chanoprivsneeded(int fd, Channel &channel);
	std::string chanoprivsneeded(Client &client, Channel &channel);
	std::string inviting(Client &issuer, Channel &channel);
	std::string inviterrpl(Client &issuer, Client &target, Channel &channel);
	std::string inviteonlychan(Client &issuer, Channel &channel);
	std::string channelisfull(Client &issuer, Channel &channel);
	std::string usernotinchannel(Client &client, Channel &channel);
	std::string kicksuccess(Client &client, Channel &channel, std::string target, std::string reason);
	std::string channelmodeis(Client &client, std::string channel);
	std::string usermodeis(Channel &channel, Client &client, std::string modeStr);
	std::string usermodeis(Client &client, std::string modeStr);
	std::string usermodeis(Client &client);
	friend class ServerTestAPI;
	
	/* server utils */
	std::string	getDatetime(void);
	std::vector<Command> parseCommands(std::string data);
	Command	messageToCommand(std::string source);
	void trimStart(std::string &str);
	void trimAll(std::string &str);
	void cleanEndOfTransmission(std::string &str);

public:
	~Server(void);
	Server(std::string const &passwd, int const &port);
	
	void startServer();
	void static	stop();

	bool evalChanMode(Client &client, std::vector<std::string> args);
	bool evalUserMode(Client &client, std::vector<std::string> args);
	Client *getClientByNick(std::string nickname);
};

void 												signalHandler(int signal);
