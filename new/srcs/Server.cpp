#include <Server.hpp>

bool Server::online = true;

// constructor
Server::Server(std::string const &passwd, int const &port) {
	if (isPortInvalid(port))
		throw std::invalid_argument("Invalid port");

	if (isPasswdInvalid(passwd))
		throw std::invalid_argument("Invalid passwd");

	this->passwd = passwd;
	this->port = port;
	this->socket = 0;
	this->creationDatetime = getDatetime();

	LOGGER = Logger("Server");
}


// destructor
Server::~Server() {
	for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); it++) {
		if (fcntl((*it).fd, F_SETFL, O_NONBLOCK) == false) {
			close((*it).fd);
		}
    }
	close(socket);
	pollFds.clear();
	LOGGER.info("~Server", "Shutting down server...");
}


// methods
bool Server::isPortInvalid(int port) {
	return port <= 0 || port > 65535;
}

bool Server::isPasswdInvalid(std::string passwd) {
	return passwd.empty() 
		|| passwd.find_first_of(FORBIDDEN_CHARS) != std::string::npos;
}

void Server::stop() {
	online = false;
}

void signalHandler(int signal) {
	if (signal == SIGINT) {
		Server::stop();
	}
}

void Server::setupSocket() {
	std::signal(SIGINT, signalHandler);

	LOGGER.info("setupSocket", "Creating socket...");
	socket = ::socket(AF_INET, SOCK_STREAM, 0);

	if (socket < 0) {
		throw std::runtime_error("Failed to create socket");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	LOGGER.info("setupSocket", "Binding socket to ipv4 address...");
	if (bind(socket, (struct sockaddr *)&(address), sizeof(address))) {
		throw std::runtime_error("Failed to bind socket");
	}

	LOGGER.info("setupSocket", "Listening on socket...");
	if (listen(socket, SOMAXCONN)) {
		throw std::runtime_error("Failed to listen");
	}
}

void Server::startServer() {
	setupSocket();
	int events;
	struct pollfd server = {socket, POLLIN, 0};

	pollFds.push_back(server);
	LOGGER.info("startServer", "Starting server...");

	while (online) {
		events = poll(pollFds.data(), static_cast<nfds_t>(pollFds.size()), -1);
		if (events >= 0) {
			handleServerEvents();
			handleClientEvents();
			handleDisconnectionEvents();
			handleEmptyChannelEvents();
		}
	}
}
