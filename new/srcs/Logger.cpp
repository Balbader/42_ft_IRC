#include <Logger.hpp>

Logger::~Logger(void) {}

Logger::Logger(void) {
	this->className = "main";
}

Logger::Logger(std::string className) {
	this->className = className;
}

Logger::Logger(Logger const & src) {
	*this = src;
}

Logger &Logger::operator=(Logger const &rhs) {
	if (this != &rhs)
		this->className = rhs.className;
	return *this;
}

std::string Logger::date(void) {
    time_t currentTime = time(NULL);
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));
    return timestamp;
}

void Logger::debug(std::string method, std::string msg) {
	std::cout << date() << " [" << this->className << "] [" << method << "] DEBUG " << msg << std::endl;
}

void Logger::info(std::string method, std::string msg) {
	std::cout << date() << " [" << this->className << "] [" << method << "] INFO " << msg << std::endl;
}

void Logger::warn(std::string method, std::string msg) {
	std::cout << date() << " [" << this->className << "] [" << method << "] WARN " << msg << std::endl;
}

void Logger::error(std::string method, std::string msg) {
	std::cerr << date() << " [" << this->className << "] [" << method << "] ERROR " << msg << std::endl;
}
