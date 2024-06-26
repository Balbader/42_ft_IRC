#include <Server.hpp>

void Server::pass(Client &client, Command &command) {

    // logging the authentication attempt
	LOGGER.info("pass", "Client " + client.getNickname() + " is trying to authenticate");

    // check for password arguments
	if (command.args.size() == 0) {
		client.setSendData(needmoreparams(client, "PASS"));
		return;
	} else if (client.getRegistration() & PASS_FLAG) { // check for previous authentication
		client.setSendData(alreadyregistered(client));
		return;
	} else if (command.args[0] != passwd) { // password validation
		client.setSendData(passwdmismatch(client));
		return;
	}

    // setting client as authenticated
	client.setKnowPassword(true);
	client.setRegistration(PASS_FLAG);
}

void Server::user(Client &client, Command &command) {

    // logging the registration attempt
	LOGGER.info("user", "Client " + client.getNickname() + " is trying registry username");

    // check for sufficient arguments
	if (command.args.size() < 4) {
		client.setSendData(needmoreparams(client, "USER"));
		return;
	} else if (client.getRegistration() & USER_FLAG) { // check for previous registration
		client.setSendData(alreadyregistered(client));
		return;
	}

    // set client details
	client.setUsername(command.args[0]);
	client.setHostname(command.args[1]);
	client.setServername(command.args[2]);
	client.setRealname(command.args[3].substr(1));
	client.setRegistration(USER_FLAG);
}

void Server::nick(Client &client, Command &command) {

    // logging the nickname change attempt
	LOGGER.info("nick", "Client " + client.getNickname() + " is trying to change nickname");

    // check for nickname argument
	if (command.args.size() == 0) {
		client.setSendData(nonicknamegiven(client));
		return;
	} else if (!validNickname(command.args[0])) { // validate the new nickname
		client.setSendData(erroneusnickname(client, command.args[0]));
		return;
	} else if (nicknameAlreadyExists(command.args[0])) { // check for nickname uniqueness
		client.setSendData(nicknameinuse(client, command.args[0]));
		return;
	}

    // change the nickname
	std::string oldNick = client.getNickname();
	client.setNickname(command.args[0]);

    // send confirmation
	if (client.getRegistration() & NICK_FLAG) {
		client.setSendData(":localhost 001 " + client.getNickname() + "\r\n");
	} else {
		client.setSendData(changednickname(client, client.getNickname()));
	}

    // update registration status
	client.setRegistration(NICK_FLAG);

    // logging the successful nickname change
	LOGGER.info("nick", "Client " + client.getNickname() + " changed nickname");
}

void Server::oper(Client &client, Command &command) {

    // logging the authentication attempt
	LOGGER.info("oper", "Client " + client.getNickname() + " is trying to authenticate as operator");

    // check for sufficient arguments
	if (command.args.size() < 2) {
		client.setSendData(needmoreparams(client, command.cmd));
		return;
	} else if (command.args[1] != OPER_PASS) { // password validation
		client.setSendData(passwdmismatch(client));
		return;
	} else if (command.args[0] != OPER_USER) { // username validation
		client.setSendData(nooperhost(client));
		return;
	}

    // granting operator mode
	if (client.setMode('o', true)) {
		client.setSendData(youreoper(client));
	}
}

