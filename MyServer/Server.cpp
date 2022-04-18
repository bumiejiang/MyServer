#include <stdlib.h>
#include <string.h>
#include "Server.h"
#include "Command.pb.h"

int TakeOutStr(char strRes[], char strSrc[], int len, int srcLen)
{
	memcpy(strRes, strSrc, len);
	strRes[len] = '\0';
	memcpy(strSrc, strSrc + len, srcLen - len);
	strSrc[srcLen - len] = '\0';
	return 1;
}

Server::Server(const char* IP,int Port) :
	m_IP(IP),
	m_Port(Port)
{
	//	pool = new ThreadPool(2, 10);
}

Server::~Server()
{
}

void Server::run()
{
	m_ListenFd = socket_bind(m_IP, m_Port);
	
	if (listen(m_ListenFd, MAX_LISTEN_QUEUE) == -1)
	{
		perror("listen error: ");
		exit(1);
	}
	do_epoll(m_ListenFd);
}

int Server::socket_bind(const char* ip, const int port)
{
	int  listenfd;
	struct sockaddr_in servaddr;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		perror("socket error:");
		exit(1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
	servaddr.sin_port = htons(port);
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("bind error: ");
		exit(1);
	}
	return listenfd;
}

void Server::do_epoll(int listenfd)
{
	struct epoll_event events[MAX_EPOLL_EVENTS];
	int ret;
	//创建一个描述符
	m_EpollFd = epoll_create(MAX_FD_SIZE);
	//添加监听描述符事件
	AddEvent(listenfd, EPOLLIN);
	printf("server start\n");
	while(1)
	{
		//获取已经准备好的描述符事件
		ret = epoll_wait(m_EpollFd, events, MAX_EPOLL_EVENTS, -1);
		handle_events(events, ret, listenfd);
	}
	close(m_EpollFd);
}

void Server::handle_events(epoll_event* events, int num, int listenfd)
{
	//进行events遍历
	for (int i = 0; i < num; i++)
	{
		int fd = events[i].data.fd;
		//根据描述符的类型和事件类型进行处理
		if ((fd == listenfd) && (events[i].events & EPOLLIN))
			handle_accept(listenfd);
		else if (events[i].events & EPOLLIN)
			DoRecv(fd);
		else if (events[i].events & EPOLLOUT)
			//	do_send(fd);
			printf("send event???");
	}
}

void Server::handle_accept(int listenfd)
{
	int clifd;
	struct sockaddr_in cliaddr;
	socklen_t  cliaddrlen = sizeof(cliaddr);
	clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
	if (clifd == -1)
	{
		perror("accept error:");
		return;
	}
		
	//添加一个客户描述符和事件
	AddEvent(clifd, EPOLLIN);
	printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
	DoAccept(clifd);

}

void Server::AddEvent(int fd, int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(m_EpollFd, EPOLL_CTL_ADD, fd, &ev);
	printf("%d put in epoll\n", fd);
}

void Server::ModifyEvent(int fd, int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(m_EpollFd, EPOLL_CTL_MOD, fd, &ev);
}

void Server::DeleteEvent(int fd, int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(m_EpollFd, EPOLL_CTL_DEL, fd, &ev);
	printf("%d delete\n", fd);
}


void Server::DoRecv(int fd)
{
	int nReadRes = read(fd, m_MessagePool[fd].MessageBuf + m_MessagePool[fd].MessageBufLen, MESSAGE_BUF_SIZE - m_MessagePool[fd].MessageBufLen);
	printf("Message:%s\n", m_MessagePool[fd].MessageBuf);
	if (nReadRes == -1)
	{
		perror("read error:");
		DoDisconnect(fd);
		return;
	}
	else if (nReadRes == 0)
	{
		printf("client close.\n");
		DoDisconnect(fd);
		return;
	}
	printf("nread:%d\n", nReadRes);
	m_MessagePool[fd].MessageBufLen += nReadRes;
	m_MessagePool[fd].MessageBuf[m_MessagePool[fd].MessageBufLen] = '\0';
	

	//解析message
	while (1)
	{

		if (m_MessagePool[fd].IsReadingHead) //解析消息头获取消息长度
		{
			if (m_MessagePool[fd].MessageBufLen < HEAD_LEN)
			{
				return;
			}
			char strHead[HEAD_LEN + 1];
			if (!TakeOutStr(strHead, m_MessagePool[fd].MessageBuf, HEAD_LEN, m_MessagePool[fd].MessageBufLen))
			{
				printf("takeOutStr error\n");
				return;
			}
			m_MessagePool[fd].ContentLen = atoi(strHead);
			m_MessagePool[fd].IsReadingHead = false;
			m_MessagePool[fd].MessageBufLen -= HEAD_LEN;
		}

		if (m_MessagePool[fd].MessageBufLen < m_MessagePool[fd].ContentLen)
		{
			return;
		}

		//获取消息内容
		char strContent[MAX_CONTENT_LEN];
		if (!TakeOutStr(strContent, m_MessagePool[fd].MessageBuf, m_MessagePool[fd].ContentLen, m_MessagePool[fd].MessageBufLen))
		{
			printf("takeOutStr error\n");
			return;
		}
		m_MessagePool[fd].MessageBufLen -= m_MessagePool[fd].ContentLen;

		printf("recv content:%s\n", strContent);

		GameMsg::Command msgCommand2;
		if (!msgCommand2.ParseFromString(std::string(strContent, m_MessagePool[fd].ContentLen)))
		{
			printf("before send Command parse failed\n");
			return;
		}
		int commandID = msgCommand2.commandid();
		GameMsg::CommandParams commandParams = msgCommand2.commandparams();
		printf("Before send CommandID:%d\n", commandID);

	//	Message msgCommand(strContent);
		
	//	DoCommand(fd, commandID, commandParam);
		m_CommandFunction[commandID](fd, commandParams);

		m_MessagePool[fd].IsReadingHead = true;
	}
	
}

