#ifndef  _TCPKERNEL_H
#define _TCPKERNEL_H

#include "IKernel.h"
#include "CMySql.h"
#include "MyThreadPool.h"
#include <iostream>
#include <time.h>
using namespace std;

class TCPKernel;
typedef void (TCPKernel::*PFUN)(SOCKET sock,char* szbuf);

struct ProtocolMap
{
	PackType m_nType;   //协议
	PFUN     m_npfun;   //函数地址
};

//观众信息
struct Audience_Info
{
	long long m_uid;//观众UID
	SOCKET m_sock;
	HANDLE m_hEvent;
};

struct Room_Info
{
	long long m_ownerid;//房主UID
	long long m_roomid;
	goodinfo* m_good;//猜测的物品信息
	int m_passnum;//答对的人数
	HANDLE m_hThreadGameTimer;
	char m_szbuffer[_DEF_STREAMPACKDEF];
	SOCKET m_sock;
	HANDLE   m_hSemaphore;
	bool isGameon;
};

struct Timer_Thread_Info{
	TCPKernel* m_pKernel;
	Room_Info* pRoom;
};

class TCPKernel :public IKernel
{
public:
	TCPKernel();

	virtual ~TCPKernel();
public:
	 bool Open();
	 void Close();
	 bool DealData(SOCKET sock,char* szbuf);

	 int GetRandomGoodsID();//取随机物品ID（算法防重复）
public:
	void RegisterRq(SOCKET sock,char* szbuf);//客户端注册请求
	void LoginRq(SOCKET sock,char* szbuf);//客户端登录请求
	void StreamInfoRq(SOCKET sock,char* szbuf);//转发流（图片流头和内容两部分）
	void SelectRoomRq(SOCKET sock,char* szbuf);//玩家进房

	void CreateRoomRq(SOCKET sock,char* szbuf);//房主创建房间请求
	void GameStartRq(SOCKET sock,char* szbuf);//游戏开始请求
	void PlayerMessageRq(SOCKET sock,char* szbuf);//玩家消息请求（服务器转发+判断）

	void PlayerLeaveRoomRq(SOCKET sock,char* szbuf);//玩家请求离开房间

	void LogoutRq(SOCKET sock,char* szbuf);//玩家请求下线
public:
	CMySql m_sql;
	CMyThreadPool m_threadpool;


	std::list<long long> m_OnlinePlayer;
	std::list<Room_Info*> m_lstRoomInfo;
	std::map<Room_Info*,std::list<Audience_Info*>> m_mapRoomToAudience;

	
	HANDLE m_hNullEvent;//空事件，使用WaitForSingleObject延迟时用到
	
	static  unsigned _stdcall ThreadGameTimer( void * );

};



class StreamItask : public Itask
{
public:
	StreamItask(Room_Info* pRoom,Audience_Info* pAudience,TCPKernel* pKernel)
	{
		m_pRoom = pRoom;
		m_pAudience = pAudience;
		m_pKernel = pKernel;
	}
	~StreamItask()
	{}
public:
	void RunItask();
private:
	Room_Info* m_pRoom;  //视频流
	Audience_Info* m_pAudience; //观众
	TCPKernel*   m_pKernel; //网络

};

#endif
