#include "../headers/Request.hpp"

// Constructor
Request::Request(): args(), command(), invalidMessage(false) {};

// Copy Constructor
Request::Request( const Request& x ) { *this = x; };

// Destructor
Request::~Request() {};

// Assignment operator
Request & Request::operator= ( const Request& rhs ) {
	if (this == &rhs)
		return (*this);

	this->args = rhs.args;
	this->command = rhs.command;
	this->invalidMessage = rhs.invalidMessage;

	return (*this);
}
