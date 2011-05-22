/*/
	frameErrorLog.cpp (2005.06.30)
/*/

#include <wfc.h>

#include "frameErrorLog.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameErrorLog, CFrameWnd)

BEGIN_MESSAGE_MAP(frameErrorLog, CFrameWnd)
	//{{AFX_MSG_MAP(frameErrorLog
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

frameErrorLog::frameErrorLog()
{
	lpFont = NULL;
}

frameErrorLog::~frameErrorLog()
{
	if(lpFont != NULL)
		delete lpFont;
}

BOOL frameErrorLog::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
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

CATCHCATCH("frameErrorLog::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}