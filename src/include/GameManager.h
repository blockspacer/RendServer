/*
 * GameManager.h
 *
 *  Created on: Sep 2, 2014
 *      Author: gdeforest
 */

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#define RATE 128		// ticks/s
#define MAX_PLAYERS 16
#define TIMEOUT 10		// seconds

#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "Player.h"

typedef struct {
	unsigned int ticker;
	PlayerData playersData[MAX_PLAYERS];
} Tick;

enum { JOIN, RESPAWN, LEAVE };

typedef struct {
	char type;
	char playerId;
	char spawnpoint;
} ServerMessage;

#include "PlayerManager.h"
#include "../server/include/TcpServer.h"
#include "../server/include/UdpServer.h"

class TcpServer;
class UdpServer;

class GameManager {
	private:
		PlayerManager* pm;
		pthread_mutex_t playerLock;
		boost::asio::io_service* ioService;
		TcpServer* tcpServer;
		UdpServer* udpServer;
		Tick* gamestate;
		char autoIncrementId;

		void Run();
		static void* RunHelper(void *context);
		GameManager(GameManager const&);		// Don't Implement.
        void operator = (GameManager const&);	// Don't implement
        GameManager() {};
		virtual ~GameManager();
	public:
		void Start();
		ServerMessage Respawn(string ip, char playerId);
		ServerMessage Leave(string ip, char playerId);
		ServerMessage AcceptJoin(string ip);

		Tick* GetGamestate() { return gamestate; }
		int GetTick() { return gamestate->ticker; }
		pthread_mutex_t* GetPlayerLock() { return &playerLock; };

		void init(boost::asio::io_service& ioService);
		static GameManager& GetInstance() {
	    	static GameManager gm;
	    	return gm;
		}
};




#endif /* GAMEMANAGER_H_ */
