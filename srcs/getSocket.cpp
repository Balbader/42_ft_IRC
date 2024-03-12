#include "../headers/Server.hpp"

// Set up a server socket that listens for incoming connections on aspecified port.
void Server::_getSocket(std::string Port) {
	int yes = 1;
	int status;
	struct addrinfo hint, *serverinfo, *tmp;

	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET; // IPv4 protocol
	hint.ai_socktype = SOCK_STREAM; // TCP socket
	hint.ai_protocol = getprotobyname("TCP")->p_proto; // set protocol to TCP

	status = getaddrinfo("0.0.0.0", Port.c_str(), &hint, &serverinfo);
	if (status != 0) {
		std::cout << "getaddrinfo() error: " << gai_strerror(status) << std::endl;
		exit(-1);
	}

	// iterate through serverinfo and try to create socket
	for (tmp = serverinfo; tmp != NULL; tmp = tmp->ai_next) {
		this->_socketfd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (this->_socketfd < 0)
			continue;

		// if success: set SO_REUSEADDR to allow for reuse of local addresses
		setsockopt(this->_socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

		// bind the socket to the address and port
		if (bind(this->_socketfd, tmp->ai_addr, tmp->ai_addrlen) < 0) {
			close(this->_socketfd);
			continue;
		}

		// if bind success -> break;
		break;
	}

	// free memory allocated by getaddrinfo
	freeaddrinfo(serverinfo);

	// Error handling
	if (tmp == NULL) { // indicates that the socket was not successfully bound
		std::cout << "bind() error: " << strerror(errno) << std::endl;
		exit(-1);
	}

	// if socket successfully created and bound
	if (listen(this->_socketfd, this->_max_online_c) == -1) {
		std::cout << "listen() error: " << strerror(errno) << std::endl;
		exit(-1);
	}
}
