#include <Server.hpp>
#include <sstream>

void Server::handleServerEvents() {
    // check for incoming connections events
	if (pollFds.at(0).revents & POLLIN) {

        // logging the event
		LOGGER.info("serverEventHandling", "New server event caught");

        //adding a new client
		addNewClient();
	}
}

void Server::handleClientEvents() {
    // iterate over cllient events
	std::vector<pollfd>::iterator it = pollFds.begin() + 1;
	for (; it < pollFds.end(); it++) {
		Client &client = clients.at((*it).fd);

        // handle different tyes of network evets
		if ((*it).revents & POLLIN) { // POLLIN : data to read
			LOGGER.info("clientEventHandling", "POLLIN caught");
			std::string data = receiveDataFromSocket((*it).fd);
			std::vector<Command> commands = parseCommands(data);
			executeCommands(client, commands);
		} else if ((*it).revents & POLLOUT) { // POLLOUT : ready to send data
			sendDataThroughSocket(client);
		} else if ((*it).revents & POLLERR) { // POLLERR : err occured
			LOGGER.info("clientEventHandling", "POLLERR caught");
			unexpectedDisconnectHandling((*it).fd);
		} else if ((*it).revents & POLLHUP) { // POLLHUP : hang up
	 		LOGGER.info("clientEventHandling", "POLLHUP caught");
		} else if ((*it).revents & POLLNVAL) { // POLLNVAL : invalid request
			LOGGER.info("clientEventHandling", "POLLNVAL caught");
		}
	}
}

void Server::handleDisconnectionEvents() {
    // iterate through clients in reverse
	std::map<int, Client>::reverse_iterator it = clients.rbegin();

    // check for disconnection flag
	for (; it != clients.rend(); it++) {
		if ((it->second).getToDisconnect()) {

            // log disconnection event
			std::ostringstream logMessage;
			logMessage << "Disconnecting client on fd " << it->first;
			LOGGER.info("disconnectHandling", logMessage.str());

            // eject the client
			ejectClient(it->first, QUITED);

            // break after handling
			break;
		}
	}
}

void Server::handleEmptyChannelEvents() {
    // initialize iterator and list for deletion
	std::map<std::string, Channel>::iterator it;
	it = channels.begin();
	std::vector<std::string> toDelete;

    // identify empty channels
	while (it != channels.end()) {
		if (it->second.getClients().size() == 0)
			toDelete.push_back(toIrcUpperCase(it->second.getName()));
		it++;
	}

    // remove empty channels
	std::size_t i = 0;
	while (i < toDelete.size()) {
		LOGGER.info("handleEmptyChannelEvents", "Removing empty channel " + toDelete.at(i));
		channels.erase(toDelete.at(i));
		i++;
	}
}
