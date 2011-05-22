/*/
	frameDTCReader.h (2005.07.31)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "ecuExplorer.h"

class frameDTCReader : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameDTCReader)

public:
	frameDTCReader();
	virtual ~frameDTCReader();

	void ResizeSplitter();
	void OnUpdateCurrentDTC(WPARAM wParam,LPARAM lParam);
	void OnUpdateHistoricDTC(WPARAM wParam,LPARAM lParam);
	void OnPauseDTC();

public:
	CFont* lpFont;
	CSplitterWnd* lpSplitter;
	ecuExplorer* lpParent;

public:
	//{{AFX_VIRTUAL(frameDTCReader)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	DECLARE_MESSAGE_MAP()
};