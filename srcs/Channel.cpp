#include <Channel.hpp>

// constructor
Channel::Channel(void) {
	host		= "localhost";
	creator		= NULL;
	userLimit	= USER_CHANNEL_LIMIT;
	initialized = false;
	LOGGER		= Logger("Channel");
}

// destructor
Channel::~Channel(void) {
	LOGGER.info("~Channel", "Deleting channel " + name);
    clients.clear();
    invited.clear();
}


// getters
std::string Channel::getName() const { return name; };

std::string Channel::getTopic() const { return topic; }

Client &Channel::getCreator() { return *creator; };

std::set<char> &Channel::getMode() { return modes; }

unsigned int Channel::getUserLimit() const { return userLimit; };

std::map<Client *, unsigned int> &Channel::getClients() { return clients; }

std::string Channel::getStrModes() {
	std::string modeStr = "";

	if (modes.size() > 0) {
		modeStr += "+";
		std::set<char>::iterator it = modes.begin();
		while (it != modes.end()) {
			modeStr += (*it);
			it++;
		}
		if (modeStr.find('k') != std::string::npos) {
			modeStr.append(" ");
			modeStr.append(this->password);
		}
	}
	return (modeStr);
};

std::map<Client *, unsigned int>::iterator Channel::getClientByNick(std::string clientNickname) {
	LOGGER.info("getClientByNick", "Looking for client " + clientNickname);
	std::map<Client *, unsigned int>::iterator it = clients.begin();
	while (it != clients.end()) {
		if (it->first->getNickname() == clientNickname) return (it);
		it++;
	}
	return (it);
};


// setters
void Channel::setTopic(std::string topic) { this->topic = topic; };
void Channel::setPassword(std::string password) { this->password = password; }
void Channel::setUserLimit(unsigned int limit) { userLimit = limit; };

bool Channel::setOperator(std::string clientNickname, bool newValue) {
	std::ostringstream logMessage;

	logMessage << "Setting operator status of " << clientNickname << " to " << newValue;
	LOGGER.info("setOperator", logMessage.str());

	std::map<Client *, unsigned int>::iterator it = getClientByNick(clientNickname);
	if (it != clients.end()) {
		if (newValue)
			it->second |= USER_OPERATOR;
		else
			it->second &= (~USER_OPERATOR);
		return true;
	}
	return (false);
};

void Channel::setSpeaker(std::string clientNickname, bool newValue) {
	std::ostringstream logMessage;

	logMessage << "Setting speaker status of " << clientNickname << " to " << newValue ;
	LOGGER.info("setSpeaker", logMessage.str());

	std::map<Client *, unsigned int>::iterator it = getClientByNick(clientNickname);
	if (it != clients.end()) {
		if (newValue)
			it->second |= USER_SPEAKER;
		else
			it->second &= (~USER_SPEAKER);
	}
};


// methods
bool Channel::isInitialized() { return initialized; }

void Channel::addClient(Client &client) { clients.insert(std::make_pair(&client, 0)); }

void Channel::removePassword() { this->password = ""; };

void Channel::removeClient(Client &client) {
	LOGGER.info("removeClient", "removing client " + client.getNickname());
	if (creator == &client) {
		LOGGER.info("removeClient", "client is creator");
		creator = NULL;
	}
	if (clients.size() > 0) {
		clients.erase(&client);
		asureOperator();
	}
}

void Channel::broadcast(Client &sender, std::string message, bool toSend) {
	std::map<Client *, unsigned int>::iterator it = clients.begin();

	LOGGER.info("broadcast", "Broadcasting message to all clients in channel " + name);
	for (; it != clients.end(); it++) {
		if (it->first == &sender && toSend) {
			(*it->first).setSendData(message);
		} else if (it->first != &sender) {
			(*it->first).setSendData(message);
		}
	}
}

bool Channel::toggleMode(char mode, bool on) {
	std::size_t before = modes.size();
	std::ostringstream logMessage;

	logMessage << "Toggling mode " << mode << " to " <<on;
	LOGGER.info("toggleMode", logMessage.str());
	if (on)
		modes.insert(mode);
	else
		modes.erase(mode);
	return !(before == modes.size());
}

void Channel::initialize(std::string name, std::string password, Client &op) {
	LOGGER.info("initialize", "Initializing channel " + name);
	this->name	   = name;
	this->password = password;
	this->clients.insert(std::make_pair(&op, USER_OPERATOR));
	this->initialized = true;
	this->modes.insert('t');
	this->creator = &op;
}

void Channel::initialize(std::string name, Client &op) {
	LOGGER.info("initialize", "Initializing channel " + name);
	this->name = name;
	this->clients.insert(std::make_pair(&op, USER_OPERATOR));
	this->initialized = true;
	this->modes.insert('t');
	this->creator = &op;
}

bool Channel::evalPassword(std::string psw) { return (password == psw); }

bool Channel::isInvited(std::string nickname) {
	if (invited.find(toIrcUpperCase(nickname)) != invited.end()) {
		return true;
	}
	return false;
};

void Channel::addInvite(std::string nickname) {
	invited.insert(toIrcUpperCase(nickname));
};

void Channel::removeInvited(std::string nickname) {
	invited.erase(toIrcUpperCase(nickname));
};

void Channel::asureOperator() {
	LOGGER.info("asureOperator", "Checking the need for a new oper");
	std::map<Client *, unsigned int>::iterator it;
	it = clients.begin();
	while (it != clients.end()) {
		if (it->second & USER_OPERATOR)
			return;
		it++;
	}
	it = clients.begin();
	if (it != clients.end()) {
		this->setOperator(it->first->getNickname(), true);
		LOGGER.info("asureOperator", "Setting " + it->first->getNickname() + " as a new operator");
		std::stringstream ss;
		ss << ":" << it->first->getNickname();
		ss << " MODE";
		ss << " " << this->getName();
		ss << " " << "+o " << it->first->getNickname();
		ss << "\r\n";
		this->broadcast(*it->first, ss.str(), true);
	}
};
