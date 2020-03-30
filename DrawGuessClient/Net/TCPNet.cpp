#include "stdafx.h"
#include "TCPNet.h"


IKernel *TCPNet::m_pKernel = NULL;
TCPNet::TCPNet(IKernel *pKernel)
{
	m_sockClient = NULL;
	m_pKernel = pKernel;
	m_bFlagQuit = true;
	m_hThreadRecv = NULL;
}


TCPNet::~TCPNet(void)
{

}
bool TCPNet::InitNetWork()
{
	//初始化
	WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        return false;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        UnInitNetWork();
        return false;
    }

	
	m_sockClient = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(m_sockClient == INVALID_SOCKET )
	{
		 UnInitNetWork();
		 return false;
	}

	sockaddr_in  addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	addrServer.sin_port = htons(_DEFPORT);
	if(SOCKET_ERROR ==connect(m_sockClient,(const sockaddr*)&addrServer,sizeof(addrServer)))
	{
		 UnInitNetWork();
		 return false;
	}
	
	//创建线程 -- recv
	m_hThreadRecv = (HANDLE) _beginthreadex(NULL,0,&TCPNet::ThreadRecv,this,0,0);
	return true;
}


unsigned _stdcall TCPNet::ThreadRecv( void * lpvoid){
	TCPNet* pthis = (TCPNet*)lpvoid;
	char *pszbuf = NULL;
	int nRelReadNum;
	int nPackSize;

	while(pthis->m_bFlagQuit){

		//先接受包大小
		nRelReadNum =  recv(pthis->m_sockClient,(char*)&nPackSize,sizeof(int),0);
		if(nRelReadNum <=0){
			continue;
		}
		pszbuf =  new char[nPackSize];
		int offset = 0;
		//数据包
		while(nPackSize){
			nRelReadNum =  recv(pthis->m_sockClient,pszbuf + offset,nPackSize,0);
			offset += nRelReadNum;
			nPackSize -= nRelReadNum;
		}

		//处理
		m_pKernel->DealData(pszbuf);

		delete []pszbuf;
		pszbuf = NULL;
	}
	return 0;
}

void TCPNet::UnInitNetWork()
{
	m_bFlagQuit = false;
	if(m_hThreadRecv )
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hThreadRecv,100))
			TerminateThread(m_hThreadRecv,-1);
		
		CloseHandle(m_hThreadRecv);
		m_hThreadRecv = NULL;

	
	}
	
	WSACleanup();
	if(m_sockClient)
	{
		closesocket(m_sockClient);
		m_sockClient = NULL;
	}
}

bool TCPNet::SendData(char* szbuf,int nlen)
{
	if(!szbuf || nlen <=0)
		return false;

	//包大小 --4
	if(send(m_sockClient,(const char*)&nlen,sizeof(int),0)<0)
		return false;

	if(send(m_sockClient,szbuf,nlen,0)<0)
		return false;
	
	return true;
}