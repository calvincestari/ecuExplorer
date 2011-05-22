/*/
	frameTroubleCodes.cpp (2005.07.31)
/*/

#include <wfc.h>

#include "frameTroubleCodes.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"
#include "frameList.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameTroubleCodes, CFrameWnd)

BEGIN_MESSAGE_MAP(frameTroubleCodes, CFrameWnd)
	//{{AFX_MSG_MAP(frameTroubleCodes
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

frameTroubleCodes::frameTroubleCodes()
{
	lpFont = NULL;
	lpSplitter = NULL;
	lpParent = NULL;
}

frameTroubleCodes::~frameTroubleCodes()
{
	if(lpSplitter != NULL)
		delete lpSplitter;
	if(lpFont != NULL)
		delete lpFont;
}

BOOL frameTroubleCodes::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(!CFrameWnd::OnCreateClient(lpcs,pContext))
		return FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	if((lpSplitter = new CSplitterWnd) == NULL)
	{
		sError.Format("Memory Error - frameTroubleCodes::OnClientCreate() [lpSplitter] : %i [0x%X]",GetLastError(),GetLastError());
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
	lpFrameList->AddColumn("Subaru Trouble Code",125);
	lpFrameList->AddColumn("Subaru Trouble Code Description",500);

EXCEPTION_BOOKMARK(__LINE__)
	lpFrameList = (frameList*)lpSplitter->GetPane(1,0);
	lpFrameList->SetFont(lpFont);
	lpFrameList->SetViewStyle(LVS_REPORT,LVS_EX_FULLROWSELECT);
	lpFrameList->AddColumn("OBD Trouble Code",125);
	lpFrameList->AddColumn("OBD Trouble Code Description",500);

CATCHCATCH("frameTroubleCodes::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

long frameTroubleCodes::LoadDTC_Subaru(CPtrList* pStorage)
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_LABELITEM lpItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpFrameList = NULL;
	lpFrameList = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	pos = pStorage->GetHeadPosition();
	while(pos != NULL)
	{
		lpItem = (LPSTRUCT_LABELITEM)pStorage->GetNext(pos);
		lpFrameList->lpList->InsertItem(lpFrameList->lpList->GetItemCount(),lpItem->szID);
		lpFrameList->lpList->SetItemText(lpFrameList->lpList->GetItemCount()-1,1,lpItem->szLabel);
	}
	lpFrameList->SortTextItems(0,TRUE,0,-1);

CATCHCATCH("frameTroubleCodes::LoadDTC_Subaru()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

long frameTroubleCodes::LoadDTC_OBD(CPtrList* pStorage)
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_LABELITEM lpItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpFrameList = NULL;
	lpFrameList = (frameList*)lpSplitter->GetPane(1,0);

EXCEPTION_BOOKMARK(__LINE__)
	pos = pStorage->GetHeadPosition();
	while(pos != NULL)
	{
		lpItem = (LPSTRUCT_LABELITEM)pStorage->GetNext(pos);
		lpFrameList->lpList->InsertItem(lpFrameList->lpList->GetItemCount(),lpItem->szID);
		lpFrameList->lpList->SetItemText(lpFrameList->lpList->GetItemCount()-1,1,lpItem->szLabel);
	}
	lpFrameList->SortTextItems(0,TRUE,0,-1);

CATCHCATCH("frameTroubleCodes::LoadDTC_OBD()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameTroubleCodes::ResizeSplitter()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	::GetClientRect(lpParent->lpMainFrame->splitterWndLeft.m_hWnd,&rect);
	lpSplitter->SetRowInfo(0,(rect.bottom * lpParent->GetRegistryValue("Settings","splitterTroubleCodes",50)) / 100,40);
	lpSplitter->RecalcLayout();

CATCHCATCH("frameTroubleCodes::ResizeSplitter()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}