void Server::privmsg(Client &client, Command &command) {
	std::stringstream ss;
	std::string		  ch_prefix = CHANNEL_PREFIX;

    // logging the message sending attempt
	LOGGER.info("privmsg", "Client " + client.getNickname() + " is trying to send a privmsg");

    // check for sufficient arguments
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

    // constructing the message
	ss << client.getClientPrefix();
	ss << " PRIVMSG";
	ss << " ";
	ss << command.args[0];
	ss << " ";
	ss << command.args[1];
	ss << "\r\n";

    // message dellivery to a channel
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
	} else { // message delivery to a client
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

    // check for sufficient arguments
	if (command.args.size() < 1) {
		client.setSendData(needmoreparams(client, "JOIN"));
		return;
	}

    // logging the join attempt
	LOGGER.info("join", "Client " + client.getNickname() + " is trying to join channel " + command.args[0]);

    // validate channel name
	if (!validChannelName(command.args[0])) {
		client.setSendData(nosuchchannel(client, command.args[0]));
		return;
	}

    // password handling
	bool sentPassword = command.args.size() > 1;
	if (sentPassword) {
		bool v = validatePassword(command.args[1]);
		if (!v) {
			client.setSendData(needmoreparams(client, "JOIN"));
			return;
		}
	}

    // channel initialization
  

	Channel &ch = channels[toIrcUpperCase(command.args[0])];
	if (!ch.isInitialized()) {
		if (sentPassword) {
			ch.initialize(command.args[0], command.args[1], client);
		} else {
      // std::cout << "do i explode here?"<< command.args[1] << std::endl;
			ch.initialize(command.args[0], client);
		}
	}

	std::set<char> &chModes = ch.getMode();

    // user limit check
	if (chModes.find('l') != chModes.end()) {
		if (ch.getClients().size() >= ch.getUserLimit()) {
			return client.setSendData(channelisfull(client, ch));
		}
	}

    // invite only check
	if (chModes.find('i') != chModes.end()) {
		if (!ch.isInvited(client.getNickname())) {
			return client.setSendData(inviteonlychan(client, ch));
		}
		ch.removeInvited(client.getNickname());
		return successfulJoin(client, ch);
	}

    // password evaluation for channel join
	if (sentPassword) {
		if (!ch.evalPassword(command.args[1]))
			return client.setSendData(badchannelkey(client, ch.getName()));
	} else {
		if (!ch.evalPassword(""))
			return client.setSendData(badchannelkey(client, ch.getName()));
	}

    // successfulJoin ^^
	successfulJoin(client, ch);
}

void Server::successfulJoin(Client &client, Channel &ch) {
	std::stringstream ss;

    // add client to channel
	ch.addClient(client);

    // add channel to client's list
	client.addChannel(ch);

    // create join message
	ss << client.getClientPrefix();
	ss << " JOIN :";
	ss << ch.getName();
	ss << "\r\n";

    // broadcast join message
	ch.broadcast(client, ss.str(), true);

    // hanndle channel topic
	if (ch.getTopic() != "") {
		client.setSendData(topic(client, ch));
	} else {
		client.setSendData(notopic(client, ch));
	}

    // broadcast names list
	ch.broadcast(client, namreply(client, ch, true), true);
}

void Server::who(Client &client, Command &command) {
	std::stringstream ss;

    // logging the who message
	LOGGER.info("who", "Client " + client.getNickname() + " is trying to get info about server");

    // check for command arguments 
	if (command.args.size() < 1) {
		client.setSendData(needmoreparams(client, "WHO"));
		return;
	}

    // search for matching channels
	std::map<std::string, Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++) {
		if (it->first == toIrcUpperCase(command.args[0])) {
			return client.setSendData(whoreply(client, (it->second)));
		}
	}

    // handle no match found
	client.setSendData(nosuchnick(client, command.args[0]));
}

void Server::topic(Client &client, Command &command) {
	std::stringstream ss;

    // check command arguments
	if (command.args.size() < 1) {
		client.setSendData(needmoreparams(client, "TOPIC"));
		return;
	}

    // logging the topic change attempt
	LOGGER.info("topic", "Client " + client.getNickname() + " is trying to change topic of channel " + command.args[0]);

    // retrieve the channel and manage topic query
	Channel &ch = channels[toIrcUpperCase(command.args[0])];

	if (command.args.size() == 1) {
		if (ch.getTopic() != "") {
			return client.setSendData(topic(client, ch));
		} else {
			return client.setSendData(notopic(client, ch));
		}
	}

    // check client's membership and permissions
	std::map<Client *, uint> cls = ch.getClients();
	std::map<Client *, uint>::iterator it = cls.find(&client);
	std::set<char> md = ch.getMode();

	if (it == cls.end()) {
		return client.setSendData(notonchannel(client, ch.getName()));
	}

    // permission to change topic
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

    // logging client information
	LOGGER.info("whois", "Client " + client.getNickname() + " is trying to get info");

    // check for adequate command arguments
	if (command.args.size() < 1) {
		client.setSendData(needmoreparams(client, "WHOIS"));
		return;
	}

    // search for the target client
	std::map<int, Client>::iterator it;

	for (it = clients.begin(); it != clients.end(); it++) {
		if ((it->second).getNickname() == command.args[0]) {
			client.setSendData(whoisreply(client));
			return;
		}
	}

    // handle no matching nickname
	client.setSendData(nosuchnick(client, command.args[0]));
}

