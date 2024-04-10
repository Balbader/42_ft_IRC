#include <Server.hpp>

void Server::pass(Client &client, Command &command) {
	LOGGER.info("pass", "Client " + client.getNickname() + " is trying to authenticate");
	if (command.args.size() == 0) {
		client.setSendData(needmoreparams(client, "PASS"));
		return;
	} else if (client.getRegistration() & PASS_FLAG) {
		client.setSendData(alreadyregistered(client));
		return;
	} else if (command.args[0] != passwd) {
		client.setSendData(passwdmismatch(client));
		return;
	}

	client.setKnowPassword(true);
	client.setRegistration(PASS_FLAG);
}

void Server::user(Client &client, Command &command) {
	LOGGER.info("user", "Client " + client.getNickname() + " is trying registry username");
	if (command.args.size() < 4) {
		client.setSendData(needmoreparams(client, "USER"));
		return;
	} else if (client.getRegistration() & USER_FLAG) {
		client.setSendData(alreadyregistered(client));
		return;
	}

	client.setUsername(command.args[0]);
	client.setHostname(command.args[1]);
	client.setServername(command.args[2]);
	client.setRealname(command.args[3].substr(1));
	client.setRegistration(USER_FLAG);
}

void Server::nick(Client &client, Command &command) {
	LOGGER.info("nick", "Client " + client.getNickname() + " is trying to change nickname");
	if (command.args.size() == 0) {
		client.setSendData(nonicknamegiven(client));
		return;
	} else if (!validNickname(command.args[0])) {
		client.setSendData(erroneusnickname(client, command.args[0]));
		return;
	} else if (nicknameAlreadyExists(command.args[0])) {
		client.setSendData(nicknameinuse(client, command.args[0]));
		return;
	}
	std::string oldNick = client.getNickname();
	client.setNickname(command.args[0]);
	if (client.getRegistration() & NICK_FLAG) {
		client.setSendData(":localhost 001 " + client.getNickname() + "\r\n");
	} else {
		client.setSendData(changednickname(client, client.getNickname()));
	}
	client.setRegistration(NICK_FLAG);
	LOGGER.info("nick", "Client " + client.getNickname() + " changed nickname");
}

void Server::oper(Client &client, Command &command) {
	LOGGER.info("oper", "Client " + client.getNickname() + " is trying to authenticate as operator");
	if (command.args.size() < 2) {
		client.setSendData(needmoreparams(client, command.cmd));
		return;
	} else if (command.args[1] != OPER_PASS) {
		client.setSendData(passwdmismatch(client));
		return;
	} else if (command.args[0] != OPER_USER) {
		client.setSendData(nooperhost(client));
		return;
	}

	if (client.setMode('o', true)) {
		client.setSendData(youreoper(client));
	}
}

void Server::privmsg(Client &client, Command &command) {
	std::stringstream ss;
	std::string		  ch_prefix = CHANNEL_PREFIX;

	LOGGER.info("privmsg", "Client " + client.getNickname() + " is trying to send a privmsg");
	if (command.args.size() == 1) {
		if (ch_prefix.find(command.args[0].at(0)) != std::string::npos)
			return client.setSendData(norecipient(client, "PRIVMSG"));
		else
			return client.setSendData(notexttosend(client));
	}

	if (command.args.size() < 2) {
		client.setSendData(needmoreparams(client, "PRIVMSG"));
		return;
	}
	ss << client.getClientPrefix();
	ss << " PRIVMSG";
	ss << " ";
	ss << command.args[0];
	ss << " ";
	ss << command.args[1];
	ss << "\r\n";

	if (ch_prefix.find(command.args[0].at(0)) != std::string::npos) {
		std::map<std::string, Channel>::iterator it =
			channels.find(toIrcUpperCase(command.args[0]));

		if (it == channels.end())
			return client.setSendData(nosuchnick(client, command.args[0]));
		else {
			Channel &ch = it->second;
			if (ch.getClients().find(&client) == ch.getClients().end())
				return client.setSendData(notonchannel(client, ch.getName()));
			else
				return ch.broadcast(client, ss.str(), false);
		}
	} else {
		std::map<int, Client>::iterator it = clients.begin();

		for (; it != clients.end(); it++) {
			if (it->second.getNickname() == command.args[0]) {
				return it->second.setSendData(ss.str());
			}
		}
		if (it == clients.end()) {
			return client.setSendData(nosuchnick(client, command.args[0]));
		}
	}
}

