#pragma once
#include "Command.pb.h"

/////////////////////////////////////
// 服务器底层配置
/////////////////////////////////////
#define MAX_LISTEN_QUEUE 1			//监听队列最大承载数（大于0就行）
#define MAX_FD_SIZE 1000			//最多连接数
#define MAX_EPOLL_EVENTS 100		//每次提取Event最大数量
#define MAX_FD_NUM 30000			//最大fd数量
#define PER_FRAME_TIME 33			//每帧间隔（ms）
#define MAX_FUNC_NUM 1024			//最大回调函数数量
typedef void (*CommandFunction)(int fd, GameMsg::CommandParams cmdParams);

/////////////////////////////////////
// 消息格式配置
/////////////////////////////////////
#define MESSAGE_BUF_SIZE 2048		//消息缓冲区长度
#define HEAD_LEN 4					//头长度
#define MAX_CONTENT_LEN	1000		//最大消息内容长度

/////////////////////////////////////
// 服务器应用配置
/////////////////////////////////////
#define MAX_PLAYER_NUM 1000			//玩家最大数量
#define MAX_ROOM_NUM 100			//最多房间数量
#define MAX_ROOM_PLAYER 10			//房间内玩家最大数量

/////////////////////////////////////
// 客户端消息ID
/////////////////////////////////////
enum EClientMessageID
{
	CMSG_NONE,
	CMSG_REGIST,			//注册
	CMSG_LOGIN,				//登录
	CMSG_JOIN,				//加入房间
	CMSG_GAMEOPERA,			//游戏内操作
};

/////////////////////////////////////
// 服务端消息ID
/////////////////////////////////////
enum EServerMessageID
{
	SMSG_NONE,
	SMSG_REGIST,			//注册
	SMSG_LOGIN,				//登录
	SMSG_JOIN,				//加入房间
	SMSG_GAMEOPERA,			//游戏内操作
};

///////////////////////////////////
// 结构体定义
///////////////////////////////////
struct Message
{
	Message()
	{
		commandID = 0;
		commandParams = GameMsg::CommandParams::default_instance();
	}
	Message(const char strMessage[])
	{
		GameMsg::Command msgCommand;
		if (!msgCommand.ParseFromString(std::string(strMessage)))
		{
			printf("Command parse failed\n");
			return;
		}
		commandID = msgCommand.commandid();
		commandParams = msgCommand.commandparams();
	}
	Message(int cmdID, GameMsg::CommandParams cmdParams = GameMsg::CommandParams::default_instance())
	{
		commandID = cmdID;
		commandParams.CopyFrom(cmdParams);
	}
	std::string ToString()
	{
		GameMsg::Command msgCommand;
		msgCommand.set_commandid(commandID);
		msgCommand.mutable_commandparams()->CopyFrom(commandParams);
		std::string strCommand;
		if (!msgCommand.SerializeToString(&strCommand))
		{
			printf("Serialize falied");
			return nullptr;
		}
		return strCommand;
	}
	int commandID;
	int playerID;
	GameMsg::CommandParams commandParams;
};

////ID管理（fd转pID，pID转fd）
//struct IDManager
//{
//	int fdList[MAX_PLAYER_NUM];
//	int playerIDList[MAX_FD_NUM];
//	IDManager()
//	{
//		memset(fdList, -1, sizeof(fdList));
//		memset(playerIDList, -1, sizeof(playerIDList));
//	}
//	int AddFd(int fd)
//	{
//		for (int i = 0; i < MAX_PLAYER_NUM; i++)
//		{
//			if (fdList[i] != -1)
//			{
//				fdList[i] = fd;
//				playerIDList[fd] = i;
//				return i;
//			}
//		}
//	}
//	void RemoveFd(int fd)
//	{
//		int pID = playerIDList[fd];
//		playerIDList[fd] = -1;
//		fdList[pID] = -1;
//	}
//	inline int GetFd(int pID)
//	{
//		return fdList[pID];
//	}
//	inline int GetPID(int fd)
//	{
//		return playerIDList[fd];
//	}
//};
//


//房间信息
/*struct Room
{
	int playerNum;
	int playerList[MAX_ROOM_PLAYER];
	Message m_Messages;
	Room()
	{
		playerNum = 0;
		memset(playerList, -1, sizeof(playerList));
	}
	int AddPlayer(int pID)
	{
		if (playerNum >= MAX_ROOM_PLAYER)
		{
			return -1;
		}

		for (int i = 0; i < MAX_ROOM_PLAYER; i++)
		{
			if (playerList[i] == -1)
			{
				playerList[i] = pID;
				playerNum++;
				return i;
			}
		}
		return -1;
	}
	void RemovePlayer(int pID)
	{
		for (int i = 0; i < MAX_ROOM_PLAYER; i++)
		{
			if (playerList[i] == pID)
			{
				playerList[i] = -1;
				playerNum--;
				break;
			}
		}
		return;
	}
	inline int GetPID(int roomPID)
	{
		return playerList[roomPID];
	}
	int GetRoomPID(int pID)
	{
		for (int i = 0; i < MAX_ROOM_PLAYER; i++)
		{
			if (playerList[i] == pID)
			{
				return i;
			}
		}
		return -1;
	}
};
*/
