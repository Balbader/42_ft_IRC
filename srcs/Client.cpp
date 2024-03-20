#include "../headers/Client.hpp"

// ===============================================================> Constructor
Client::Client(): _clientfd(0), _Auth(false), _Registered(false), _isOperator(false), _NickName(), _UserName(), _FullName(), _Host("le_basilou"), _ID(), _remotaddr(), _addrlen(), _modes(), _joinedChannels() {}
Client::Client( int fd ): _clientfd(fd), _Auth(false), _Registered(false), _isOperator(false), _NickName(), _UserName(), _FullName(), _Host("le_basilou"), _ID(), _remotaddr(), _addrlen(), _modes(), _joinedChannels() {}


// ==========================================================> Copy Constructor
Client::Client( const Client& x ): _Host(x._Host) { *this = x; }


// =======================================================> Assignment operator
Client & Client::operator=( const Client& rhs ) {
	if (this == &rhs)
		return (*this);

	this->_clientfd = rhs._clientfd;
	this->_Registered = rhs._Registered;
	this->_isOperator = rhs._isOperator;
	this->_NickName = rhs._NickName;
	this->_UserName = rhs._UserName;
	this->_FullName = rhs._FullName;
	this->_Auth = rhs._Auth;
	this->_ID = rhs._ID;
	this->_Auth = rhs._Auth;
	this->_remotaddr = rhs._remotaddr;
	this->_addrlen = rhs._addrlen;
	this->_joinedChannels.insert(rhs._joinedChannels.begin(), rhs._joinedChannels.end());

	return (*this);
}


// ================================================================> Destructor
Client::~Client() {};


// ===================================================================> Getters
std::string	Client::getUserName() const { return (this->_UserName); }
std::string	Client::getNickName() const { return (this->_NickName); }
std::string	Client::getFullName() const { return (this->_FullName); }
std::string Client::getHost() const { return (this->_Host); }
std::string Client::getID()	const { return (this->_ID); }
bool Client::getAuth() const { return (this->_Auth); }
int	Client::getClientfd() const { return (this->_clientfd); }
int	Client::getRegistered() const { return (this->_Registered); }
int	Client::getisOperator() const { return (this->_isOperator); }


// FIX: need to check the mode as theses are for users and not for chanels
int	Client::getMode(char mode) const {
	if (mode == 'a')
		return this->_modes.away;
	else if (mode == 'i')
		return this->_modes.invisible;
	else if (mode == 'w')
		return this->_modes.wallops;
	else if (mode == 'r')
		return this->_modes.restricted;
	else if (mode == 'o')
		return this->_modes.op;
	else if (mode == 'O')
		return this->_modes.localOp;
	else if (mode == 's')
		return this->_modes.server;
	return (0);
}


// ===================================================================> Setters
void Client::setUserName(std::string UserName) { this->_UserName = UserName; }
void Client::setNickName(std::string NickName) { this->_NickName = NickName; }
void Client::setFullName(std::string FullName) { this->_FullName = FullName; }
void Client::setID(std::string ID) { this->_ID = ID; }
void Client::setClientfd(int Clientfd) { this->_clientfd = Clientfd; }
void Client::setRegistered(int Registered) { this->_Registered = Registered; }
void Client::setAuth(int Auth) { this->_Auth = Auth; }

void Client::setIsOperator(int isOperator) {
	this->_isOperator = isOperator;
	this->_modes.op = isOperator;
	this->_modes.localOp = isOperator;
}

void Client::setMode(int value, char mode) {
	if (mode == 'i')
		this->_modes.invisible = value;
	else if (mode == 'w')
		this->_modes.wallops = value;
	else if (mode == 'r' && value == 1)
		this->_modes.restricted = value;
	else if (mode == 'o' && value == 0)
		this->_modes.op = value;
	else if (mode == 'O' && value == 0)
		this->_modes.localOp = value;
	else if (mode == 's')
		this->_modes.server = value;
}


// ===================================================================> Methods

