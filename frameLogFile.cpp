/*/
	frameLogFile.cpp (2005.07.31)
/*/

#include <wfc.h>

#include "frameLogFile.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"
#include "frameList.h"
#include "frameGrid.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

DWORD WINAPI callbackLogFile(void* lpClass,CListCtrl* lpList,int iIndex,BOOL bState);

IMPLEMENT_DYNCREATE(frameLogFile, CFrameWnd)

BEGIN_MESSAGE_MAP(frameLogFile, CFrameWnd)
	//{{AFX_MSG_MAP(frameLogFile
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

frameLogFile::frameLogFile()
{
	lpFont = NULL;
	lpSplitter = NULL;
}

frameLogFile::~frameLogFile()
{
	if(lpSplitter != NULL)
		delete lpSplitter;
	if(lpFont != NULL)
		delete lpFont;
}

BOOL frameLogFile::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
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
		sError.Format("Memory Error - frameLogFile::OnClientCreate() [lpSplitter] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
	}

	lpSplitter->CreateStatic(this,2,1);
	lpSplitter->CreateView(0,0,RUNTIME_CLASS(frameList),CSize(100,100),pContext);
	lpSplitter->CreateView(1,0,RUNTIME_CLASS(frameGrid),CSize(100,100),pContext);

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
	frameList* lpFrameList = (frameList*)lpSplitter->GetPane(0,0);
	lpFrameList->RegisterCallback(callbackLogFile);
	lpFrameList->SetFont(lpFont);
	lpFrameList->SetViewStyle(LVS_LIST,LVS_EX_CHECKBOXES);

EXCEPTION_BOOKMARK(__LINE__)
	frameGrid* lpFrameGrid = (frameGrid*)lpSplitter->GetPane(1,0);
	lpFrameGrid->SetFont(lpFont);

CATCHCATCH("frameLogFile::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

long frameLogFile::LoadLogFile(LPCTSTR szFilename)
{
	CString sError;
	FILE* stream = NULL;
	char szBuffer[1024];
	int iReplaceCount = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((stream = fopen(szFilename,"rt")) == NULL)
	{
		sError.Format("File Error - LoadLogFile [fopen] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError);
		return F_FILE_ERROR;
	}

EXCEPTION_BOOKMARK(__LINE__)
	frameGrid* lpFrameGrid = (frameGrid*)lpSplitter->GetPane(1,0);
	frameList* lpFrameList = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	fgets(&szBuffer[0],1024,stream);
	lpFrameList->AddCSVList(szBuffer);
	lpFrameGrid->AddCSVRow(szBuffer,TRUE);

EXCEPTION_BOOKMARK(__LINE__)
	while(!feof(stream))
	{
		fgets(&szBuffer[0],1024,stream);
		lpFrameGrid->AddCSVRow(szBuffer);
	}

	fclose(stream);

EXCEPTION_BOOKMARK(__LINE__)
	lpFrameList->lpParent = (void*)this;

CATCHCATCH("frameLogFile::LoadLogFile()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

DWORD WINAPI callbackLogFile(void* lpClass,CListCtrl* lpList,int iIndex,BOOL bState)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpClass == NULL) return ERR_SUCCESS;

EXCEPTION_BOOKMARK(__LINE__)
	frameGrid* lpFrameGrid = (frameGrid*)((frameLogFile*)lpClass)->lpSplitter->GetPane(1,0);
	frameList* lpFrameList = (frameList*)((frameLogFile*)lpClass)->lpSplitter->GetPane(0,0);
	
	lpFrameGrid->SetColumnState(lpFrameList->lpList->GetItemText(iIndex,0),bState);

CATCHCATCH("callbackLogFile()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}