void Server::quit(Client &client, Command &command) {
	std::stringstream ss;

    // logging the quit action
	LOGGER.info("quit", "Client " + client.getNickname() + " is quitting");


    // constructing the quit command
	ss << ":" << client.getNickname();
	if (command.args.size()) {
		ss << " QUIT " << command.args[0];
	} else {
		ss << " QUIT :Gone to have lunch";
	}
	ss << "\r\n";

    // removing client from channels and broadcasting quit
	std::vector<Channel *>::iterator it = client.getChannels().begin();
	for (; it != client.getChannels().end(); it++) {
		(*it)->removeClient(client);
		(*it)->broadcast(client, ss.str(), false);
	}

    // logging the quit command for debugging
	LOGGER.debug("quit", ss.str());

    // setting client to disconnect
	client.setToDisconnect(true);
}

void Server::ping(Client &client, Command &command) {
	std::stringstream ss;

    // logging the ping action
	LOGGER.info("ping", "Client " + client.getNickname() + " is pinging");

    // constructing the pong response
	ss << ":localhost PONG localhost";
	if (command.args.size()) {
		ss << " :" << command.args[0];
	}
	ss << "\r\n";

    // sending the response back to the client
	client.setSendData(ss.str());
}

void Server::part(Client &client, Command &command) {
	std::stringstream ss;

    // checking command arguments
	if (command.args.size() < 1) {
		return client.setSendData(needmoreparams(client, "PART"));
	}

    // logging the departure attempt
	LOGGER.info("part", "Client " + client.getNickname() + " is trying to leave channel");

    // finding the channel
	std::map<std::string, Channel>::iterator it = channels.find(toIrcUpperCase(command.args[0]));

	if (it == channels.end()) {
		return client.setSendData(nosuchchannel(client, command.args[0]));
	}

    // checking if client is in channel
	std::vector<Channel *> chs = client.getChannels();
	std::vector<Channel *>::iterator chanIt = chs.begin();
	while (chanIt != chs.end()) {
		if (toIrcUpperCase((*chanIt)->getName()) == toIrcUpperCase(command.args[0])) {
			if (command.args.size() != 2)
				return removeClientFromChannel(client, **chanIt, ":" + client.getNickname());
			else
				return removeClientFromChannel(client, **chanIt, command.args[1]);
		}
		chanIt++;
	}

    // hanndling no membership
	return client.setSendData(notonchannel(client, command.args[0]));
}

