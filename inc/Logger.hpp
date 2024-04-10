#pragma once

#include <iostream>
#include <cstdio>
#include <string>

class Logger {
private:
	std::string	className;
	std::string	date(void);

public:
	~Logger(void);
	Logger(void);
	Logger(std::string className);
	Logger(Logger const & src);

	Logger &operator=(Logger const & rhs);

	void debug(std::string method, std::string msg);
	void info(std::string method, std::string msg);
	void warn(std::string method, std::string msg);
	void error(std::string method, std::string msg);
};
