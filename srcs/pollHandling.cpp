#include "../headers/Server.hpp"

/*
 * Add a new file descriptor (typically representing a new client connection)
 * to the server's array of pollfd structures, which is used with the poll system
 * call to monitor multiple file descriptors for events like read availability.
 * This function also manages the dynamic resizing of the pollfd array and
 * the addition of the new client to the server's client management.
*/
void Server::_addToPoll(int newfd) {

	// check if the current number of online clients has reached the maximum capacity.
	if (this->_online_c == this->_max_online_c) {

		// doubles the _max_online_c
		this->_max_online_c *= 2;

		// reallocate the _pfds array to accomodate more pollfd structures
		this->_pfds = (struct pollfd *)realloc(this->_pfds, this->_max_online_c);
	}

	// set the file descriptor 'fd' to the file descriptor of the new client
	this->_pfds[this->_online_c].fd = newfd;

	// set events to POLLIN: indicates that the server is interested in read events on the new fd
	this->_pfds[this->_online_c].events = POLLIN;

	// insert a new 'Client' object in the _clients map
	this->_clients.insert(std::pair<int, Client*>(newfd, new Client(newfd)));

	// increment the _online_c counter
	this->_online_c++;
}


// remove all file descriptor associated with a client from the server 'pollfdf' array
// update the server's record accordingly
// This function is called when a client disconnects or when the server needs to close the connection
void Server::_removeFromPoll(int i) {
	// store the fd to remove
	int fdToRemove = this->_pfds[i].fd;

	// close the fd
	close(fdToRemove);

	// Erase the client user the original file descriptor
	this->_clients.erase(fdToRemove);

	// Move the last pollfd struct to the current position
	this->_pfds[i] = this->_pfds[this->_online_c - 1];

	// Decrement the count of online clients
	this->_online_c--;
}
