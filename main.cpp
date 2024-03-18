#include "headers/Server.hpp"

int main(int ac, char **av)
{
	if (ac != 3) {
		std::cout << "Usage ./ircserv <port> <password>" << std::endl;
		exit(-1);
	}

	// Instanciate a Server object 'srv' name HOMESWEETHOME, with a max of 10 channels
	// wiht command line arguments for port and password
	Server srv("SERVER", 10, av[1], av[2]);

	try {
		srv.startServer();
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
};
