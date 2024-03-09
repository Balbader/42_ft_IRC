#include "../headers/Server.hpp"

// Handle reception and process of client request, managing client connections
void Server::_ClientRequest(int i) {

	// buffer to store received data
	char buf[6000];

	// id the sender file descriptor
	int sender_fd = this->_pfds[i].fd;

	// store the number of bytes received
	int nbytes = recv(sender_fd, buf, sizeof(buf), 0);

	if (nbytes <= 0) { // check for error or disconnection
		if (nbytes == 0) // client has closed the connection
			std::cout << "[" << currentDateTime() << "]: socket " << sender_fd << " hung up" << std::endl;
		else // < 0 = error detected
			std::cout << "recv() error: " << strerror(errno) << std::endl;

		// close the client's socket and remove the client from the poll
		close(sender_fd);
		_removeFromPoll(i);
	}

	else {
		std::string message(buf, strlen(buf) - 1);

		if (message[message.size() - 1] == '\r')
			message.erase(message.end() - 1);

		// parse the client's request
		std::string ret = _parsing(message, this->_pfds[i].fd);
		if (send(sender_fd, ret.c_str(), ret.length(), 0) == -1)
			std::cout << "send() error: " << strerror(errno) << std::endl;
	}

	// clear & reset buffer for next request
	memset(&buf, 0, 6000);
}

Request	Server::_splitRequest(std::string req)
{
	Request	request;
	size_t	i = 0;
	size_t	j = 0;

	if (req[i] == ' ' || !req[i]) {
		request.invalidMessage = true;
		return (request);
	}

	j = i;
	while (req[i]) {
		if (req[i] == ' ') {
			if (req[i + 1] == ' ') {
				request.invalidMessage = true;
				return (request);
			}

			request.args.push_back(req.substr(j, i - j));

			while (req[i] == ' ')
				i++;

			j = i;
		}

		if (req[i] == ':') {
			if (req[i - 1] != ' ') {
				request.invalidMessage = true;
				return (request);
			}

			request.args.push_back(req.substr(i + 1, req.length() - i));
			request.command = request.args[0];
			request.args.erase(request.args.begin());

			return (request);
		}
		i++;
	}

	if (i && req[j])
		request.args.push_back(req.substr(j, i - j));

	request.command = request.args[0];
	request.args.erase(request.args.begin());

	return (request);
}
