#include <utils.hpp>

void replaceString(std::string& subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

std::string toIrcUpperCase(std::string s) {
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	replaceString(s, "{", "[");
	replaceString(s, "}", "]");
	replaceString(s, "|", "\\");

	return s;
}

bool validatePassword(std::string password) {
	if (password.find(',') != std::string::npos) return false;
	return true;
}