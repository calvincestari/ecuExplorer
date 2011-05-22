/*/
	frameDataItems.h (2005.07.31)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "ecuExplorer.h"

class frameDataItems : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameDataItems)

public:
	frameDataItems();
	virtual ~frameDataItems();

	long LoadDataItems(CMapWordToPtr* pStorage);
	void SortByColumn(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL SortTextItems(int nCol,BOOL bAscending,int low,int high);
	void OnContextMenu(CWnd* pWnd,CPoint ptMousePos);
	void OnMarkAsActive();
	void OnMarkAsInactive();

public:
	CListCtrl* lpList;
	CFont* lpFont;
	ecuExplorer* lpParent;

public:
	//{{AFX_VIRTUAL(frameDataItems)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(frameDataItems)
	afx_msg void OnSize(UINT nType,int cx,int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};