#include <sys/types.h>
#include "Config.hpp"

//---------------------------------------------------------------> Constructors
Config::Config() {
	if (DEBUG == 1)
		std::cout << ORANGE << "Config base constructor called" << RESET << std::endl;

	for (uint i = 0U; !Config::_arrayValues[i].first.empty(); ++i) {
		this->_lookupValues.insert(_arrayValues[i]);
	}
}

//----------------------------------------------------------------> Destructors
Config::~Config() {
	if (DEBUG == 1)
		std::cout << ORANGE << "Config base destructor called" << RESET << std::endl;

	this->_lookupValues.clear();
}

//--------------------------------------------------------------------> Methods
// Load the configuration file to initalize the value
// return true if the initialization was done successfully || false if unsuccessful
bool Config::init(char const* filename) {

	std::ifstream infile;
	std::string line;
	std::string name;
	std::string value;
	size_t posEqual;

	infile.open(filename);

	if (infile.is_open() == false) {
		std::cerr << RED << "Config: getConfig: open() failed" << RESET << std::endl;
		return false;
	}

	while (infile.good() && std::getline(infile, line)) {

		if (line[0] == '#')
			continue;

		else if ((posEqual = line.find('=')) != std::string::npos) {

			// Get name and value
			name = line.substr(0, posEqual);
			value = line.substr(posEqual + 1, line.size());

			// Trim name and value
			name.erase(0, name.find_first_not_of(' '));
			name.erase(name.find_last_not_of(' ') + 1);
			value.erase(0, value.find_first_not_of(' '));
			value.erase(value.find_last_not_of(' ') + 1);

			if (!name.compare("oper") && value.empty() == false) // Set operators
			{
				std::string	operName;
				std::string	operPass;
				std::string::const_iterator	citBegin = value.begin();
				std::string::const_iterator	citSep;

				while (citBegin != value.end()) {

					for (citSep = citBegin; citSep != value.end() && *citSep != ':'; citSep++);

					if (citSep == value.end())
						break;

					operName = std::string(citBegin, citSep);
					citSep++;

					for (citBegin = citSep; citBegin != value.end() && *citBegin != ','; citBegin++);

					operPass = std::string(citSep, citBegin);

					if (citBegin != value.end())
						citBegin++;

					this->_lookupValues.insert(std::pair<std::string const, std::string const>("oper_" + operName, operPass));
				}
			}
			else if (this->_lookupValues.find(name) != this->_lookupValues.end() && value.empty() == false) // Set the value if name exist
				this->_lookupValues[name] = value;

			name.clear();
			value.clear();
		}
	}
	if (infile.eof() == false) {
		std::cerr << "Config: getConfig: an error occured\n";
		return false;
	}

	return true;
}

std::map<std::string const, std::string>::iterator	Config::end(void)
{
	return this->_lookupValues.end();
}

std::map<std::string const, std::string>::iterator	Config::find(std::string const &key)
{
	return this->_lookupValues.find(key);
}

std::map<std::string const, std::string>::const_iterator	Config::end(void) const
{
	return this->_lookupValues.end();
}

std::map<std::string const, std::string>::const_iterator	Config::find(std::string const &key) const
{
	return this->_lookupValues.find(key);
}

//-------------------------------------------------------------------> Overload
// Find and returns an element of the config based on the given key
std::string	&Config::operator[](std::string const &key)
{
	return this->_lookupValues[key];
}

std::pair<std::string const, std::string const>	Config::_arrayValues[] = {
	std::pair<std::string const, std::string const>("server_name", "ircserv"),
	std::pair<std::string const, std::string const>("server_version", "1.0"),
	std::pair<std::string const, std::string const>("motd", "config/motd.txt"),
	std::pair<std::string const, std::string const>("host", "127.0.0.1"),
	std::pair<std::string const, std::string const>("max_user", "1024"),
	std::pair<std::string const, std::string const>("ping", "10"),
	std::pair<std::string const, std::string const>("timeout", "30"),
	std::pair<std::string const, std::string const>("backlog", "1024"),
	std::pair<std::string const, std::string const>("oper_admin", "admin"),
	// std::pair<std::string const, std::string const>("oper_name", "admin"),
	// std::pair<std::string const, std::string const>("oper_password", "admin"),
	std::pair<std::string const, std::string const>("", "")
};
