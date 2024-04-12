#include <Server.hpp>

void Server::addNewClient(void) {
    // initial logging and client limit check
	LOGGER.info("addNewClient", "Adding new client...");
	if (pollFds.size() >= MAX_CLIENTS) {
		LOGGER.error("addNewClient", "The maximum number of clients connected to the server has been reached. The connection will be rejected");
		return;
	}

    // Prepare for accepting connection
	struct sockaddr_in	clientAddr;
	int	addrlen = sizeof(clientAddr);
	LOGGER.info("addNewClient", "Accepting connection...");
	int fd = accept(socket, (struct sockaddr *)&clientAddr, (socklen_t *)&addrlen);

    // connection error Handling
	if (fd < 0) {
		LOGGER.error("addNewClient", "Failed to accept connection");
		return;
	}

    // register new client
	Client client(fd, inet_ntoa(clientAddr.sin_addr));
	clients.insert(std::pair<int, Client>(fd, client));

    // setup polling for the client
	pollfd pollFd = {fd, POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL, 0};
	pollFds.push_back(pollFd);

    // log connection establishment
	std::ostringstream logMessage;
	logMessage << "New connection established with " << client.getHostname() << " on fd " << fd;
	LOGGER.info("addNewClient", logMessage.str());
}

void Server::unexpectedDisconnectHandling(int fd) {
    // retrieve the cllient object
	Client &client = clients.at(fd);
	std::stringstream ss;

    // logging the disconnection Handling
	LOGGER.info("unexpectedDisconnectHandling", "Handling unexpected disconnection...");
    
    // check registration status
	if (client.getRegistration() == (NICK_FLAG | USER_FLAG | PASS_FLAG)) {
		ss << ":" << client.getNickname();
		ss << " QUIT: Client exited unexpectedly";
		ss << "\r\n";

        // remove client from channels and notify other clients
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

    // set client to disconnect
	client.setToDisconnect(true);
}

void Server::ejectClient(int clientFd, int reason) {
    // initialize and log start of ejection
	std::ostringstream logMessage;
	logMessage << "Ejecting client on fd " << clientFd;
	LOGGER.info("ejectClient", logMessage.str());

    // iterate over poll descriptors
	std::vector<pollfd>::iterator it = pollFds.begin();
	for (; it < pollFds.end(); it++) {
		if ((*it).fd == clientFd) {
			close(clientFd);
			std::memset(&(*it), 0, sizeof(pollfd));
			pollFds.erase(it);
			break;
		}
	}

    // remove client from server's client list
	clients.erase(clientFd);

    // logging the reason for ejection
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
