// Registerwin.cpp : 实现文件
//

#include "stdafx.h"
#include "DrawGuessClient.h"
#include "Registerwin.h"
#include "afxdialogex.h"


// Registerwin 对话框

IMPLEMENT_DYNAMIC(Registerwin, CDialogEx)

Registerwin::Registerwin(CWnd* pParent /*=NULL*/)
	: CDialogEx(Registerwin::IDD, pParent)
	, m_Genderbox(_T("男"))
	, m_usernamebox(_T(""))
	, m_passwordbox(_T(""))
	, m_mailbox(_T(""))
{

}

Registerwin::~Registerwin()
{
}

void Registerwin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBO1, m_Genderbox);
	DDX_Text(pDX, IDC_EDIT1, m_usernamebox);
	DDX_Text(pDX, IDC_EDIT2, m_passwordbox);
	DDX_Text(pDX, IDC_EDIT3, m_mailbox);
}


BEGIN_MESSAGE_MAP(Registerwin, CDialogEx)
	ON_BN_CLICKED(IDC_REGBUTTON2, &Registerwin::OnBnClickedRegbutton2)
END_MESSAGE_MAP()


// Registerwin 消息处理程序


void Registerwin::OnBnClickedRegbutton2()
{
	UpdateData(TRUE); //获取数据
	if(m_Genderbox != "" && m_usernamebox != "" && m_passwordbox != "" && m_mailbox != ""){
		STRU_REGISTER_RQ srr;
		srr.m_nType = _DEF_PROTOCOL_REGISTER_RQ;
		WideCharToMultiByte(CP_ACP,0,m_passwordbox,-1,srr.m_szPassWord,_DEF_SIZE,0,0);
		WideCharToMultiByte(CP_ACP,0,m_usernamebox,-1,srr.m_szUserName,_DEF_SIZE,0,0);
		WideCharToMultiByte(CP_ACP,0,m_mailbox,-1,srr.m_szEmail,_DEF_SIZE,0,0);
		if(m_Genderbox == _T("男")){
			srr.m_szGender = 1;
		}else if(m_Genderbox == _T("女")){
			srr.m_szGender = 0;
		}else{
			srr.m_szGender = 2;
		}

		theApp.m_pTCPKernel->SendData((char*)&srr,sizeof(srr));
	}else{
		MessageBox(_T("请完整填写所有信息！"),_T("注册失败"),MB_OK);
	}

}


void Registerwin::PostNcDestroy()
{
	delete this;
	CDialogEx::PostNcDestroy();
}
