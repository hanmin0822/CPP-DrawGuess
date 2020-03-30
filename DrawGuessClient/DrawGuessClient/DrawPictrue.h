#pragma once
#include "afxwin.h"
#include "Resource.h"
#include "DrawGuessClient.h"

class CDrawPictrue :
	public CStatic
{
public:
	CDrawPictrue(void);
	~CDrawPictrue(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);



public:
	CPoint m_Point;
	bool   m_bStart;
	HANDLE m_pen;
	COLORREF pencolor;
	int pendegree;
	bool m_iseraser;

	COleStreamFile dpist;

	void ClearScreen();
};

