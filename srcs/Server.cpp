#include <Server.hpp>

bool Server::online = true;

// constructor
Server::Server(std::string const &passwd, int const &port) {
	if (isPortInvalid(port))
		throw std::invalid_argument("Invalid port");

	if (isPasswdInvalid(passwd))
		throw std::invalid_argument("Invalid passwd");

  this->maxClientsFlag = false;
	this->passwd = passwd;
	this->port = port;
	this->socket = 0;
	this->creationDatetime = getDatetime();

	LOGGER = Logger("Server");
}


// destructor
Server::~Server() {
	for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); it++) {
		if (fcntl((*it).fd, F_SETFL, O_NONBLOCK) == -1) {
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

/*
 * The setupSocket method in the Server class configures and initializes the server's main listening socket,
 * which is essential for accepting incoming client connections.
 * This method encapsulates the whole process from creating the socket to
 * listening for connections, including error handling. 
*/
void Server::setupSocket() {
    // signal handling
	std::signal(SIGINT, signalHandler);

    // creating the socket
	LOGGER.info("setupSocket", "Creating socket...");
	socket = ::socket(AF_INET, SOCK_STREAM, 0);

    // error handling for socket creation
	if (socket < 0) {
		throw std::runtime_error("Failed to create socket");
	}

    // setting socket address
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

    // binding the socket
	LOGGER.info("setupSocket", "Binding socket to ipv4 address...");
	if (bind(socket, (struct sockaddr *)&(address), sizeof(address))) {
		throw std::runtime_error("Failed to bind socket");
	}

    // Listening on the socket
	LOGGER.info("setupSocket", "Listening on socket...");
	if (listen(socket, SOMAXCONN)) {
		throw std::runtime_error("Failed to listen");
	}
}

/*
 * The startServer method in the Server class is responsible for initiating the server's main operations,
 * including setting up the listening socket and entering the main server loop where it handles various network events.
 * This method is central to the server's functionality, ensuring that it can continuously process incoming
 * and outgoing data, client connections, and disconnections. 
*/
void Server::startServer() {
    // setup listening socket
	setupSocket();

    // initialize polling
	int events;
	struct pollfd server = {socket, POLLIN, 0};
	pollFds.push_back(server);

    // server startuup logging
	LOGGER.info("startServer", "Starting server...");

    // main server loop
	while (online) {
		events = poll(pollFds.data(), static_cast<nfds_t>(pollFds.size()), 0);
    // std::cout << "events== "<< events << std::endl;
		if (events > 0) {
			handleServerEvents();
			handleClientEvents();
			handleDisconnectionEvents();
			handleEmptyChannelEvents();
		}
	}
}
