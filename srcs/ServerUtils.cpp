#include <Server.hpp>

std::string Server::getDatetime() {
	std::time_t currentTime = std::time(NULL);
	std::tm*	localTime	= std::localtime(&currentTime);

	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);

	return std::string(buffer);
}

void Server::trimStart(std::string& str) {
	std::size_t start = 0;
	std::size_t end	  = str.length();

	while (start < end && str[start] == ' ') {
		start++;
	}
	str = str.substr(start);
}

void Server::trimAll(std::string& str) {
	std::size_t start = 0;
	std::size_t end	  = str.length();

	while (start < end && str[start] == ' ') {
		start++;
	}

	while (end > start && str[end - 1] == ' ') {
		end--;
	}
	str = str.substr(start, end - start);
}

/*
 * The cleanEndOfTransmission function in the Server class is designed to modify a string by removing
 * all instances of the End of Transmission (EOT) character, which is represented as '\4' in ASCII.
 * This character is typically used to indicate the end of data transmission, and it may be necessary
 * to remove it from strings received from network communications or before further processing or storage.
*/
void Server::cleanEndOfTransmission(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '\4'), str.end());
}

/*
 * The messageToCommand function in the Server class is designed to parse a raw string message received
 * from a client into a structured Command object. This parsing accounts for IRC-style message formats,
 * extracting components like the prefix, command, and arguments. 
*/
Command Server::messageToCommand(std::string source) {
	Command c;
	std::string token;

    // clean end transmission
	cleanEndOfTransmission(source);

    // check and extract prefix
	if (source[0] == ':') {
		std::istringstream sourceStream(source);
		std::getline(sourceStream, c.prefix, ' ');
		source = source.substr(source.find(c.prefix) + c.prefix.size());
	}

    // trim leading spaces
	trimStart(source);

    // extract command
	std::istringstream sourceStream(source);
	std::getline(sourceStream, c.cmd, ' ');
	source = source.substr(source.find(c.cmd) + c.cmd.size());
    // trim leading spaces
	trimStart(source);

    // extract arguments
	size_t colonPos = source.find(':');
	if (colonPos != std::string::npos) {
		std::string beforeColon = source.substr(0, colonPos);
		std::string afterColon	= source.substr(colonPos);

		std::istringstream beforeColonSteam(beforeColon);

		while (std::getline(beforeColonSteam, token, ' ')) {
			trimAll(token);
			if (token.size() != 0)
                c.args.push_back(token);
		}
		c.args.push_back(afterColon);
	} else {
		std::istringstream sourceStream(source);

		while (std::getline(sourceStream, token, ' ')) {
			trimAll(token);
			if (token.size() != 0)
                c.args.push_back(token);
		}
	}

    // return command
	return c;
}

std::vector<Command> Server::parseCommands(std::string data) {
	std::vector<Command> commands;
	size_t pos;

    // logging the parsing activity
	LOGGER.info("parseCommands", "Parsing commands from data: " + data);

    // command extraction loop
	while (data.find("\r\n") != std::string::npos) {
		pos = data.find("\r\n");
		if (pos > 0) {
			commands.push_back(messageToCommand(data.substr(0, pos)));
			data.erase(0, pos + 2);
		}
	}

    // return parsed commands
	return commands;
}

bool Server::evalChanMode(Client &client, std::vector<std::string> args) {
    // initial setup and logging
	std::string toggleMode = "it";
	std::string cmdsWithParams = "lko";
	std::string cmdPrefix = "+-";
	std::string allModes = toggleMode + cmdsWithParams;

	LOGGER.info("evalChanMode", "Evaluating channel mode");

    // check command validity
	if (args.size() < 2) {
		client.setSendData(needmoreparams(client, "MODE"));
		return false;
	}

    // validate mode prefix
	std::string &modes = args[1];
	if (cmdPrefix.find(modes[0]) == std::string::npos) {
		client.setSendData(unknowncommand(client, "MODE"));
		return false;
	}

    // mode parsing loop
	char prefix = modes.at(0);
	modes.erase(0, 1);
	std::pair<bool, unsigned int> togglePair = std::make_pair(false, 0);
	std::pair<bool, unsigned int> paramPair = std::make_pair(false, 0);
	std::string::iterator chars = modes.begin();

	while (chars != modes.end()) {
		if (toggleMode.find(chars.base()) != std::string::npos) {
			togglePair.first = true;
			togglePair.second += 1;
		} else if (cmdsWithParams.find(chars.base()) != std::string::npos) {
			paramPair.first = true;
			paramPair.second += 1;
		}
		chars++;
	}

    // error check for mixed modes
	if (togglePair.first && paramPair.first) {
		client.setSendData(unknownmode(client, '!'));
		return false;
	}

    // parameter requirements check
	if (togglePair.first)
		return true;

	if (paramPair.first) {
		if (args.size() < 3 && modes.at(0) == 'b') {
			return true;
		}
		if (prefix == '-' && modes.at(0) == 'k') {
			return true;
		}
		if (paramPair.second > 1 || args.size() < 2) {
			client.setSendData(needmoreparams(client, "MODE"));
			return false;
		}
		return true;
	}

    // fallback for improper mode handling
	client.setSendData(channelmodeis(client, args[0]));
	return false;
}

/*
 *The evalUserMode function in the Server class is designed to evaluate and validate
 the user mode settings requested by a client. User modes are settings that alter
 how the server and other clients interact with the user. 
*/
bool Server::evalUserMode(Client &client, std::vector<std::string> args) {
    // logging the mode evaluation
	LOGGER.info("evalUserMode", "Evaluating user mode");

    // verify user identity
	if (!(toIrcUpperCase(client.getNickname()) == toIrcUpperCase(args[0]))) {
		client.setSendData(usersdontmatch(client));
		return false;
	}

    // check mode prefix validity
	std::string allowedFlags = "iwso";
	if (args[1].at(0) != '+' && args[1].at(0) != '-') {
		client.setSendData(unknownmodeflag(client));
		return false;
	}

    // mode string length check
	if (args[1].size() < 2) {
		return false;
	}

    // validate mode flags
	size_t i = 1;
	while (i < args[1].size()) {
		if (allowedFlags.find(args[1].at(i), 0) == std::string::npos) {
			client.setSendData(unknownmodeflag(client));
			return false;
		}
		i++;
	}

    // successful mode validation
	return true;
}

Client *Server::getClientByNick(std::string nickname) {
	Client *c = NULL;

    // logging the search operation
	LOGGER.info("getClientByNick", "Looking for client " + nickname);

    // iteratin through clients
	std::map<int, Client>::iterator it;
	it = clients.begin();
	while (it != clients.end()) {
		if (toIrcUpperCase(nickname) ==
			toIrcUpperCase(it->second.getNickname())) {
			c = &it->second;
			break;
		}
		it++;
	}

    // return the result
	return c;
};
