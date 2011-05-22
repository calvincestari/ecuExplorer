/*/
	frameDTCReader.cpp (2005.07.31)
/*/

#include <wfc.h>

#include "frameDTCReader.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"
#include "frameList.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameDTCReader, CFrameWnd)

BEGIN_MESSAGE_MAP(frameDTCReader, CFrameWnd)
	//{{AFX_MSG_MAP(frameDTCReader
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(MESSAGE_UPDATECURRENTDTC,OnUpdateCurrentDTC)
	ON_MESSAGE(MESSAGE_UPDATEHISTORICDTC,OnUpdateHistoricDTC)
END_MESSAGE_MAP()

frameDTCReader::frameDTCReader()
{
	lpFont = NULL;
	lpSplitter = NULL;
	lpParent = NULL;
}

frameDTCReader::~frameDTCReader()
{
	if(lpSplitter != NULL)
		delete lpSplitter;
	if(lpFont != NULL)
		delete lpFont;
}

BOOL frameDTCReader::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
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
	if((lpSplitter = new CSplitterWnd) == NULL)
	{
		sError.Format("Memory Error - frameDTCReader::OnClientCreate() [lpSplitter] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
	}

	lpSplitter->CreateStatic(this,2,1);
	lpSplitter->CreateView(0,0,RUNTIME_CLASS(frameList),CSize(100,100),pContext);
	lpSplitter->CreateView(1,0,RUNTIME_CLASS(frameList),CSize(100,100),pContext);

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

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpFrameList = NULL;
	lpFrameList = (frameList*)lpSplitter->GetPane(0,0);
	lpFrameList->SetFont(lpFont);
	lpFrameList->SetViewStyle(LVS_REPORT,LVS_EX_FULLROWSELECT);
	lpFrameList->AddColumn("Current Trouble Code",150);
	lpFrameList->AddColumn("Current Trouble Code Description",400);

EXCEPTION_BOOKMARK(__LINE__)
	lpFrameList = (frameList*)lpSplitter->GetPane(1,0);
	lpFrameList->SetFont(lpFont);
	lpFrameList->SetViewStyle(LVS_REPORT,LVS_EX_FULLROWSELECT);
	lpFrameList->AddColumn("Historic Trouble Code",150);
	lpFrameList->AddColumn("Historic Trouble Code Description",400);

CATCHCATCH("frameDTCReader::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameDTCReader::ResizeSplitter()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	::GetClientRect(lpParent->lpMainFrame->splitterWndLeft.m_hWnd,&rect);
	lpSplitter->SetRowInfo(0,(rect.bottom * lpParent->GetRegistryValue("Settings","splitterDTCReader",50)) / 100,40);
	lpSplitter->RecalcLayout();

CATCHCATCH("frameDTCReader::ResizeSplitter()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameDTCReader::OnUpdateCurrentDTC(WPARAM wParam,LPARAM lParam)
{
	CString sError;
	LPSTRUCT_DTCITEM lpDTCItem = (LPSTRUCT_DTCITEM)lParam;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_current = NULL;
	lpList_current = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	LVFINDINFO findInfo;
	findInfo.flags = LVFI_PARTIAL|LVFI_STRING;
	findInfo.psz = lpDTCItem->szID;

	if(lpList_current->lpList->FindItem(&findInfo) == -1)
	{
		lpList_current->lpList->InsertItem(lpList_current->lpList->GetItemCount(),lpDTCItem->szID);
		lpList_current->lpList->SetItemText(lpList_current->lpList->GetItemCount()-1,1,lpDTCItem->szLabel);
	}

CATCHCATCH("frameDTCReader::OnUpdateCurrentDTC()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameDTCReader::OnUpdateHistoricDTC(WPARAM wParam,LPARAM lParam)
{
	CString sError;
	LPSTRUCT_DTCITEM lpDTCItem = (LPSTRUCT_DTCITEM)lParam;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_historic = NULL;
	lpList_historic = (frameList*)lpSplitter->GetPane(1,0);

EXCEPTION_BOOKMARK(__LINE__)
	LVFINDINFO findInfo;
	findInfo.flags = LVFI_PARTIAL|LVFI_STRING;
	findInfo.psz = lpDTCItem->szID;

	if(lpList_historic->lpList->FindItem(&findInfo) == -1)
	{
		lpList_historic->lpList->InsertItem(lpList_historic->lpList->GetItemCount(),lpDTCItem->szID);
		lpList_historic->lpList->SetItemText(lpList_historic->lpList->GetItemCount()-1,1,lpDTCItem->szLabel);
	}

CATCHCATCH("frameDTCReader::OnUpdateHistoricDTC()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameDTCReader::OnPauseDTC()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpParent->bThreadDTCQuery == TRUE)
		lpParent->StopDTCQueryThread();
	else
		lpParent->StartDTCQueryThread();

CATCHCATCH("frameRealtime::OnPauseDataCapture()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}