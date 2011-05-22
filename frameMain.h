/*/
	frameMain.h (2005.06.17)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "AW_CMultiViewSplitter.h"

class frameMain : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameMain)

public:
	frameMain();
	virtual ~frameMain();

	BOOL PostCreateClient(CRect* lpRect);

public:
	AW_CMultiViewSplitter splitterWndMain;
	AW_CMultiViewSplitter splitterWndLeft;
	void* lpParent;

public:
	//{{AFX_VIRTUAL(frameMain)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext *pContext);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(frameMain)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType,int cx,int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};