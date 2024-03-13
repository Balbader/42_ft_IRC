#include "../headers/Server.hpp"

std::string	Server::_joinChannel( Request request, int i ) {
	int j = 1;

	// check if client is registered
	if (!this->_clients[i]->getRegistered())
		return (_printMessage("451", this->_clients[i]->getNickName(), ":You have not registered"));

	// check the number of parameters
	if (request.args.size() == 0)
		return (_printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters"));

	// if arg is 0 leave all channels
	if (request.args[0] == "0")
		return(this->_clients[i]->leaveAllChannels());

	// parse the list of channels the client wants to join
	std::vector<std::string> parsChannels(_commaSeparator(request.args[0]));
	std::vector<std::string> parsKeys;
	// make sure channels and keys are seperated by commas
	if (request.args.size() == 2)
		parsKeys = _commaSeparator(request.args[1]);


	// iterate  over the list of channels and attempt to join each one
	// if keys are provided, it uses them to join private channels
	std::vector<std::string>::iterator itChannels = parsChannels.begin();
	std::vector<std::string>::iterator itKeys = parsKeys.begin();
	while (itChannels != parsChannels.end() && j == 1) {

		if ( itKeys != parsKeys.end())
			j = _createPrvChannel(*itChannels, *itKeys, i);
		else
			j = _createChannel(*itChannels, i);

		if (j == BADCHANMASK)
			return (_printMessage("476", this->_clients[i]->getNickName(), *itChannels + " :Bad Channel Mask"));

		if (j == BANNEDFROMCHAN)
			return (_printMessage("474", this->_clients[i]->getNickName(), *itChannels + " :Cannot join channel (+b)"));

		if (j == TOOMANYCHANNELS )
			return (_printMessage("405", this->_clients[i]->getNickName(), *itChannels + " :You have joined too many channels"));

		if (j == BADCHANNELKEY )
			return (_printMessage("475", this->_clients[i]->getNickName(), *itChannels + " :Cannot join channel (+k)"));

		if (j == CHANNELISFULL )
			return (_printMessage("471", this->_clients[i]->getNickName(), *itChannels + " :Cannot join channel (+l)"));

		if (j == NOSUCHCHANNEL)
			return (_printMessage("403", this->_clients[i]->getNickName(), *itChannels + " :No such channel"));

		if (itKeys != parsKeys.end())
			itKeys++;

		itChannels++;
	}
	--itChannels;

	// if success return empty string
	return ("");
}

int	Server::_createChannel( std::string ChannelName, int CreatorFd ) {

	// search for ChannelName in _allChannels
	std::map<std::string, Channel *>::iterator it = this->_allChannels.find(ChannelName);
	if (it == this->_allChannels.end()) {
		if (ChannelName[0] != '&' && ChannelName[0] != '#' && ChannelName[0] != '+' && ChannelName[0] != '!')
			return (BADCHANMASK);

		// if name is valid, create new channel
		Channel *channel = new Channel(ChannelName, this->_clients[CreatorFd]);

		// insert it to _allChannels map
		this->_allChannels.insert(std::pair<std::string, Channel *>(ChannelName, channel));

		// add the creator as a member of the channel
		this->_clients[CreatorFd]->joinChannel( ChannelName, channel );
	}

	// if channel already exist
	else {
		if (it->second->getKey().empty()) { // if no key required

			int i = 0;

			// add creator as operator or member based on their status
			if (this->_clients[CreatorFd]->getisOperator() == true)
				i = it->second->addOperator(this->_clients[CreatorFd]);
			else
				i = it->second->addMember(this->_clients[CreatorFd]);

			// if already a member
			if (i == USERISJOINED)
				this->_clients[CreatorFd]->joinChannel( it->first, it->second );
			else if (i == USERALREADYJOINED)
				return (USERALREADYJOINED);
			else if (i == BANNEDFROMCHAN)
				return (BANNEDFROMCHAN);

			// once the creator is aded, server sends various messages to notify other clients
			_sendall(CreatorFd, this->_clients[CreatorFd]->getUserPerfix() + "JOIN " + ChannelName + "\n");
			_sendall(CreatorFd, _printMessage("332", this->_clients[CreatorFd]->getNickName(), ChannelName + " :" + it->second->getTopic()));
			_sendall(CreatorFd, _printMessage("353", this->_clients[CreatorFd]->getNickName() + " = " + ChannelName, it->second->listAllUsers()));
			_sendall(CreatorFd, _printMessage("353", this->_clients[CreatorFd]->getNickName() + " " + ChannelName, ":End of NAMES list"));

			// send 'JOIN' message to all users in the channel to notify them that a new member has joined
			std::string reply = "JOIN " + ChannelName + "\n";
			_sendToAllUsers(it->second, CreatorFd, reply);

			return (USERISJOINED);
		}
	}
	return (USERISJOINED);
}

// ensures secure handling of private channels by requiring a key for creation of joining
// therefor adding an extra layer of privacy and control over who can access the channel
int	Server::_createPrvChannel( std::string ChannelName, std::string ChannelKey, int CreatorFd) {

	// search for ChannelName in _allChannels
	std::map<std::string, Channel *>::iterator it = this->_allChannels.find(ChannelName);

	// if channel doesn't exist
	if (it == this->_allChannels.end()) {
		if (ChannelName[0] != '&' && ChannelName[0] != '#' && ChannelName[0] != '+' && ChannelName[0] != '!')
			return (BADCHANMASK);

		Channel *channel = new Channel(ChannelName, ChannelKey, this->_clients[CreatorFd]);
		this->_allChannels.insert(std::pair<std::string, Channel *>(ChannelName, channel));
		this->_clients[CreatorFd]->joinChannel(ChannelName, channel);
	}

	else { // if channel exist
		if (it->second->getKey() == ChannelKey) { // check if provided key matches ChannelKey
			int i = 0;

			if (this->_clients[CreatorFd]->getisOperator() == true)
				i = it->second->addOperator(this->_clients[CreatorFd]);
			else
				i = it->second->addMember(this->_clients[CreatorFd]);

			if (i == USERISJOINED)
				this->_clients[CreatorFd]->joinChannel( it->first, it->second );
			else if (i == USERALREADYJOINED)
				return (USERALREADYJOINED);
			else if (i == BANNEDFROMCHAN)
				return (BANNEDFROMCHAN);

			_sendall(CreatorFd, this->_clients[CreatorFd]->getUserPerfix() + "JOIN " + ChannelName + "\n");
			_sendall(CreatorFd, _printMessage("332", this->_clients[CreatorFd]->getNickName(), ChannelName + " :" + it->second->getTopic()));
			_sendall(CreatorFd, _printMessage("353", this->_clients[CreatorFd]->getNickName() + " = " + ChannelName, it->second->listAllUsers()));
			_sendall(CreatorFd, _printMessage("353", this->_clients[CreatorFd]->getNickName() + ChannelName, ":End of NAMES list"));
			std::string reply = "JOIN " + ChannelName + "\n";
			_sendToAllUsers(it->second, CreatorFd, reply);

			return (USERISJOINED);
		}
		else
			return (BADCHANNELKEY);
	}
	return (USERISJOINED);
}


// parse a comma-seperated list of values into individual strings
std::vector<std::string> Server::_commaSeparator(std::string arg) {
	std::vector<std::string> ret;
	size_t pos = 0;

	// everytime a comma is encountered, extract substring from beginning of the string up to the comma
	while ((pos = arg.find(",")) != std::string::npos) {
		// add substring to ret
		ret.push_back(arg.substr(0, pos));
		// erase the added part from original string
		arg.erase(0, pos + 1);
	}

	// last substring is added to ret
	ret.push_back(arg.substr(0, pos));
	return (ret);
}
