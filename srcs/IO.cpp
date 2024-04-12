#include <Server.hpp>

/*
 * The receiveDataFromSocket method in the Server class handles the task of receiving data
 * from a client socket. It reads from the socket, processes the received data,
 * and handles potential disconnection scenarios.
*/
std::string Server::receiveDataFromSocket(int fd) {
    // initialization
	char	buffer[BUFFER_SIZE];
	ssize_t bytesReceived;
	bool	keepReading = true;
	std::string bufferStr = "";
	
    // logging
	std::ostringstream logMessage;
	logMessage << "Receiving data from fd " << fd;
	LOGGER.info("receiveDataFromSocket", logMessage.str());

    // reading loop
	while (keepReading) {
		std::memset(buffer, 0, BUFFER_SIZE);
		bytesReceived = recv(fd, buffer, BUFFER_SIZE, 0);

        // check for data receipt
		if (bytesReceived <= 0) {
			unexpectedDisconnectHandling(fd);
			keepReading = false;
		} else {
			bufferStr.append(buffer);
			if (bytesReceived < BUFFER_SIZE) {
				keepReading = false;
			}
		}
	}

    // return accumulated data
	return bufferStr;
}


/*
 * The sendDataThroughSocket method in the Server class is designed to handle the sending of data
 * to a specific client. This method encapsulates the necessary steps to communicate with the client
 * over a network socket, including error handling and logging
*/
void Server::sendDataThroughSocket(Client &client) {
	int	bytesSent;

    // initialize logging
	std::ostringstream logMessage;
	logMessage << "Sending data to fd " << client.getFd();

    // check data availability
	if (client.getSendData().size()) {

        // log the sending attempt
		LOGGER.info("sendDataThroughSocket", logMessage.str());

        // send data
		bytesSent = send(client.getFd(), client.getSendData().c_str(), client.getSendData().size(), 0);

        // error handling
		if (bytesSent < 0) {
			LOGGER.error("sendDataThroughSocket", "Failed to send data");
		} else { // log sent data and reset buffer
			LOGGER.info("sendDataThroughSocket", client.getSendData());
			client.resetSendData(bytesSent);
		}
	}
}
