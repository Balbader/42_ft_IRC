#include "../headers/Server.hpp"

std::string	Server::_parsing(std::string message, int i) {

	// Initiate a 'request' object which parses the input message
	Request	request(_splitRequest(message));

	if (request.invalidMessage)
		return ("Invalid message!\n");

	if (request.command == "PASS")
		return (_setPassWord(request, i)); // sets the password for a session or connection
	else if (request.command == "NICK")
		return (_setNickName(request, i)); // sets or change client nickname
	else if (request.command == "USER")
		return (_setUserName(request, i)); // sets username
	else if (request.command == "OPER")
		return (_setOper(request, i)); // grant operator privileges to a user
	else if (request.command == "MODE")
		return (_setMode(request, i)); // set a mode for user or channel
	else if (request.command == "PRIVMSG")
		return (_privmsg(request, i)); // send private message to user or channel
	else if (request.command == "NOTICE")
		return (_notice(request, i)); // sends notice to a user or a channel
	else if (request.command == "HELP")
		return (_printHelpInfo()); // display help info
	else if (request.command == "JOIN")
		return (_joinChannel(request, i)); // Joins a channel
	else if (request.command == "TOPIC")
		return (_topic(request, i)); // Sets or gets the topic of a channel
	else if (request.command == "KICK")
		return (_kick(request, i)); // Removes a user from a channel
	else if (request.command == "PART")
		return (_part(request, i)); // Leaves a channel
	else if (request.command == "QUIT")
		return (_quit(request, i)); // Disconnects from the server
	else
		return ("Invalid command\n");
}

// Handles the NOTICE command used to send a message to a specific user or channel
// similar to PRIVMSG but should not generate any automatic reply
std::string	Server::_notice(Request request, int i) {

	// check if client is registered
	if (!this->_clients[i]->getRegistered())
		return (_printMessage("451", this->_clients[i]->getNickName(), ":You have not registered"));

	// check args count
	if (request.args.size() < 2)
		return (_printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters"));

	// if all good -> send message
	if (request.args.size() == 2)
		_privToUser(request.args[0], request.args[1], "NOTICE", i);

	// return empty string: indicates there is no immediate server message to be
	// sent back to the client who issued the NOTICE command.
	return ("");
}

// find the file descriptor associated with the client based on client nickname
int Server::_findFdByNickName(std::string NickName) {

	for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
		if (it->second->getNickName() == NickName)
			return (it->second->getClientfd());
	}

	return (USERNOTINCHANNEL);
}

