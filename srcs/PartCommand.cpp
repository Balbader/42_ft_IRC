#include "../headers/Server.hpp"

// Handle a user's departure from a chat channel
int	Server::_partChannel( std::string ChannelName, int i, std::string message, int isPart ) {

	// look for the specific channel in _allChannels
	std::map<std::string, Channel *>::iterator itCh = this->_allChannels.find(ChannelName);
	if (itCh == this->_allChannels.end()) // if channel doesn't exist
		return (NOSUCHCHANNEL);
	else { // if channel exist

		// find user role
		std::pair<Client*, int> user(itCh->second->findUserRole(i));

		// if user not in channel
		if (user.second == -1)
			return (NOTINCHANNEL);

		else {

			// if user is member
			if (user.second == 0)
				itCh->second->removeMember(i);

			// if user is operator
			else if (user.second == 1)
				itCh->second->removeOperator(i);

			// if user has a voice in channel
			else
				itCh->second->removeVoice(i);

			// update status of client
			user.first->leaveChannel(itCh->second->getName());

			if (isPart == 1) { // indicate that this is a standard channel part operation (i.e. not kicked or disconnected)

				// init PART message
				std::string reply = "PART " + ChannelName;
				if (message.empty())
					reply.append("\n");
				else
					reply.append(" " + message + "\n");

				// send message to all remaining users in the channel
				_sendToAllUsers(itCh->second, i, reply);
			}
		}
	}
	return (0);
}


// process a request from a client to leave one or more channels in a chat server
std::string	Server::_part( Request request, int i ) {

	// check if client is registered
	if (!this->_clients[i]->getRegistered())
		return (_printMessage("451", this->_clients[i]->getNickName(), ":You have not registered"));

	// parameters validation
	if (request.args.size() == 0)
		return (_printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters"));

	// parse the channel names from the first argument
	std::vector<std::string> parsChannels(_commaSeparator(request.args[0]));

	// iterate over each channel in parsChannels
	for (std::vector<std::string>::iterator it = parsChannels.begin(); it != parsChannels.end(); ++it) {

		int j = 0;

		// if there is a second argument provided int the request
		if (request.args.size() == 2)
			j = _partChannel(*it, i, request.args[1], 1); // include message in the part notification sent to other users in the channel
		else
			j = _partChannel(*it, i, "", 1); // no message to include

		if (j == NOSUCHCHANNEL)
			return (_printMessage("403", this->_clients[i]->getNickName(), *it + " :No such channel"));

		if (j == NOTINCHANNEL /* Not in channel */)
			return (_printMessage("442", this->_clients[i]->getNickName(), *it + " :You're not on that channel"));
	}

	// if success returns empty string 
	return ("");
}
