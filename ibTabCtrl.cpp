// ibTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Tab.h"
#include "ibTabCtrl.h"


// CibTabCtrl

IMPLEMENT_DYNAMIC(CibTabCtrl, CTabCtrl)
CibTabCtrl::CibTabCtrl()
{
	m_iCurSel = -1;
}

CibTabCtrl::~CibTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CibTabCtrl, CTabCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_WM_MOVE()
END_MESSAGE_MAP()



// CibTabCtrl message handlers


void CibTabCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int iTab;
	int iPaneCount=0;
	CRect r;
	iTab=GetCurSel();
	CDialog *p;
	m_iCurSel=iTab;
	while(iPaneCount<TabPanes.GetCount())
	{
		p = TabPanes.GetAt(iPaneCount); 
		p->ShowWindow(SW_HIDE);
		iPaneCount++;
	}
	p = TabPanes.GetAt(m_iCurSel);
	GetWindowRect(r);
	p->SetWindowPos(&CWnd::wndBottom,r.left+3,r.top+25,r.Width()-7,r.Height()-30,SWP_SHOWWINDOW);  
	p->ShowWindow(SW_SHOW); 
	*pResult = 0;
}


void CibTabCtrl::AddTabPane(CString strCaption,CDialog * pDlg)
{
	TabCaptions.Add(strCaption); 
	TabPanes.Add(pDlg); 
	TC_ITEM tci;
	tci.mask = TCIF_TEXT;
	tci.pszText = (LPSTR)(LPCTSTR)strCaption;
	tci.cchTextMax = strCaption.GetLength();
	InsertItem((TabCaptions.GetCount()-1),&tci);
}

void CibTabCtrl::OnMove(int x, int y)
{
	CTabCtrl::OnMove(x, y);
	CDialog *p;
	CRect r;
	if(m_iCurSel>-1)
	{
		p = TabPanes.GetAt(m_iCurSel);
		GetWindowRect(r);
		p->SetWindowPos(&CWnd::wndBottom,r.left+3,r.top+25,r.Width()-7,r.Height()-30,SWP_SHOWWINDOW);  
		p->ShowWindow(SW_SHOW); 
	}
}


void CibTabCtrl::SetDefaultPane(int iPaneIndex)
{
	CDialog *p;
	CRect r;
	m_iCurSel = iPaneIndex;
	if(iPaneIndex<TabPanes.GetCount())
	{
		SetCurSel(iPaneIndex);
		p = TabPanes.GetAt(iPaneIndex);
		GetWindowRect(r);
		p->SetWindowPos(&CWnd::wndBottom,r.left+3,r.top+25,r.Width()-7,r.Height()-30,SWP_SHOWWINDOW);  
		p->ShowWindow(SW_SHOW); 
	}
}
