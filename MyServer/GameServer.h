#pragma once
#include "Server.h"
#include <thread>

class GameServer :
    public Server
{
public:
	GameServer(const char* ip, int port);
	~GameServer();

private:
	std::thread* pGameThread[MAX_ROOM_NUM];

	//»Øµ÷º¯Êý
	static void RecvLogin(int fd, GameMsg::CommandParams cmdParams);
	static void RecvJoin(int fd, GameMsg::CommandParams cmdParams);
};

