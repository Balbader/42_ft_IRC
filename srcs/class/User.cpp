#include <unistd.h>
#include "User.hpp"
#include "Server.hpp"

// --------------------------------------------------------> Private Attributes
// The available modes are:
// - a: away
// - i: invisible
// - o: operator
std::string const	User::_availableModes("aio");

std::string const	User::_availableNicknameChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");


// ---------------------------------------------------------------> Constructor

User::User(sockaddr_in const &addr, int sockfd) :
	_addr(addr),
	_socket(sockfd),
	_nickname("*"),
	_username(),
	_servname(),
	_realname(),
	_password(),
	_awayMsg(),
	_modes(),
	_msg(),
	_isRegistered(),
	_waitingForPong(ALIVETIME),
	_lookupChannels()
{
	if (DEBUG)
		std::cout << LBLUE << "User base constructor called" << RESET << std::endl;

	time(&_lastActivity);
}

User::User(User const &src) :
	_addr(src._addr),
	_socket(src._socket),
	_nickname(src._nickname),
	_username(src._username),
	_servname(src._servname),
	_realname(src._realname),
	_password(src._password),
	_awayMsg(src._awayMsg),
	_modes(src._modes),
	_msg(src._msg),
	_isRegistered(src._isRegistered),
	_waitingForPong(src._waitingForPong),
	_lastActivity(src._lastActivity),
	_lookupChannels(src._lookupChannels)
{
	if (DEBUG)
		std::cout << LBLUE << "User copy constructor called" << RESET << std::endl;
}

// ----------------------------------------------------------------> Destructor
User::~User(void) {
	if (DEBUG)
		std::cout << LBLUE << "User destructor called" << RESET << std::endl;

	if (this->_socket != -1)
		close(this->_socket);

	this->_socket = -1;
}

// -------------------------------------------------------------------> Methods
// Add a new channel in which the user is.
void	User::addChannel(Channel &channel) {
	this->_lookupChannels.insert(std::pair<std::string const, Channel *>(channel.getName(), &channel));
}

// Remove a channel in which the user is.
void	User::delChannel(std::string const &channelName) {
	this->_lookupChannels.erase(channelName);
}

// init socket and address of channel in which the user is.
bool	User::init(int const &socket, sockaddr_in const &addr) {
	this->_socket = socket;
	this->_addr = addr;
	return true;
}

void	User::updateLastActivity(void) {
	time(&this->_lastActivity);
}

// -------------------------------------------------------------------> Getters
int const& User::getSocket(void) const { return this->_socket; }

bool const& User::getWaitingForPong(void) const { return this->_waitingForPong; }
bool const& User::getIsRegistered(void) const { return this->_isRegistered; }

sockaddr_in const& User::getAddr(void) const { return this->_addr; }
time_t const& User::getLastActivity(void) const { return this->_lastActivity; }

std::map<std::string const, Channel *const> const& User::getLookupChannels(void) const { return this->_lookupChannels; }

std::string const& User::getAvailableModes(void) { return User::_availableModes; }
std::string const& User::getAwayMsg(void) const { return this->_awayMsg; }
std::string const& User::getAvailableNicknameChars(void) { return User::_availableNicknameChars; }
std::string const& User::getHostname(void) const { return this->_hostname; }
std::string const& User::getMask(void) const { return this->_mask; }
std::string const& User::getModes(void) const { return this->_modes; }
std::string const& User::getMsg(void) const { return this->_msg; }
std::string const& User::getNickname(void) const { return this->_nickname; }
std::string const& User::getPassword(void) const { return this->_password; }
std::string const& User::getRealname(void) const { return this->_realname; }
std::string const& User::getServname(void) const { return this->_servname; }
std::string const& User::getUsername(void) const { return this->_username; }


// -------------------------------------------------------------------> Setters
void User::setAddr(sockaddr_in const &addr) { this->_addr = addr; }
void User::setAwayMsg(std::string const &awayMsg) { this->_awayMsg = awayMsg; }
void User::setHostname(std::string const &hostname) { this->_hostname = hostname; }
void User::setIsRegistered(bool const isRegistered) { this->_isRegistered = isRegistered; }
void User::setMask(std::string const &mask) { this->_mask = mask; }
void User::setMask(void) { this->_mask = this->_nickname + '!' + this->_username + '@' + this->_servname; }
void User::setMsg(std::string const &msg) { this->_msg = msg; }
void User::setModes(std::string const &modes) { this->_modes = modes; }
void User::setNickname(std::string const &nickname) { this->_nickname = nickname; }
void User::setPassword(std::string const &password) { this->_password = password; }
void User::setRealname(std::string const &realname) { this->_realname = realname; }
void User::setServname(std::string const &servname) { this->_servname = servname; }
void User::setSocket(int const sockfd) { this->_socket = sockfd; }
void User::setUsername(std::string const &username) { this->_username = username; }
void User::setWaitingForPong(bool const waitingForPong) { this->_waitingForPong = waitingForPong; }
