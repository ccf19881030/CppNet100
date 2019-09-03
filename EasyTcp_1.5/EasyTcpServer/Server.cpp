#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "EasyTcpServer.hpp"

using namespace std;

int main(int argc, char *agrv[])
{
	printf("等待客户端连接...\n");

	EasyTcpServer easyServer;
	easyServer.InitSock();
	easyServer.Bind("127.0.0.1", 4567);
	easyServer.Listen(5);
	//easyServer.Accept();

	while (easyServer.IsRun())
	{
		easyServer.OnRun();
	}

	easyServer.Close();

	printf("服务端已退出，任务结束\n");

	getchar();

	return 0;
}