void Server::join(Client &client, Command &command) {
	if (command.args.size() < 1) {
		client.setSendData(needmoreparams(client, "JOIN"));
		return;
	}
	LOGGER.info("join", "Client " + client.getNickname() + " is trying to join channel " + command.args[0]);
	if (!validChannelName(command.args[0])) {
		client.setSendData(nosuchchannel(client, command.args[0]));
		return;
	}

	bool sentPassword = command.args.size() > 1;

	if (sentPassword) {
		bool v = validatePassword(command.args[1]);
		if (!v) {
			client.setSendData(needmoreparams(client, "JOIN"));
			return;
		}
	}

	Channel &ch = channels[toIrcUpperCase(command.args[0])];

	if (!ch.isInitialized()) {
		if (sentPassword) {
			ch.initialize(command.args[0], command.args[1], client);
		} else {
			ch.initialize(command.args[0], client);
		}
	}

	std::set<char> &chModes = ch.getMode();
	if (chModes.find('l') != chModes.end()) {
		if (ch.getClients().size() >= ch.getUserLimit()) {
			return client.setSendData(channelisfull(client, ch));
		}
	}
	if (chModes.find('i') != chModes.end()) {
		if (!ch.isInvited(client.getNickname())) {
			return client.setSendData(inviteonlychan(client, ch));
		}
		ch.removeInvited(client.getNickname());
		return successfulJoin(client, ch);
	}
	if (sentPassword) {
		if (!ch.evalPassword(command.args[1]))
			return client.setSendData(badchannelkey(client, ch.getName()));
		
	} else {
		if (!ch.evalPassword(""))
			return client.setSendData(badchannelkey(client, ch.getName()));
	}
	successfulJoin(client, ch);
}

void Server::successfulJoin(Client &client, Channel &ch) {
	std::stringstream ss;

	ch.addClient(client);
	client.addChannel(ch);
	ss << client.getClientPrefix();
	ss << " JOIN :";
	ss << ch.getName();
	ss << "\r\n";

	ch.broadcast(client, ss.str(), true);

	if (ch.getTopic() != "") {
		client.setSendData(topic(client, ch));
	} else {
		client.setSendData(notopic(client, ch));
	}
	ch.broadcast(client, namreply(client, ch, true), true);
};

void Server::who(Client &client, Command &command) {
	std::stringstream ss;

	LOGGER.info("who", "Client " + client.getNickname() + " is trying to get info about server");
	if (command.args.size() < 1) {
		client.setSendData(needmoreparams(client, "WHO"));
		return;
	}
	std::map<std::string, Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++) {
		if (it->first == toIrcUpperCase(command.args[0])) {
			return client.setSendData(whoreply(client, (it->second)));
		}
	}

	client.setSendData(nosuchnick(client, command.args[0]));
}

void Server::topic(Client &client, Command &command) {
	std::stringstream ss;

	if (command.args.size() < 1) {
		client.setSendData(needmoreparams(client, "TOPIC"));
		return;
	}
	LOGGER.info("topic", "Client " + client.getNickname() + " is trying to change topic of channel " + command.args[0]);

	Channel &ch = channels[toIrcUpperCase(command.args[0])];

	if (command.args.size() == 1) {
		if (ch.getTopic() != "") {
			return client.setSendData(topic(client, ch));
		} else {
			return client.setSendData(notopic(client, ch));
		}
	}

	std::map<Client *, uint>		   cls = ch.getClients();
	std::map<Client *, uint>::iterator it  = cls.find(&client);
	std::set<char>					   md  = ch.getMode();

	if (it == cls.end()) {
		return client.setSendData(notonchannel(client, ch.getName()));
	}

	if (find(md.begin(), md.end(), 't') != md.end()) {
		std::map<Client *, uint> cls = ch.getClients();

		std::map<Client *, uint>::iterator it = cls.find(&client);

		if (it != cls.end() && it->second & USER_OPERATOR) {
			ch.setTopic(command.args[1]);
			return ch.broadcast(client, topic(client, ch), true);
		} else {
			return client.setSendData(chanoprivsneeded(client, ch));
		}
	} else {
		ch.setTopic(command.args[1]);
		return ch.broadcast(client, topic(client, ch), true);;
	}
}

