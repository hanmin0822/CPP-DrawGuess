#ifndef  _TCPNET_H
#define _TCPNET_H
#include "IKernel.h"
#include "INet.h"
#include "Packdef.h"
class TCPNet :public INet
{
public:
	TCPNet(IKernel *pKernel);
	virtual ~TCPNet();
public:
	bool InitNetWork();
	void UnInitNetWork();
	bool SendData(char* szbuf,int nlen);
public:
	static unsigned _stdcall ThreadRecv( void * lpvoid);
public:
	static IKernel *m_pKernel;
	SOCKET m_sockClient;
	HANDLE m_hThreadRecv;
	bool   m_bFlagQuit;
};

#endif
