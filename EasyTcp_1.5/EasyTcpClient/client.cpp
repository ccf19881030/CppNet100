
#include <thread>

#include "EasyTcpClient.hpp"

// �������� �߳���ں���
void cmdThread(EasyTcpClient* easyClient)
{
	while (true)
	{
		// ������������
		char cmdBuf[128] = { 0 };
		printf("���������[exit | login | logout | other]\n");
		scanf("%s", &cmdBuf);

		// ��������
		if (0 == strcmp(cmdBuf, "exit"))
		{
			easyClient->Close();
			printf("�˳�cmdThread�߳�...\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			// 5.�������������������
			Login login;
			strcpy(login.userName, "ccf");
			strcpy(login.passWord, "ccfPwd");

			easyClient->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			// 5.�������������������
			Logout logout;
			strcpy(logout.userName, "ccf");
			easyClient->SendData(&logout);
		}
		else
		{
			printf("��֧�ֵ��������������.\n");
		}
	}
}

int main(int argc, char *argv[])
{
	EasyTcpClient easyClient;
	//easyClient.InitSock();
	easyClient.Connect("192.168.191.1", 4567);

	// ѭ����������
	// ����UI�߳�
	std::thread thread_(cmdThread, &easyClient);
	thread_.detach();

	while (easyClient.IsRun())
	{
		easyClient.OnRun();
	}

	easyClient.Close();

	printf("�ͻ������˳�...\n");

	getchar();

	return 0;
}

