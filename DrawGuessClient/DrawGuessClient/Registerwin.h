#pragma once


// Registerwin 对话框

class Registerwin : public CDialogEx
{
	DECLARE_DYNAMIC(Registerwin)

public:
	Registerwin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Registerwin();

// 对话框数据
	enum { IDD = IDD_REGDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRegbutton2();
	virtual void PostNcDestroy();
	CString m_Genderbox;
	CString m_usernamebox;
	CString m_passwordbox;
	CString m_mailbox;
};
