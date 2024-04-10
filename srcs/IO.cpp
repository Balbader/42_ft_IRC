#include <Server.hpp>

std::string Server::receiveDataFromSocket(int fd) {
	char	buffer[BUFFER_SIZE];
	ssize_t bytesReceived;
	bool	keepReading = true;
	std::string bufferStr = "";
	
	std::ostringstream logMessage;
	logMessage << "Receiving data from fd " << fd;
	LOGGER.info("receiveDataFromSocket", logMessage.str());
	while (keepReading) {
		std::memset(buffer, 0, BUFFER_SIZE);
		bytesReceived = recv(fd, buffer, BUFFER_SIZE, 0);
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
	return bufferStr;
}

void Server::sendDataThroughSocket(Client &client) {
	int	bytesSent;

	std::ostringstream logMessage;
	logMessage << "Sending data to fd " << client.getFd();
	if (client.getSendData().size()) {
		LOGGER.info("sendDataThroughSocket", logMessage.str());
		bytesSent = send(client.getFd(), client.getSendData().c_str(), client.getSendData().size(), 0);
		if (bytesSent < 0) {
			LOGGER.error("sendDataThroughSocket", "Failed to send data");
		} else {
			LOGGER.info("sendDataThroughSocket", client.getSendData());
			client.resetSendData(bytesSent);
		}
	}
}