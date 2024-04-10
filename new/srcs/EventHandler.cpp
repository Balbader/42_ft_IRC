#include <Server.hpp>
#include <sstream>

void Server::handleServerEvents() {
	if (pollFds.at(0).revents & POLLIN) {
		LOGGER.info("serverEventHandling", "New server event caught");
		addNewClient();
	}
}

void Server::handleClientEvents() {
	std::vector<pollfd>::iterator it = pollFds.begin() + 1;
	for (; it < pollFds.end(); it++) {
		Client &client = clients.at((*it).fd);
		if ((*it).revents & POLLIN) {
			LOGGER.info("clientEventHandling", "POLLIN caught");
			std::string data = receiveDataFromSocket((*it).fd);
			std::vector<Command> commands = parseCommands(data);
			executeCommands(client, commands);
		} else if ((*it).revents & POLLOUT) {
			sendDataThroughSocket(client);
		} else if ((*it).revents & POLLERR) {
			LOGGER.info("clientEventHandling", "POLLERR caught");
			unexpectedDisconnectHandling((*it).fd);
		} else if ((*it).revents & POLLHUP) {
			LOGGER.info("clientEventHandling", "POLLHUP caught");
		} else if ((*it).revents & POLLNVAL) {
			LOGGER.info("clientEventHandling", "POLLNVAL caught");
		}
	}
}

void Server::handleDisconnectionEvents() {
	std::map<int, Client>::reverse_iterator it = clients.rbegin();

	for (; it != clients.rend(); it++) {
		if ((it->second).getToDisconnect()) {
			std::ostringstream logMessage;
			logMessage << "Disconnecting client on fd " << it->first;
			LOGGER.info("disconnectHandling", logMessage.str());
			ejectClient(it->first, QUITED);
			break;
		}
	}
}

void Server::handleEmptyChannelEvents() {
	std::map<std::string, Channel>::iterator it;
	it = channels.begin();
	std::vector<std::string> toDelete;
	while (it != channels.end()) {
		if (it->second.getClients().size() == 0)
			toDelete.push_back(toIrcUpperCase(it->second.getName()));
		it++;
	}
	std::size_t i = 0;
	while (i < toDelete.size()) {
		LOGGER.info("handleEmptyChannelEvents", "Removing empty channel " + toDelete.at(i));
		channels.erase(toDelete.at(i));
		i++;
	}
}
