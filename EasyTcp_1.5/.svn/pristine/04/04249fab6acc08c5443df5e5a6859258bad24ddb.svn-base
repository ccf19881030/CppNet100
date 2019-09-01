
#include <thread>

#include "EasyTcpClient.hpp"

// 命令输入 线程入口函数
void cmdThread(EasyTcpClient* easyClient)
{
	while (true)
	{
		// 输入请求命令
		char cmdBuf[128] = { 0 };
		printf("请输入命令：[exit | login | logout | other]\n");
		scanf("%s", &cmdBuf);

		// 处理请求
		if (0 == strcmp(cmdBuf, "exit"))
		{
			easyClient->Close();
			printf("退出cmdThread线程...\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			// 5.向服务器发送命令请求
			Login login;
			strcpy(login.userName, "ccf");
			strcpy(login.passWord, "ccfPwd");

			easyClient->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			// 5.向服务器发送命令请求
			Logout logout;
			strcpy(logout.userName, "ccf");
			easyClient->SendData(&logout);
		}
		else
		{
			printf("不支持的命令，请重新输入.\n");
		}
	}
}

int main(int argc, char *argv[])
{
	EasyTcpClient easyClient;
	//easyClient.InitSock();
	easyClient.Connect("127.0.0.1", 4567);

	// 循环输入命令
	// 启动UI线程
	std::thread thread_(cmdThread, &easyClient);
	thread_.detach();

	while (easyClient.IsRun())
	{
		easyClient.OnRun();
	}

	easyClient.Close();

	printf("客户端已退出...\n");

	getchar();

	return 0;
}

