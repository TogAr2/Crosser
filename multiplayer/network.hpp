#ifndef CROSSER_NETWORK_HPP
#define CROSSER_NETWORK_HPP

#include <SFML/Network.hpp>
#include "api-utils.hpp"
#include "location.hpp"

class PacketNumber {
public:
	static const int CONNECT = 0;
	static const int DISCONNECT = 1;
	static const int INITIAL_INFO = 2;
	static const int MOVE = 3;
	static const int TILE_UPDATE = 4;
};

class Network {
	static sf::TcpSocket* clientSocket;
	static sf::SocketSelector selector;

public:
	static std::string ip;
	static unsigned short port;

	static bool client;
	static bool serverUp;

	static void startServer();
	static void sendPacketToAll(sf::Packet &packet, int skip = -1);

	static void connect();
	static void disconnect();
	static void receive();

	static void sendMove(crs::Direction direction);
	static void sendTileUpdate(const crs::Location& location, const crs::TileType& type);
};


#endif //CROSSER_NETWORK_HPP
