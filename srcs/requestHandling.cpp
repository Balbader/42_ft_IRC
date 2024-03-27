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
        // split "message" et creer un tableau de string splitees selon \n
        // ouvrir une boucle qui s'arrete quand le tableau est fini
        // a chaque iteration => reste du code
		std::string ret = _parsing(message, this->_pfds[i].fd);
        std::cout << "ret :::: " << ret << std::endl;
        std::cout << "[server]: " << sender_fd << " _ClientRequest :" << message << std::endl;
		if (send(sender_fd, ret.c_str(), ret.length(), 0) == -1)
			std::cout << "send() error: " << strerror(errno) << std::endl;
        // if (ret == "CAP LS")
        //     return ;
	}

	// clear & reset buffer for next request
	memset(&buf, 0, 6000);
}

// Parse a raw request string from a client and organize the data into a structured 'Request' object.
// FIX: need to add a condition in regards '\n' as irssi requests are a bit funky compared to nc
// it seems that the return messages are not parsed correctly and when a connection is attempted in irssi,
// the server sends back serveral confirmation messages at the same time
Request	Server::_splitRequest(std::string req)
{
	// init request
	Request	request;
	size_t	i = 0;
	size_t	j = 0;

	// check if first char is ' ' || null
	if (req[i] == ' ' || !req[i]) {
		request.invalidMessage = true;
		return (request);
	}

	// parse
	j = i;
	while (req[i]) {
		// When a ' ' is encountered, means end of a command or argument
		if (req[i] == ' ') {
			if (req[i + 1] == ' ') {
				request.invalidMessage = true;
				return (request);
			}

			// extract substring and add to 'args' vector
			request.args.push_back(req.substr(j, i - j));

			// skip additional ' '
			while (req[i] == ' ')
				i++;

			j = i;
		}

		// When a ':' is encountered, means end of a command or argument
		if (req[i] == ':') {
			// check if last char is ' '
			if (req[i - 1] != ' ') {
				request.invalidMessage = true;
				return (request);
			}

			// extract substring from the char afer the ':' and add to 'args' vector
			request.args.push_back(req.substr(i + 1, req.length() - i));

			// set command field of the request object to the first argument
			request.command = request.args[0];

			// remove the argument from 'args' vector
			request.args.erase(request.args.begin());

			return (request);
		}
		i++;
	}

	// in case no ':' -> check if there is a remaining argument to add to 'args'
	if (i && req[j])
		request.args.push_back(req.substr(j, i - j));

	// set command field of the request object to the first argument
	request.command = request.args[0];

	// remove the argument from 'args' vector
	request.args.erase(request.args.begin());

	return (request);
}
