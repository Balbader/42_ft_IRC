#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include <User.hpp>


class User;

class Channel {
public:
	Channel(std::string const&);
	virtual ~Channel();

	void addUser(User &);
	void delUser(std::string const&)

	bool empty() const;

	std::map<std::string const, User *const>::iterator begin();
	std::map<std::string const, User *const>::iterator end();
	std::map<std::string const, User *const>::iterator find(std::string const &);

	std::map<std::string const, User *const>::const_iterator begin();
	std::map<std::string const, User *const>::const_iterator end();
	std::map<std::string const, User *const>::const_iterator find(std::string const &);

	std::string const& getName() const;
	std::string const& getTopic() const;
	std::string const& getModes() const;

	static std::string const& getAvailableModes();

	void setName(std::string const&);
	void setTopic(std::string const&);
	void setModes(std::string const&);

private:
	std::string _name;
	std::string _topic;
	std::string _modes;

	std::map<std::string const, User *const> _lookupUsers;
	static std::string const _availableModes;
	
};

#endif // !CHANNEL_HPP
