#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "EasyTcpServer.hpp"

using namespace std;

int main(int argc, char *agrv[])
{
	printf("�ȴ��ͻ�������...\n");

	EasyTcpServer easyServer;
	easyServer.InitSock();
	easyServer.Bind("192.168.191.134", 4567);
	easyServer.Listen(5);
	//easyServer.Accept();

	while (easyServer.IsRun())
	{
		easyServer.OnRun();
	}

	easyServer.Close();

	printf("��������˳����������\n");

	getchar();

	return 0;
}