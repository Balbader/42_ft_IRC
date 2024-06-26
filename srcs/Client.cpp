#include <Client.hpp>

// constructors
Client::Client() {
	this->fd = 0;
	this->registration = 0;
	this->flags = CLI_INV | CLI_WALLOP | CLI_NOTICE;
	this->knowPassword = false;
	this->welcome = false;
	this->toDisconnect = false;
}

Client::Client(int fd, std::string hostname) {
	this->fd = fd;
	this->registration = 0;
	this->flags = CLI_INV | CLI_WALLOP | CLI_NOTICE;
	this->knowPassword = false;
	this->welcome = false;
	this->toDisconnect = false;
	this->hostname = hostname;
}

// destructor
Client::~Client() {
	Logger("Client").info("~Client", "client destructor called");
	std::size_t i = 0;
	while (i < channels.size()) {
		channels.at(i)->removeClient((*this));
		i++;
	}
	Logger("Client").info("~Cliente", "client destructor finished");
}


// getters
int	Client::getFd() const { return fd; }
int	Client::getRegistration() const { return registration; }
bool Client::getWelcome() const { return welcome; }
bool Client::getToDisconnect() const { return toDisconnect; }
bool Client::getOp() const { return flags & CLI_OPER; }
std::string Client::getNickname() const { return nickname; }
std::string Client::getUsername() const { return username; }
std::string Client::getHostname() const { return hostname; }
std::string Client::getSendData() const { return sendData; }
std::string Client::getServername() const { return servername; }
std::string Client::getRealname() const { return realname; }
std::vector<Channel *> &Client::getChannels() { return channels; }

std::string Client::getClientPrefix() {
	std::stringstream ss;

	ss << ":" << getNickname();
	ss << "!" << getUsername();
	ss << "@" << getHostname();
	return (ss.str());
};

std::string Client::getModesStr() const {
	std::string modes = "";

	if (CLI_OPER & flags) {
		modes.append(1, 'o');
	}
	if (CLI_INV & flags) {
		modes.append(1, 'i');
	}
	if (CLI_WALLOP & flags) {
		modes.append(1, 'w');
	}
	if (CLI_NOTICE & flags) {
		modes.append(1, 's');
	}
	if (modes.size() > 0) modes.insert(0, 1, '+');
	return modes;
};


// setters
void Client::setWelcome(bool value) { welcome = value; }
void Client::setKnowPassword(bool value) { knowPassword = value; }
void Client::setRegistration(int flag) { registration |= flag; }
void Client::setUsername(std::string name) { username = name; }
void Client::setHostname(std::string name) { hostname = name; }
void Client::setRealname(std::string name) { realname = name; }
void Client::setServername(std::string name) { servername = name; }
void Client::setSendData(std::string data) { sendData += data; }
void Client::setToDisconnect(bool value) { toDisconnect = value; }

void Client::setNickname(std::string name) {
	if (name.length() > 9) {
		nickname = name.substr(0, 9);
	} else {
		nickname = name;
	}
}

void Client::setOp(bool value) {
	if (value)
		flags |= CLI_OPER;
	else
		flags = flags & ~CLI_OPER;
}

bool Client::setOperator(bool on) {
	unsigned int before = flags;

	if (on) {
		flags |= CLI_OPER;
	} else {
		flags = flags & ~CLI_OPER;
	}
	if (flags == before) {
		return false;
	}
	return true;
};

bool Client::setInvisible(bool on) {
	unsigned int before = flags;

	if (on) {
		flags |= CLI_INV;
	} else {
		flags = flags & ~CLI_INV;
	}
	if (flags == before) {
		return false;
	}
	return true;
};

bool Client::setNotice(bool on) {
	unsigned int before = flags;

	if (on) {
		flags |= CLI_NOTICE;
	} else {
		flags = flags & ~CLI_NOTICE;
	}
	if (flags == before) {
		return false;
	}
	return true;
};

bool Client::setWallop(bool on) {
	unsigned int before = flags;

	if (on) {
		flags |= CLI_WALLOP;
	} else {
		flags = flags & ~CLI_WALLOP;
	}
	if (flags == before) {
		return false;
	}
	return true;
};

bool Client::setMode(char mode, bool on) {
	bool changed = false;
	switch (mode) {
		case 'i':
			changed = this->setInvisible(on);
			break;
		case 'w':
			changed = this->setWallop(on);
			break;
		case 's':
			changed = this->setNotice(on);
			break;
		case 'o':
			changed = this->setOperator(on);
			break;
		default:
			break;
	}
	return changed;
};


// methods
void Client::resetSendData(int len) { sendData = sendData.substr(len); }

void Client::removeChannel(Channel &ch) {
	channels.erase(find(channels.begin(), channels.end(), &ch));
}

void Client::addChannel(Channel &ch) { channels.push_back(&ch); };

bool Client::isVisible() const { return !(this->flags & CLI_INV); };
