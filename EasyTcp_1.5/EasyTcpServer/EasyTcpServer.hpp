#ifndef _EASYTCPSERVER_HPP_
#define _EASYTCPSERVER_HPP_

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <WinSock2.h>

	#pragma comment(lib, "ws2_32.lib")
#else
	#include <unistd.h>	    // uni std
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <string.h>

	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif

#include "MessageHeader.hpp"

class EasyTcpServer
{
public:
	EasyTcpServer()
	{
		m_sock = INVALID_SOCKET;
	}

	virtual ~EasyTcpServer()
	{
		Close();
	}

	// 初始化socket
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
			printf("<Socket=%d>关闭旧的连接...\n", (int)m_sock);
			Close();
		}

		m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_sock == INVALID_SOCKET) {
			printf("创建套接字失败!\n");
			return -1;
		}
		else
		{
			printf("创建套接字<socket=%d>成功...\n", (int)m_sock);

			return 0;
		}
	}

	// 绑定IP和端口号
	int Bind(const char* ip, unsigned short port)
	{
		/*if (INVALID_SOCKET == m_sock)
		{
			InitSock();
		}*/

		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_port = htons(port);
#ifdef _WIN32
		if (ip)
		{
			service.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else
		{
			service.sin_addr.S_un.S_addr = INADDR_ANY;
		}
#else
		if (ip)
		{
			service.sin_addr.s_addr = inet_addr(ip);
		}
		else
		{
			service.sin_addr.s_addr = INADDR_ANY;
		}
#endif
		int ret = bind(m_sock, (sockaddr*)&service, sizeof(service));
		if (SOCKET_ERROR == ret) 
		{
			printf("bind() failed.绑定网络端口号<%d>失败\n", port);
		}
		else
		{
			printf("绑定网络端口<%d>成功...\n", port);
		}

		return ret;
	}

	// 监听端口号
	int Listen(int maxConnNum)
	{
		int ret = listen(m_sock, maxConnNum);
		if (SOCKET_ERROR == ret) 
		{
			printf("socket=<%d>错误，监听网络端口失败...\n", (int)m_sock);
		}
		else
		{
			printf("socket=<%d>监听网络端口成功...\n", (int)m_sock);
		}

		return ret;
	}

	// 接收客户端连接
	SOCKET Accept()
	{
		// 4. accept 等待接受客户端连接
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);
		SOCKET ClientSocket = INVALID_SOCKET;
#ifndef _WIN32
		ClientSocket = accept(m_sock, (sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#else
		ClientSocket = accept(m_sock, (sockaddr*)&clientAddr, &nAddrLen);
#endif
		if (INVALID_SOCKET == ClientSocket)
		{
			printf("socket=<%d>错误,接收到无效客户端Socket...\n", (int)m_sock);
		}
		else
		{
			// 有新的客户端加入，向之前的所有客户端群发消息
			NewUserJoin userJoin;
			SendDataToAll(&userJoin);

			m_clientVec.push_back(ClientSocket);
			// 客户端连接成功，则显示客户端连接的IP地址和端口号
			printf("socket=<%d>新客户端<sokcet=%d>加入,Ip地址：%s,端口号：%d\n",
				(int)m_sock, (int)ClientSocket, inet_ntoa(clientAddr.sin_addr),
				ntohs(clientAddr.sin_port));
		}

		return ClientSocket;
	}
	// 关闭Sokcet
	void Close()
	{
		if (INVALID_SOCKET != m_sock)
		{
#ifdef _WIN32
			for (int n = (int)m_clientVec.size() - 1; n >= 0; n--)
			{
				closesocket(m_clientVec[n]);
			}
			closesocket(m_sock);
			// 清除Windows Socket环境
			WSACleanup();
#else
			for (int n = (int)m_clientVec.size() - 1; n >= 0; n--)
			{
				close(m_clientVec[n]);
			}
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
			// Berkeley sockets
			fd_set readfds;			// 描述符(socket)集合
			fd_set writefds;
			fd_set exceptfds;

			// 清理集合
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_ZERO(&exceptfds);

			// 将描述符(socket)加入集合
			FD_SET(m_sock, &readfds);
			FD_SET(m_sock, &writefds);
			FD_SET(m_sock, &exceptfds);

			// 获取套接字最大描述符
			SOCKET maxSock = m_sock;

			for (int n = (int)m_clientVec.size() - 1; n >= 0; n--)
			{
				FD_SET(m_clientVec[n], &readfds);

				if (m_clientVec[n] > maxSock)
				{
					maxSock = m_clientVec[n];
				}
			}

			// 设置超时时间 select 非阻塞
			timeval timeout = { 1, 0 };

			// nfds是一个整数值，是指fd_set集合中所有描述符(socket)的范围，而不是数量
			// 即是所有文件描述符最大值+1 在Windows中这个参数可以写0
			//int ret = select(ListenSocket + 1, &readfds, &writefds, &exceptfds, NULL);
			int ret = select(maxSock + 1, &readfds, &writefds, &exceptfds, &timeout);
			if (ret < 0)
			{
				printf("select任务结束...\n");
				Close();
				return false;
			}

			// 是否有数据可读
			// 判断描述符(socket)是否在集合中
			if (FD_ISSET(m_sock, &readfds))
			{
				FD_CLR(m_sock, &readfds);

				Accept();
			}

			for (int n = (int)m_clientVec.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(m_clientVec[n], &readfds))
				{
					if (-1 == RecvData(m_clientVec[n]))
					{

						auto iter = m_clientVec.begin() + n;
						if (iter != m_clientVec.end())
						{
							m_clientVec.erase(iter);
						}
					}
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

	// 接收数据 处理粘包 拆分包
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
			printf("客户端<Socket=%d>已退出，任务结束...\n", sock);
			return -1;
		}

		recv(sock, szRecv + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader), 0);

		// 响应网络消息
		OnNetMsg(sock, pHeader);

		return 0;
	}

	// 响应网络消息
	virtual void OnNetMsg(SOCKET clientSock, DataHeader* pHeader)
	{
		// 6、处理请求
		switch (pHeader->cmd)
		{
		case CMD_LOGIN:
		{
			Login *login = (Login*)pHeader;

			printf("收到客户端<Socket=%d>请求：CMD_LOGIN, 数据长度：%d, userName：%s Password： %s\n",
				m_sock, login->dataLength, login->userName, login->passWord);
			// 忽略判断用户名和密码是否正确的过程
			LoginResult ret;
			send(clientSock, (char*)&ret, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGOUT:
		{
			Logout *logout = (Logout*)pHeader;
			
			printf("收到客户端<Socket=%d>请求：CMD_LOGOUT, 数据长度：%d, userName：%s\n",
				m_sock, logout->dataLength, logout->userName);
			LogoutResult ret;
			send(clientSock, (char*)&ret, sizeof(LogoutResult), 0);
		}
		break;
		default:
		{
			DataHeader header = { 0, CMD_ERROR };
			send(clientSock, (char*)&header, sizeof(header), 0);
			break;
		}
		}
	}

	// 发送数据(单发) 发送给指定的客户端Socket
	int SendData(SOCKET sock, DataHeader* pHeader)
	{
		if (IsRun() && pHeader)
		{
			return send(sock, (const char*)pHeader, pHeader->dataLength, 0);
		}

		return SOCKET_ERROR;
	}

	// 发送数据(群发)
	void SendDataToAll(DataHeader* pHeader)
	{
		for (int n = (int)m_clientVec.size() - 1; n >= 0; n--)
		{
			SendData(m_clientVec[n], pHeader);
		}
	}

private:
	SOCKET m_sock;
	std::vector<SOCKET>	m_clientVec;	// 客户端套接字列表
};

#endif	// ! _EASYTCPSERVER_HPP_
