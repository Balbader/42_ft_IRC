#include "headers/Server.hpp"

int main(int ac, char **av)
{
	if (ac != 3) {
		std::cout << "Usage ./ircserv <port> <password>" << std::endl;
		exit(-1);
	}

	Server srv("DEEZ.NUTS", 10, av[1], av[2]);

	try {
		srv.startServer();
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
};