void Server::whois(Client &client, Command &command) {
	std::stringstream ss;

	LOGGER.info("whois", "Client " + client.getNickname() + " is trying to get info");
	if (command.args.size() < 1) {
		client.setSendData(needmoreparams(client, "WHOIS"));
		return;
	}

	std::map<int, Client>::iterator it;

	for (it = clients.begin(); it != clients.end(); it++) {
		if ((it->second).getNickname() == command.args[0]) {
			client.setSendData(whoisreply(client));
			return;
		}
	}

	client.setSendData(nosuchnick(client, command.args[0]));
}

void Server::quit(Client &client, Command &command) {
	std::stringstream ss;

	LOGGER.info("quit", "Client " + client.getNickname() + " is quitting");
	ss << ":" << client.getNickname();
	if (command.args.size()) {
		ss << " QUIT " << command.args[0];
	} else {
		ss << " QUIT :Gone to have lunch";
	}
	ss << "\r\n";
	std::vector<Channel *>::iterator it = client.getChannels().begin();
	for (; it != client.getChannels().end(); it++) {
		(*it)->removeClient(client);
		(*it)->broadcast(client, ss.str(), false);
	}
	LOGGER.debug("quit", ss.str());
	client.setToDisconnect(true);
}

void Server::ping(Client &client, Command &command) {
	std::stringstream ss;

	LOGGER.info("ping", "Client " + client.getNickname() + " is pinging");
	ss << ":localhost PONG localhost";
	if (command.args.size()) {
		ss << " :" << command.args[0];
	}
	ss << "\r\n";

	client.setSendData(ss.str());
}

void Server::part(Client &client, Command &command) {
	std::stringstream ss;

	if (command.args.size() < 1) {
		return client.setSendData(needmoreparams(client, "PART"));
	}
	LOGGER.info("part", "Client " + client.getNickname() + " is trying to leave channel");

	std::map<std::string, Channel>::iterator it =
		channels.find(toIrcUpperCase(command.args[0]));

	if (it == channels.end()) {
		return client.setSendData(nosuchchannel(client, command.args[0]));
	}

	std::vector<Channel *> chs = client.getChannels();

	std::vector<Channel *>::iterator chanIt = chs.begin();
	while (chanIt != chs.end()) {
		if (toIrcUpperCase((*chanIt)->getName()) == toIrcUpperCase(command.args[0])) {
			if (command.args.size() != 2)
				return removeClientFromChannel(client, **chanIt,
											   ":" + client.getNickname());
			else
				return removeClientFromChannel(client, **chanIt, command.args[1]);
		}
		chanIt++;
	}
	return client.setSendData(notonchannel(client, command.args[0]));
}

void Server::notice(Client &client, Command &command) {
	std::stringstream ss;

	LOGGER.info("notice", "Client " + client.getNickname() + " is trying to send a notice");
	if (command.args.size() < 2) {
		return;
	}

	ss << client.getClientPrefix();
	ss << " NOTICE";
	ss << " ";
	ss << command.args[0];
	ss << " ";
	ss << command.args[1];
	ss << "\r\n";

	std::string ch_prefix = CHANNEL_PREFIX;
	if (ch_prefix.find(command.args[0].at(0)) != std::string::npos) {
		Channel &ch = channels[toIrcUpperCase(command.args[0])];
		ch.broadcast(client, ss.str(), false);
		return;
	} else {
		std::map<int, Client>::iterator it = clients.begin();

		for (; it != clients.end(); it++) {
			if (it->second.getNickname() == command.args[0]) {
				it->second.setSendData(ss.str());
				return;
			}
		}
	}
}

