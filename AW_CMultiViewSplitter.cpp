// AW_CMultiViewSplitter.cpp: implementation of the AW_CMultiViewSplitter class.
//
// Written by Caroline Englebienne of AniWorld, Inc.
// Copyright (c) 2000 AniWorld, Inc. 
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AW_CMultiViewSplitter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AW_CMultiViewSplitter::AW_CMultiViewSplitter()
{
	m_nIDCounter = 1;
}

AW_CMultiViewSplitter::~AW_CMultiViewSplitter()
{

}

int AW_CMultiViewSplitter::AddView(int nRow, int nCol, CRuntimeClass * pViewClass, 
								   CCreateContext* pContext)
{
	// hide the current view of the pane
	int PreviousID = HideCurrentView(nRow, nCol);

	// create the new view, if fail, set the previous view current 
	if (CreateView(nRow, nCol, pViewClass, CSize(10,10), pContext) == 0)
	{
		if (PreviousID != -1)
			SetCurrentView(nRow, nCol, PreviousID);
		return -1;
	}

	// get and store the new view
	int NewViewID = m_nIDCounter;
	CWnd* pNewWnd = GetPane(nRow, nCol);
	CPoint pane(nRow, nCol);
	long paneID = MAKELONG(pane.x,pane.y);
	m_mapViewPane.insert(map<int, long>::value_type(NewViewID,paneID));
	m_mapIDViews.insert(map<int, CWnd*>::value_type(NewViewID, pNewWnd));

	// set the new view current
	SetCurrentView(nRow, nCol, NewViewID);

	RedrawWindow();
	m_nIDCounter ++;
	return NewViewID;
}

void AW_CMultiViewSplitter::ShowView(int nViewID)
{
	if (GetView(nViewID) == NULL)
		return;

	// find the pane containing the view 
	CPoint pane;
	GetPaneFromViewID(nViewID, &pane);

	// switch views
	HideCurrentView(pane.x, pane.y);
	SetCurrentView(pane.x, pane.y, nViewID);

	RecalcLayout();
}

CWnd* AW_CMultiViewSplitter::GetView(int nViewID)
{
	map<int, CWnd*>::iterator itView;

	itView = m_mapIDViews.find(nViewID);
	if(itView==m_mapIDViews.end())
		return NULL;
	else
		return (*itView).second;
}

CWnd* AW_CMultiViewSplitter::GetCurrentView(int nRow, int nCol, int * nCurID)
{
	long paneID = MAKELONG(nRow,nCol);
	
	map<long, int>::iterator itCur;
	itCur = m_mapCurrentViews.find(paneID);
	if (itCur == m_mapCurrentViews.end())
		return NULL;
	else
	{
		int PreviousID = (*itCur).second;
		*nCurID = PreviousID;
		return GetView(PreviousID);
	}
}

void AW_CMultiViewSplitter::SetCurrentView(int nRow, int nCol, int nViewID)
{
	long paneID = MAKELONG(nRow,nCol);

	map<long, int>::iterator itCur;
	itCur = m_mapCurrentViews.find(paneID);
	if (itCur != m_mapCurrentViews.end())
		(*itCur).second = nViewID;
	else
		m_mapCurrentViews.insert(map<long,int>::value_type(paneID,nViewID));

	CWnd * pView = GetView(nViewID);
	pView->SetDlgCtrlID(IdFromRowCol(nRow, nCol));
	pView->ShowWindow(SW_SHOW);
}

int AW_CMultiViewSplitter::HideCurrentView(int nRow, int nCol)
{
	int prevID;
	CWnd * pCurView = GetCurrentView(nRow, nCol, &prevID);
	if (pCurView == NULL)
		return -1;
	else
	{
		pCurView->SetDlgCtrlID(0);
		pCurView->ShowWindow(SW_HIDE);	
		return prevID;
	}
}

void AW_CMultiViewSplitter::GetPaneFromViewID(int nViewID, CPoint * pane)
{
	map<int, long>::iterator itPane;
	itPane = m_mapViewPane.find(nViewID);
	if (itPane==m_mapViewPane.end())
	{
		pane = NULL;
		return;
	}
	long paneID = (*itPane).second;
	CPoint p(paneID);
	pane->x = p.x;
	pane->y = p.y;
}

