/*#include <logger.hpp>
#include <iostream>
#include "network.hpp"

void Network::startServer() {
	sf::TcpListener listener;

	if (listener.listen(3000) != sf::Socket::Done) {
		std::cerr << Logger::error << "Could not start server!";
	}

	serverUp = true;

	while (serverUp) {
		sf::TcpSocket client;
		if (listener.accept(client) != sf::Socket::Done) {
			std::cerr << Logger::error << "Failed to precess incoming socket connection!";
		}

		clients.push_back(client);
	}
}
*/