#include "LobbyServer.h"

LobbyServer* g_LobbyServer;

LobbyServer::LobbyServer(const char* ip, int port) :Server(ip, port)
{
	database = new DataBase();
	database->Connect("10.0.128.206", "game", "game123u", "OLGame");
	for (int i = 0; i < MAX_ROOM_NUM; i++)
	{
		roomList[i] = new GameRoom(this);
	}
	BindClientCommandFunction(CMSG_REGIST, &LobbyServer::RecvRegist);
	BindClientCommandFunction(CMSG_LOGIN, &LobbyServer::RecvLogin);
	BindClientCommandFunction(CMSG_JOIN, &LobbyServer::RecvJoin);
	BindClientCommandFunction(CMSG_GAMEOPERA, &LobbyServer::RecvGameOperation);
}

LobbyServer::~LobbyServer()
{
}

void LobbyServer::RecvRegist(int fd, GameMsg::CommandParams cmdParams)
{
	GameMsg::LoginInfo msgLogin = cmdParams.logininfo();
	std::string strUserName = msgLogin.username();
	std::string strPassword = msgLogin.password();
	int nRes = g_LobbyServer->database->AddAccount(strUserName, strPassword);

	g_LobbyServer->SendCommand(fd, SMSG_REGIST);
}

void LobbyServer::RecvLogin(int fd, GameMsg::CommandParams cmdParams)
{
	printf("Login\n");
	GameMsg::LoginInfo msgLogin = cmdParams.logininfo();
	std::string strUserName = msgLogin.username();
	std::string strPassword = msgLogin.password();
	int nPlayerID = g_LobbyServer->database->QueryAccount(strUserName, strPassword);

	g_LobbyServer->playerList[fd].fd = fd;
	g_LobbyServer->playerList[fd].playerID = nPlayerID;


//	Message sendMessage(SMSG_LOGIN, nPlayerID);
	g_LobbyServer->SendCommand(fd, SMSG_LOGIN);
}

void LobbyServer::RecvJoin(int fd, GameMsg::CommandParams cmdParams)
{
	bool bJoin = false;
	int roomID;
	//ÕÒÎ´ÂúµÄ·¿¼ä
	for (roomID = 0; roomID < MAX_ROOM_NUM; roomID++)
	{
		if (!g_LobbyServer->roomList[roomID]->IsFull() && !g_LobbyServer->roomList[roomID]->IsPlaying())
		{
			bJoin = g_LobbyServer->roomList[roomID]->PlayerJoinIn(fd, g_LobbyServer->playerList[fd].playerID);
			break;
		}
	}
	if (!bJoin)
	{
		printf("join failed\n");
		return;
	}
	g_LobbyServer->playerList[fd].roomID = roomID;
	//Message sendMessage(SMSG_JOIN, g_LobbyServer->playerList[fd].playerID);
	g_LobbyServer->SendCommand(fd, SMSG_JOIN);
}

void LobbyServer::RecvGameOperation(int fd, GameMsg::CommandParams cmdParams)
{
	int roomID = g_LobbyServer->playerList[fd].roomID;
	if (roomID == -1)
	{
		return;
	}
	g_LobbyServer->roomList[roomID]->AddPlayerOperation(fd, cmdParams);
}

