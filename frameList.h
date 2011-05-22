/*/
	frameList.h (2005.07.30)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "frameGrid.h"
#include "ecuExplorer.h"

typedef DWORD (WINAPI* callback)(void*,CListCtrl*,int,BOOL);

class frameList : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameList)

public:
	frameList();
	virtual ~frameList();

	long AddColumn(LPCTSTR szColumnName,int iWidth);
	long AddCSVList(LPCTSTR szData);
	void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	long SetViewStyle(DWORD dwNewStyle,DWORD dwNewExtendedStyle);
	void SortByColumn(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL SortTextItems(int nCol,BOOL bAscending,int low,int high);
	long AddItem(LPCTSTR szText);
	BOOL RegisterCallback(callback lpFunction);

public:
	CFont* lpFont;
	CListCtrl* lpList;
	int iColumns;
	void* lpParent;
	ecuExplorer* lpApp;
	callback fpCallback;

public:
	//{{AFX_VIRTUAL(frameList)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(frameList)
	afx_msg void OnSize(UINT nType,int cx,int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};