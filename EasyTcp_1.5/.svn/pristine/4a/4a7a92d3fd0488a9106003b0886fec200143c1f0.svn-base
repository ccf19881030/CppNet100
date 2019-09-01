#ifndef _MESSAGEHEADER_H_
#define _MESSAGEHEADER_H_

// 要考虑字节对齐问题（32位和64位，平台和系统）
enum CMDTYPE
{
	CMD_LOGIN,			// 登录
	CMD_LOGIN_RESULT,	// 登录返回结果
	CMD_LOGOUT,			// 登出
	CMD_LOGOUT_RESULT,	// 登出返回结果
	CMD_NEW_USER_JOIN,	// 新用户加入
	CMD_ERROR			// 错误
};

// 消息头
struct DataHeader
{
	int	cmd;			// 命令类型
	int dataLength;		// 消息体的数据长度
};

// 消息体
// DataPackage
// 登录
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

// 登录结果
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

// 登出
struct Logout : public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

// 登出结果
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

// 新用户加入
// 登出
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
