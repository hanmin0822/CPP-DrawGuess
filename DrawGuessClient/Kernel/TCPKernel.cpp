#include "stdafx.h"
#include "TCPKernel.h"
#include "TCPNet.h"
#include "DrawGuessClient.h"
#include "Registerwin.h"
#include "Mainwin.h"
TCPKernel::TCPKernel()
{
	m_pNet = new TCPNet(this);
}

TCPKernel::~TCPKernel()
{
	if(m_pNet)
	{
		delete m_pNet;
		m_pNet = NULL;
	}
}

ProtocolMap m_ProtocolMapEntries[] = 
{
	{_DEF_PROTOCOL_REGISTER_RS,&TCPKernel::RegisterRs},
	{_DEF_PROTOCOL_LOGIN_RS,&TCPKernel::LoginRs},
	{_DEF_PROTOCOL_STREAMINFO_RQ,&TCPKernel::StreamHeaderRq},
	{_DEF_PROTOCOL_STREAMCONTENT_RQ,&TCPKernel::StreamConentRq},

	{_DEF_PROTOCOL_CREATEROOM_RS,&TCPKernel::CreateRoomRs},
	{_DEF_PROTOCOL_PLAYERNUMRENEW_RS,&TCPKernel::ReNewPlayerNum},
	{_DEF_PROTOCOL_GOODSINFO_RS,&TCPKernel::GetGoodsInfoRs},
	{_DEF_PROTOCOL_PLAYERMESSAG_RS,&TCPKernel::GetPlayerMessageRs},
	{_DEF_PROTOCOL_TIMEOVER_RS,&TCPKernel::GameOverRs},
	{_DEF_PROTOCOL_CLOSEROOM_RS,&TCPKernel::CloseRoomRs},

	{0,0}
};

void TCPKernel::StreamHeaderRq(char* szbuf){
	 StreamInfo *psi = (StreamInfo*)szbuf;
	 colsf.CreateMemoryStream(NULL);
	 m_nFileSize = psi->m_nLen;
	 colsf.SeekToBegin();
	 m_nPos = 0;
}

void TCPKernel::StreamConentRq(char* szbuf)
{
	StreamInfo *psi = (StreamInfo*)szbuf;
	
	TRACE("%d",psi->m_nLen);

	colsf.Write(psi->m_szContent,psi->m_nLen);

	m_nPos += psi->m_nLen;
	if(m_nPos == m_nFileSize)
	{
		//图片接收完毕

		//显示图片
		//((Mainwin*)theApp.m_pMainWnd)->m_dlg.PostMessage(UM_STREAMMSG);

		colsf.SeekToBegin();
		int ns = colsf.GetLength();

		CImage m_new;
		m_new.Load(colsf.GetStream());
		CDC *pDc = theApp.m_pMainWnd->GetDlgItem(IDC_DRAWPIC)->GetDC();
		SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);

		CRect rectControl;
		CRect rectPicture;
		theApp.m_pMainWnd->GetDlgItem(IDC_DRAWPIC)->GetClientRect(rectControl);
		rectPicture = CRect(rectControl.TopLeft(), CSize((int)rectControl.Width(), (int)rectControl.Height()));

		m_new.Draw(pDc->m_hDC, rectPicture);

		m_nPos = 0;
		m_nFileSize = 0;
	}

}

void TCPKernel::CreateRoomRs(char* szbuf){
	STRU_CREATEROOM_RS * crrs = (STRU_CREATEROOM_RS *)szbuf;
	theApp.all_roomid = crrs->m_roomid;
	
	theApp.m_pMainWnd->PostMessage(UM_CREATROOMMSG);
}

void TCPKernel::ReNewPlayerNum(char* szbuf){
	STRU_PLAYERNUMRENEW_RS *prnrs = (STRU_PLAYERNUMRENEW_RS *)szbuf;
	theApp.all_playerNum = prnrs->m_playernum;
	if(theApp.all_identity != 1){
		theApp.all_roomid = prnrs->m_roomid;
	}
	
	theApp.m_pMainWnd->PostMessage(UM_PLAYERNUMRENEWMSG);

}

void TCPKernel::GetGoodsInfoRs(char* szbuf){
	STRU_GOODSINFO_RS *gsrs = (STRU_GOODSINFO_RS *)szbuf;
	theApp.all_goodinfo.m_gid = 0;//暂未实现
	strcpy_s(theApp.all_goodinfo.m_ThingName,gsrs->m_ThingName);
	strcpy_s(theApp.all_goodinfo.m_FirstHint,gsrs->m_FirstHint);
	strcpy_s(theApp.all_goodinfo.m_SecondHint,gsrs->m_SecondHint);
	strcpy_s(theApp.all_goodinfo.m_ThirdHint,gsrs->m_ThirdHint);
	strcpy_s(theApp.all_goodinfo.m_AuthorName,gsrs->m_AuthorName);
	theApp.all_goodinfo.m_GuessTime = gsrs->m_GuessTime;

	theApp.all_isGuessSuccess = 0;
	theApp.all_isGameOn = true;

	theApp.m_pMainWnd->PostMessage(UM_GETGOODSINFOMSG);
}

