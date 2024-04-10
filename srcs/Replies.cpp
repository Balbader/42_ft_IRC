#include <Server.hpp>

std::string Server::needmoreparams(Client &client, std::string command) {
	std::stringstream ss;

	LOGGER.info("needmoreparams", "Sending needmoreparams response to " + client.getNickname());
	ss << ":localhost 461 " << client.getNickname();
	ss << " " << command << " :Not enough parameters";
	ss << "\r\n";

	return ss.str();
}

std::string Server::alreadyregistered(Client &client) {
	std::stringstream ss;

	LOGGER.info("alreadyregistered", "Sending alreadyregistered response to " + client.getNickname());
	ss << ":localhost 462 " << client.getNickname();
	ss << " :You may not reregister";
	ss << "\r\n";

	return ss.str();
}

std::string Server::passwdmismatch(Client &client) {
	std::stringstream ss;

	LOGGER.info("passwdmismatch", "Sending passwdmismatch response to " + client.getNickname());
	ss << ":localhost 464 " << client.getNickname();
	ss << " :Password incorrect";
	ss << "\r\n";

	return ss.str();
}

std::string Server::nonicknamegiven(Client &client) {
	std::stringstream ss;

	LOGGER.info("nonicknamegiven", "Sending nonicknamegiven response to " + client.getNickname());
	ss << ":localhost 431 " << client.getNickname();
	ss << " :No nickname given";
	ss << "\r\n";

	return ss.str();
}

std::string Server::erroneusnickname(Client &client, std::string nickname) {
	std::stringstream ss;

	LOGGER.info("erroneusnickname", "Sending erroneusnickname response to " + client.getNickname());
	ss << ":localhost 432 " << client.getNickname();
	ss << " " << nickname << " :Erroneus nickname";
	ss << "\r\n";

	return ss.str();
}

std::string Server::nicknameinuse(Client &client, std::string nickname) {
	std::stringstream ss;

	LOGGER.info("nicknameinuse", "Sending nicknameinuse response to " + client.getNickname());
	ss << ":localhost 433 " << client.getNickname();
	ss << " " << nickname << " :Nickname is already in use";
	ss << "\r\n";

	return ss.str();
}

std::string Server::changednickname(Client &client, std::string nickname) {
	std::stringstream ss;
	
	LOGGER.info("welcome", "Sending welcome response to " + nickname);
	ss << ":localhost 001 " << client.getNickname();
	ss << " :Welcome to the FT_IRC server " << client.getNickname();
	ss << "\r\n";

	return ss.str();
}

std::string Server::welcome(Client &client) {
	std::stringstream ss;

	LOGGER.info("welcome", "Sending welcome response to " + client.getNickname());
	ss << ":localhost 001 " << client.getNickname();
	ss << " :Welcome to the FT_IRC server " << client.getNickname();
	ss << "\r\n";

	ss << ":localhost 002 " << client.getNickname();
	ss << " :Your host is localhost, running version 0.1";
	ss << "\r\n";

	ss << ":localhost 003 " << client.getNickname();
	ss << " :This server was created " << creationDatetime;
	ss << "\r\n";

	ss << ":localhost 004 " << client.getNickname();
	ss << " localhost 0.1 iowstRb- biklmnopstvrRcCNuMTD";
	ss << "\r\n";

	return ss.str();
}

std::string Server::motd(Client &client) {
	std::stringstream ss;

	LOGGER.info("motd", "Sending motd response to " + client.getNickname());
	ss << ":localhost 375 " << client.getNickname();
	ss << " :- localhost Message of the day -";
	ss << "\r\n";

	ss << ":localhost 372 " << client.getNickname();
	ss << " :- Welcome to FT_IRC!";
	ss << "\r\n";

	ss << ":localhost 376 " << client.getNickname();
	ss << " :End of MOTD command.";
	ss << "\r\n";

	return ss.str();
}

std::string Server::nooperhost(Client &client) {
	std::stringstream ss;

	LOGGER.info("nooperhost", "Sending nooperhost response to " + client.getNickname());
	ss << ":localhost 491 " << client.getNickname();
	ss << " :No O-lines for your host";
	ss << "\r\n";

	return ss.str();
}

std::string Server::youreoper(Client &client) {
	std::stringstream ss;

	LOGGER.info("youreoper", "Sending youreoper response to " + client.getNickname());
	ss << ":localhost 381 " << client.getNickname();
	ss << " :You are now an IRC operator";
	ss << "\r\n";

	return ss.str();
}