// check if the client is currently joined to a channel with a specific name
int Client::isJoined(std::string ChannelName) const {
	if (this->_joinedChannels.find(ChannelName) != this->_joinedChannels.end())
		return (1);

	return (0);
}

// allow the client to join a specific channel, assuming the client is not already a member of the channel
void Client::joinChannel(std::string ChannelName, Channel* channel) {
	if (!isJoined(ChannelName))
		this->_joinedChannels.insert(std::pair<std::string, Channel*>(ChannelName, channel));
}

// return a string of all the channels the client has joined
std::string Client::JoinedChannels() const {

	std::string	channels;

	for (std::map<std::string, Channel*>::const_iterator it = this->_joinedChannels.begin(); it != _joinedChannels.end(); ++it) {
		channels.append(BLUE + it->first + RESET + ":\n");
		channels.append(YELLOW "\tChannel Name: " RESET + it->first + "\n");
		channels.append(YELLOW "\tChannel Name inside class: " RESET + it->second->getName() + "\n");
		channels.append(YELLOW "\tChannel Creator: " RESET + it->second->getCreator()->getFullName() + "\n");
	}

	return (channels);
}

// delete channel name from _joinedChannels list
void Client::leaveChannel( std::string ChannelName ) {
	this->_joinedChannels.erase(ChannelName);
}

// remove client from all the channels ther are currently joined in
std::string	Client::leaveAllChannels() {

	for (std::map<std::string, Channel*>::iterator it = this->_joinedChannels.begin(); it != this->_joinedChannels.end(); ++it) {

		// find the client role and remove client from the channel based on that role
		std::pair<Client *, int> user(it->second->findUserRole(this->_clientfd));

		if (user.second == 0)
			it->second->removeMember(this->_clientfd);
		else if (user.second == 1)
			it->second->removeOperator(this->_clientfd);
		else
			it->second->removeVoice(this->_clientfd);

		// notify client they left the channel
		user.first->leaveChannel(it->second->getName());

		// reassigning it to first element
		it = this->_joinedChannels.begin();
	}
	return ("");
}

std::string	Client::getUserInfo() const {

	std::string	userInfo;

    std::stringstream ss;
    ss << this->_joinedChannels.size();
    std::string tmp = ss.str();

	userInfo.append("User Name: " + this->_UserName + "\n");
	userInfo.append("Full Name: " + this->_FullName + "\n");
	userInfo.append("Nick Name: " + this->_NickName + "\n");
	userInfo.append("Host: " + this->_Host + "\n");
	userInfo.append("Joined Channels: " + tmp + "\n");
	userInfo.append("\n");

	return (userInfo);
}

// Create a string representation of a table listing details of all the channels the client has joined.
// The table includes:
// the channel name,
// the number of online users,
// the creator of the channel,
// and the channel's topic
std::string	Client::getAllChannels() const {
	std::string channels(RESET " Channel Name " YELLOW "█ " RESET "Online Users " YELLOW "█ " RESET "Creator Of Channel " YELLOW "█ " RESET "          Channel Topic          " YELLOW "█\n");
	std::map<std::string, Channel*>::const_iterator it = this->_joinedChannels.begin();
    std::stringstream ss;
    ss << it->second->getOnlineUsers();
    std::string size = ss.str();

	while (it != this->_joinedChannels.end()) {
		channels.append(RESET + fillIt(it->first, 12));
		channels.append(RESET + fillIt(size, 7));
		channels.append(RESET + fillIt(it->second->getCreator()->getFullName(), 18));
		channels.append(RESET + fillIt(it->second->getTopic(), 32));
		channels.append("\n");
		it++;
	}

	if (this->_joinedChannels.size() == 0) {
		channels.append( RESET "YOU JOINED NO CHANNEL\n");
	}

	channels.append(RESET "\n\n");
	return (channels);
}

std::string	Client::getUserPerfix() const { return (":" + this->_NickName + "!" + this->_UserName + "@" + this->_Host + " "); }

std::map<std::string, Channel*>	Client::getJoinedChannels() const { return (this->_joinedChannels); }
