#include "../headers/Server.hpp"

std::string	Server::_welcomemsg(void) {
	std::string welcome = RED;

	welcome.append("Welcome young padawan!\n");
	welcome.append(BLUE);
	welcome.append("You need to login so you can start chatting OR you can send HELP to see how :) \n");
	welcome.append(RESET);

	return (welcome);
}

int	Server::_sendall(int destfd, std::string message) {
	int total = 0; // track total number of bytes sent
	int bytesleft = message.length(); // keep track of how many bytes still need to be sent
	int b; // used to store the result of each send call


	// print to console the descriptor of the destination and the message that is being sent
    // std::cout << "[server]: " << destfd << " _sendall : " << message << std::endl;

	while (total < (int)message.length()) {
		b = send(destfd, message.c_str() + total, bytesleft, 0);
        std::cout << "[server]: " << destfd << " _sendall : " << message << std::endl;

		if (b == -1) // if an error has occured
            break;

		total += b;
		bytesleft -= b;
	}

	// if success returns 0 else returns -1
	return (b == -1 ? -1 : 0);
}


// ensures that all active participants in a channel receive messages and updates
std::string	Server::_sendToAllUsers( Channel *channel, int senderFd, std::string message) {

	// retrieves al lusers in channel
	std::map<int, Client*> allusers = channel->getAllUsers();

	// init reply message
	std::string reply = this->_clients[senderFd]->getUserPerfix();
	reply.append(message);

	// iterate over each user
	for (std::map<int, Client*>::iterator it = allusers.begin(); it != allusers.end(); ++it) {
		// check if user's fd is not senderFd
		// this is to make sure that the message is not sent back to the sender
		if (senderFd != it->first)
			if (_sendall(it->first, reply) == -1) {
				std::cout << "_sendall() error: " << strerror(errno) << std::endl;
				return ("");
			}
	}
	return ("");
}
