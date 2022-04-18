#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>

#include "ServerDefine.h"

class Server
{
public:
	Server(const char* mIP, const int mPort);
	virtual ~Server();
	void run();
	void BindClientCommandFunction(EClientMessageID msgID, CommandFunction commandFunction);

	void DoSend(int fd, const char strSendData[]);
	void DoRedioSend(int sendFd[], int fdNum, const char strSendData[]);
	void DoRedioSendWithoutSelf(int sendFd[], int fdNum, int selfFd, const char strSendData[]);

	void SendCommand(int fd, int commandID, 
		GameMsg::CommandParams commandParams = GameMsg::CommandParams::default_instance());
	void RedioSendCommand(int sendFd[], int fdNum, int commandID, 
		GameMsg::CommandParams commandParams = GameMsg::CommandParams::default_instance());
	void RedioSendCommandWithoutSelf(int sendFd[], int fdNum, int selfFd, int commandID,
		GameMsg::CommandParams commandParams = GameMsg::CommandParams::default_instance());

protected:
	//添加事件
	void AddEvent(int fd, int state);
	//修改事件
	void ModifyEvent(int fd, int state);
	//删除事件
	void DeleteEvent(int fd, int state);

	virtual void DoRecv(int fd);
	virtual void DoAccept(int fd);
	virtual void DoDisconnect(int fd);


	//void recv_login(int fd, const char CommandParam[]);
	//void recv_join(int fd, const char CommandParam[]);
	//void recv_move(int fd, const char CommandParam[]);

	//void send_login(int fd,int pID);
	//void send_otherJoin(int fd, int pID);
	//void send_move(int fd, const char CommandParam[]);

private:
	const char*	m_IP;
	const int	m_Port;

	int	m_ListenFd;
	int	m_EpollFd;

	CommandFunction m_CommandFunction[MAX_FUNC_NUM];

	//IDManager m_IDManager;
	struct MessagePool
	{
		char MessageBuf[MESSAGE_BUF_SIZE + 1];
		int MessageBufLen;
		bool IsReadingHead;
		int ContentLen;
	};
	MessagePool m_MessagePool[MAX_PLAYER_NUM];

	//ThreadPool* pool;

	//创建套接字并进行绑定
	int socket_bind(const char* ip, int port);
	//IO多路复用epoll
	void do_epoll(int listenfd);
		//事件处理函数
	void handle_events(struct epoll_event* events, int num, int listenfd);
	//处理接收到的连接
	void handle_accept(int listenfd);

};



