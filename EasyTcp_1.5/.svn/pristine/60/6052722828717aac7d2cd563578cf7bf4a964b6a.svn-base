#ifndef _EASYTCPCLIENT_HPP_
#define _EASYTCPCLIENT_HPP_

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include<Windows.h>
	#include <WinSock2.h>
	#pragma comment(lib, "ws2_32.lib")
#else
	#include <unistd.h>		// uni std
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <string.h>

	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif

#include <stdio.h>
#include "MessageHeader.hpp"

class EasyTcpClient
{
public:
	EasyTcpClient()
	{
		m_sock = INVALID_SOCKET;
	}

	virtual ~EasyTcpClient()
	{
		if (m_sock != INVALID_SOCKET)
		{
			Close();
		}
	}

	// 初始化Sokcet
	int InitSock()
	{
		// 启动Windows Socket 2.x环境
#ifdef _WIN32
//----------------------
// Initialize Winsock
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR)
		{
			printf("WSAStartup() 错误，创建套接字库失败!\n");
			return -1;
		}
#endif

		// 建立一个Sokcet
		if (m_sock != INVALID_SOCKET)
		{
			printf("<Socket=%d>关闭旧的连接...\n", m_sock);
			Close();
		}

		m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_sock == INVALID_SOCKET) {
			printf("创建套接字失败!\n");
			return -1;
		}
		else
		{
			printf("创建套接字<socket=%d>成功...\n", m_sock);

			return 0;
		}	
	}

	// 连接服务器
	bool Connect(const char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == m_sock)
		{
			InitSock();
		}

	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
		sockaddr_in clientService = {};
		clientService.sin_family = AF_INET;
#ifdef _WIN32
		clientService.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		clientService.sin_addr.s_addr = inet_addr(ip);
#endif
		clientService.sin_port = htons(port);

		// Connect to server.
		printf("<socket=%d>正在连接服务器[%s:%d].\n", m_sock, ip, port);
		if (connect(m_sock, (sockaddr*)&clientService, sizeof(sockaddr_in)) == SOCKET_ERROR) {
			printf("<socket=%d>连接服务器[%s:%d]失败.\n", m_sock, ip, port);
			return false;
		}
		else {
			printf("客户端<socket=%d>成功服务器[%s:%d]成功.\n", m_sock, ip, port);
			return true;
		}
	}

	// 关闭套接字
	void Close()
	{
		if (INVALID_SOCKET != m_sock)
		{
#ifdef _WIN32
			closesocket(m_sock);
			// 清除Windows Socket环境
			WSACleanup();
#else
			close(m_sock);
#endif
			m_sock = INVALID_SOCKET;

		}
	}

	// 处理网络消息
	bool OnRun()
	{
		if (IsRun())
		{
			fd_set fdReads;
			FD_ZERO(&fdReads);

			FD_SET(m_sock, &fdReads);

			timeval timeout = { 0, 0 };
			int ret = select(m_sock + 1, &fdReads, NULL, NULL, &timeout);
			if (ret < 0)
			{
				printf("<socket=%d>select任务结束1...\n", m_sock);
				Close();
				return false;
			}

			if (FD_ISSET(m_sock, &fdReads))
			{
				FD_CLR(m_sock, &fdReads);

				// 有数据可读，可以处理了
				if (-1 == RecvData(m_sock))
				{
					printf("select任务结束2\n");
					Close();
					return false;
				}
			}

			return true;
		}
		
		return false;
	}

	// 是否工作中
	bool IsRun()
	{
		return m_sock != INVALID_SOCKET;
	}

	// 接收数据 处理粘包、拆分包
	int RecvData(SOCKET sock)
	{
		// 缓冲区(4096字节)
		char szRecv[4096] = {};
		// 5、接收客户端的请求
		// 先接收消息头
		int recvLen = (int)recv(sock, szRecv, sizeof(DataHeader), 0);
		DataHeader *pHeader = (DataHeader*)szRecv;
		if (recvLen <= 0)
		{
			printf("<socket=%d>与服务器断开连接，任务结束...", m_sock);
			return -1;
		}

		recv(sock, szRecv + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader), 0);

		OnNetMsg(pHeader);

		return 0;
	}

	// 响应网络消息
	void OnNetMsg(DataHeader* pHeader)
	{
		// 6、处理请求
		switch (pHeader->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult *loginRes = (LoginResult*)pHeader;

			printf("<socket=%d>收到服务器消息：CMD_LOGIN_RESULT, 数据长度：%d, result：%d\n",
				m_sock, loginRes->dataLength, loginRes->result);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult *logoutRes = (LogoutResult*)pHeader;

			printf("<socket=%d>收到服务器消息：CMD_LOGOUT_RESULT, 数据长度：%d, result：%d\n",
				m_sock, logoutRes->dataLength, logoutRes->result);
		}
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin *userJoin = (NewUserJoin*)pHeader;

			printf("<socket=%d>收到服务器消息：CMD_NEW_USER_JOIN, 数据长度：%d\n",
				m_sock, userJoin->dataLength);
		}
		break;
		}
	}

	// 发送数据
	int SendData(DataHeader* pHeader)
	{
		if (IsRun() && pHeader)
		{
			return send(m_sock, (const char*)pHeader, pHeader->dataLength, 0);
		}

		return SOCKET_ERROR;
	}

private:
	SOCKET m_sock;
};

#endif	// !_EASYTCPCLIENT_HPP_
