#include <Server.hpp>

void Server::addNewClient(void) {
	LOGGER.info("addNewClient", "Adding new client...");
	if (pollFds.size() >= MAX_CLIENTS) {
		LOGGER.error("addNewClient", "The maximum number of clients connected to the server has been reached. The connection will be rejected");
		return;
	}
	struct sockaddr_in	clientAddr;
	int	addrlen = sizeof(clientAddr);
	LOGGER.info("addNewClient", "Accepting connection...");
	int fd = accept(socket, (struct sockaddr *)&clientAddr, (socklen_t *)&addrlen);
	if (fd < 0) {
		LOGGER.error("addNewClient", "Failed to accept connection");
		return;
	}
	Client client(fd, inet_ntoa(clientAddr.sin_addr));
	clients.insert(std::pair<int, Client>(fd, client));
	pollfd pollFd = {fd, POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL, 0};
	pollFds.push_back(pollFd);
	std::ostringstream logMessage;
	logMessage << "New connection established with " << client.getHostname() << " on fd " << fd;
	LOGGER.info("addNewClient", logMessage.str());
}

void Server::unexpectedDisconnectHandling(int fd) {
	Client		   		&client = clients.at(fd);
	std::stringstream	ss;

	LOGGER.info("unexpectedDisconnectHandling", "Handling unexpected disconnection...");
	if (client.getRegistration() == (NICK_FLAG | USER_FLAG | PASS_FLAG)) {
		ss << ":" << client.getNickname();
		ss << " QUIT: Client exited unexpectedly";
		ss << "\r\n";

		std::vector<Channel *>::iterator it = client.getChannels().begin();
		for (; it != client.getChannels().end(); it++) {
			(*it)->removeClient(client);
			std::map<Client *, uint>::iterator itb = (*it)->getClients().begin();
			std::map<Client *, uint>::iterator ite = (*it)->getClients().end();
			for (; itb != ite; itb++) {
				itb->first->setSendData(ss.str());
			}
		}
	}
	client.setToDisconnect(true);
}

void Server::ejectClient(int clientFd, int reason) {
	std::vector<pollfd>::iterator it = pollFds.begin();
	std::ostringstream logMessage;
	logMessage << "Ejecting client on fd " << clientFd;
	LOGGER.info("ejectClient", logMessage.str());
	for (; it < pollFds.end(); it++) {
		if ((*it).fd == clientFd) {
			close(clientFd);
			std::memset(&(*it), 0, sizeof(pollfd));
			pollFds.erase(it);
			break;
		}
	}
	clients.erase(clientFd);
	std::ostringstream logReason;

	switch (reason) {
		case LOSTCONNECTION:
			logReason << "Client connection lost. (fd : " << clientFd << ")";
			LOGGER.info("ejectClient", logReason.str());
			break;
		case QUITED:
			logReason << "Client left. (fd : " << clientFd << ")";
			LOGGER.info("ejectClient", logReason.str());
			break;
		case KICKED:
			logReason << "Client kicked. (fd : " << clientFd << ")";
			LOGGER.info("ejectClient", logReason.str());
			break;
		default:
			logReason << "Client successfully ejected. (fd : " << clientFd << ")";
			LOGGER.info("ejectClient", logReason.str());
	}
}

void Server::removeChannel(std::string name) { channels.erase(name); }