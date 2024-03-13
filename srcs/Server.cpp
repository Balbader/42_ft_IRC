#include "../headers/Server.hpp"

// Constructor
Server::Server() : _name(), _password(), _socketfd(0), _clients(), _pfds(NULL), _online_c(0), _max_online_c(0), _prefix(":"), _allChannels(), _unavailableUserName(), _clientNicknames() {}

// Overload
Server::Server(std::string Name, int max_online, std::string Port, std::string Password): _clients() {
	this->_name = Name;
	this->_max_online_c = max_online + 1;
	this->_password = Password;
	this->_online_c = 0;
	this->_pfds = new struct pollfd[max_online];
	_getSocket(Port);
	this->_pfds[0].fd = this->_socketfd;
	this->_pfds[0].events = POLLIN;
	this->_online_c++;
}

// Destructor
Server::~Server() {
	if (this->_pfds)
		delete [] this->_pfds;

	for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != _clients.end(); ++it) {
		delete it->second;
	}
	this->_clients.clear();

	for (std::map<std::string, Channel*>::iterator itC = this->_allChannels.begin(); itC != _allChannels.end(); ++itC) {
		delete itC->second;
	}
	this->_allChannels.clear();
}

// Methods
std::string	Server::_printMessage(std::string num, std::string nickname, std::string message) {
	if (nickname.empty())
		nickname = "*";

	return (":" + this->_name + " " + num + " " + nickname + " " + message + "\n");
}

void sendClient(int fd_client, std::string msg) {

    std::cout << "[server]: " << fd_client << " sent this request :" << msg << "\n" << std::endl;

    if (send(fd_client, msg.c_str(), msg.length(), 0) == -1)
        std::cout << "send() error: " << strerror(errno) << std::endl;
}

// handle new client connection
// accepts the connection
// updates the poll array
// sends a welcome message
// logs the event
void Server::_newClient(void) {
    struct sockaddr_storage remotaddr;
    socklen_t addrlen = sizeof(remotaddr);

	// accept new connection on the server's listening socket '_socketfd'
	// stores the client's address in 'remotaddr'
	// stores the file descriptor for the new connection in newfd
    int newfd = accept(this->_socketfd, (struct sockaddr*)&remotaddr, &addrlen);

	// if it fails
    if (newfd == -1) {
        std::cerr << "accept() error: " << strerror(errno) << std::endl;
    }
	
	// if success
	else {
        _addToPoll(newfd); // Add the new client to the poll array

        std::string welcome = _welcomemsg();
        if (!welcome.empty()) {
            sendClient(newfd, welcome); // Send a welcome message to the client
        }

        // Logging the new connection
        char ipstr[INET6_ADDRSTRLEN];
        void *addr = &((struct sockaddr_in*)&remotaddr)->sin_addr;
        inet_ntop(remotaddr.ss_family, addr, ipstr, sizeof(ipstr));
        std::cout << "[" << currentDateTime() << "]: New connection from " << ipstr << " on socket " << newfd << std::endl;
    }
}


// main loop responsible for handling incoming connections and client requests
void Server::startServer(void) {
	
	// infinite loop continuously checking for new events
	while (77) {
		int poll_count = poll(this->_pfds, this->_online_c, -1);

		// check error
		if (poll_count == -1) {
			std::cout << "poll() error: " << strerror(errno) << std::endl;
			exit(-1);
		}

		// iterates through the array of 'pollfd' structures
		for (int i = 0; i < this->_online_c; i++) {
			if (this->_pfds[i].revents & POLLIN) {
				if (this->_pfds[i].fd == this->_socketfd)
					_newClient();			// If listener is ready to read, handle new connection
				else
					_ClientRequest(i);		// If not the listener, we're just a regular client
			}
		}
	}
}

std::string	Server::_getPassword() const {
    return (this->_password);
}
