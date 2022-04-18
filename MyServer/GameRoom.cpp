#include "GameRoom.h"
#include "Server.h"

GameRoom::GameRoom(Server* gameServer)
{
	playerNum = 0;
	memset(playerFdList, -1, sizeof(playerFdList));
	memset(playerIDList, -1, sizeof(playerIDList));
	memset(bPlayerIsReady, false, sizeof(bPlayerIsReady));
	bIsGameRunning = false;
	pGameThread = new std::thread(&GameRoom::GameThreadFunc, this);
	m_Server = gameServer;
}

GameRoom::~GameRoom()
{
	bIsGameRunning = false;
}

int GameRoom::PlayerJoinIn(int fd, int playerID)
{
	for (int i = 0; i < MAX_ROOM_PLAYER; i++)
	{
		if (playerIDList[i] == -1)
		{
			playerIDList[i] = playerID;
			bPlayerIsReady[i] = false;
			playerNum++;
			return i;
		}
	}
	return -1;
}

void GameRoom::PlayerLeave(int playerID)
{
	int playerIndex = GetPlayerIndex(playerID);
	if (playerIndex == -1)
	{
		printf("player not exit!");
		return;
	}
	playerIDList[playerIndex] = -1;
	playerNum--;
}

void GameRoom::PlayerReady(int playerID)
{
	int playerIndex = GetPlayerIndex(playerID);
	if (playerIndex == -1)
	{
		printf("player not exit!");
		return;
	}
	bPlayerIsReady[playerIndex] = true;

	//检测是否全部准备
	for (int i = 0; i < MAX_ROOM_PLAYER; i++)
	{
		if (bPlayerIsReady[i] = false)
		{
			return;
		}
	}
	StartGame();
}

void GameRoom::PlayerNotReady(int playerID)
{
	if (bIsGameRunning)
	{
		return;
	}

	int playerIndex = GetPlayerIndex(playerID);
	if (playerIndex == -1)
	{
		printf("player not exit!");
		return;
	}
	bPlayerIsReady[playerIndex] = false;
}

void GameRoom::AddPlayerOperation(int playerID, GameMsg::CommandParams cmdParams)
{

	Message message(SMSG_LOGIN, cmdParams);
	operationQueueLock.lock();
	playerOperationQueue.push(message);
	operationQueueLock.unlock();
}

int GameRoom::GetPlayerIndex(int playerID)
{
	for (int i = 0; i < MAX_ROOM_PLAYER; i++)
	{
		if (playerIDList[i] == playerID)
		{
			return i;
		}
	}
	return -1;
}

void GameRoom::StartGame()
{
	bIsGameRunning = true;
	pGameThread->join();
}

void GameRoom::GameThreadFunc()
{
	int lastTime = time(0);// 获取时间;
	while (bIsGameRunning)
	{
		int deltaTime = time(0) - lastTime;
		while (deltaTime < PER_FRAME_TIME)
		{
			usleep(1000);
			deltaTime = time(0) - lastTime;
		}
		lastTime += PER_FRAME_TIME;

		operationQueueLock.lock();
		while (!playerOperationQueue.empty())
		{
			m_Server->DoRedioSend(playerFdList, MAX_ROOM_PLAYER, playerOperationQueue.front().ToString().c_str());
		}
		operationQueueLock.unlock();

	}
}