void Server::notice(Client &client, Command &command) {
	std::stringstream ss;

    // logging the notice attempt
	LOGGER.info("notice", "Client " + client.getNickname() + " is trying to send a notice");

    // argument check
	if (command.args.size() < 2) {
		return;
	}

    // constructing the notice command
	ss << client.getClientPrefix();
	ss << " NOTICE";
	ss << " ";
	ss << command.args[0];
	ss << " ";
	ss << command.args[1];
	ss << "\r\n";

    // sending notice to a channel
	std::string ch_prefix = CHANNEL_PREFIX;
	if (ch_prefix.find(command.args[0].at(0)) != std::string::npos) {
		Channel &ch = channels[toIrcUpperCase(command.args[0])];
		ch.broadcast(client, ss.str(), false);
		return;
	} else {
        // sending notice to a specific client
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
    // initial setup
	std::string toggleMode = "it";
	std::string cmdsWithParams = "lko";
	std::string cmdPrefix = "+-";

    // channel and client validation
	std::map<std::string, Channel>::iterator it = getChannelByName(command.args[0]);

	if (it == channels.end()) {
		return (client.setSendData(nosuchchannel(client, "MODE")));
	}
	Channel	&ch = it->second;
	std::map<Client *, unsigned int>::iterator clients = ch.getClientByNick(client.getNickname());
	if (!(clients->second & USER_OPERATOR)) {
		client.setSendData(chanoprivsneeded(client.getFd(), ch));
		return;
	}

    // mode command prefic check
	if (cmdPrefix.find(command.args[1][0]) == std::string::npos)
		return (client.setSendData(unknownmode(client, command.args[0][1])));

    // Parsing mode changes
	bool on = command.args[1][0] == '+';
	std::string modesChanged;
	modesChanged.insert(modesChanged.begin(), command.args[1][0]);
	command.args[1].erase(0, 1);

	std::set<char> chFlags;
	char usrFlag = 0;
	while (command.args[1].size() > 0) {
		if (cmdsWithParams.find(command.args[1][0]) != std::string::npos) {
			usrFlag = command.args[1][0];
		} else {
			chFlags.insert(command.args[1][0]);
		}
		command.args[1].erase(0, 1);
	}

    // applying mode changes
	std::set<char>::iterator modeIt;
	for (std::size_t i = 0; i < toggleMode.size(); i++) {
		modeIt = chFlags.find(toggleMode.at(i));
		if (modeIt != chFlags.end()) {
			if (ch.toggleMode(*modeIt, on))
				modesChanged += *modeIt;
		}
	}

    // handling parameter-based modes
	std::stringstream ss;
	int	lim = 0;
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

    // broadcasting changes
	if (modesChanged.size() > 1)
		return ch.broadcast(client, usermodeis(ch, client, modesChanged), true);
}

void Server::userMode(Client &client, Command &command) {

    // extracting mode changes
	std::string inputModes = command.args[1];
	std::string changes;

    // determining mode operation
	bool on = inputModes.at(0) == '+' ? true : false;
	changes.append(1, inputModes.at(0));

    // storing mode flags
	std::set<char> flags;
	size_t i = 1;
	while (i < inputModes.size()) {
		flags.insert(inputModes.at(i));
		i++;
	}

    // applying mode changes
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

    // handling no changes
	if (changes.length() == 1) return;

    // sending mode changes to client
	return client.setSendData(usermodeis(client, changes));
}

void Server::mode(Client &client, Command &command) {
	std::string ch_prefix = CHANNEL_PREFIX;

    // logging the mode change attempt
	LOGGER.info("mode", "Client " + client.getNickname() + " is trying to change mode");

    // check command argument count
	if (command.args.size() < 2) {
        // no arguments provided
		if (command.args.empty())
			return client.setSendData(needmoreparams(client, "MODE"));
        // single argument provided
        // channel mode inquiry
		if (ch_prefix.find(command.args.at(0).at(0)) != std::string::npos)
			return client.setSendData(channelmodeis(client, command.args[0]));
		else {
            // user mode inquiry
			if (toIrcUpperCase(client.getNickname()) == toIrcUpperCase(command.args[0]))
				return client.setSendData(usermodeis(client));
			else
				return client.setSendData(usersdontmatch(client));
		}
	}

    // evaluating and applying mode changes
	if (ch_prefix.find(command.args[0].at(0)) != std::string::npos) {
        // channel mode changes
		if (evalChanMode(client, command.args)) {
			return channelMode(client, command);
		}
	} else {
        // user mode changes
		if (evalUserMode(client, command.args)) {
			return userMode(client, command);
		}
		return;
	}
}

bool Server::validNickname(std::string nickname) {
    // check for empty or digit-prefixed nickname
	if (nickname.empty() || isdigit(nickname[0])) {
		return false;
	}

    // check for disallowed characters
	std::string disallowedChars = FORBIDDEN_CHARS;
	for (size_t i = 0; i < nickname.length(); ++i) {
		if (disallowedChars.find(nickname[i]) != std::string::npos) {
			return false;
		}
	}

    // return true if valid
	return true;
}

bool Server::nicknameAlreadyExists(std::string nickname) {
    // initialize client iterator
	std::map<int, Client>::iterator it = clients.begin();

    // convert nickname to uppercase
	std::string uppercaseNickname = toIrcUpperCase(nickname);

    // iterate over clients
	for (; it != clients.end(); it++) {
		if (toIrcUpperCase((it->second).getNickname()) == uppercaseNickname)
			return true;
	}

    // return false if no matches found
	return false;
}

bool Server::validChannelName(std::string name) {
    // check for empty string
	if (name.length() < 1)
        return false;

	std::string prefixes = CHANNEL_PREFIX;
    // check for valid prefix
	if (prefixes.find(name.at(0)) == std::string::npos)
        return false;
    // check for excessive length
	if (name.length() > 200)
        return false;

    // convert name to uppercase
	std::string insensitiveName = toIrcUpperCase(name);

    // check for forbiden characters
	std::string forbiddenChars	= " \a,:";
	for (std::size_t i = 0; i < forbiddenChars.size(); i++) {
		if (insensitiveName.find(forbiddenChars.at(i)) != std::string::npos)
			return false;
	}

    // return true if all checks pass
	return true;
}

void Server::kick(Client &client, Command &command) {
    // checking command arguments
	if (command.args.size() < 2) {
		return client.setSendData(needmoreparams(client, "KICK"));
	}

    // logging the kick attempt
	LOGGER.info("kick", "Client " + client.getNickname() + " is trying to kick " + command.args[1] + " from channel " + command.args[0]);

    // finding the channel
	Channel *ch = NULL;
	std::map<std::string, Channel>::iterator it = getChannelByName(command.args[0]);
	if (it != channels.end()) {
		ch = &it->second;
	}
	if (ch == NULL) {
		return client.setSendData(nosuchchannel(client, command.args[0]));
	}

    // finding the target client
	std::map<Client *, unsigned int>::iterator target;
	target = ch->getClientByNick(command.args[1]);
	if (target == ch->getClients().end()) {
		return client.setSendData(usernotinchannel(client, (*ch)));
	}

    // verifying the issuer's privileges
	std::map<Client *, unsigned int>::iterator issuer;
	issuer = ch->getClientByNick(client.getNickname());
	if (issuer == ch->getClients().end()) {
		return client.setSendData(notonchannel(client, ch->getName()));
	}
	if (!(issuer->second & USER_OPERATOR))
		return client.setSendData(chanoprivsneeded((*issuer->first), (*ch)));

    // handling the kick reason
	std::string targetName = target->first->getNickname();
	std::string reason = client.getNickname();
	if (command.args.size() > 2 && !command.args.at(2).empty()) {
		reason = command.args.at(2).find(':') == 0 ? command.args.at(2).substr(1) : command.args.at(2);
	}

    // broadcasting the kick and removing the client
	ch->broadcast(client, kicksuccess(client, *ch, targetName, reason), true);
	ch->removeClient(*target->first);
	return;
}

void Server::invite(Client &client, Command &command) {

    // check command arguments
	if (command.args.size() < 2) {
		return client.setSendData(needmoreparams(client, "INVITE"));
	}

    // logging the invitation attempt
	LOGGER.info("invite", "Client " + client.getNickname() + " is trying to invite " + command.args[0] + " to channel " + command.args[1]);
	std::map<std::string, Channel>::iterator ch_it;

    // finding the channel
	ch_it = this->getChannelByName(command.args[1]);
	if (ch_it == channels.end()) {
		return client.setSendData(nosuchnick(client, command.args[1]));
	}
	Channel *chan = &ch_it->second;

    // finding the terget client
	Client *target = getClientByNick(command.args[0]);
	if (target == NULL) {
		return client.setSendData(nosuchnick(client, command.args[0]));
	}

    // checking the inviter's channel membership
	std::map<Client *, unsigned int>::iterator issuer_it;
	issuer_it = chan->getClientByNick(client.getNickname());
	if (issuer_it == chan->getClients().end()) {
		return client.setSendData(notonchannel(client, client.getNickname()));
	}

    // checking inviter's operator status
	bool isOper = issuer_it->second & USER_OPERATOR;

    // handling channel modes
	std::set<char> &chanModes = chan->getMode();
	if (chanModes.find('i') != chanModes.end() && !isOper) {
		return client.setSendData(chanoprivsneeded(client, (*chan)));
	}

    // adding the invitation and notifying clients
	chan->addInvite(target->getNickname());
	client.setSendData(inviting(client, (*chan)));
	return target->setSendData(inviterrpl(client, (*target), (*chan)));
}
