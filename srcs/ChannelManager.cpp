#include <Server.hpp>

std::map<std::string, Channel>::iterator Server::getChannelByName(std::string channelName) {
    // convert channel name to UpperCase
	std::string upperInput = toIrcUpperCase(channelName);

    // logging the search
	LOGGER.info("getChannelByName", "Looking for channel " + upperInput);

    // iterate over channels
	std::map<std::string, Channel>::iterator it = channels.begin();
	while (it != channels.end()) {
		std::string upperChannel = toIrcUpperCase((*it).first);
		if (upperInput == upperChannel) {
			return it;
		}
		it++;
	}

    // return channel's end if not found
	return channels.end();
}

void Server::removeClientFromChannel(Client &client, Channel &channel, std::string message) {
    // logging the removal
	std::ostringstream logMessage;
	logMessage << "Removing client " << client.getNickname() << " from channel " << channel.getName();
	LOGGER.info("removeClientFromChannel", logMessage.str());

    // removing client from channel
	channel.removeClient(client);
	client.removeChannel(channel);

    // channel empty check
	if (channel.getClients().size() == 0) {
		this->channels.erase(toIrcUpperCase(channel.getName()));
	} else { // Broadcasting Part message
		std::stringstream ss;

		ss << ":";
		ss << client.getNickname();
		ss << " PART";
		ss << " " << channel.getName();
		ss << " " << message;
		ss << "\r\n";

		channel.broadcast(client, ss.str(), false);
	}
}

void Server::broadcastMessage(int fd, std::string message) {
    // initialize log message
	std::ostringstream logMessage;
	logMessage << "Broadcasting message to all clients except fd " << fd;

    // logging the broadcast message
	LOGGER.info("broadcastMessage", logMessage.str());

    // iterating over clients
	std::map<int, Client>::iterator it = clients.begin();
	for (; it != clients.end(); it++) {
		if ((it->first) != fd) {
			(it->second).setSendData(message);
		}
	}
}
