#ifndef  _IKERNEL_H
#define _IKERNEL_H

#include "INet.h"

class IKernel
{
public:
	IKernel(){}
	virtual ~IKernel(){}
public:
	virtual bool Open() = 0;
	virtual void Close() = 0;
	virtual bool DealData(char* szbuf) = 0;
	virtual bool SendData(char* szbuf,int nlen) = 0;
	virtual bool SendPicData(COleStreamFile* pstream,int nlen) = 0;
protected:
	INet *m_pNet;
	
};





#endif