std::string Server::unknowncommand(Client &client, std::string command) {
	std::stringstream ss;

	LOGGER.info("unknowncommand", "Sending unknowncommand response to " + client.getNickname());
	ss << ":localhost 421 " << client.getNickname();
	ss << " " << command;
	ss << " :Unknown command";
	ss << "\r\n";

	return ss.str();
}

std::string Server::nosuchchannel(Client &client, std::string name) {
	std::stringstream ss;

	LOGGER.info("nosuchchannel", "Sending nosuchchannel response to " + client.getNickname());
	ss << ":localhost 403 " << client.getNickname();
	ss << " " << name;
	ss << " :No such channel";
	ss << "\r\n";

	return ss.str();
}

std::string Server::topic(Client &client, Channel &channel) {
	std::stringstream ss;

	LOGGER.info("topic", "Sending topic response to " + client.getNickname());
	ss << ":localhost 332 " << client.getNickname();
	ss << " " << channel.getName();
	ss << " " << channel.getTopic();
	ss << "\r\n";

	return ss.str();
}

std::string Server::notopic(Client &client, Channel &channel) {
	std::stringstream ss;

	LOGGER.info("notopic", "Sending notopic response to " + client.getNickname());
	ss << ":localhost 331 " << client.getNickname();
	ss << " " << channel.getName();
	ss << " :No topic is set";
	ss << "\r\n";

	return ss.str();
}

std::string Server::namreply(Client &client, Channel &channel, bool found) {
	std::stringstream ss;

	LOGGER.info("namreply", "Sending namreply response to " + client.getNickname());
	if (found) {
		Client *creator = &channel.getCreator();

		ss << ":localhost 353 " << client.getNickname();
		ss << " = " << channel.getName() << " :";
		if (creator) {
			ss << "!" << creator->getNickname() << " ";
		}

		std::map<Client *, unsigned int>::iterator cli =
			channel.getClients().begin();

		while (cli != channel.getClients().end()) {
			if (cli->second & USER_OPERATOR) {
				ss << "@";
			}
			ss << (*cli->first).getNickname() << " ";
			cli++;
		}
		ss << "\r\n";
	}

	ss << ":localhost 366 " << client.getNickname();
	ss << " " << channel.getName();
	ss << " :End of NAMES list";
	ss << "\r\n";

	return ss.str();
}

std::string Server::whoisreply(Client &client) {
	std::stringstream ss;

	LOGGER.info("whoisreply", "Sending whoisreply response to " + client.getNickname());
	ss << ":localhost 311 " << client.getNickname();
	ss << " " << client.getNickname();
	ss << " " << client.getUsername();
	ss << " localhost";
	if (client.getOp()) {
		ss << " *";
	}
	ss << " " << client.getRealname();
	ss << "\r\n";

	ss << ":localhost 319 " << client.getNickname();
	ss << " " << client.getNickname();
	ss << " :";

	std::vector<Channel *>::iterator it = client.getChannels().begin();

	for (; it != client.getChannels().end(); it++) {
		std::map<Client *, uint> clients = (*it)->getClients();
		uint					 modes	 = clients[&client];

		if (modes & USER_OPERATOR) {
			ss << "@" << (*it)->getName();
			ss << " ";
		} else {
			ss << (*it)->getName();
			ss << " ";
		}
	}

	ss << "\r\n";

	ss << ":localhost 312 " << client.getNickname();
	ss << " " << client.getNickname();
	ss << " localhost";
	ss << " :Your host is localhost, running version 0.1";
	ss << "\r\n";

	ss << ":localhost 317 " << client.getNickname();
	ss << " " << client.getNickname();
	ss << " 12345678";
	ss << " 98765432";
	ss << "\r\n";

	ss << ":localhost 318 " << client.getNickname();
	ss << " " << client.getNickname();
	ss << " :End of WHOIS list";
	ss << "\r\n";

	return ss.str();
}

