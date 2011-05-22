/*/
	frameList.cpp (2005.07.30)
/*/

#include <wfc.h>

#include "frameList.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"
#include "frameResourceTree.h"
#include "frameRealtime.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameList, CFrameWnd)

BEGIN_MESSAGE_MAP(frameList, CFrameWnd)
	//{{AFX_MSG_MAP(frameList
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED,1,OnItemChanged)
	ON_NOTIFY(HDN_ITEMCLICK,0,SortByColumn)
END_MESSAGE_MAP()

frameList::frameList()
{
	lpList = NULL;
	lpFont = NULL;
	iColumns = 0;
	lpParent = NULL;
	fpCallback = NULL;
	lpApp = NULL;
}

frameList::~frameList()
{
	if(lpList != NULL)
		delete lpList;
	if(lpFont != NULL)
		delete lpFont;
}

BOOL frameList::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(!CFrameWnd::OnCreateClient(lpcs,pContext))
		return FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
	if((lpList = new CListCtrl) == NULL)
	{
		sError.Format("Memory Error - frameList::OnClientCreate() [lpList] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return -1;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(!lpList->Create(WS_CHILD | WS_VISIBLE | LVS_SINGLESEL,rect,this,1))
	{
		sError.Format("CListCtrl::Create Failed With %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return -1;
	}

EXCEPTION_BOOKMARK(__LINE__)
	lpFont = new CFont;
	LOGFONT lf;

	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 0xfffffff5;
	lf.lfWeight = FW_REGULAR;
	lf.lfOutPrecision = OUT_STROKE_PRECIS;
	lf.lfClipPrecision = CLIP_STROKE_PRECIS;
	lf.lfQuality = DRAFT_QUALITY;
	lf.lfPitchAndFamily = 0x22;
	lf.lfCharSet = 0;
	strcpy(lf.lfFaceName,"Tahoma");

	lpFont->CreateFontIndirect(&lf);
	lpList->SetFont(lpFont,TRUE);

CATCHCATCH("frameList::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameList::OnSize(UINT nType,int cx,int cy)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
	lpList->MoveWindow(0,0,rect.right,rect.bottom,TRUE);

CATCHCATCH("frameList::OnSize()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

long frameList::AddColumn(LPCTSTR szColumnName,int iWidth)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpList->InsertColumn(iColumns++,szColumnName,LVCFMT_LEFT,iWidth);

	sError.Format("list column added - %s",szColumnName);
	OutputString(sError);

CATCHCATCH("frameList::AddColumn()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long frameList::SetViewStyle(DWORD dwNewStyle,DWORD dwNewExtendedStyle)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(dwNewStyle == LVS_ICON)
		lpList->ModifyStyle(LVS_REPORT | LVS_SMALLICON | LVS_LIST,dwNewStyle);
	else if(dwNewStyle == LVS_REPORT)
		lpList->ModifyStyle(LVS_ICON | LVS_SMALLICON | LVS_LIST,dwNewStyle);
	else if(dwNewStyle == LVS_SMALLICON)
		lpList->ModifyStyle(LVS_ICON | LVS_REPORT | LVS_LIST,dwNewStyle);
	else if(dwNewStyle == LVS_LIST)
		lpList->ModifyStyle(LVS_ICON | LVS_REPORT | LVS_SMALLICON,dwNewStyle);

EXCEPTION_BOOKMARK(__LINE__)
	DWORD dwExtendedStyle = lpList->GetExtendedStyle();
	dwExtendedStyle |= dwNewExtendedStyle;
	lpList->SetExtendedStyle(dwExtendedStyle);	

CATCHCATCH("frameList::SetViewStyle()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long frameList::AddCSVList(LPCTSTR szData)
{
	CString sError;
	CString sColumn;
	int iPos = 0;
	int iCol = -1;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	sError = szData;
	sError.Remove(0xA);
	sError.Remove(0xD);
	while(sError.GetLength() > 0)
	{
EXCEPTION_BOOKMARK(__LINE__)
		if((iPos = sError.Find(",",0)) < 0)
		{
			if(sError.GetLength() > 0)
				sColumn = sError;
			else
				break;
		}
		else
			sColumn = sError.Mid(0,iPos);

		lpList->InsertItem(lpList->GetItemCount(),sColumn);
		lpList->SetCheck(lpList->GetItemCount()-1,TRUE);

		sColumn += ",";
		sError.Delete(0,sColumn.GetLength());
	}

CATCHCATCH("frameList::AddCSVList()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

void frameList::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if(pNMListView->uOldState == 0 && pNMListView->uNewState == 0)
		return;

	BOOL bPrevState = (BOOL)(((pNMListView->uOldState &	LVIS_STATEIMAGEMASK)>>12)-1);
	if(bPrevState < 0)
		bPrevState = 0;

	BOOL bChecked = (BOOL)(((pNMListView->uNewState & LVIS_STATEIMAGEMASK)>>12)-1);
	if(bChecked < 0)
		bChecked = 0;

	if (bPrevState == bChecked)
		return;

//	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpParent->lpMainFrame->splitterWndLeft.GetPane(0,0);
//	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->idView_Realtime);
//	if(lpFrameRealtime->fpCSVCapture != NULL)
//	{
//		lpList->SetCheck(pNMListView->iItem,!bChecked);
//		MessageBox("Data items cannot be selected/deselected while live data is being written to a log file.\nStop the log file capture and you may change the item selection.","Invalid Action",MB_ICONWARNING | MB_OK);
//		return;
//	}

	if(fpCallback != NULL)
		fpCallback(lpParent,lpList,pNMListView->iItem,bChecked);

CATCHCATCH("frameList::OnItemChanged()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

BOOL frameList::SortTextItems(int nCol,BOOL bAscending,int low,int high)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if( nCol >= ((CHeaderCtrl*)lpList->GetDlgItem(0))->GetItemCount() )
		return FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	if( high == -1 ) high = lpList->GetItemCount() - 1;

	int lo = low;
	int hi = high;
	CString midItem;

	if( hi <= lo ) return FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	midItem = lpList->GetItemText( (lo+hi)/2, nCol );

EXCEPTION_BOOKMARK(__LINE__)
	// loop through the list until indices cross
	while( lo <= hi )
	{
		// rowText will hold all column text for one row
		CStringArray rowText;

EXCEPTION_BOOKMARK(__LINE__)
		// find the first element that is greater than or equal to 
		// the partition element starting from the left Index.
		if( bAscending )
			while( ( lo < high ) && ( lpList->GetItemText(lo, nCol) < midItem ) )
				++lo;
		else
			while( ( lo < high ) && ( lpList->GetItemText(lo, nCol) > midItem ) )
				++lo;

EXCEPTION_BOOKMARK(__LINE__)
		// find an element that is smaller than or equal to 
		// the partition element starting from the right Index.
		if( bAscending )
			while( ( hi > low ) && ( lpList->GetItemText(hi, nCol) > midItem ) )
				--hi;
		else
			while( ( hi > low ) && ( lpList->GetItemText(hi, nCol) < midItem ) )
				--hi;

EXCEPTION_BOOKMARK(__LINE__)
		// if the indexes have not crossed, swap
		// and if the items are not equal
		if( lo <= hi )
		{
EXCEPTION_BOOKMARK(__LINE__)
			// swap only if the items are not equal
			if( lpList->GetItemText(lo, nCol) != lpList->GetItemText(hi, nCol))
			{
EXCEPTION_BOOKMARK(__LINE__)
				if(lpApp != NULL)
				{
					if(lpApp->hThreadLiveQuery != NULL)
					{
						LPSTRUCT_LIVEBITITEM lpLiveBitItem_lo = lpApp->GetLiveDataItem(lo,lpList->GetItemData(lo));
						LPSTRUCT_LIVEBITITEM lpLiveBitItem_hi = lpApp->GetLiveDataItem(hi,lpList->GetItemData(hi));
						lpLiveBitItem_lo->iListIndex = hi;
						lpLiveBitItem_hi->iListIndex = lo;
					}
				}
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount = ((CHeaderCtrl*)lpList->GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
				int i;
				for( i=0; i<nColCount; i++)
					rowText[i] = lpList->GetItemText(lo, i);
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED | LVIS_FOCUSED |  LVIS_SELECTED | LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;

				lpList->GetItem( &lvitemlo );
				lpList->GetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					lpList->SetItemText(lo, i, lpList->GetItemText(hi, i));

				lvitemhi.iItem = lo;
				lpList->SetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					lpList->SetItemText(hi, i, rowText[i]);

				lvitemlo.iItem = hi;
				lpList->SetItem( &lvitemlo );
			}

			++lo;
			--hi;
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
		SortTextItems( nCol, bAscending , low, hi);

EXCEPTION_BOOKMARK(__LINE__)
	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
		SortTextItems( nCol, bAscending , lo, high );

CATCHCATCH("frameList::SortTextItems()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameList::SortByColumn(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	NMLISTVIEW* pLV = (NMLISTVIEW*)pNMHDR;
	SortTextItems(pLV->iItem,TRUE,0,-1);

CATCHCATCH("frameList::SortByColumn()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

long frameList::AddItem(LPCTSTR szText)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpList->InsertItem(lpList->GetItemCount(),szText);

CATCHCATCH("frameList::AddItem()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

BOOL frameList::RegisterCallback(callback lpFunction)
{
	if(lpFunction == NULL) return FALSE;

	fpCallback = lpFunction;
	return TRUE;
}