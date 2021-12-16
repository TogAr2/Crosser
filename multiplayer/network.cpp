#include <logger.hpp>
#include <iostream>
#include <thread>
#include "network.hpp"
#include "../game/game.hpp"
#include "../render/render.hpp"

std::string Network::ip = "localhost";
unsigned short Network::port = 3000;

bool Network::client = false;
bool Network::serverUp = false;

sf::TcpSocket* Network::clientSocket = nullptr;
sf::SocketSelector Network::selector = sf::SocketSelector();

void Network::startServer() {
	client = false;

	std::thread thread([]{
		sf::TcpListener listener;

		if (listener.listen(3000) != sf::Socket::Done) {
			std::cerr << Logger::error << "Could not start server!" << std::endl;
		}

		listener.setBlocking(false);

		serverUp = true;
		std::cout << Logger::info << "Server started!" << std::endl;

		selector.add(listener);

		std::optional<Game>* optionalGame = Render::get()->getGame();
		Game* game = optionalGame->operator->();

		while (serverUp) {
			auto* socket = new sf::TcpSocket();

			if (listener.accept(*socket) == sf::Socket::Done) {
				std::uniform_int_distribution<unsigned char> dist(0, 255);

				auto *connected = (RemotePlayer *) game->newPlayer(new crs::Location(width / 2, height / 2),sf::Color(dist(game->random), dist(game->random), dist(game->random)), socket);

				game->players[connected->getId()] = connected;
				selector.add(*socket);

				std::cout << Logger::info << "Client connected: " << connected->getId() << std::endl;

				sf::Packet connectPacket;

				connectPacket << PacketNumber::CONNECT;
				connectPacket << connected->getId();

				connectPacket << connected->getColor().r;
				connectPacket << connected->getColor().g;
				connectPacket << connected->getColor().b;
				connectPacket << connected->getLocation()->getX();
				connectPacket << connected->getLocation()->getY();

				sendPacketToAll(connectPacket, connected->getId());

				sf::Packet initialInfo;

				initialInfo << PacketNumber::INITIAL_INFO;
				initialInfo << connected->getId();

				for (auto &pair : game->players) {
					Player *player = pair.second;

					initialInfo << player->getId();
					initialInfo << player->getColor().r;
					initialInfo << player->getColor().g;
					initialInfo << player->getColor().b;
					initialInfo << player->getLocation()->getX();
					initialInfo << player->getLocation()->getY();
				}

				initialInfo << -1;
				for (auto &loopX : game->map) {
					for (auto &loopY : loopX) {
						initialInfo << crs::to_string(loopY.getType());
					}
				}

				connected->getSocket()->send(initialInfo);
				connected->getSocket()->setBlocking(false);
			}

			if (selector.wait(sf::milliseconds(10))) {
				//Receive data
				for (auto & pair : game->players) {
					Player* player = pair.second;

					if (player->isRemote() && selector.isReady(*player->getRemote()->getSocket())) {
						sf::Packet received;
						if (player->getRemote()->getSocket()->receive(received) == sf::Socket::Done) {
							int packetNumber, id;

							received >> packetNumber;
							received >> id;

							if (packetNumber == PacketNumber::DISCONNECT) {
								sendPacketToAll(received, player->getId());

								std::cout << Logger::info << "Client disconnected: " << player->getId() << std::endl;
								selector.remove(*player->getRemote()->getSocket());
								delete game->players[player->getId()];
								game->players.erase(player->getId());
								break;
							} else if (packetNumber == PacketNumber::MOVE) {
								sendPacketToAll(received, player->getId());

								std::string directionString;
								received >> directionString;
								crs::Direction direction = crs::directionValueof(directionString);

								game->requestMove(game->players[id], direction);
							}
						}
					}
				}
			}
		}
	});
	thread.detach();
}

void Network::sendPacketToAll(sf::Packet & packet, int skip) {
	for (auto & pair : Render::get()->getGame()->operator->()->players) {
		Player* player = pair.second;

		if (skip == player->getId()) continue;
		if (!player->isRemote()) continue;

		if (player->getRemote()->getSocket()->send(packet) != sf::Socket::Done) {
			std::cout << Logger::error << "Error sending packet in sendPacketToAll!" << std::endl;
		}
	}
}