std::string Server::whoreply(Client &client, Channel &channel) {
	std::stringstream						   ss;
	std::map<Client *, unsigned int>		   clients = channel.getClients();
	std::map<Client *, unsigned int>::iterator cli	   = clients.begin();

	LOGGER.info("whoreply", "Sending whoreply response to " + client.getNickname());
	for (; cli != clients.end(); cli++) {
		if (!cli->first->isVisible()) {
			continue;
		}
		ss << ":localhost 352 " << client.getNickname();
		ss << " " << channel.getName();
		ss << " " << (*cli->first).getUsername();
		ss << " " << (*cli->first).getHostname();
		ss << " " << (*cli->first).getServername();
		ss << " " << (*cli->first).getNickname();

		if ((*cli->first).getOp()) {
			ss << " G";
		} else {
			ss << " H";
		}
		if (cli->second & USER_OPERATOR) {
			ss << "@";
		}

		ss << " :0";
		ss << " " << (*cli->first).getRealname();
		ss << "\r\n";
	}

	ss << ":localhost 315";
	ss << " " << client.getNickname();
	ss << " " << channel.getName();
	ss << " :End of WHO list";
	ss << "\r\n";

	return ss.str();
}

std::string Server::nosuchserver(Client &client, std::string name) {
	std::stringstream ss;

	LOGGER.info("nosuchserver", "Sending nosuchserver response to " + client.getNickname());
	ss << ":localhost 402";
	ss << " " << client.getNickname();
	ss << " " << name;
	ss << " :No such server";
	ss << "\r\n";

	return ss.str();
}

std::string Server::badchannelkey(Client &client, std::string channel) {
	std::stringstream ss;

	LOGGER.info("badchannelkey", "Sending badchannelkey response to " + client.getNickname());
	ss << ":localhost 475";
	ss << " " << client.getNickname();
	ss << " " << channel;
	ss << " :Cannot join channel (+k)";
	ss << "\r\n";

	return ss.str();
}

std::string Server::unknownmode(Client &client, char c) {
	std::stringstream ss;

	LOGGER.info("unknownmode", "Sending unknownmode response to " + client.getNickname());
	ss << ":localhost 472";
	ss << " " << client.getNickname();
	ss << " " << c;
	ss << " :is unknown mode char to me";
	ss << "\r\n";

	return ss.str();
}

std::string Server::usermodeis(Channel &channel, Client &client, std::string modeStr) {
	std::stringstream ss;

	LOGGER.info("usermodeis", "Sending usermodeis response to " + client.getNickname());
	ss << ":" << client.getNickname();
	ss << " MODE";
	ss << " " << channel.getName();
	ss << " " << modeStr;
	ss << "\r\n";
	return ss.str();
}

std::string Server::usermodeis(Client &client, std::string modeStr) {
	std::stringstream ss;

	LOGGER.info("usermodeis", "Sending usermodeis response to " + client.getNickname());
	ss << ":" << client.getNickname();
	ss << " MODE 221";
	ss << " :" << modeStr;
	ss << "\r\n";
	return ss.str();
}

std::string Server::usermodeis(Client &client) {
	std::stringstream ss;

	LOGGER.info("usermodeis", "Sending usermodeis response to " + client.getNickname());
	ss << ":localhost 221";
	ss << " " << client.getNickname();
	ss << " " << client.getModesStr();
	ss << "\r\n";
	return ss.str();
}

std::string Server::channelmodeis(Client &client, std::string channel) {
	std::stringstream ss;

	LOGGER.info("channelmodeis", "Sending channelmodeis response to " + client.getNickname());
	std::map<std::string, Channel>::iterator ch_it = getChannelByName(channel);
	if (ch_it == channels.end()) {
		return nosuchchannel(client, channel);
	}
	Channel *ch = &(ch_it->second);

	ss << ":localhost 324";
	ss << " " << client.getNickname();
	ss << " " << ch->getName();
	ss << " " << ch->getStrModes();
	ss << "\r\n";

	return ss.str();
}

std::string Server::usersdontmatch(Client &client) {
	std::stringstream ss;

	LOGGER.info("usersdontmatch", "Sending usersdontmatch response to " + client.getNickname());
	ss << ":localhost 502";
	ss << " " << client.getNickname();
	ss << " :Cannot change mode for other users";
	ss << "\r\n";
	return ss.str();
};

std::string Server::unknownmodeflag(Client &client) {
	std::stringstream ss;

	LOGGER.info("unknownmodeflag", "Sending unknownmodeflag response to " + client.getNickname());
	ss << ":localhost 501";
	ss << " " << client.getNickname();
	ss << " :Unknown MODE flag";
	ss << "\r\n";
	return ss.str();
};
std::string Server::notonchannel(Client &client, std::string name) {
	std::stringstream ss;

	LOGGER.info("notonchannel", "Sending notonchannel response to " + client.getNickname());
	ss << ":localhost 442";
	ss << " " << client.getNickname();
	ss << " " << name;
	ss << " :You're not on that channel";
	ss << "\r\n";

	return ss.str();
}

