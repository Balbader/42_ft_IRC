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

void Server::cleanEndOfTransmission(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '\4'), str.end());
}

Command Server::messageToCommand(std::string source) {
	Command c;
	std::string token;

	cleanEndOfTransmission(source);
	if (source[0] == ':') {
		std::istringstream sourceStream(source);
		std::getline(sourceStream, c.prefix, ' ');
		source = source.substr(source.find(c.prefix) + c.prefix.size());
	}
	trimStart(source);
	std::istringstream sourceStream(source);
	std::getline(sourceStream, c.cmd, ' ');
	source = source.substr(source.find(c.cmd) + c.cmd.size());
	trimStart(source);
	size_t colonPos = source.find(':');
	if (colonPos != std::string::npos) {
		std::string beforeColon = source.substr(0, colonPos);
		std::string afterColon	= source.substr(colonPos);

		std::istringstream beforeColonSteam(beforeColon);

		while (std::getline(beforeColonSteam, token, ' ')) {
			trimAll(token);
			if (token.size() != 0) c.args.push_back(token);
		}
		c.args.push_back(afterColon);
	} else {
		std::istringstream sourceStream(source);

		while (std::getline(sourceStream, token, ' ')) {
			trimAll(token);
			if (token.size() != 0) c.args.push_back(token);
		}
	}
	return c;
}

std::vector<Command> Server::parseCommands(std::string data) {
	std::vector<Command> commands;
	size_t	pos;

	LOGGER.info("parseCommands", "Parsing commands from data: " + data);
	while (data.find("\r\n") != std::string::npos) {
		pos = data.find("\r\n");
		if (pos > 0) {
			commands.push_back(messageToCommand(data.substr(0, pos)));
			data.erase(0, pos + 2);
		}
	}
	return commands;
}

bool Server::evalChanMode(Client &client, std::vector<std::string> args) {
	std::string toggleMode = "it";
	std::string cmdsWithParams = "lko";
	std::string cmdPrefix = "+-";
	std::string allModes = toggleMode + cmdsWithParams;

	LOGGER.info("evalChanMode", "Evaluating channel mode");
	if (args.size() < 2) {
		client.setSendData(needmoreparams(client, "MODE"));
		return false;
	}

	std::string &modes = args[1];

	if (cmdPrefix.find(modes[0]) == std::string::npos) {
		client.setSendData(unknowncommand(client, "MODE"));
		return false;
	}

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
	if (togglePair.first && paramPair.first) {
		client.setSendData(unknownmode(client, '!'));
		return false;
	}

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

	client.setSendData(channelmodeis(client, args[0]));
	return false;
}

bool Server::evalUserMode(Client &client, std::vector<std::string> args) {
	LOGGER.info("evalUserMode", "Evaluating user mode");
	if (!(toIrcUpperCase(client.getNickname()) == toIrcUpperCase(args[0]))) {
		client.setSendData(usersdontmatch(client));
		return false;
	}

	std::string allowedFlags = "iwso";
	if (args[1].at(0) != '+' && args[1].at(0) != '-') {
		client.setSendData(unknownmodeflag(client));
		return false;
	}
	if (args[1].size() < 2) {
		return false;
	}

	size_t i = 1;
	while (i < args[1].size()) {
		if (allowedFlags.find(args[1].at(i), 0) == std::string::npos) {
			client.setSendData(unknownmodeflag(client));
			return false;
		}
		i++;
	}
	return true;
}

Client *Server::getClientByNick(std::string nickname) {
	Client *c = NULL;

	LOGGER.info("getClientByNick", "Looking for client " + nickname);
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
	return c;
};
