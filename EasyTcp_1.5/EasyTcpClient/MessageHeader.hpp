#ifndef _MESSAGEHEADER_H_
#define _MESSAGEHEADER_H_

// Ҫ�����ֽڶ������⣨32λ��64λ��ƽ̨��ϵͳ��
enum CMDTYPE
{
	CMD_LOGIN,			// ��¼
	CMD_LOGIN_RESULT,	// ��¼���ؽ��
	CMD_LOGOUT,			// �ǳ�
	CMD_LOGOUT_RESULT,	// �ǳ����ؽ��
	CMD_NEW_USER_JOIN,	// ���û�����
	CMD_ERROR			// ����
};

// ��Ϣͷ
struct DataHeader
{
	int	cmd;			// ��������
	int dataLength;		// ��Ϣ������ݳ���
};

// ��Ϣ��
// DataPackage
// ��¼
struct Login : public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};

// ��¼���
struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

// �ǳ�
struct Logout : public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

// �ǳ����
struct LogoutResult : public DataHeader
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

// ���û�����
// �ǳ�
struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};

#endif	// _MESSAGEHEADER_H_