// set or view the topic of a channel
std::string	Server::_topic(Request request, int i) {

	// check if client is registered
	if (!this->_clients[i]->getRegistered())
		return (_printMessage("451", this->_clients[i]->getNickName(), ":You have not registered"));

	// check args count
	if (request.args.size() == 0)
		return (_printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters"));

	if (request.args.size() == 1) { // 1 argument -> assumes the client wants to view the topic of the specific channel

		if (this->_allChannels.find(request.args[0])->second->getTopic().empty()) // if no topic is set
			return (_printMessage("331", this->_clients[i]->getNickName(), request.args[0] + " :No topic is set"));

		else // if a topic is set
			return (_printMessage("332", this->_clients[i]->getNickName(), request.args[0] + " :" + this->_allChannels.find(request.args[0])->second->getTopic()));
	}

	// if there are 2 arguments
	// find the channel in _allChannels using the name provided 'request.args[0]'
	std::map<std::string, Channel*>::iterator it = this->_allChannels.find(request.args[0]);

	if (it != this->_allChannels.end()) {
		// determine user's role
		std::pair<Client *, int> user = it->second->findUserRole(i);

		// if the user is an operator
		if (user.second == 1) {
			// topic is set to new value
			it->second->setTopic(request.args[1]);

			// 'TOPIC' message is broadcast to all users in the channel
			std::string reply = "TOPIC " + it->second->getName() + ":" + request.args[1] + "\n";
			_sendToAllUsers(it->second, i, reply);
		}
		else if (user.second == -1) // user not in channel
			return (_printMessage("442", this->_clients[i]->getNickName(), request.args[0] + " :You're not on that channel"));
		else // if user is not an operator
			return (_printMessage("482", this->_clients[i]->getNickName(), request.args[0] + " :You're not channel operator"));
	}

	// if none of the conditions trigger a return statement, return empty string
	// indicates there is no specific error or status message to return to the client
	return ("");
}

// Validate the mode string provided in client's request
bool Server::_validMode(Request request) {
	char	c = request.args[1][1];

	// check if lenth is 2 or starts with a '+' for 'enabling mode' or a '-' for 'desabling mode'
	if (request.args[1].length() != 2 || (request.args[1][0] != '-' && request.args[1][0] != '+'))
		return false;

	// check if c is valid mode
	if (c != 'a' && c != 'i' && c != 'w' && c != 'r' && c != 'o' && c != 'O' && c != 's')
		return false;

	return true;
}

// NOTE: refactored version of this function underneath

/*
std::string	Server::_printUserModes(std::string ret, int i) {

    std::stringstream ss_a;
    ss_a << this->_clients[i]->getMode('a');
    std::string tmp_a = ss_a.str();
	ret.append("a: " + tmp_a);

    std::stringstream ss_i;
    ss_i << this->_clients[i]->getMode('i');
    std::string tmp_i = ss_i.str();
	ret.append("\ni: " + tmp_i);

    std::stringstream ss_w;
    ss_w << this->_clients[i]->getMode('w');
    std::string tmp_w = ss_w.str();
	ret.append("\nw: " + tmp_w);

    std::stringstream ss_r;
    ss_r << this->_clients[i]->getMode('r');
    std::string tmp_r = ss_r.str();
	ret.append("\nr: " + tmp_r);

    std::stringstream ss_o;
    ss_o << this->_clients[i]->getMode('o');
    std::string tmp_o = ss_o.str();
	ret.append("\no: " + tmp_o);

    std::stringstream ss_O;
    ss_O << this->_clients[i]->getMode('O');
    std::string tmp_O = ss_O.str();
	ret.append("\nO: " + tmp_O);

    std::stringstream ss_s;
    ss_s << this->_clients[i]->getMode('s');
    std::string tmp_s = ss_s.str();
	ret.append("\ns: " + tmp_s + "\n");

	return ret;
}
*/

// append user mode to a string and returns it
std::string Server::_printUserModes(std::string ret, int i) {
    char modes[] = {'a', 'i', 'w', 'r', 'o', 'O', 's'};
    std::stringstream ss;

    for (long unsigned int j = 0; j < sizeof(modes) / sizeof(modes[0]); ++j) {
        ss << "\n" << modes[j] << ": " << this->_clients[i]->getMode(modes[j]);
    }

    ret.append(ss.str());
	// BUG: in case the client's mode is 's', need to append "\n"
    return ret;
}

// Process a mode change request and updates the client's mode if request is valid
std::string	Server::_setMode(Request request, int i) {

	// checks if client is registered
	if (!this->_clients[i]->getRegistered())
		return (_printMessage("451", this->_clients[i]->getNickName(), ":You have not registered"));

	// check args count
	if (request.args.size() < 2) {
		std::string	ret;

		// if only one argument and it matches the client's nickname
		if (request.args.size() == 1 && request.args[0] == this->_clients[i]->getNickName())
			ret = _printUserModes(ret, i);

		// if argument count incorrect
        std::stringstream ss;
        ss << 461;
        std::string tmp = ss.str();
		ret.append(tmp + "ERR_NEEDMOREPARAMS\n\tPASS :Not enough parameters\n");
		return (ret);
	}

	// check if nicknames match
	if (request.args[0] != this->_clients[i]->getNickName())
		return (_printMessage("502", this->_clients[i]->getNickName(), ":Cannot change mode for other users"));

	// check if requested mode is valid
	if (!_validMode(request))
		return (_printMessage("501", this->_clients[i]->getNickName(), ":Unknown MODE flag"));

	// check flag
	if (request.args[1][0] == '+')
		this->_clients[i]->setMode(true, request.args[1][1]);
	else
		this->_clients[i]->setMode(false, request.args[1][1]);

	// return confirmation message with mode change that was requested
	return (_printMessage("221", this->_clients[i]->getNickName(), request.args[1]));
}

std::string	Server::_setOper(Request request, int i) {
	if (!this->_clients[i]->getRegistered())
		return (_printMessage("451", this->_clients[i]->getNickName(), ":You have not registered"));

	if (request.args.size() < 2)
		return (_printMessage("461", this->_clients[i]->getNickName(), "PASS :Not enough parameters"));

	if (request.args[0] != "ADMIN")
		return (_printMessage("464", this->_clients[i]->getNickName(), ":Username/Password incorrect"));

	// if (request.args[1] != "DEEZNUTS")
	// 	return (_printMessage("464", this->_clients[i]->getNickName(), ":Username/Password incorrect"));

	this->_clients[i]->setIsOperator(true);
	return (_printMessage("381", this->_clients[i]->getNickName(), ":You are now an IRC operator"));
}

std::string	Server::_setPassWord(Request request, int i) {
	if (request.args.size() < 1)
		return (_printMessage("461", this->_clients[i]->getNickName(), "PASS :Not enough parameters"));

	if (this->_clients[i]->getRegistered())
		return (_printMessage("462", this->_clients[i]->getNickName(), ":Unauthorized command (already registered)"));

	if (request.args[0] != this->_password)
		return (_printMessage("997", this->_clients[i]->getNickName(), ":Incorrect password"));
	else
		this->_clients[i]->setAuth(true);

	return ("");
}

std::string	Server::_setNickName(Request request, int i) {
	if (!this->_clients[i]->getAuth())
		return (_printMessage("998", this->_clients[i]->getNickName(), ":You need to authenticate first"));

	if (request.args.size() < 1)
		return (_printMessage("431", this->_clients[i]->getNickName(), ":No nickname given"));

	int	j = 0;
	while (request.args[0][j]) {
		if (!isalnum(request.args[0][j]) && request.args[0][j] != '-' && request.args[0][j] != '\r')
			return (_printMessage("432", this->_clients[i]->getNickName(), request.args[0] + " :Erroneous nickname"));
		j++;
	}

	if (std::find(this->_clientNicknames.begin(), this->_clientNicknames.end(), request.args[0]) != this->_clientNicknames.end())
		return (_printMessage("433", this->_clients[i]->getNickName(), request.args[0] + " :Nickname is already in use"));

	this->_clients[i]->setNickName(request.args[0]);
	this->_clientNicknames.push_back(this->_clients[i]->getNickName());

	if (this->_clients[i]->getUserName() != "") {
		this->_clients[i]->setID(this->_clients[i]->getNickName() + "!" + this->_clients[i]->getUserName() + "@" + this->_clients[i]->getHost());
		this->_clients[i]->setRegistered(true);
		return (_printMessage("001", this->_clients[i]->getNickName(), "Welcome to the Internet Relay Network " + this->_clients[i]->getID()));
	}

	return ("");
}

std::string	Server::_setUserName(Request request, int i) {
	if (!this->_clients[i]->getAuth())
		return (_printMessage("998", this->_clients[i]->getNickName(), ":You need to authenticate first"));

	if (this->_clients[i]->getRegistered())
		return (_printMessage("462", this->_clients[i]->getNickName(), ":Unauthorized command (already registered)"));

	if (request.args.size() < 4)
		return (_printMessage("461", this->_clients[i]->getNickName(), "USER :Not enough parameters"));

	this->_clients[i]->setUserName(request.args[0]);
	this->_clients[i]->setFullName(request.args[3]);

	if (this->_clients[i]->getNickName() != "") {
		this->_clients[i]->setID(this->_clients[i]->getNickName() + "!" + this->_clients[i]->getUserName() + "@" + this->_clients[i]->getHost());
		this->_clients[i]->setRegistered(true);
		return (_printMessage("001", this->_clients[i]->getNickName(), "Welcome to the Internet Relay Network " + this->_clients[i]->getID()));
	}

	return ("");
}

std::string	Server::_quit(Request request, int i) {
	std::string ret = this->_clients[i]->getUserPerfix() + "QUIT ";

	if (request.args.size())
		ret.append(":" + request.args[0] + "\n");
	else
		ret.append("\n");

	std::map<std::string, Channel *> channels = this->_clients[i]->getJoinedChannels();
	std::map<std::string, Channel *>::iterator it = channels.begin();

	while (it != channels.end()) {
		_sendToAllUsers(it->second, i, ret);
		it++;
	}

	this->_clients[i]->leaveAllChannels();
	close(this->_clients[i]->getClientfd());
	_removeFromPoll(i);

	return ("QUIT");
}

std::string	Server::_printHelpInfo() {
	std::string	helpInfo;

	helpInfo.append(GREEN);
	helpInfo.append("STEP 1: PASS\n");
	helpInfo.append(RESET);
	helpInfo.append("\tUse PASS command to set a password. e.g: PASS [Server Password]\n\n");
	helpInfo.append(GREEN);
	helpInfo.append("STEP 2: NICK\n");
	helpInfo.append(RESET);
	helpInfo.append("\tUse NICK command to set a nickname. e.g: NICK deezNuts69\n\n");
	helpInfo.append(GREEN);
	helpInfo.append("STEP 3: USER\n");
	helpInfo.append(RESET);
	helpInfo.append("\tUse USER command to register your username and fullname.e.g: USER deez * * :Deez Nuts\n\n");

	return (helpInfo);
}
