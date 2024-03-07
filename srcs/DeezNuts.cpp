#include "../headers/Server.hpp"


std::string	Server::_getBotMessage() {
	std::string Greeting = BLUE;
	Greeting.append("\n\n\n\n\n\n\n██████████████████████████████████████████████████████████████████████████████████\n");
	Greeting.append("█                                                                                █\n");
	Greeting.append("█\t ██████╗ ███████╗███████╗███████╗███╗   ██╗██╗   ██╗████████╗███████╗\t █\n");
	Greeting.append("█\t ██╔══██╗██╔════╝██╔════╝╚══███╔╝████╗  ██║██║   ██║╚══██╔══╝██╔════╝\t █\n");
	Greeting.append("█\t ██║  ██║█████╗  █████╗    ███╔╝ ██╔██╗ ██║██║   ██║   ██║   ███████╗\t █\n");
	Greeting.append("█\t ██║  ██║██╔══╝  ██╔══╝   ███╔╝  ██║╚██╗██║██║   ██║   ██║   ╚════██║\t █\n");
	Greeting.append("█\t ██████╔╝███████╗███████╗███████╗██║ ╚████║╚██████╔╝   ██║   ███████║\t █\n");
	Greeting.append("█\t ╚═════╝ ╚══════╝╚══════╝╚══════╝╚═╝  ╚═══╝ ╚═════╝    ╚═╝   ╚══════╝\t █\n");
	Greeting.append("█                                                                                █\n");
	Greeting.append("██████████████████████████████████████████████████████████████████████████████████\n");
	Greeting.append(BLUE "█                                                                                █\n");
	Greeting.append(BLUE "█\t" RED " Usage: DeezNuts [COMMAND_NUMBER] [MORE_OPTIONS]" BLUE "\r\t\t\t\t\t\t\t\t\t\t █\n");
	Greeting.append(BLUE "█\t How Can I Help You: (You can use The following list of commands)\r\t\t\t\t\t\t\t\t\t\t █\n");
	Greeting.append(BLUE "█\t " CYAN "[0]" RESET " : to List all Your stats" BLUE "\r\t\t\t\t\t\t\t\t\t\t █\n");
	Greeting.append(BLUE "█\t " CYAN "[1]" RESET " : to List all Your Joined Channels" BLUE "\r\t\t\t\t\t\t\t\t\t\t █\n");
	Greeting.append(BLUE "█\t " CYAN "[2]" RESET " : to see How many user online" BLUE "\r\t\t\t\t\t\t\t\t\t\t █\n");
	Greeting.append(BLUE "█\t " CYAN "[3]" RESET " : to List all Channels in Server" BLUE "\r\t\t\t\t\t\t\t\t\t\t █\n");
	Greeting.append(BLUE "█\t " CYAN "[4]" RESET " : to List stats of specific Channel" BLUE "\r\t\t\t\t\t\t\t\t\t\t █\n");
	Greeting.append(BLUE "█\t " CYAN "[5]" RESET " : to List Infos about the Server" BLUE "\r\t\t\t\t\t\t\t\t\t\t █\n");
	Greeting.append(BLUE "█                                                                                █\n");
	Greeting.append(BLUE "██████████████████████████████████████████████████████████████████████████████████\n\n\n\n");
	Greeting.append(RESET);
	return (Greeting);
}

std::string	Server::_DeezNuts( Request req, int i ) {
	std::string Greeting(_getBotMessage());

	if (req.args.size() == 0) {
		return (Greeting);
	}

	if (req.args.size() > 0) {
		if (req.args[0] == "0")
			return (this->_clients[i]->getUserInfo());

		else if (req.args[0] == "1")
			return (this->_clients[i]->getAllChannels());

		else if (req.args[0] == "2") {
            std::stringstream ss_online_c;
            ss_online_c << this->_online_c - 1;
            std::string tmp_online_c = ss_online_c.str();
            return ("Online Users: " + tmp_online_c + "\n");
        }

		else if (req.args[0] == "3")
			return (_listAllChannels());

		else if (req.args[0] == "4") {
			if (req.args.size() == 2)
				return(_channelInfo(req.args[1], i));
			else
				return ("Usage of this Command: DEEZNUTS 4 [CHANNEL NAME]\n");
		}

		else if (req.args[0] == "5")
			return (_serverInfo());

		else
			return (Greeting);
	}
	return (Greeting);
}

