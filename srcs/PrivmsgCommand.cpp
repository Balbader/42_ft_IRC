#include "../headers/Server.hpp"

// handle send private messages
std::string	Server::_privmsg(Request request, int i) {
	
	// check if client is registered
	if (!this->_clients[i]->getRegistered())
		return (_printMessage("451", this->_clients[i]->getNickName(), ":You have not registered"));

	// parameters validation
	// checks if there are exactly 2 arguments : recipient and a message
	if (request.args.size() < 2)
		return (_printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters"));

	// if yes
	if (request.args.size() == 2) {

		// checks if the recipient argument contains a comma
		if (request.args[0].find(",") != std::string::npos)
			return (_printMessage("401", this->_clients[i]->getNickName(), request.args[0].append(" :Too many recipients.")));

		// checks if the recipient is not a channel
		if (request.args[0][0] != '&' && request.args[0][0] != '#' && request.args[0][0] != '+' && request.args[0][0] != '!')
			return (_privToUser(request.args[0], request.args[1], "PRIVMSG", i)); // send private message to user

		// if it is a channel
		return (_privToChannel(request.args[0], request.args[1], i)); // broadcast message to all users in specified channel
	}

	// if success returns empty string
	return ("");
}

std::string Server::_privToUser(std::string User, std::string message, std::string cmd, int i) {

	// find client fd associated with nickname
	int userFd = _findFdByNickName(User);

	if (userFd == USERNOTFOUND)
		return (_printMessage("401", this->_clients[i]->getNickName(), User.append(" :No such nick/channel")));

	// init reply
	std::string reply = this->_clients[i]->getUserPerfix();
	reply.append(cmd + " " + User + " :" + message + "\n");

	// send reply to the recipient
	if (_sendall(userFd, reply) == -1)
				std::cout << "_sendall() error: " << strerror(errno) << std::endl;

	// FIX:
	// inform sender if message could not be delivered
	// inform sender if message was successfully sent or recieved
	// Ensure that private messages can't be intercepted or read by unauthorized users or admin
	return ("");
}


// Handle the forwarding of a private message from a client to all users in a specified channel
std::string Server::_privToChannel(std::string ChannelName, std::string message, int i) {

	// Search for the channel by name
	std::map<std::string, Channel*>::iterator it = this->_allChannels.find(ChannelName);

	if (it != this->_allChannels.end()) {
		
		// check sender's role
		std::pair<Client*, int> user = it->second->findUserRole(i);

		// if user not in channel
		if (user.second == -1 )
			return (_printMessage("404", this->_clients[i]->getNickName(), ChannelName.append(" :Cannot send to channel")));

		// init private message
		std::string msg("PRIVMSG " + ChannelName + " :" + message + "\n");

		// send message to all users in channel except sender
		_sendToAllUsers(it->second, i, msg);
	}

	else
		// if channel name not found
		return (_printMessage("401", this->_clients[i]->getNickName(), ChannelName.append(" :No such nick/channel")));

	return ("");
}

