#include <Server.hpp>

std::map<std::string, Channel>::iterator Server::getChannelByName(std::string channelName) {
	std::string upperInput = toIrcUpperCase(channelName);

	LOGGER.info("getChannelByName", "Looking for channel " + upperInput);
	std::map<std::string, Channel>::iterator it = channels.begin();
	while (it != channels.end()) {
		std::string upperChannel = toIrcUpperCase((*it).first);
		if (upperInput == upperChannel) {
			return it;
		}
		it++;
	}
	return channels.end();
};

void Server::removeClientFromChannel(Client &client, Channel &channel, std::string message) {
	std::ostringstream logMessage;
	logMessage << "Removing client " << client.getNickname() << " from channel " << channel.getName();
	LOGGER.info("removeClientFromChannel", logMessage.str());
	channel.removeClient(client);
	client.removeChannel(channel);
	if (channel.getClients().size() == 0) {
		this->channels.erase(toIrcUpperCase(channel.getName()));
	} else {
		std::stringstream ss;

		ss << ":";
		ss << client.getNickname();
		ss << " PART";
		ss << " " << channel.getName();
		ss << " " << message;
		ss << "\r\n";

		channel.broadcast(client, ss.str(), false);
	}
};

void Server::broadcastMessage(int fd, std::string message) {
	std::map<int, Client>::iterator it = clients.begin();
	std::ostringstream logMessage;
	logMessage << "Broadcasting message to all clients except fd " << fd;
	LOGGER.info("broadcastMessage", logMessage.str());
	for (; it != clients.end(); it++) {
		if ((it->first) != fd) {
			(it->second).setSendData(message);
		}
	}
}