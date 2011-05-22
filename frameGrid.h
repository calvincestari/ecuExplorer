/*/
	frameGrid.h (2005.07.30)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "GridCtrl.h"

class frameGrid : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameGrid)

public:
	frameGrid();
	virtual ~frameGrid();

	long AddCSVRow(LPCTSTR szData,BOOL bHeader = FALSE);
	long SetColumnState(LPCTSTR szColumn,BOOL bShow);
	long BuildGrid(unsigned short usX,unsigned short usY);
	long SetData_xAxis(unsigned char* cBuffer,unsigned short usLength,unsigned char cType,unsigned char cDecimals);
	long SetData_yAxis(unsigned char* cBuffer,unsigned short usLength,unsigned char cType,unsigned char cDecimals);
	long SetData_zAxis(unsigned char* cBuffer,unsigned short usLength,unsigned char cType,unsigned char cDecimals);

public:
	CFont* lpFont;
	CGridCtrl* lpGrid;

public:
	//{{AFX_VIRTUAL(frameGrid)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(frameGrid)
	afx_msg void OnSize(UINT nType,int cx,int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};