#if defined(ENABLE_EDIT_ROM)
/*/
	frameTableEditor.cpp (2006.01.21)
/*/

#include <wfc.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "frameTableEditor.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameTableEditor, CFrameWnd)

BEGIN_MESSAGE_MAP(frameTableEditor, CFrameWnd)
	//{{AFX_MSG_MAP(frameTableEditor
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

frameTableEditor::frameTableEditor()
{
	lpFont = NULL;
	lpParent = NULL;
	lpMapItem = NULL;
	lpRomItem = NULL;
	iOffset_ram = 0;

	buffer_xAxis = NULL;
	buffer_yAxis = NULL;
	buffer_zAxis = NULL;
}

frameTableEditor::~frameTableEditor()
{
	if(lpFont != NULL)
		delete lpFont;

	if(buffer_xAxis != NULL)
		free(buffer_xAxis);
	if(buffer_yAxis != NULL)
		free(buffer_yAxis);
	if(buffer_zAxis != NULL)
		free(buffer_zAxis);
}

void frameTableEditor::OnSize(UINT nType,int cx,int cy)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	CFrameWnd::OnSize(nType,cx,cy);

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
//	lpGrid->MoveWindow(rect.left,rect.top,rect.right,rect.bottom,TRUE);

CATCHCATCH("frameTableEditor::OnSize()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

BOOL frameTableEditor::PreCreateWindow(CREATESTRUCT& cs)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	cs.lpszName = "Table Editor";
	cs.style = WS_CAPTION | WS_THICKFRAME | WS_SYSMENU;
	cs.dwExStyle = WS_EX_TOOLWINDOW | WS_EX_CLIENTEDGE;
	cs.hMenu = NULL;
	cs.cx = 350;
	cs.cy = 175;

CATCHCATCH("frameTableEditor::PreCreateWindow()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return -1;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

BOOL frameTableEditor::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(!CFrameWnd::OnCreateClient(lpcs,pContext))
		return FALSE;

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
	CRect rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
	if((lpGrid = new frameGrid) == NULL)
	{
		sError.Format("Memory Error - callbackRomTables [lpGrid] : %i [0x%X]",GetLastError()); 
		OutputString(sError,TRUE);
		return F_NO_MEMORY;
	}
	lpGrid->Create(NULL,NULL,WS_CHILD | WS_VISIBLE,rect,this,NULL,0,pContext);

CATCHCATCH("frameTableEditor::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}
/*
long frameTableEditor::LoadRomTable(int fhROM)
{
	CString sError;
	unsigned char cLengthX = 0;
	unsigned char cLengthY = 0;
	unsigned char cLengthZ = 0;
	unsigned char cMultiplierX = 0;
	unsigned char cMultiplierY = 0;
	unsigned char cMultiplierZ = 0;
	unsigned long* lpOffset = NULL;
	unsigned char cMapType;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMapItem == NULL)
		return F_NULL_TABLE_DEFINITION;
	if(lpRomItem == NULL)
		return F_NULL_ROM_DEFINITION;

EXCEPTION_BOOKMARK(__LINE__)
	if((lpRomItem->mapOffset.Lookup((WORD)lpMapItem->iIndex_lookup,(void*&)lpOffset)) == 0)
		return F_INVALID_MAP_TYPE_OFFSET;

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMapItem->axisY.cLength_label > 0 && lpMapItem->axisY.cType_unit > 0)
	{
		_lseek(fhROM,*lpOffset+iOffset_ram,SEEK_SET);
		_read(fhROM,&cMapType,1);

EXCEPTION_BOOKMARK(__LINE__)
		switch(cMapType)
		{
		case MAP_3D_y16x8d8:
			cMultiplierX = 1;
			cMultiplierY = 2;
			cMultiplierZ = 1;
			break;
		case MAP_3D_y16x8d16:
			cMultiplierX = 1;
			cMultiplierY = 2;
			cMultiplierZ = 2;
			break;
		case MAP_3D_y16x16d8:
			cMultiplierX = 2;
			cMultiplierY = 2;
			cMultiplierZ = 1;
			break;
		case MAP_3D_y16x16d16:
			cMultiplierX = 2;
			cMultiplierY = 2;
			cMultiplierZ = 2;
			break;
		case MAP_2D_y8d8:
			cMultiplierX = 0;
			cMultiplierY = 1;
			cMultiplierZ = 1;
			break;
		case MAP_2D_y16d8:
			cMultiplierX = 0;
			cMultiplierY = 2;
			cMultiplierZ = 1;
			break;
		case MAP_2D_y8d16:
			cMultiplierX = 0;
			cMultiplierY = 1;
			cMultiplierZ = 2;
			break;
		case MAP_2D_y16d16:
			cMultiplierX = 0;
			cMultiplierY = 2;
			cMultiplierZ = 2;
			break;
		}
	}
	else
	{
		cMultiplierX = 0;
		cMultiplierY = 0;
		cMultiplierZ = lpMapItem->usDataMap_BlockLength;

		switch(cMultiplierZ)
		{
		case 2:
			cMapType = MAP_DATA_d16;
			break;
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMapItem->axisX.cLength_label > 0 && lpMapItem->axisX.cType_unit > 0 && lpMapItem->axisY.cLength_label > 0 && lpMapItem->axisY.cType_unit > 0)
	{
		_lseek(fhROM,*lpOffset+iOffset_ram-1,SEEK_SET);
		_read(fhROM,&cLengthX,1);
		_lseek(fhROM,*lpOffset+iOffset_ram-1-((cLengthX+1)*cMultiplierX)-1,SEEK_SET);
		_read(fhROM,&cLengthY,1);
		cLengthZ = ((cLengthY+1)*cMultiplierY)*((cLengthX+1)*cMultiplierX);
	}
	else if(lpMapItem->axisY.cLength_label > 0 && lpMapItem->axisY.cType_unit > 0)
	{
		return ERR_SUCCESS;
//		_lseek(fhROM,*lpOffset+iOffset_ram-1,SEEK_SET);
//		_read(fhROM,&cLengthY,1);
//		cLengthZ = (cLengthY+1)*cMultiplierY;
	}
	else
		cLengthZ = lpMapItem->usDataMap_NumberOfItems - 1;

	TRACE("Map[%s] cLengthX[%i] cLengthY[%i] cLengthZ[%i]\n",lpMapItem->szLabel,cLengthX,cLengthY,cLengthZ);

	if(strstr(lpMapItem->szLabel,"CEL "))
		DWORD abc = 9;

EXCEPTION_BOOKMARK(__LINE__)
	if(buffer_xAxis != NULL)
		free(buffer_xAxis);
	if(buffer_yAxis != NULL)
		free(buffer_yAxis);
	if(buffer_zAxis != NULL)
		free(buffer_zAxis);

EXCEPTION_BOOKMARK(__LINE__)
	if(cLengthX > 0 || cMultiplierX > 0)
	{
		if((buffer_xAxis = (unsigned char*)malloc((size_t)((cLengthX+1)*cMultiplierX))) == NULL)
		{
			sError.Format("Memory Error - LoadRomTable [buffer_xAxis] : %i [0x%X]",GetLastError()); 
			OutputString(sError,TRUE);
			return F_NO_MEMORY;
		}
		_lseek(fhROM,*lpOffset+iOffset_ram-1-((cLengthX+1)*cMultiplierX),SEEK_SET);
		_read(fhROM,buffer_xAxis,((cLengthX+1)*cMultiplierX));
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(cLengthY > 0 || cMultiplierY > 0)
	{
		if((buffer_yAxis = (unsigned char*)malloc((size_t)((cLengthY+1)*cMultiplierY))) == NULL)
		{
			sError.Format("Memory Error - LoadRomTable [buffer_yAxis] : %i [0x%X]",GetLastError()); 
			OutputString(sError,TRUE);
			return F_NO_MEMORY;
		}
		_lseek(fhROM,*lpOffset+iOffset_ram-1-((cLengthX+1)*cMultiplierX)-1-((cLengthY+1)*cMultiplierY),SEEK_SET);
		_read(fhROM,buffer_yAxis,((cLengthY+1)*cMultiplierY));
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(cLengthZ > 0 || cMultiplierZ > 0)
	{
		if((buffer_zAxis = (unsigned char*)malloc((size_t)((cLengthZ+1)*cMultiplierZ))) == NULL)
		{
			sError.Format("Memory Error - LoadRomTable [buffer_zAxis] : %i [0x%X]",GetLastError()); 
			OutputString(sError,TRUE);
			return F_NO_MEMORY;
		}
		if(cMapType == MAP_DATA_d8 || cMapType == MAP_DATA_d16 || cMapType == MAP_DATA_d32)
			_lseek(fhROM,*lpOffset+iOffset_ram,SEEK_SET);
		else
			_lseek(fhROM,*lpOffset+iOffset_ram+1,SEEK_SET);
		_read(fhROM,buffer_zAxis,((cLengthZ+1)*cMultiplierZ));
	}

EXCEPTION_BOOKMARK(__LINE__)
	frameGrid* lpFrameGrid = (frameGrid*)lpSplitter->GetPane(0,0);
	lpFrameGrid->BuildGrid(cLengthX+1,cLengthY+1);
	lpFrameGrid->SetData_xAxis(buffer_xAxis,((cLengthX+1)*cMultiplierX),cMapType,lpMapItem->axisX.cDecimals);
	lpFrameGrid->SetData_yAxis(buffer_yAxis,((cLengthY+1)*cMultiplierY),cMapType,lpMapItem->axisY.cDecimals);
	lpFrameGrid->SetData_zAxis(buffer_zAxis,((cLengthZ+1)*cMultiplierZ),cMapType,lpMapItem->axisZ.cDecimals);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpFrameList = (frameList*)lpSplitter->GetPane(1,0);
	lpFrameList->SetViewStyle(LVS_REPORT,LVS_EX_FULLROWSELECT);
	lpFrameList->AddColumn("Information",500);
	sError.Format("X-Axis : %s",lpMapItem->axisX.szLabel);
	lpFrameList->AddItem(sError);
	sError.Format("Y-Axis : %s",lpMapItem->axisY.szLabel);
	lpFrameList->AddItem(sError);
	sError.Format("Data : %s",lpMapItem->axisZ.szLabel);
	lpFrameList->AddItem(sError);

CATCHCATCH("frameTableEditor::LoadRomTable()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}*/
#endif