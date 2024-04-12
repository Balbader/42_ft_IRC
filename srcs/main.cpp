#include <string>
#include <exception>
#include <Server.hpp>
#include <Logger.hpp>

int main(int ac, char **av) {
    std::string password;
    int port;

    try {
        // argument check
        if (ac != 3)
			throw std::invalid_argument("Invalid number of arguments");

        // parse command line arguments
        port = std::atoi(av[1]);
        password = av[2];

        // server initialization and start
        Server server(password, port);
        server.startServer();
    } catch (const std::exception& e) { // exception handling
        Logger().error("main", e.what());
        return EXIT_FAILURE; 
    } 

    return EXIT_SUCCESS;
}
