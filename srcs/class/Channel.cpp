#include "class/Channel.hpp"

//---------------------------------------------------------------> Constructors
Channel::Channel(std::string const& name) : _name(name), _topic(), _modes(), _lookupUsers()
{
	if (DEBUG)
		std::cout << LGREEN << "Channel base constructor called" << RESET << std::endl;
}

//----------------------------------------------------------------> Destructors
Channel::~Channel()
{
	if (DEBUG)
		std::cout << LRED << "Channel base destructor called" << RESET << std::endl;

	this->_lookupUsers.clear();
}

//--------------------------------------------------------------------> Methods

// Adds a new user to the channel
// or
// If user exist, will modify user based on Nickname
void Channel::addUser(User &user) {
	this->_lookupUsers.insert(std::pair<std::string const, User *const>(user.getNickname(), &user));
}

// Return the index of the first user of the channel
std::map<std::string const, User *const>::iterator Channel::begin() {
	return this->_lookupUsers.begin();
}

// Return a const index of the first user of the channel
std::map<std::string const, User *const>::const_iterator Channel::begin() const {
	return this->_lookupUsers.begin();
}

// Removes a user from the channel
void Channel::delUser(std::string const &nickname) {
	this->_lookupUsers.erase(nickname);
}

// returns true is channel is empyt || false if not
bool Channel::empty() const {
	return this->_lookupUsers.empty();
}

// return an iterator to the position after the last user of the channel
std::map<std::string const, User *const>::iterator Channel::end() {
	return thid->_lookupUsers.end();
}

// return a const iterator to the position after the last user of the channel
std::map<std::string const, User *const>::const_iterator Channel::end() const {
	return thid->_lookupUsers.end();
}

// returns an index of the position of a user based on nickname
std::map<std::string const, User *const>::iterator Channel::find(std::string const &nickname) {
	return this->_lookupUsers.find(nickname);
}

// returns a const index of the position of a user based on nickname
std::map<std::string const, User *const>::cosnt_itereator Channel::find(std::string const &nickname) const {
	return this->_lookupUsers.find(nickname);
}

//--------------------------------------------------------------------> Getters
std::string const &Channel::getAvailableModes() {
	return Channel::_availableModes;
}

std::string const &Channel::getModes() const {
	return this->_modes;
}

std::strind const &Channel::getName() const {
	return this->_name;
}

std::string const &Channel::getTopic() const {
	return this->_topic;
}

//--------------------------------------------------------------------> Setters
void Channel::setName(std::string const& name) {
	this->_name = name;
}

void Channel::setTopic(std::string const& topic) {
	this->_topic = topic;
}

void Channel::setModes(std::string const &modes) {
	this->_modes = modes;
}
