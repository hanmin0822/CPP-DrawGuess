// Hallwin.cpp : 实现文件
//

#include "stdafx.h"
#include "DrawGuessClient.h"
#include "Hallwin.h"
#include "afxdialogex.h"
#include "Mainwin.h"

// Hallwin 对话框

IMPLEMENT_DYNAMIC(Hallwin, CDialogEx)

Hallwin::Hallwin(CWnd* pParent /*=NULL*/)
	: CDialogEx(Hallwin::IDD, pParent)
{

}

Hallwin::~Hallwin()
{
}

void Hallwin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROOMNUMEDT, m_RoomIDEdit);
}


BEGIN_MESSAGE_MAP(Hallwin, CDialogEx)
	ON_BN_CLICKED(IDC_CREATROOM, &Hallwin::OnBnClickedCreatroom)
	ON_MESSAGE(UM_CREATROOMMSG,&Hallwin::CreateRoomMsg)
	ON_MESSAGE(UM_PLAYERNUMRENEWMSG,&Hallwin::PlayerEnterRoomMsg)
	ON_MESSAGE(UM_ENTERFAILMSG,&Hallwin::PlayerEnterRoomFailMsg)

	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENTERROOM, &Hallwin::OnBnClickedEnterroom)
END_MESSAGE_MAP()


// Hallwin 消息处理程序


void Hallwin::OnBnClickedCreatroom()
{
	STRU_CREATEROOM_RQ crrq;
	crrq.m_nType = _DEF_PROTOCOL_CREATEROOM_RQ;
	crrq.m_roomid = 0;
	crrq.m_uid = theApp.all_userid;
	WideCharToMultiByte(CP_ACP,0,theApp.all_username,-1,crrq.m_PlayerName,_DEF_SIZE,0,0);

	theApp.m_pTCPKernel->SendData((char*)&crrq,sizeof(crrq));

}

LRESULT Hallwin::CreateRoomMsg(WPARAM W,LPARAM L){
	UpdateData(TRUE);
	Mainwin dlg;
	theApp.all_identity = 1;
	theApp.all_playerNum = 1;
	theApp.m_pMainWnd = &dlg;
	EndDialog(IDOK);
	dlg.DoModal();
	return 0;
}

LRESULT Hallwin::PlayerEnterRoomMsg(WPARAM W,LPARAM L){
	UpdateData(TRUE);
	Mainwin dlg;
	theApp.all_identity = 2;
	theApp.m_pMainWnd = &dlg;
	EndDialog(IDOK);
	dlg.DoModal();
	return 0;
}

void Hallwin::OnClose()
{
	STRU_LOGOUT_RQ lorq;
	lorq.m_nType = _DEF_PROTOCOL_LOGOUT_RQ;
	lorq.m_userid = theApp.all_userid;

	theApp.m_pTCPKernel->SendData((char*)&lorq,sizeof(lorq));

	CDialogEx::OnClose();
}

void Hallwin::OnBnClickedEnterroom()
{
	UpdateData(TRUE);
	STRU_ENTERROOM_RQ errq;
	errq.m_nType = _DEF_PROTOCOL_ENTERROOM_RQ;
	errq.m_uid = theApp.all_userid;
	WideCharToMultiByte(CP_ACP,0,theApp.all_username,-1,errq.m_PlayerName,_DEF_SIZE,0,0);

	CString RoomID;
	m_RoomIDEdit.GetWindowTextW(RoomID);
	char rid[_DEF_SIZE];
	WideCharToMultiByte(CP_ACP,0,RoomID,-1,rid,_DEF_SIZE,0,0);

	errq.m_roomid = _atoi64(rid);

	theApp.m_pTCPKernel->SendData((char*)&errq,sizeof(errq));
}

BOOL Hallwin::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	fontA.CreatePointFont(200,L"黑体");

	m_RoomIDEdit.SetFont(&fontA);


	return TRUE;
}

LRESULT Hallwin::PlayerEnterRoomFailMsg(WPARAM W,LPARAM L){
	MessageBox(_T("进房失败，可能是无法查找到房间或状态异常！"),_T("提示"),MB_OK);
	return 0;
}