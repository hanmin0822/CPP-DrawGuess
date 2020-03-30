#ifndef  _TCPNET_H
#define _TCPNET_H

#include "INet.h"
#include "IKernel.h"
class TCPNet : public INet
{
public:
	TCPNet(IKernel *pKernel);
	virtual ~TCPNet();
public:
	bool InitNetWork();
	void UnInitNetWork();
	bool SendData(SOCKET sock,char* szbuf,int nlen);
	bool SendPicData(SOCKET sock,char* szbuf,int nlen);
public:
	static  unsigned _stdcall ThreadAccept( void * );
	static  unsigned _stdcall ThreadRecv( void * );
private:
	SOCKET m_sockListen;
	std::list<HANDLE> m_lsthThreadRecv;	//接受的SOCK对象线程列表
	HANDLE m_hThreadAccept;
	static bool   m_bFlagQuit;
	static IKernel *m_pKernel;
};





#endif