void Server::channelMode(Client &client, Command &command) {
	std::string toggleMode	   = "it";
	std::string cmdsWithParams = "lko";
	std::string cmdPrefix	   = "+-";

	std::map<std::string, Channel>::iterator it = getChannelByName(command.args[0]);

	if (it == channels.end()) {
		return (client.setSendData(nosuchchannel(client, "MODE")));
	}
	Channel	&ch = it->second;
	std::map<Client *, unsigned int>::iterator clients =
		ch.getClientByNick(client.getNickname());
	if (!(clients->second & USER_OPERATOR)) {
		client.setSendData(chanoprivsneeded(client.getFd(), ch));
		return;
	}

	if (cmdPrefix.find(command.args[1][0]) == std::string::npos)
		return (client.setSendData(unknownmode(client, command.args[0][1])));

	bool		on = command.args[1][0] == '+';
	std::string modesChanged;
	modesChanged.insert(modesChanged.begin(), command.args[1][0]);
	command.args[1].erase(0, 1);

	std::set<char> chFlags;
	char		   usrFlag = 0;
	while (command.args[1].size() > 0) {
		if (cmdsWithParams.find(command.args[1][0]) != std::string::npos) {
			usrFlag = command.args[1][0];
		} else {
			chFlags.insert(command.args[1][0]);
		}
		command.args[1].erase(0, 1);
	}

	std::set<char>::iterator modeIt;
	for (std::size_t i = 0; i < toggleMode.size(); i++) {
		modeIt = chFlags.find(toggleMode.at(i));
		if (modeIt != chFlags.end()) {
			if (ch.toggleMode(*modeIt, on))
				modesChanged += *modeIt;
		}
	}

	std::stringstream ss;
	int				  lim = 0;
	switch (usrFlag) {
		case 'l':
			if (on) {
                if (command.args.size() < 3) {
                    return (client.setSendData(needmoreparams(client, "MODE")));
                }
                ss << command.args[2];
                if (!(ss >> lim) || lim <= 0) {
                    return (client.setSendData(needmoreparams(client, "MODE")));
                }
                ch.setUserLimit(lim);
                modesChanged += "l " + command.args[2];
            } else {
                ch.setUserLimit(0);
                modesChanged += "l *";
            }
            ch.toggleMode('l', on);
            break;
		case 'o':
			if (ch.setOperator(command.args[2], on)) {
				modesChanged += "o " + command.args[2];
			} else {
				return;
			}
			break;
		case 'v':
			ch.setSpeaker(command.args[2], on);
			break;
		case 'k':
			ch.toggleMode('k', on);
			if (on) {
				ch.setPassword(command.args[2]);
				modesChanged += "k " + command.args[2];
			} else {
				ch.removePassword();
				modesChanged += "k *";
			}
			break;
		default:
			break;
	}
	if (modesChanged.size() > 1)
		return ch.broadcast(client, usermodeis(ch, client, modesChanged), true);
}

void Server::userMode(Client &client, Command &command) {
	std::string inputModes = command.args[1];
	std::string changes;

	bool on = inputModes.at(0) == '+' ? true : false;
	changes.append(1, inputModes.at(0));

	std::set<char> flags;
	size_t		   i = 1;
	while (i < inputModes.size()) {
		flags.insert(inputModes.at(i));
		i++;
	}

	std::set<char>::iterator it = flags.begin();
	while (it != flags.end()) {
		if (*it == 'o' && on) {
			it++;
			continue;
		}
		if (client.setMode(*it, on)) {
			changes.append(1, *it);
		}
		it++;
	}
	if (changes.length() == 1) return;
	return client.setSendData(usermodeis(client, changes));
};

void Server::mode(Client &client, Command &command) {
	std::string ch_prefix = CHANNEL_PREFIX;
	LOGGER.info("mode", "Client " + client.getNickname() + " is trying to change mode");
	if (command.args.size() < 2) {
		if (command.args.empty())
			return client.setSendData(needmoreparams(client, "MODE"));
		if (ch_prefix.find(command.args.at(0).at(0)) != std::string::npos)
			return client.setSendData(channelmodeis(client, command.args[0]));
		else {
			if (toIrcUpperCase(client.getNickname()) == toIrcUpperCase(command.args[0]))
				return client.setSendData(usermodeis(client));
			else
				return client.setSendData(usersdontmatch(client));
		}
	}
	if (ch_prefix.find(command.args[0].at(0)) != std::string::npos) {
		if (evalChanMode(client, command.args)) {
			return channelMode(client, command);
		}
	} else {
		if (evalUserMode(client, command.args)) {
			return userMode(client, command);
		}
		return;
	}
}

