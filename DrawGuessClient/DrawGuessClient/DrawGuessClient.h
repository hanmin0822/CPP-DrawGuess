
// DrawGuessClient.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "TCPKernel.h"

// CDrawGuessClientApp:
// 有关此类的实现，请参阅 DrawGuessClient.cpp
//

class CDrawGuessClientApp : public CWinApp
{
public:
	IKernel *m_pTCPKernel;


	CString all_username;//全局：用户名
	long long all_userid;//全局：用户ID
	long long all_roomid;//全局：所在房间ID 为-1时代表还没进房间
	int all_identity;//全局：身份0代表未知，1代表房主，2代表观众
	int all_isGuessSuccess;//全局：玩家是否已猜中结果的标志变量，0代表未猜中，1代表猜中，2代表未在游戏状态
	int all_playerNum;//全局：当前玩家人数，为-1代表未进入游戏状态
	goodinfo all_goodinfo;//全局：当前游戏中物品信息
	int all_lefttime;//全局：剩下的时间
	bool all_isGameOn;//全局：游戏是否开始

public:
	CDrawGuessClientApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CDrawGuessClientApp theApp;