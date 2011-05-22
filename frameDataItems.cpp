/*/
	frameDataItems.cpp (2005.07.31)
/*/

#include <wfc.h>

#include "frameDataItems.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"
#include "frameResourceTree.h"
#include "frameRealtime.h"
#include "frameList.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameDataItems, CFrameWnd)

BEGIN_MESSAGE_MAP(frameDataItems, CFrameWnd)
	//{{AFX_MSG_MAP(frameDataItems
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_NOTIFY(HDN_ITEMCLICK,0,SortByColumn)
	ON_COMMAND(ID_POPUP_MARKASACTIVE,OnMarkAsActive)
	ON_COMMAND(ID_POPUP_MARKASINACTIVE,OnMarkAsInactive)
END_MESSAGE_MAP()

frameDataItems::frameDataItems()
{
	lpList = NULL;
	lpFont = NULL;
}

frameDataItems::~frameDataItems()
{
	if(lpList != NULL)
		delete lpList;
	if(lpFont != NULL)
		delete lpFont;
}

BOOL frameDataItems::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
{
	CString sError;

TRYTRY

	m_bAutoMenuEnable = FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	if(!CFrameWnd::OnCreateClient(lpcs,pContext))
		return FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
	if((lpList = new CListCtrl) == NULL)
	{
		sError.Format("Memory Error - frameDataItems::OnClientCreate() [lpList] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return -1;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(!lpList->Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,rect,this,1))
	{
		sError.Format("CListCtrl::Create Failed With %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return -1;
	}

EXCEPTION_BOOKMARK(__LINE__)
	DWORD dwExtendedStyle = lpList->GetExtendedStyle();
	dwExtendedStyle |= LVS_EX_FULLROWSELECT;
	lpList->SetExtendedStyle(dwExtendedStyle);	

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
	lpList->SetFont(lpFont);

EXCEPTION_BOOKMARK(__LINE__)
	lpList->InsertColumn(0,"Name",LVCFMT_LEFT,300);
	lpList->InsertColumn(1,"Status",LVCFMT_LEFT,100);
	lpList->InsertColumn(2,"Type",LVCFMT_LEFT,100);
	lpList->InsertColumn(3,"Unit",LVCFMT_LEFT,100);

CATCHCATCH("frameDataItems::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameDataItems::OnSize(UINT nType,int cx,int cy)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
	lpList->MoveWindow(rect.left,rect.top,rect.right,rect.bottom,TRUE);

CATCHCATCH("frameDataItems::OnSize()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

long frameDataItems::LoadDataItems(CMapWordToPtr* pStorage)
{
	CString sError;
	POSITION posByte = NULL;
	POSITION posBit = NULL;
	LPSTRUCT_LIVEBYTEITEM lpByteItem = NULL;
	LPSTRUCT_LIVEBITITEM lpBitItem = NULL;
	unsigned short usIndex = 0;
	unsigned short usActive = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	posByte = pStorage->GetStartPosition();
	while(posByte != NULL)
	{
		pStorage->GetNextAssoc(posByte,usIndex,(void*&)lpByteItem);
		
		posBit = lpByteItem->listBitItems.GetHeadPosition();
		while(posBit != NULL)
		{
			lpBitItem = (LPSTRUCT_LIVEBITITEM)lpByteItem->listBitItems.GetNext(posBit);
			if(lpBitItem->cBit == 0 && lpBitItem->cByte == 0)
			{
				lpList->InsertItem(lpList->GetItemCount(),lpBitItem->szName);

				if(lpParent->GetProfileInt("Settings",lpBitItem->szName,99) == 99)
					lpParent->WriteProfileInt("Settings",lpBitItem->szName,0);
				if((usActive = lpParent->GetProfileInt("Settings",lpBitItem->szName,usActive)) == 0)
					lpList->SetItemText(lpList->GetItemCount()-1,1,"Inactive");
				else
					lpList->SetItemText(lpList->GetItemCount()-1,1,"Active");

				switch(lpBitItem->cType)
				{
				case TYPE_TEMPERATURE:
					lpList->SetItemText(lpList->GetItemCount()-1,2,"Temperature");
					break;
				case TYPE_PRESSURE:
				case TYPE_CORRECTED_BOOST:
					lpList->SetItemText(lpList->GetItemCount()-1,2,"Pressure");
					break;
				case TYPE_AIRFUEL:
					lpList->SetItemText(lpList->GetItemCount()-1,2,"Air/Fuel");
					break;
				case TYPE_SPEED:
					lpList->SetItemText(lpList->GetItemCount()-1,2,"Speed");
					break;
				case TYPE_INJECTOR:
					lpList->SetItemText(lpList->GetItemCount()-1,2,"Injector");
					break;
				case TYPE_GENERAL:
					lpList->SetItemText(lpList->GetItemCount()-1,2,"General");
					break;
				case TYPE_SWITCH:
					lpList->SetItemText(lpList->GetItemCount()-1,2,"Switch");
					break;
				default:
					lpList->SetItemText(lpList->GetItemCount()-1,2,"Unknown");
					break;
				}

				lpList->SetItemText(lpList->GetItemCount()-1,3,lpBitItem->szUnit);
			}
		}
	}
	SortTextItems(0,TRUE,0,-1);

CATCHCATCH("frameDataItems::LoadKnownDataItems()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

BOOL frameDataItems::SortTextItems(int nCol,BOOL bAscending,int low,int high)
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

CATCHCATCH("frameDataItems::SortTextItems()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameDataItems::SortByColumn(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	NMLISTVIEW* pLV = (NMLISTVIEW*)pNMHDR;
	SortTextItems(pLV->iItem,TRUE,0,-1);

CATCHCATCH("frameDataItems::SortByColumn()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameDataItems::OnContextMenu(CWnd* pWnd,CPoint ptMousePos)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	ScreenToClient(&ptMousePos);

EXCEPTION_BOOKMARK(__LINE__)
	CMenu menu;
	CMenu* pPopup = NULL;

	menu.LoadMenu(IDR_POPUP_DATAITEM);
	pPopup = menu.GetSubMenu(0);

	pPopup->EnableMenuItem(ID_POPUP_MARKASACTIVE,MF_BYCOMMAND | MF_GRAYED);
	pPopup->EnableMenuItem(ID_POPUP_MARKASINACTIVE,MF_BYCOMMAND | MF_GRAYED);

EXCEPTION_BOOKMARK(__LINE__)
	if(lpList->GetSelectedCount() > 0)
	{
		int iIndex = -1;
		while((iIndex = lpList->GetNextItem(iIndex,LVNI_SELECTED)) != -1)
		{
			if(lpList->GetItemText(iIndex,1) == "Active")
				pPopup->EnableMenuItem(ID_POPUP_MARKASINACTIVE,MF_BYCOMMAND | MF_ENABLED);
			else if(lpList->GetItemText(iIndex,1) == "Inactive")
				pPopup->EnableMenuItem(ID_POPUP_MARKASACTIVE,MF_BYCOMMAND | MF_ENABLED);
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	ClientToScreen(&ptMousePos);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN,ptMousePos.x,ptMousePos.y,this);

CATCHCATCH("frameDataItems::OnContextMenu()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameDataItems::OnMarkAsActive()
{
	CString sError;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;
	unsigned short usCapture = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpList->GetSelectedCount() > 0)
	{
		sError.Empty();
		int iIndex = -1;
		while((iIndex = lpList->GetNextItem(iIndex,LVNI_SELECTED)) != -1)
		{
			if(lpList->GetItemText(iIndex,1) == "Active")
				return;

			lpList->SetItemText(iIndex,1,"Active");
			sError = lpList->GetItemText(iIndex,0);
			lpParent->WriteProfileInt("Settings",sError,1);
			break;
		}
	}

	if(sError.GetLength() == 0)
		return;

	if(lpParent->lpProtocolSSM->sECU.GetLength() == 0)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	if((lpLiveBitItem = lpParent->GetLiveDataItem(sError)) == NULL)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpParent->lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));

	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpFrameRealtime->lpSplitter->GetPane(0,0);

	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpFrameRealtime->lpSplitter->GetPane(1,0);

EXCEPTION_BOOKMARK(__LINE__)
	if(lpLiveBitItem->cType == TYPE_SWITCH)
	{
		lpList_digital->lpList->InsertItem(lpList_digital->lpList->GetItemCount(),lpLiveBitItem->szName);
		lpList_digital->lpList->SetItemText(lpList_digital->lpList->GetItemCount()-1,4,lpLiveBitItem->szUnit);
		lpList_digital->lpList->SetItemData(lpList_digital->lpList->GetItemCount()-1,lpLiveBitItem->cType);
		lpLiveBitItem->iListIndex = lpList_digital->lpList->GetItemCount()-1;

		if(lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,99) == 99)
			lpParent->WriteProfileInt("Capture",lpLiveBitItem->szName,0);
		usCapture = lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,usCapture);
		lpList_digital->lpList->SetCheck(lpList_digital->lpList->GetItemCount()-1,(BOOL)usCapture);
	}
	else
	{
		lpList_analogue->lpList->InsertItem(lpList_analogue->lpList->GetItemCount(),lpLiveBitItem->szName);
		lpList_analogue->lpList->SetItemText(lpList_analogue->lpList->GetItemCount()-1,4,lpLiveBitItem->szUnit);
		lpList_analogue->lpList->SetItemData(lpList_analogue->lpList->GetItemCount()-1,lpLiveBitItem->cType);
		lpLiveBitItem->iListIndex = lpList_analogue->lpList->GetItemCount()-1;

		if(lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,99) == 99)
			lpParent->WriteProfileInt("Capture",lpLiveBitItem->szName,0);
		usCapture = lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,usCapture);
		lpList_analogue->lpList->SetCheck(lpList_analogue->lpList->GetItemCount()-1,(BOOL)usCapture);
	}

CATCHCATCH("frameDataItems::OnMarkAsActive()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameDataItems::OnMarkAsInactive()
{
	CString sError;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;
	unsigned short usCapture = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpList->GetSelectedCount() > 0)
	{
		sError.Empty();
		int iIndex = -1;
		while((iIndex = lpList->GetNextItem(iIndex,LVNI_SELECTED)) != -1)
		{
			if(lpList->GetItemText(iIndex,1) == "Inactive")
				return;

			lpList->SetItemText(iIndex,1,"Inactive");
			sError = lpList->GetItemText(iIndex,0);
			lpParent->WriteProfileInt("Settings",sError,0);
		}
	}

	if(sError.GetLength() == 0)
		return;

	if(lpParent->lpProtocolSSM->sECU.GetLength() == 0)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	if((lpLiveBitItem = lpParent->GetLiveDataItem(sError)) == NULL)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpParent->lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));

	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpFrameRealtime->lpSplitter->GetPane(0,0);

	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpFrameRealtime->lpSplitter->GetPane(1,0);

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	if(lpLiveBitItem->cType == TYPE_SWITCH)
	{
		while((iIndex = lpList_digital->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
		{
			if(lpList_digital->lpList->GetItemText(iIndex,0) == sError)
				lpList_digital->lpList->DeleteItem(iIndex);
		}
	}
	else
	{	
		while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
		{
			if(lpList_analogue->lpList->GetItemText(iIndex,0) == sError)
				lpList_analogue->lpList->DeleteItem(iIndex);
		}
	}
	lpParent->lpProtocolSSM->RemoveLiveQueryItem(lpLiveBitItem);

CATCHCATCH("frameDataItems::OnMarkAsInactive()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}