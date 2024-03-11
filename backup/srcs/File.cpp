#include "../headers/File.hpp"

// Constructor
File::File() {}

// Constructor operload
File::File( std::string Name, std::string Path, std::string Sender, std::string Receiver ): Name(Name), Path(Path), sender(Sender), receiver(Receiver) {}

// Copy constructor
File::File( const File &x ) { *this = x; }

// Destructor
File::~File() {}

// Assignment operator
File & File::operator=( const File &rhs )
{
	if (this == &rhs)
		return (*this);

	this->Name = rhs.Name;
	this->Path = rhs.Path;
	this->sender = rhs.sender;
	this->receiver = rhs.receiver;

	return (*this);
}