void TCPKernel::GetPlayerMessageRs(char* szbuf){
	STRU_PLAYERMESSAG_RS *pmrs = (STRU_PLAYERMESSAG_RS *)szbuf;

	Mainwin * pwin = (Mainwin *)theApp.m_pMainWnd;
	CString historychat;
	pwin->m_chatEdit.GetWindowTextW(historychat);
	
	if(pmrs->m_GuessResult == 1){
		USES_CONVERSION;
		historychat = historychat + A2T(pmrs->m_playername) + _T(":\r\n");
		historychat = historychat + _T("[正确答案]\r\n\r\n");
		//判断是否是自己答对了
		if(theApp.all_username == A2T(pmrs->m_playername)){
			theApp.all_isGuessSuccess = 1;
		}

	}else{
		USES_CONVERSION;
		historychat = historychat + A2T(pmrs->m_playername) + _T(":\r\n");
		historychat = historychat + A2T(pmrs->m_MessageContent) + _T("\r\n\r\n");
	}


	pwin->m_chatEdit.SetWindowTextW(historychat);
}

void TCPKernel::GameOverRs(char* szbuf){
	STRU_TIMEOVER_RS * tors = (STRU_TIMEOVER_RS *)szbuf;
	
	Mainwin * pwin = (Mainwin *)theApp.m_pMainWnd;
	CString historychat;
	pwin->m_chatEdit.GetWindowTextW(historychat);

	CString infostr;
	USES_CONVERSION;
	infostr.Format(_T("【系统】\r\n时间到，游戏结束，本轮正确答案为[%s]\r\n本轮猜对人数：%d\r\n本题来源：%s\r\n\r\n"),A2T(theApp.all_goodinfo.m_ThingName),tors->m_PassNum,A2T(theApp.all_goodinfo.m_AuthorName));
	historychat = historychat + infostr;
	pwin->m_chatEdit.SetWindowTextW(historychat);
	pwin->m_TimerTag.SetWindowTextW(_T(""));
	pwin->KillTimer(11);

	theApp.all_isGameOn = false;
	pwin->m_GameStartBtn.EnableWindow(TRUE);
}

void TCPKernel::CloseRoomRs(char* szbuf){
	theApp.m_pMainWnd->PostMessage(UM_CLOSEROOMMSG);
}

void TCPKernel::EnterRoomRs(char* szbuf){
	//后续可以修改，目前一律认为进房失败
	theApp.m_pMainWnd->PostMessage(UM_ENTERFAILMSG);
}

 //以下为已完成且调试通过代码

void TCPKernel::RegisterRs(char* szbuf)
 {
	STRU_REGISTER_RS *psrr = (STRU_REGISTER_RS*)szbuf;
    TCHAR *pSzResult = _T("注册失败");
	if(psrr->m_szResult == _register_success)
		pSzResult = _T("注册成功");

	AfxMessageBox(pSzResult);

 }

void TCPKernel::LoginRs(char* szbuf)
 {
	STRU_LOGIN_RS *psrr = (STRU_LOGIN_RS*)szbuf;
    TCHAR *pSzResult = _T("登录失败");
	if(psrr->m_szResult == _login_success)
	{
		//切换窗口
		theApp.all_userid = psrr->m_userid;
		USES_CONVERSION;
		theApp.all_username = A2T(psrr->m_szUserName);
		theApp.m_pMainWnd->PostMessage(UM_LOGINMSG);
		return;
	}

	AfxMessageBox(pSzResult);
 }

bool TCPKernel::Open() 
{
	

	if(!m_pNet->InitNetWork())
		return false;


	return true;
}

void TCPKernel::Close()
{
	
	m_pNet->UnInitNetWork();
	
}

bool TCPKernel::DealData(char* szbuf)
{
	PackType *pType =(PackType*)szbuf;
	int i = 0;
	while(1)
	{
		if(m_ProtocolMapEntries[i].m_nType == *pType)
		{
			(this->*m_ProtocolMapEntries[i].m_npfun)(szbuf);
			break;
		}
		else if(m_ProtocolMapEntries[i].m_nType == 0 && m_ProtocolMapEntries[i].m_npfun  ==0)
			break;

		i++;
	}
	//处理协议包

	return true;
}

bool TCPKernel::SendData(char* szbuf,int nlen)
{
	if(!m_pNet->SendData(szbuf,nlen))
		return false;

	return true;
}

bool TCPKernel::SendPicData(COleStreamFile* pstream,int nlen){
	//发送文件信息（文件大小）
	StreamInfo su;
	su.m_nType = _DEF_PROTOCOL_STREAMINFO_RQ;
	su.m_UserId = theApp.all_userid;
	su.m_nLen = nlen;
	ZeroMemory(su.m_szContent,4096);
	SendData((char*)&su,sizeof(su));
	
	
	//读文件内容并发送
	pstream->SeekToBegin();
	while(1)
	{
		su.m_nType = _DEF_PROTOCOL_STREAMCONTENT_RQ;
		memset(su.m_szContent,0,sizeof(su.m_szContent)/sizeof(char));
		pstream->Read(su.m_szContent,sizeof(su.m_szContent));
		
		if(nlen > sizeof(su.m_szContent))
		{
			su.m_nLen = sizeof(su.m_szContent);
			SendData((char*)&su,sizeof(su));
		}
		else{
			su.m_nLen = nlen;
			SendData((char*)&su,sizeof(su));
			break;
		}

		nlen = nlen - sizeof(su.m_szContent);
	}

	return true;

}