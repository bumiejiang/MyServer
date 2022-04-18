#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include "ServerDefine.h"
#include "Server.h"

class GameRoom
{
public:
	GameRoom(Server* gameServer);
	~GameRoom();

	inline int GetPlayerNum() { return playerNum; };
	inline bool IsFull() { return playerNum >= MAX_ROOM_PLAYER; };
	inline bool IsPlaying() { return bIsGameRunning; };

	int PlayerJoinIn(int fd, int playerID);
	void PlayerLeave(int playerID);
	void PlayerReady(int playerID);
	void PlayerNotReady(int playerID);

	void AddPlayerOperation(int playerID, GameMsg::CommandParams cmdParams);

private:
	Server* m_Server;
	int playerNum;
	int playerFdList[MAX_ROOM_PLAYER];
	int playerIDList[MAX_ROOM_PLAYER];
	bool bPlayerIsReady[MAX_ROOM_PLAYER];
	bool bIsGameRunning;
	std::thread* pGameThread;
	mutable std::mutex operationQueueLock;
	std::queue<Message> playerOperationQueue;


	int GetPlayerIndex(int playerID);

	void StartGame();
	void GameThreadFunc();
};

