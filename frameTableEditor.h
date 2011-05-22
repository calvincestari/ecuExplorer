#if defined(ENABLE_EDIT_ROM)
/*/
	frameTableEditor.h (2006.01.21)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "definitionLocal.h"
#include "ecuExplorer.h"
#include "frameGrid.h"

class frameTableEditor : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameTableEditor)

public:
	frameTableEditor();
	virtual ~frameTableEditor();

	BOOL PostCreateClient(CRect* lpRect);
//	long LoadRomTable(int fhROM);

public:
	CFont* lpFont;
	ecuExplorer* lpParent;
	LPSTRUCT_MAPITEM lpMapItem;
	LPSTRUCT_ROMITEM lpRomItem;
	int iOffset_ram;
	frameGrid* lpGrid;

private:
	unsigned char* buffer_xAxis;
	unsigned char* buffer_yAxis;
	unsigned char* buffer_zAxis;

public:
	//{{AFX_VIRTUAL(frameRomTable)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(frameTableEditor)
	afx_msg void OnSize(UINT nType,int cx,int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif