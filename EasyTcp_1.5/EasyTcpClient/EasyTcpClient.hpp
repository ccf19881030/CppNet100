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

	// ��ʼ��Sokcet
	int InitSock()
	{
		// ����Windows Socket 2.x����
#ifdef _WIN32
//----------------------
// Initialize Winsock
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR)
		{
			printf("WSAStartup() ���󣬴����׽��ֿ�ʧ��!\n");
			return -1;
		}
#endif

		// ����һ��Sokcet
		if (m_sock != INVALID_SOCKET)
		{
			printf("<Socket=%d>�رվɵ�����...\n", m_sock);
			Close();
		}

		m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_sock == INVALID_SOCKET) {
			printf("�����׽���ʧ��!\n");
			return -1;
		}
		else
		{
			printf("�����׽���<socket=%d>�ɹ�...\n", m_sock);

			return 0;
		}	
	}

	// ���ӷ�����
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
		printf("<socket=%d>�������ӷ�����[%s:%d].\n", m_sock, ip, port);
		if (connect(m_sock, (sockaddr*)&clientService, sizeof(sockaddr_in)) == SOCKET_ERROR) {
			printf("<socket=%d>���ӷ�����[%s:%d]ʧ��.\n", m_sock, ip, port);
			return false;
		}
		else {
			printf("�ͻ���<socket=%d>�ɹ�������[%s:%d]�ɹ�.\n", m_sock, ip, port);
			return true;
		}
	}

	// �ر��׽���
	void Close()
	{
		if (INVALID_SOCKET != m_sock)
		{
#ifdef _WIN32
			closesocket(m_sock);
			// ���Windows Socket����
			WSACleanup();
#else
			close(m_sock);
#endif
			m_sock = INVALID_SOCKET;

		}
	}

	// ����������Ϣ
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
				printf("<socket=%d>select�������1...\n", m_sock);
				Close();
				return false;
			}

			if (FD_ISSET(m_sock, &fdReads))
			{
				FD_CLR(m_sock, &fdReads);

				// �����ݿɶ������Դ�����
				if (-1 == RecvData(m_sock))
				{
					printf("select�������2\n");
					Close();
					return false;
				}
			}

			return true;
		}
		
		return false;
	}

	// �Ƿ�����
	bool IsRun()
	{
		return m_sock != INVALID_SOCKET;
	}

	// �������� ����ճ������ְ�
	int RecvData(SOCKET sock)
	{
		// ������(4096�ֽ�)
		char szRecv[4096] = {};
		// 5�����տͻ��˵�����
		// �Ƚ�����Ϣͷ
		int recvLen = (int)recv(sock, szRecv, sizeof(DataHeader), 0);
		DataHeader *pHeader = (DataHeader*)szRecv;
		if (recvLen <= 0)
		{
			printf("<socket=%d>��������Ͽ����ӣ��������...", m_sock);
			return -1;
		}

		recv(sock, szRecv + sizeof(DataHeader), pHeader->dataLength - sizeof(DataHeader), 0);

		OnNetMsg(pHeader);

		return 0;
	}

	// ��Ӧ������Ϣ
	void OnNetMsg(DataHeader* pHeader)
	{
		// 6����������
		switch (pHeader->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult *loginRes = (LoginResult*)pHeader;

			printf("<socket=%d>�յ���������Ϣ��CMD_LOGIN_RESULT, ���ݳ��ȣ�%d, result��%d\n",
				m_sock, loginRes->dataLength, loginRes->result);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult *logoutRes = (LogoutResult*)pHeader;

			printf("<socket=%d>�յ���������Ϣ��CMD_LOGOUT_RESULT, ���ݳ��ȣ�%d, result��%d\n",
				m_sock, logoutRes->dataLength, logoutRes->result);
		}
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin *userJoin = (NewUserJoin*)pHeader;

			printf("<socket=%d>�յ���������Ϣ��CMD_NEW_USER_JOIN, ���ݳ��ȣ�%d\n",
				m_sock, userJoin->dataLength);
		}
		break;
		}
	}

	// ��������
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