std::string Server::chanoprivsneeded(int fd, Channel &channel) {
	std::stringstream ss;
	Client		   &client = clients.at(fd);

	LOGGER.info("chanoprivsneeded", "Sending chanoprivsneeded response to " + client.getNickname());
	ss << ":localhost 482";
	ss << " " << client.getNickname();
	ss << " " << channel.getName();
	ss << " :You're not channel operator";
	ss << "\r\n";

	return ss.str();
}

std::string Server::kicksuccess(Client &client, Channel &ch, std::string target, std::string reason) {
	std::stringstream ss;

	LOGGER.info("kicksuccess", "Sending kicksuccess response to " + client.getNickname());
	ss << ":" << client.getNickname();
	ss << " KICK " << ch.getName();
	ss << " " << target;
	ss << " :" << reason;
	ss << "\r\n";
	return ss.str();
};

std::string Server::usernotinchannel(Client &client, Channel &channel) {
	std::stringstream ss;

	LOGGER.info("usernotinchannel", "Sending usernotinchannel response to " + client.getNickname());
	ss << ":localhost 441";
	ss << " " << client.getNickname();
	ss << " " << channel.getName();
	ss << " :They aren't on that channel";
	ss << "\r\n";

	return ss.str();
}

std::string Server::chanoprivsneeded(Client &client, Channel &channel) {
	std::stringstream ss;

	LOGGER.info("chanoprivsneeded", "Sending chanoprivsneeded response to " + client.getNickname());
	ss << ":localhost 482";
	ss << " " << client.getNickname();
	ss << " " << channel.getName();
	ss << " :You're not channel operator";
	ss << "\r\n";

	return ss.str();
}

std::string Server::norecipient(Client &client, std::string name) {
	std::stringstream ss;

	LOGGER.info("norecipient", "Sending norecipient response to " + client.getNickname());
	ss << ":localhost 411";
	ss << " " << client.getNickname();
	ss << " :No recipient given (";
	ss << name << ")";
	ss << "\r\n";

	return ss.str();
}

std::string Server::notexttosend(Client &client) {
	std::stringstream ss;

	LOGGER.info("notexttosend", "Sending notexttosend response to " + client.getNickname());
	ss << ":localhost 412";
	ss << " " << client.getNickname();
	ss << " :No text to send";
	ss << "\r\n";

	return ss.str();
}

std::string Server::nosuchnick(Client &client, std::string name) {
	std::stringstream ss;

	LOGGER.info("nosuchnick", "Sending nosuchnick response to " + client.getNickname());
	ss << ":localhost 401";
	ss << " " << client.getNickname();
	ss << " " << name;
	ss << " :No such nick/channel";
	ss << "\r\n";

	return ss.str();
}

std::string Server::inviting(Client &issuer, Channel &channel) {
	std::stringstream ss;

	LOGGER.info("inviting", "Sending inviting response to " + issuer.getNickname());
	ss << ":localhost 341 " << issuer.getNickname();
	ss << " " << channel.getName();
	ss << " " << issuer.getNickname();
	ss << "\r\n";

	return ss.str();
}

std::string Server::inviterrpl(Client &issuer, Client &target, Channel &channel) {
	std::stringstream ss;

	LOGGER.info("inviterrpl", "Sending inviterrpl response to " + issuer.getNickname());
	ss << ":" << issuer.getNickname();
	ss << " INVITE";
	ss << " " << target.getNickname();
	ss << " :" << channel.getName();
	ss << "\r\n";

	return ss.str();
}

std::string Server::inviteonlychan(Client &issuer, Channel &channel) {
	std::stringstream ss;

	LOGGER.info("inviteonlychan", "Sending inviteonlychan response to " + issuer.getNickname());
	ss << ":localhost 473";
	ss << " " << issuer.getNickname();
	ss << " " << channel.getName();
	ss << " :Cannot join channel (+i)";
	ss << "\r\n";

	return ss.str();
}

std::string Server::channelisfull(Client &issuer, Channel &channel) {
	std::stringstream ss;

	LOGGER.info("channelisfull", "Sending channelisfull response to " + issuer.getNickname());
	ss << ":localhost 471";
	ss << " " << issuer.getNickname();
	ss << " " << channel.getName();
	ss << " :Cannot join channel (+l)";
	ss << "\r\n";

	return ss.str();
}
