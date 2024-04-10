#pragma once

#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <Channel.hpp>
#include <Client.hpp>

typedef struct s_command {
	std::string	prefix;
	std::string	cmd;
	std::vector<std::string> args;
} Command;

std::string toIrcUpperCase(std::string s);
void replaceString(std::string &subject, std::string &search, std::string &replace);
bool validatePassword(std::string password);
