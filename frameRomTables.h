/*/
	frameRealtime.h (2005.07.31)
/*/
#pragma once

#include <wfc.h>
#include <sys/timeb.h>

#include "stdafx.h"
#include "ecuExplorer.h"
#include "STabCtrl.h"

class frameRomTables : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameRomTables)

public:
	frameRomTables();
	virtual ~frameRomTables();

	void AddRomTable(LPCTSTR szName,unsigned char cType,unsigned long ulOffset);

public:
	CFont* lpFont;
	ecuExplorer* lpParent;
	CSTabCtrl* lpTab;

public:
	//{{AFX_VIRTUAL(frameRomTables)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(frameCommSerial)
	afx_msg void OnSize(UINT nType,int cx,int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};