void Network::connect() {
	client = true;

	delete clientSocket;
	clientSocket = new sf::TcpSocket();
	if (clientSocket->connect(Network::ip, Network::port, sf::seconds(5)) != sf::Socket::Done) {
		std::cout << Logger::error << "Failed to connect to server!" << std::endl;
	} else {
		clientSocket->setBlocking(false);
		std::cout << Logger::info << "Connected to " << Network::ip << ":" << port << std::endl;
	}
}

void Network::disconnect() {
	sf::Packet packet;

	packet << PacketNumber::DISCONNECT;
	packet << Render::get()->getGame()->operator->()->clientPlayer->getId();

	if (clientSocket->send(packet) != sf::Socket::Done) {
		std::cout << Logger::info << "Failed to disconnect from server!" << std::endl;
	} else {
		std::cout << Logger::info << "Disconnected" << std::endl;
	}
}

void Network::receive() {
	std::optional<Game>* optionalGame = Render::get()->getGame();
	Game* game = optionalGame->operator->();

	sf::Packet received;
	int packetNumber, id;
	if (clientSocket->receive(received) == sf::Socket::Done) {
		received >> packetNumber;
		received >> id;

		if (packetNumber == PacketNumber::INITIAL_INFO) {
			game->players.erase(game->clientPlayer->getId());
			game->clientPlayer->setId(id);
			game->players[id] = game->clientPlayer;
			std::cout << Logger::info << "Got player id " << id << std::endl;

			while (!received.endOfPacket()) {
				int id2;
				received >> id2;
				if (id2 == -1) break;

				unsigned char r, g, b;
				int x, y;

				received >> r >> g >> b >> x >> y;

				if (id2 == id) { //If the current info is this client
					game->clientPlayer->setColor(sf::Color(r, g, b));
					game->clientPlayer->setLocation(new crs::Location(x, y));
				} else {
					game->players[id2] = new Player(new crs::Location(x, y), sf::Color(r, g, b), id2);
				}
			}

			for (auto & loopX : game->map) {
				for (auto & loopY : loopX) {
					std::string string;
					received >> string;
					crs::TileType type = crs::tileTypeValueof(string);
					loopY.setType(type);
				}
			}
		} else if (packetNumber == PacketNumber::CONNECT) {
			std::cout << Logger::info << "New player connected: " << id << std::endl;

			unsigned char r, g, b;
			int x, y;

			received >> r >> g >> b >> x >> y;

			game->players[id] = new Player(new crs::Location(x, y), sf::Color(r, g, b), id);
		} else if (packetNumber == PacketNumber::DISCONNECT) {
			std::cout << Logger::info << "Player disconnected: " << id << std::endl;

			delete game->players[id];
			game->players[id] = nullptr;
		} else if (packetNumber == PacketNumber::MOVE) {
			std::string directionString;
			received >> directionString;
			crs::Direction direction = crs::directionValueof(directionString);

			game->movePlayer(game->players[id], direction);
		} else if (packetNumber == PacketNumber::TILE_UPDATE) {
			int x, y;
			std::string tileString;
			received >> x >> y;
			received >> tileString;
			crs::TileType tileType = crs::tileTypeValueof(tileString);

			game->map[x][y].setType(tileType);
		} else if (packetNumber == PacketNumber::SCORE_UPDATE) {
			int score;
			received >> score;

			game->clientPlayer->setScore(score);
		}
	}
}

void Network::sendMove(crs::Direction direction) {
	std::optional<Game>* optionalGame = Render::get()->getGame();
	Game* game = optionalGame->operator->();

	sf::Packet packet;
	packet << PacketNumber::MOVE;
	packet << game->clientPlayer->getId();

	packet << crs::to_string(direction);

	if (Network::client) {
		clientSocket->send(packet);
	} else {
		sendPacketToAll(packet, game->clientPlayer->getId());
	}
}

void Network::sendTileUpdate(const crs::Location& location, const crs::TileType& type) {
	std::optional<Game>* optionalGame = Render::get()->getGame();
	Game* game = optionalGame->operator->();

	sf::Packet packet;
	packet << PacketNumber::TILE_UPDATE;
	packet << -1;

	packet << location.getX();
	packet << location.getY();
	packet << crs::to_string(type);

	sendPacketToAll(packet, game->clientPlayer->getId());
}

void Network::sendScoreUpdate(sf::TcpSocket *socket, int score) {
	sf::Packet packet;
	packet << PacketNumber::SCORE_UPDATE;
	packet << -1;

	packet << score;

	if (socket->send(packet) != sf::Socket::Done) {
		std::cout << Logger::error << "Error sending score update packet!" << std::endl;
	}
}
