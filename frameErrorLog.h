/*/
	frameErrorLog.h (2005.07.30)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"

class frameErrorLog : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameErrorLog)

public:
	frameErrorLog();
	virtual ~frameErrorLog();

public:
	CFont* lpFont;

public:
	//{{AFX_VIRTUAL(frameErrorLog)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	DECLARE_MESSAGE_MAP()
};