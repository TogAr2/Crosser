#ifndef CROSSER_NETWORK_HPP
#define CROSSER_NETWORK_HPP

#include <SFML/Network.hpp>
#include "api-utils.hpp"
#include "location.hpp"
#include "../game/game.hpp"

class PacketNumber {
public:
	static const int CONNECT = 0;
	static const int DISCONNECT = 1;
	static const int INITIAL_INFO = 2;
	static const int MOVE = 3;
	static const int TILE_UPDATE = 4;
	static const int SCORE_UPDATE = 5;
};

class Network {
	Game* game;

	sf::TcpSocket* clientSocket = nullptr;
	sf::SocketSelector selector;

	bool client;
	bool server = false;
	bool initialized = false;

public:
	static unsigned short defaultPort;

	Network(Game* game, sf::TcpSocket* socket);
	explicit Network(Game* game);
	virtual ~Network();

	void startServer();
	void stopServer();
	void sendPacketToAll(sf::Packet &packet, int skip = -1);

	static Game* createClient(const std::string &ip, unsigned int port);
	void disconnect();
	void receive();

	void sendMove(crs::Direction direction);
	void sendTileUpdate(const crs::Location& location, const crs::TileType& type);
	static void sendScoreUpdate(sf::TcpSocket* socket, int score);

	[[nodiscard]] bool isClient() const;
	[[nodiscard]] bool isServer() const;
	[[nodiscard]] bool isInitialized() const;
};

#endif //CROSSER_NETWORK_HPP
