/*/
	frameTroubleCodes.h (2005.07.31)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "ecuExplorer.h"

class frameTroubleCodes : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameTroubleCodes)

public:
	frameTroubleCodes();
	virtual ~frameTroubleCodes();

	long LoadDTC_Subaru(CPtrList* pStorage);
	long LoadDTC_OBD(CPtrList* pStorage);
	void ResizeSplitter();

public:
	CFont* lpFont;
	CSplitterWnd* lpSplitter;
	ecuExplorer* lpParent;

public:
	//{{AFX_VIRTUAL(frameTroubleCodes)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	DECLARE_MESSAGE_MAP()
};