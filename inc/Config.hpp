#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <string>

class Config {
public:
	Config();
	virtual ~Config();

	bool init(char const*);

	std::map<std::string const, std::string>::iterator end();
	std::map<std::string const, std::string>::iterator find(std::string const&);

	std::map<std::string const, std::string>::const_iterator end() const;
	std::map<std::string const, std::string>::const_iterator find(std::string const&) const;

	std::string& operator[](std::string const&);

private:
	std::map<std::string const, std:;string> _lookupValues;

	static std::pair<std::string const, std::string const> _arrayValues;

/* ArrayValues:
 * 		server_name,
 * 		server_version,
 * 		modt,
 * 		host,
 * 		max_user,
 * 		ping,
 * 		timeout,
 * 		backlog,
 * 		oper_ + name
*/
};

#endif // !CONFIG_HPP
