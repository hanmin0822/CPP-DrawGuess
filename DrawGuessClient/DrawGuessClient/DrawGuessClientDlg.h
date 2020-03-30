
// DrawGuessClientDlg.h : 头文件
//

#pragma once


// CDrawGuessClientDlg 对话框
class CDrawGuessClientDlg : public CDialogEx
{

// 构造
public:
	CDrawGuessClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DRAWGUESSCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	LRESULT LoginMsg(WPARAM W,LPARAM L);

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnClose();
	afx_msg void OnBnClickedLoginbutton();
	afx_msg void OnBnClickedRegbutton1();
	CString m_usernamemailbox;
	CString m_userpwbox;
};
