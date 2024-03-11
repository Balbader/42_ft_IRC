#include "../headers/Channel.hpp"

// Constructor
Channel::Channel() : _prefix(), _creator(), _onlineUsers(), _name(), _key(), _topic(), _members(), _operators(), _voice(), _banned() {}

// Copy Constructor
Channel::Channel( const Channel& x ) { *this = x; }

// Constructor overload
Channel::Channel(std::string channelName, Client *Creator) : _prefix(), _creator(Creator), _onlineUsers(1), _name(channelName), _key(), _topic(), _members(), _operators(), _voice(), _banned() {
	this->_operators.insert(std::pair<int, Client *>(Creator->getClientfd(), Creator));
}

Channel::Channel(std::string channelName, std::string channelKey, Client *Creator) : _prefix(), _creator(Creator), _onlineUsers(1), _name(channelName), _key(channelKey), _topic(), _members(), _operators(), _voice(), _banned() {
	this->_operators.insert(std::pair<int, Client *>(Creator->getClientfd(), Creator));
}

// Destructor
Channel::~Channel() {}

// Assignment operator
Channel& Channel::operator=( const Channel& rhs ) {
	if (this == &rhs)
		return (*this);

	this->_prefix = rhs._prefix;
	this->_onlineUsers = rhs._onlineUsers;
	this->_name = rhs._name;
	this->_members.insert(rhs._members.begin(), rhs._members.end());
	this->_operators.insert(rhs._operators.begin(), rhs._operators.end());
	this->_voice.insert(rhs._voice.begin(), rhs._voice.end());

	return (*this);
}

// Getters
char const &Channel::getPrefix() const { return this->_prefix; }
int	const &Channel::getOnlineUsers() const { return this->_onlineUsers; }
std::string	const &Channel::getName() const { return this->_name; }
std::string	const &Channel::getKey() const { return this->_key; }
std::string	const &Channel::getTopic() const { return this->_topic; }
std::map<int, Client *>	const &Channel::getMembers() const { return this->_members; }
std::map<int, Client *>	const &Channel::getOperators() const { return this->_operators; }
std::map<int, Client *>	const &Channel::getVoice() const { return this->_voice; }

Client*	Channel::getCreator() const { return (this->_creator); }

// Setters
void Channel::setPrefix(char prefix) { this->_prefix = prefix; }
void Channel::setOnlineUsers(int online) { this->_onlineUsers = online; }
void Channel::setName(std::string name) { this->_name = name; }
void Channel::setKey(std::string key) { this->_key = key; }
void Channel::setTopic(std::string topic) { this->_topic = topic; }

// Methods
int	Channel::addMember(Client *member) {

	// check if client nickname is in the _banned list
	if (std::find(this->_banned.begin(), this->_banned.end(), member->getNickName()) != this->_banned.end())
		return (BANNEDFROMCHAN);

	// check if client is already a member
	if (this->_members.find(member->getClientfd()) == this->_members.end()) {
		this->_members.insert(std::pair<int, Client *>(member->getClientfd(), member));
		this->_onlineUsers++;

		return (USERISJOINED);
	}

	return (-1);
}

int	Channel::addOperator(Client *member) {

	// check if client nickname is in the _banned list
	if (std::find(this->_banned.begin(), this->_banned.end(), member->getNickName()) != this->_banned.end())
		return (BANNEDFROMCHAN);

	// check if operator has already joined
	if (this->_operators.find(member->getClientfd()) == this->_operators.end()) {

		this->_operators.insert(std::pair<int, Client *>(member->getClientfd(), member));
		this->_onlineUsers++;

		return (USERISJOINED);
	}

	return (-1);
}

int	Channel::banUser(Client *member) {
	// check if client nickname is in _banned list
	if (std::find(this->_banned.begin(), this->_banned.end(), member->getNickName()) != this->_banned.end())
		return (BANNEDFROMCHAN);

	// if not -> add to _banned list
	this->_banned.push_back(member->getNickName());

	return (USERISBANNED);
}

void Channel::removeOperator(int i) {
	// erase operator at index i and decrement tot _onlineUsers
	this->_operators.erase(i);
	this->_onlineUsers--;
}

void Channel::removeVoice(int i) {
	// erase voice at index i and decrement tot _onlineUsers
	this->_voice.erase(i);
	this->_onlineUsers--;
}

void Channel::removeBanned(std::string NickName) {
	// remove nickname from banned list
	// NOTE: verify if != or == this->_banned.end()
	if (std::find(this->_banned.begin(), this->_banned.end(), NickName) != this->_banned.end())
		return ;

	this->_banned.erase(std::find(this->_banned.begin(), this->_banned.end(), NickName));
}

void Channel::removeMember(int i) {
	// delete membet at index i
	this->_members.erase(i);
	this->_onlineUsers--;
}

// compile and return a list of all users in a channel, including members, operators, and voice users.
std::map<int, Client*> Channel::getAllUsers() const {

	// instanciate a map allUsers with the content of _members map with the fd as key and the pointer to the client* as value
	std::map<int, Client*> allUsers(this->_members.begin(), this->_members.end());

	// insert the content of the _operators map
	allUsers.insert(this->_operators.begin(), this->_operators.end());

	// insert the content of the _voice map
	allUsers.insert(this->_voice.begin(), this->_voice.end());

	return (allUsers);
}

// Determine the role of a user within a channel based on the user's file descriptor.
// it checks three different maps (_members, _operators, _voice)
// parameter int i = fd of the client whose role is being queired
std::pair<Client*, int> Channel::findUserRole( int i ) {

	// instanciate iterator and search for file descriptor i in _members
	std::map<int, Client*>::iterator it = this->_members.find(i);
	// if found, returns a pair where first element is a pointer to the client and the second (0)
	// is an integer indicating the user's role
	if (it != this->_members.end())
		return (std::pair<Client*, int>(it->second, 0));

	it = this->_operators.find(i);
	if (it != this->_operators.end())
		return (std::pair<Client *, int>(it->second, 1));

	it = this->_voice.find(i);
	if (it != this->_voice.end())
		return (std::pair<Client *, int>(it->second, 2));

	// if not found
	return (std::pair<Client *, int>(NULL, -1));
}

// Compiles a single string  that lists all users in a channel distinguishing
// between operators, regular members and voice users.
std::string	Channel::listAllUsers() const {

	std::string	AllUsers(":");

	std::map<int, Client *>::const_iterator it = this->_operators.begin();
	while (it != this->_operators.end()) {
		// iterate over _operators map
		// "@" for operators
		AllUsers.append("@" + it->second->getNickName() + " ");
		it++;
	}

	// no prefix for members
	it = this->_members.begin();
	while (it != this->_members.end()) {
		AllUsers.append(it->second->getNickName() + " ");
		it++;
	}

	it = this->_voice.begin();
	while (it != this->_voice.end()) {
		// "+" for voice status
		AllUsers.append("+" + it->second->getNickName() + " ");
		it++;
	}

	return (AllUsers);
}
