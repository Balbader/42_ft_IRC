#include "../headers/Server.hpp"

// adjust the length of a given string to a specified length
std::string fillIt(std::string str, size_t len) {

	// if str < len, append enough spaces to str to make its length equal to len
    if (str.length() < len)
        str.insert(str.end(), len - str.length(), ' ');

	// truncates str to len-1 and append a . to the end
    else if (str.length() > len) {
        str.erase(str.begin() + len - 1, str.end());
        str.append(".");
    }
    return str;
}

// get current date and time
const std::string currentDateTime() {

    time_t now = time(NULL);
    if (now == -1) {
        // Handle error
        return "";
    }

    struct tm tstruct;
    char buf[80];
    localtime_r(&now, &tstruct); // Thread-safe version of localtime

    if (strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct) == 0) {
        // Handle error: the result string does not fit in the buffer
        return "";
    }

    return buf;
}
