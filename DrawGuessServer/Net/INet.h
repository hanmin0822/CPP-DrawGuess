#ifndef  _INET_H
#define _INET_H

#include <Winsock2.h>
#include <process.h>
#include <list>
#include <map>
#pragma comment(lib,"Ws2_32.lib")

#include "Packdef.h"

class INet
{
public:
	INet()
	{}

	virtual ~INet()
	{}
public:
	virtual bool InitNetWork() = 0;
	virtual void UnInitNetWork() = 0;
	virtual bool SendData(SOCKET sock,char* szbuf,int nlen) = 0;
	virtual bool SendPicData(SOCKET sock,char* szbuf,int nlen) = 0;
};

#endif