void Server::DoSend(int fd, const char strSend[])
{
	int sendlen = strlen(strSend);
	if (sendlen > MAX_CONTENT_LEN)
	{
		printf("send too long\n");
		return;
	}

	char strHead[HEAD_LEN + 1];
	sprintf(strHead, "%0*d", HEAD_LEN, sendlen);

	char sendBuf[HEAD_LEN + MAX_CONTENT_LEN + 1];
	sprintf(sendBuf, "%s%s", strHead, strSend);

	printf("will send:%s\n", sendBuf);

	int nsend = send(fd, sendBuf, strlen(sendBuf), 0);
	if (nsend == -1)
	{
		perror("send error:");
		DoDisconnect(fd);
	}
	else if (nsend == 0)
	{
		printf("client close.\n");
		DoDisconnect(fd);
	}
	else
	{
		printf("send complete\n");
	}
}

void Server::DoRedioSend(int arrFd[], int fdNum, const char strSendData[])
{
	for (int i = 0; i < fdNum; i++)
	{
		if (arrFd[i] > 0)
		{
			DoSend(arrFd[i], strSendData);
		}
		
	}
}

void Server::DoRedioSendWithoutSelf(int arrFd[], int fdNum, int selfFd, const char strSendData[])
{
	for (int i = 0; i < fdNum; i++)
	{
		if (arrFd[i] > 0 && arrFd[i] != selfFd)
		{
			DoSend(arrFd[i], strSendData);
		}

	}
}

void Server::SendCommand(int fd, int commandID, GameMsg::CommandParams commandParams)
{
	GameMsg::Command msgCommand;
	msgCommand.set_commandid(commandID);
	msgCommand.mutable_commandparams()->CopyFrom(commandParams);
	std::string strCommand;
	if (!msgCommand.SerializeToString(&strCommand))
	{
		printf("Serialize falied");
		return;
	}

	DoSend(fd, strCommand.c_str());
}

void Server::RedioSendCommand(int sendFd[], int fdNum, int commandID, GameMsg::CommandParams commandParams)
{
	GameMsg::Command msgCommand;
	msgCommand.set_commandid(commandID);
	msgCommand.mutable_commandparams()->CopyFrom(commandParams);
	std::string strCommand;
	if (!msgCommand.SerializeToString(&strCommand))
	{
		printf("Serialize falied");
		return;
	}

	DoRedioSend(sendFd, fdNum, strCommand.c_str());
}

void Server::RedioSendCommandWithoutSelf(int sendFd[], int fdNum, int selfFd, int commandID, GameMsg::CommandParams commandParams)
{
	GameMsg::Command msgCommand;
	msgCommand.set_commandid(commandID);
	msgCommand.mutable_commandparams()->CopyFrom(commandParams);
	std::string strCommand;
	if (!msgCommand.SerializeToString(&strCommand))
	{
		printf("Serialize falied");
		return;
	}

	DoRedioSendWithoutSelf(sendFd, fdNum, selfFd, strCommand.c_str());
}

void Server::DoAccept(int fd)
{
	memset(m_MessagePool[fd].MessageBuf, '\0', sizeof(m_MessagePool[fd].MessageBuf));
	m_MessagePool[fd].IsReadingHead = true;
	m_MessagePool[fd].ContentLen = 0;

}

void Server::DoDisconnect(int fd)
{
	//int pID = m_PlayerID[fd];
	//m_PlayerID[fd] = -1;
	//m_PlayerFD[pID] = -1;
	//
	close(fd);
	DeleteEvent(fd, EPOLLIN);
}

void Server::BindClientCommandFunction(EClientMessageID msgID, CommandFunction commandFunction)
{
	m_CommandFunction[msgID] = commandFunction;
}
