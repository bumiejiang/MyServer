#pragma once
#include "Command.pb.h"

/////////////////////////////////////
// �������ײ�����
/////////////////////////////////////
#define MAX_LISTEN_QUEUE 1			//����������������������0���У�
#define MAX_FD_SIZE 1000			//���������
#define MAX_EPOLL_EVENTS 100		//ÿ����ȡEvent�������
#define MAX_FD_NUM 30000			//���fd����
#define PER_FRAME_TIME 33			//ÿ֡�����ms��
#define MAX_FUNC_NUM 1024			//���ص���������
typedef void (*CommandFunction)(int fd, GameMsg::CommandParams cmdParams);

/////////////////////////////////////
// ��Ϣ��ʽ����
/////////////////////////////////////
#define MESSAGE_BUF_SIZE 2048		//��Ϣ����������
#define HEAD_LEN 4					//ͷ����
#define MAX_CONTENT_LEN	1000		//�����Ϣ���ݳ���

/////////////////////////////////////
// ������Ӧ������
/////////////////////////////////////
#define MAX_PLAYER_NUM 1000			//����������
#define MAX_ROOM_NUM 100			//��෿������
#define MAX_ROOM_PLAYER 10			//����������������

/////////////////////////////////////
// �ͻ�����ϢID
/////////////////////////////////////
enum EClientMessageID
{
	CMSG_NONE,
	CMSG_REGIST,			//ע��
	CMSG_LOGIN,				//��¼
	CMSG_JOIN,				//���뷿��
	CMSG_GAMEOPERA,			//��Ϸ�ڲ���
};

/////////////////////////////////////
// �������ϢID
/////////////////////////////////////
enum EServerMessageID
{
	SMSG_NONE,
	SMSG_REGIST,			//ע��
	SMSG_LOGIN,				//��¼
	SMSG_JOIN,				//���뷿��
	SMSG_GAMEOPERA,			//��Ϸ�ڲ���
};

///////////////////////////////////
// �ṹ�嶨��
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

////ID����fdתpID��pIDתfd��
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


//������Ϣ
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
