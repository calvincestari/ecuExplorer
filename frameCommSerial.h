/*/
	frameCommSerial.h (2005.06.27)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "protocolSSM.h"

class frameCommSerial : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameCommSerial)

public:
	frameCommSerial();
	virtual ~frameCommSerial();

	long EnumerateSerialPorts();
	void OnContextMenu(CWnd* pWnd,CPoint ptMousePos);
	void OnSSMDiagnostics();

public:
	protocolSSM* lpProtocolSSM;
	CWinApp* lpWinApp;
	CFont* lpFont;
	CListCtrl* lpList;

public:
	//{{AFX_VIRTUAL(frameCommSerial)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(frameCommSerial)
	afx_msg void OnSize(UINT nType,int cx,int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};