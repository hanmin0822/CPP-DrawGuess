
// DrawGuessClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DrawGuessClient.h"
#include "DrawGuessClientDlg.h"
#include "afxdialogex.h"
#include "Hallwin.h"
#include "Registerwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDrawGuessClientDlg 对话框



CDrawGuessClientDlg::CDrawGuessClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDrawGuessClientDlg::IDD, pParent)
	, m_usernamemailbox(_T("hanmin"))
	, m_userpwbox(_T("512240272"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDrawGuessClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_usernamemailbox);
	DDX_Text(pDX, IDC_EDIT2, m_userpwbox);
}

BEGIN_MESSAGE_MAP(CDrawGuessClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_MESSAGE(UM_LOGINMSG,&CDrawGuessClientDlg::LoginMsg)
	ON_BN_CLICKED(IDC_LOGINBUTTON, &CDrawGuessClientDlg::OnBnClickedLoginbutton)
	ON_BN_CLICKED(IDC_REGBUTTON1, &CDrawGuessClientDlg::OnBnClickedRegbutton1)
END_MESSAGE_MAP()


// CDrawGuessClientDlg 消息处理程序

BOOL CDrawGuessClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	if(!theApp.m_pTCPKernel->Open())
	{
		MessageBox(_T("服务器连接失败"));
	}
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDrawGuessClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDrawGuessClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDrawGuessClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDrawGuessClientDlg::OnClose()
{
	theApp.m_pTCPKernel->Close();
	
	CDialogEx::OnClose();
}


LRESULT CDrawGuessClientDlg::LoginMsg(WPARAM W,LPARAM L){
	UpdateData(TRUE);
	Hallwin dlg;
	theApp.m_pMainWnd = &dlg;
	EndDialog(IDOK);
	dlg.DoModal();
	return 0;
}

void CDrawGuessClientDlg::OnBnClickedLoginbutton()
{
	UpdateData(TRUE); //获取数据
	STRU_LOGIN_RQ srr;
	srr.m_nType = _DEF_PROTOCOL_LOGIN_RQ;
	if(m_usernamemailbox != "" && m_userpwbox != ""){
		WideCharToMultiByte(CP_ACP,0,m_userpwbox,-1,srr.m_szPassWord,_DEF_SIZE,0,0);
		WideCharToMultiByte(CP_ACP,0,m_usernamemailbox,-1,srr.m_szUserName,_DEF_SIZE,0,0);
		WideCharToMultiByte(CP_ACP,0,m_usernamemailbox,-1,srr.m_szEmail,_DEF_SIZE,0,0);
		srr.m_szGender = 3;
		theApp.m_pTCPKernel->SendData((char*)&srr,sizeof(srr));
	}else{
		MessageBox(_T("请完整填写所有信息！"),_T("登录失败"),MB_OK);
	}

}


void CDrawGuessClientDlg::OnBnClickedRegbutton1()
{
	UpdateData(TRUE);  
	Registerwin *regdlg = new Registerwin();
	regdlg->Create(IDD_REGDIALOG); //创建一个非模态对话框
    regdlg->ShowWindow(SW_SHOWNORMAL); //显示非模态对话框
}
