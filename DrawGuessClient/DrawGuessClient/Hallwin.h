#pragma once
#include "afxwin.h"


// Hallwin 对话框

class Hallwin : public CDialogEx
{
	DECLARE_DYNAMIC(Hallwin)

public:
	Hallwin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Hallwin();

// 对话框数据
	enum { IDD = IDD_HALLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCreatroom();

	LRESULT CreateRoomMsg(WPARAM W,LPARAM L);
	LRESULT PlayerEnterRoomMsg(WPARAM W,LPARAM L);

	LRESULT PlayerEnterRoomFailMsg(WPARAM W,LPARAM L);
	afx_msg void OnClose();
	afx_msg void OnBnClickedEnterroom();
	CEdit m_RoomIDEdit;
	CFont fontA;
	virtual BOOL OnInitDialog();
};
