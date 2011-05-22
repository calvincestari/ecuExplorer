/*/
	frameMain.cpp (2005.06.17)
/*/

#include <wfc.h>

#include "frameMain.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "frameCommSerial.h"
#include "frameResourceTree.h"
#include "frameErrorLog.h"
#include "AW_CMultiViewSplitter.h"
#include "ecuExplorer.h"
#include "frameTroubleCodes.h"
#include "frameRealtime.h"
#include "frameDTCReader.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameMain, CFrameWnd)

BEGIN_MESSAGE_MAP(frameMain, CFrameWnd)
	//{{AFX_MSG_MAP(frameMain
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
};

frameMain::frameMain()
{
	lpParent = NULL;
}

frameMain::~frameMain()
{}

int frameMain::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	m_bAutoMenuEnable = FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	return CFrameWnd::OnCreate(lpCreateStruct);

CATCHCATCH("frameMain::OnCreate()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return -1;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

void frameMain::OnSize(UINT nType,int cx,int cy)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	CFrameWnd::OnSize(nType,cx,cy);

EXCEPTION_BOOKMARK(__LINE__)
	if(lpParent != NULL)
	{
		switch(nType)
		{
		case SIZE_MAXIMIZED:
			nType = SW_MAXIMIZE;
			break;
		case SIZE_MINIMIZED:
		case SIZE_RESTORED:
			nType = SW_SHOW;
			break;
		}
		((ecuExplorer*)lpParent)->WriteRegistryValue("Settings","WindowState",nType);

		frameResourceTree* lpFrameResourceTree = (frameResourceTree*)((ecuExplorer*)lpParent)->lpMainFrame->splitterWndLeft.GetPane(0,0);
		frameTroubleCodes* lpFrameTroubleCodes = (frameTroubleCodes*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_TROUBLECODES));
		frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
		frameDTCReader* lpFrameDTCReader = (frameDTCReader*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUTROUBLECODEREADER));

		RECT rect;
		::GetClientRect(((ecuExplorer*)lpParent)->lpMainFrame->splitterWndMain.m_hWnd,&rect);
		((ecuExplorer*)lpParent)->lpMainFrame->splitterWndMain.SetColumnInfo(0,(rect.right * ((ecuExplorer*)lpParent)->GetRegistryValue("Settings","splitterMain",25)) / 100,40);
		((ecuExplorer*)lpParent)->lpMainFrame->splitterWndMain.RecalcLayout();
		::GetClientRect(((ecuExplorer*)lpParent)->lpMainFrame->splitterWndLeft.m_hWnd,&rect);
		((ecuExplorer*)lpParent)->lpMainFrame->splitterWndLeft.SetRowInfo(0,(rect.bottom * ((ecuExplorer*)lpParent)->GetRegistryValue("Settings","splitterLeft",85)) / 100,40);
		((ecuExplorer*)lpParent)->lpMainFrame->splitterWndLeft.RecalcLayout();
		((ecuExplorer*)lpParent)->lpMainFrame->RedrawWindow();

		lpFrameTroubleCodes->ResizeSplitter();
		lpFrameRealtime->ResizeSplitter();
		lpFrameDTCReader->ResizeSplitter();
	}

CATCHCATCH("frameMain::OnSize()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameMain::OnDestroy()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	int iCur, iMin;
	float fValue;

	::GetClientRect(splitterWndMain.m_hWnd,&rect);
	splitterWndMain.GetColumnInfo(0,iCur,iMin);
	fValue = ((float)iCur / (float)rect.right) * 100;
	((ecuExplorer*)lpParent)->WriteRegistryValue("Settings","splitterMain",(int)fValue);
	::GetClientRect(splitterWndLeft.m_hWnd,&rect);
	splitterWndLeft.GetRowInfo(0,iCur,iMin);
	fValue = ((float)iCur / (float)rect.bottom) * 100;
	((ecuExplorer*)lpParent)->WriteRegistryValue("Settings","splitterLeft",(int)fValue);

	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)splitterWndLeft.GetPane(0,0);
	frameTroubleCodes* lpFrameTroubleCodes = (frameTroubleCodes*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_TROUBLECODES));
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
	frameDTCReader* lpFrameDTCReader = (frameDTCReader*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUTROUBLECODEREADER));

	::GetClientRect(lpFrameTroubleCodes->lpSplitter->m_hWnd,&rect);
	lpFrameTroubleCodes->lpSplitter->GetRowInfo(0,iCur,iMin);
	fValue = ((float)iCur / (float)rect.bottom) * 100;
	if(iCur >= iMin) ((ecuExplorer*)lpParent)->WriteRegistryValue("Settings","splitterTroubleCodes",(int)fValue);
	::GetClientRect(lpFrameRealtime->lpSplitter->m_hWnd,&rect);
	lpFrameRealtime->lpSplitter->GetRowInfo(0,iCur,iMin);
	fValue = ((float)iCur / (float)rect.bottom) * 100;
	if(iCur >= iMin) ((ecuExplorer*)lpParent)->WriteRegistryValue("Settings","splitterRealtime",(int)fValue);
	::GetClientRect(lpFrameDTCReader->lpSplitter->m_hWnd,&rect);
	lpFrameDTCReader->lpSplitter->GetRowInfo(0,iCur,iMin);
	fValue = ((float)iCur / (float)rect.bottom) * 100;
	if(iCur >= iMin) ((ecuExplorer*)lpParent)->WriteRegistryValue("Settings","splitterDTCReader",(int)fValue);

EXCEPTION_BOOKMARK(__LINE__)
	CFrameWnd::OnDestroy();

CATCHCATCH("frameMain::OnDestroy()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

BOOL frameMain::PreCreateWindow(CREATESTRUCT& cs)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	cs.lpszName = "TARI Racing Software - ecuExplorer";

CATCHCATCH("frameMain::PreCreateWindow()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return -1;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

BOOL frameMain::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	CRect rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
	splitterWndMain.CreateStatic(this,1,2);
	splitterWndMain.AddView(0,1,RUNTIME_CLASS(CEditView),pContext);

	splitterWndLeft.CreateStatic(&splitterWndMain,2,1,WS_CHILD | WS_VISIBLE | WS_BORDER,splitterWndMain.IdFromRowCol(0,0));
	splitterWndLeft.AddView(0,0,RUNTIME_CLASS(frameResourceTree),pContext);
	splitterWndLeft.AddView(1,0,RUNTIME_CLASS(frameCommSerial),pContext);

	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)splitterWndLeft.GetPane(0,0);
	lpFrameResourceTree->lpSplitter = &splitterWndMain;

EXCEPTION_BOOKMARK(__LINE__)
	splitterWndMain.SetColumnInfo(0,(rect.Width()*40/100),0);
	splitterWndMain.SetColumnInfo(1,(rect.Width()*60/100),0);

	splitterWndLeft.SetRowInfo(0,(rect.Height() - 10),0);
	splitterWndLeft.SetRowInfo(1,10,0);

CATCHCATCH("frameMain::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return -1;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}
