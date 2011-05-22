/*/
	frameLogFile.h (2005.07.31)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"

class frameLogFile : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameLogFile)

public:
	frameLogFile();
	virtual ~frameLogFile();

	long LoadLogFile(LPCTSTR szFilename);

public:
	CFont* lpFont;
	CSplitterWnd* lpSplitter;

public:
	//{{AFX_VIRTUAL(frameLogFile)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	DECLARE_MESSAGE_MAP()
};