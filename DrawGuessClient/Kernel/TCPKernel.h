
#ifndef  _TCPKERNEL_H
#define _TCPKERNEL_H

#include "IKernel.h"

#include "Packdef.h"
class TCPKernel;
typedef  void (TCPKernel::*PFUN)(char*);
struct ProtocolMap
{
	PackType m_nType;
	PFUN     m_npfun;
};

class TCPKernel :public IKernel
{
public:
	TCPKernel();
	virtual ~TCPKernel();
public:
	 bool Open() ;
	 void Close();
	 bool DealData(char* szbuf);
	 bool SendData(char* szbuf,int nlen);
	 bool SendPicData(COleStreamFile* pstream,int nlen);
public:
	 void RegisterRs(char* szbuf);
	 void LoginRs(char* szbuf);
	 void StreamHeaderRq(char* szbuf);
	 void StreamConentRq(char* szbuf);

	 void CreateRoomRs(char* szbuf);
	 void ReNewPlayerNum(char* szbuf);
	 void GetGoodsInfoRs(char* szbuf);
	 void GetPlayerMessageRs(char* szbuf);
	 void GameOverRs(char* szbuf);
	 void CloseRoomRs(char* szbuf);
	 void EnterRoomRs(char* szbuf);

public:
	int  m_nFileSize;
	int  m_nPos;
	COleStreamFile colsf;
};





#endif

