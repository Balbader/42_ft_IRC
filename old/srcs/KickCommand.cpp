#include "../headers/Server.hpp"

// checks the user's role in the channnel and, if authorized
// executes the kick operation for each specified user
std::string	Server::_kickedFromChannel(std::string ChannelName, std::string message, std::vector<std::string> users, int i) {

	// search for ChannelName in _allChannels
	std::map<std::string, Channel*>::iterator it = this->_allChannels.find(ChannelName);

	// if found
	if (it != this->_allChannels.end()) {
		// check user role
		std::pair<Client*, int> user = it->second->findUserRole(i);

		// if user is operator
		if (user.second == 1) {

			int ret = 0;

			// iterate over the 'users' vector whick contains the nicknames of the users to be kicked
			for (std::vector<std::string>::iterator user = users.begin(); user != users.end(); ++user) {

				// finde user file descriptor
				ret = _findFdByNickName(*user);
				if (ret == USERNOTINCHANNEL)
					return (_printMessage("441", this->_clients[i]->getNickName(), (*user).append(" " + ChannelName + " :They aren't on that channel")));

				// init kick message
				std::string reply = "KICK " + ChannelName;

				if (message.empty())
					reply.append("\n");
				else
					reply.append(" " + message + "\n");

				// send kick message to all users int he channel
				_sendToAllUsers(it->second, i, reply);

				// ban user from channel
				it->second->banUser(this->_clients[ret]);

				// remove user from channel
				ret = _partChannel(ChannelName, ret, "", 0);
			}
		}

		else if (user.second == -1) // user not in channel
			return (_printMessage("442", this->_clients[i]->getNickName(), ChannelName + " :You're not on that channel"));

		else
			return (_printMessage("482", this->_clients[i]->getNickName(), ChannelName + " :You're not channel operator"));

		return ("");
	}

	// if ChannelName not found
	return (_printMessage("403", this->_clients[i]->getNickName(), ChannelName.append(" :No such channel")));
}


// Ensure that only registered users with sufficient permissions can issue kick commands
std::string	Server::_kick(Request request, int i) {

	// check if client is registered
	if (!this->_clients[i]->getRegistered())
		return (_printMessage("451", this->_clients[i]->getNickName(), ":You have not registered"));

	// checks if request contains at least 2 parameters
	if (request.args.size() < 2)
		return (_printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters"));

	// extract channels
	std::vector<std::string> channels(_commaSeparator(request.args[0]));

	// extract users
	std::vector<std::string> users(_commaSeparator(request.args[1]));

	for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {

		std::string ret;

		// if kick message is provided
		if (request.args.size() == 3)
			ret = _kickedFromChannel(*it, request.args[2], users, i); // current channel, kick message, list of users to be kicked and index of requestint client
		else
			ret = _kickedFromChannel(*it, "", users, i);

		// if returns non-empty string, means there is a problem with the kick process
		// i.e. user not in channel or not an operator
		if (!ret.empty())
			return(ret);
	}
	return ("");
}
