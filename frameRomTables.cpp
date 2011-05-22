/*/
	frameRomTables.cpp (2005.01.19)
/*/

#include <wfc.h>

#include "frameRomTables.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"
#include "frameList.h"
#include "frameTableEditor.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameRomTables, CFrameWnd)

DWORD WINAPI callbackRomTables(void* lpClass,CListCtrl* lpList,int iIndex,BOOL bState);

BEGIN_MESSAGE_MAP(frameRomTables, CFrameWnd)
	//{{AFX_MSG_MAP(frameRomTables
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

frameRomTables::frameRomTables()
{
	lpFont = NULL;
	lpTab = NULL;
	lpParent = NULL;
}

frameRomTables::~frameRomTables()
{
	if(lpFont != NULL)
		delete lpFont;
	if(lpTab != NULL)
		delete lpTab;
}

BOOL frameRomTables::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
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
	if((lpTab = new CSTabCtrl) == NULL)
	{
		sError.Format("Memory Error - frameRomTables::OnClientCreate() [lpTab] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(!lpTab->Create(WS_CHILD | WS_VISIBLE | TCS_TABS | TCS_SINGLELINE,rect,this,1))
	{
		sError.Format("CSTabCtrl::Create Failed With %i [0x%X]",GetLastError(),GetLastError());
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
	lpTab->SetFont(lpFont);

EXCEPTION_BOOKMARK(__LINE__)
	lpTab->InsertItem(TAB_FUEL,"Fuel Maps");
	lpTab->InsertItem(TAB_BOOST,"Boost Maps");
	lpTab->InsertItem(TAB_TURBO,"Turbo Maps");
	lpTab->InsertItem(TAB_IGNITION,"Ignition Maps");
	lpTab->InsertItem(TAB_AVCS,"AVCS Maps");
	lpTab->InsertItem(TAB_MISCELLANEOUS,"Miscellaneous Maps");

EXCEPTION_BOOKMARK(__LINE__)
	lpTab->CreateView(TAB_FUEL,RUNTIME_CLASS(frameList),CSize(100,100),pContext);
	lpTab->CreateView(TAB_BOOST,RUNTIME_CLASS(frameList),CSize(100,100),pContext);
	lpTab->CreateView(TAB_TURBO,RUNTIME_CLASS(frameList),CSize(100,100),pContext);
	lpTab->CreateView(TAB_IGNITION,RUNTIME_CLASS(frameList),CSize(100,100),pContext);
	lpTab->CreateView(TAB_AVCS,RUNTIME_CLASS(frameList),CSize(100,100),pContext);
	lpTab->CreateView(TAB_MISCELLANEOUS,RUNTIME_CLASS(frameList),CSize(100,100),pContext);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_fuel = NULL;
	lpList_fuel = (frameList*)lpTab->GetTab(TAB_FUEL);
	lpList_fuel->lpParent = (void*)this;
	lpList_fuel->RegisterCallback(callbackRomTables);
	lpList_fuel->SetFont(lpFont);
	lpList_fuel->SetViewStyle(LVS_REPORT,LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	lpList_fuel->AddColumn("Map Name",550);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_boost = NULL;
	lpList_boost = (frameList*)lpTab->GetTab(TAB_BOOST);
	lpList_boost->lpParent = (void*)this;
	lpList_boost->RegisterCallback(callbackRomTables);
	lpList_boost->SetFont(lpFont);
	lpList_boost->SetViewStyle(LVS_REPORT,LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	lpList_boost->AddColumn("Map Name",550);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_turbo = NULL;
	lpList_turbo = (frameList*)lpTab->GetTab(TAB_TURBO);
	lpList_turbo->lpParent = (void*)this;
	lpList_turbo->RegisterCallback(callbackRomTables);
	lpList_turbo->SetFont(lpFont);
	lpList_turbo->SetViewStyle(LVS_REPORT,LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	lpList_turbo->AddColumn("Map Name",550);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_ignition = NULL;
	lpList_ignition = (frameList*)lpTab->GetTab(TAB_IGNITION);
	lpList_ignition->lpParent = (void*)this;
	lpList_ignition->RegisterCallback(callbackRomTables);
	lpList_ignition->SetFont(lpFont);
	lpList_ignition->SetViewStyle(LVS_REPORT,LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	lpList_ignition->AddColumn("Map Name",550);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_avcs = NULL;
	lpList_avcs = (frameList*)lpTab->GetTab(TAB_AVCS);
	lpList_avcs->lpParent = (void*)this;
	lpList_avcs->RegisterCallback(callbackRomTables);
	lpList_avcs->SetFont(lpFont);
	lpList_avcs->SetViewStyle(LVS_REPORT,LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	lpList_avcs->AddColumn("Map Name",550);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_miscellaneous = NULL;
	lpList_miscellaneous = (frameList*)lpTab->GetTab(TAB_MISCELLANEOUS);
	lpList_miscellaneous->lpParent = (void*)this;
	lpList_miscellaneous->RegisterCallback(callbackRomTables);
	lpList_miscellaneous->SetFont(lpFont);
	lpList_miscellaneous->SetViewStyle(LVS_REPORT,LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	lpList_miscellaneous->AddColumn("Map Name",550);

CATCHCATCH("frameRomTables::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameRomTables::OnSize(UINT nType,int cx,int cy)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	GetClientRect(&rect);

	lpTab->MoveWindow(rect.left,rect.top,rect.right,rect.bottom,TRUE);
	lpTab->ResizeControl(&CRect(rect));

CATCHCATCH("frameRomTables::ResizeSplitter()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRomTables::AddRomTable(LPCTSTR szName,unsigned char cType,unsigned long ulOffset)
{
	CString sError;
	frameList* lpList = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	switch(cType)
	{
	case MAPTYPE_AVCS:
		lpList = (frameList*)lpTab->GetTab(TAB_AVCS);
		break;
	case MAPTYPE_BOOST:
		lpList = (frameList*)lpTab->GetTab(TAB_BOOST);
		break;
	case MAPTYPE_TURBO:
		lpList = (frameList*)lpTab->GetTab(TAB_TURBO);
		break;
	case MAPTYPE_FUEL:
		lpList = (frameList*)lpTab->GetTab(TAB_FUEL);
		break;
	case MAPTYPE_IGNITION:
		lpList = (frameList*)lpTab->GetTab(TAB_IGNITION);
		break;
	case MAPTYPE_MISCELLANEOUS:
		lpList = (frameList*)lpTab->GetTab(TAB_MISCELLANEOUS);
		break;
	default:
		sError.Format("unknown map type - Map[%s] Type[%i] Offset[0x%X]",szName,cType,ulOffset);
		OutputString(sError);
		break;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(lpList != NULL)
	{
		lpList->lpList->InsertItem(lpList->lpList->GetItemCount(),szName);
		lpList->lpList->SetItemData(lpList->lpList->GetItemCount()-1,ulOffset);
	}

CATCHCATCH("frameRomTables::AddRomTable()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

DWORD WINAPI callbackRomTables(void* lpClass,CListCtrl* lpList,int iIndex,BOOL bState)
{
	CString sError;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;
	frameTableEditor* editorTable = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpClass == NULL) return ERR_SUCCESS;

EXCEPTION_BOOKMARK(__LINE__)
	if(bState)
	{
		if((editorTable = new frameTableEditor) == NULL)
		{
			sError.Format("Memory Error - callbackRomTables [editorTable] : %i [0x%X]",GetLastError()); 
			OutputString(sError,TRUE);
			return F_NO_MEMORY;
		}
		editorTable->LoadFrame(IDR_MAINFRAME);
//		editorTable->
		editorTable->ShowWindow(SW_SHOW);
	}

CATCHCATCH("callbackRomTables()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}