bool Server::validNickname(std::string nickname) {
	if (nickname.empty() || isdigit(nickname[0])) {
		return false;
	}

	std::string disallowedChars = FORBIDDEN_CHARS;
	for (size_t i = 0; i < nickname.length(); ++i) {
		if (disallowedChars.find(nickname[i]) != std::string::npos) {
			return false;
		}
	}

	return true;
}

bool Server::nicknameAlreadyExists(std::string nickname) {
	std::map<int, Client>::iterator it = clients.begin();

	std::string uppercaseNickname = toIrcUpperCase(nickname);

	for (; it != clients.end(); it++) {
		if (toIrcUpperCase((it->second).getNickname()) == uppercaseNickname)
			return true;
	}
	return false;
}

bool Server::validChannelName(std::string name) {
	if (name.length() < 1) return false;

	std::string prefixes = CHANNEL_PREFIX;

	if (prefixes.find(name.at(0)) == std::string::npos) return false;
	if (name.length() > 200) return false;

	std::string insensitiveName = toIrcUpperCase(name);
	std::string forbiddenChars	= " \a,:";

	for (std::size_t i = 0; i < forbiddenChars.size(); i++) {
		if (insensitiveName.find(forbiddenChars.at(i)) != std::string::npos)
			return false;
	}
	return true;
};

void Server::kick(Client &client, Command &command) {
	if (command.args.size() < 2) {
		return client.setSendData(needmoreparams(client, "KICK"));
	}
	LOGGER.info("kick", "Client " + client.getNickname() + " is trying to kick " + command.args[1] + " from channel " + command.args[0]);
	Channel *ch = NULL;

	std::map<std::string, Channel>::iterator it = getChannelByName(command.args[0]);
	if (it != channels.end()) {
		ch = &it->second;
	}
	if (ch == NULL) {
		return client.setSendData(nosuchchannel(client, command.args[0]));
	}

	std::map<Client *, unsigned int>::iterator target;
	target = ch->getClientByNick(command.args[1]);
	if (target == ch->getClients().end()) {
		return client.setSendData(usernotinchannel(client, (*ch)));
	}

	std::map<Client *, unsigned int>::iterator issuer;
	issuer = ch->getClientByNick(client.getNickname());
	if (issuer == ch->getClients().end()) {
		return client.setSendData(notonchannel(client, ch->getName()));
	}
	if (!(issuer->second & USER_OPERATOR))
		return client.setSendData(chanoprivsneeded((*issuer->first), (*ch)));
	std::string targetName = target->first->getNickname();
	std::string reason = client.getNickname();
	if (command.args.size() > 2 && !command.args.at(2).empty()) {
		reason = command.args.at(2).find(':') == 0 ? command.args.at(2).substr(1) : command.args.at(2);
	}
	ch->broadcast(client, kicksuccess(client, *ch, targetName, reason), true);
	ch->removeClient(*target->first);
	return;
};

void Server::invite(Client &client, Command &command) {
	if (command.args.size() < 2) {
		return client.setSendData(needmoreparams(client, "INVITE"));
	}
	LOGGER.info("invite", "Client " + client.getNickname() + " is trying to invite " + command.args[0] + " to channel " + command.args[1]);
	std::map<std::string, Channel>::iterator ch_it;
	ch_it = this->getChannelByName(command.args[1]);
	if (ch_it == channels.end()) {
		return client.setSendData(nosuchnick(client, command.args[1]));
	}
	Channel *chan = &ch_it->second;

	Client *target = getClientByNick(command.args[0]);
	if (target == NULL) {
		return client.setSendData(nosuchnick(client, command.args[0]));
	}

	std::map<Client *, unsigned int>::iterator issuer_it;
	issuer_it = chan->getClientByNick(client.getNickname());
	if (issuer_it == chan->getClients().end()) {
		return client.setSendData(notonchannel(client, client.getNickname()));
	}

	bool isOper = issuer_it->second & USER_OPERATOR;

	std::set<char> &chanModes = chan->getMode();

	if (chanModes.find('i') != chanModes.end() && !isOper) {
		return client.setSendData(chanoprivsneeded(client, (*chan)));
	}

	chan->addInvite(target->getNickname());
	client.setSendData(inviting(client, (*chan)));
	return target->setSendData(inviterrpl(client, (*target), (*chan)));
}