std::string Server::_serverInfo() const {
	std::string server("Server Name: " + this->_name + "\n");

    std::stringstream ss_online_c;
    ss_online_c << this->_online_c - 1;
    std::string tmp_online_c = ss_online_c.str();
	server.append("Online Users: " + tmp_online_c + "\n");

    std::stringstream ss_max_online;
    ss_max_online << this->_max_online_c;
    std::string tmp_max_online = ss_max_online.str();
	server.append("Max Online Users: " + tmp_max_online + "\n");

    std::stringstream ss_allChannels;
    ss_allChannels << this->_allChannels.size();
    std::string tmp_allChannels = ss_allChannels.str();
	server.append("Number of Channels in the Server: " + tmp_allChannels + "\n");

	return (server);
}

std::string Server::_channelInfo(std::string ChannelName, int i) {
	std::map<std::string, Channel *>::const_iterator it = this->_allChannels.find(ChannelName);

	if (it != this->_allChannels.end()) {
		if (this->_clients[i]->isJoined(ChannelName)) {
			std::string Info;
			Info.append("Channel Name: " + it->second->getName() + "\n");
			Info.append("Channel Creator: " + it->second->getCreator()->getFullName() + "\n");

            std::stringstream ss_onlineUsers;
            ss_onlineUsers << it->second->getOnlineUsers();
            std::string tmp_onlineUsers = ss_onlineUsers.str();
			Info.append("Online Users: " + tmp_onlineUsers + "\n");

			Info.append("Channel Topic: " + it->second->getTopic() + "\n");

			return (Info);
		}
		else
			return ("You Need To Join th channel first\n");
	}
	return ("There's No Channel Named " + ChannelName + "!\n");
}

std::string	Server::_listAllChannels() const {
	std::string channels(YELLOW "███████████████████████████████████████████████████████████████████████████████████████\n");

	channels.append("█              █              █                    █                                  █\n");
	channels.append("█" RESET " Channel Name " YELLOW "█ " RESET "Online Users " YELLOW "█ " RESET "Creator Of Channel " YELLOW "█ " RESET "          Channel Topic          " YELLOW "█\n");
	channels.append("█              █              █                    █                                  █\n");
	channels.append("███████████████████████████████████████████████████████████████████████████████████████\n");

	std::map<std::string, Channel *>::const_iterator it = this->_allChannels.begin();
	while (it != this->_allChannels.end()) {
		channels.append("█              █              █                    █                                  █\n");
		channels.append("█ " RESET + fillIt(it->first, 12));

        std::stringstream ss_onlineUsers;
        ss_onlineUsers << it->second->getOnlineUsers();
        std::string tmp_onlineUsers = ss_onlineUsers.str();
        channels.append(YELLOW " █      " RESET + fillIt(tmp_onlineUsers, 7));

		channels.append(YELLOW " █ " RESET + fillIt(it->second->getCreator()->getFullName(), 18));
		channels.append(YELLOW " █ " RESET + fillIt(it->second->getTopic(), 32));
		channels.append(YELLOW " █\n");
		channels.append(YELLOW "█              █              █                    █                                  █\n");
		channels.append("███████████████████████████████████████████████████████████████████████████████████████\n");
		it++;
	}

	if (this->_allChannels.size() == 0) {
		channels.append("█                                                                                     █\n");
		channels.append("█                               " RESET "NO CHANNEL IN THE SERVER" YELLOW "                              █\n");
		channels.append("█                                                                                     █\n");
		channels.append("███████████████████████████████████████████████████████████████████████████████████████\n");
	}

	channels.append(RESET "\n\n");
	return (channels);
}
