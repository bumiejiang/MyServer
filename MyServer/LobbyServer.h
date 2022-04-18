#pragma once
#include "Server.h"
#include "ServerDefine.h"
#include "DataBase.h"
#include "GameRoom.h"
#include <thread>
class LobbyServer :
    public Server
{
public:
    LobbyServer(const char* ip, int port);
    ~LobbyServer();

	//�ص�����
	static void RecvRegist(int fd, GameMsg::CommandParams cmdParams);
	static void RecvLogin(int fd, GameMsg::CommandParams cmdParams);
	static void RecvJoin(int fd, GameMsg::CommandParams cmdParams);
	static void RecvGameOperation(int fd, GameMsg::CommandParams cmdParams);

private:
	///////////������Ϣ/////////////
	DataBase* database;	//���ݿ�
	struct PlayerInfo	//�����Ϣ
	{
		int fd;
		int playerID;
		std::string name;
		int roomID;
		PlayerInfo(int fd = -1, int pID = -1, std::string name = std::string(""), int roomID = -1) :
			fd(fd), playerID(pID), name(name), roomID(roomID){};
	};
	PlayerInfo playerList[MAX_PLAYER_NUM];

	///////////������Ϣ/////////////
	GameRoom* roomList[MAX_ROOM_NUM];
	

};
extern LobbyServer* g_LobbyServer;

