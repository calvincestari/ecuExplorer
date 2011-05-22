/*/
	frameGrid.cpp (2005.07.30)
/*/

#include <wfc.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "frameGrid.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameGrid, CFrameWnd)

BEGIN_MESSAGE_MAP(frameGrid, CFrameWnd)
	//{{AFX_MSG_MAP(frameGrid
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

frameGrid::frameGrid()
{
	lpFont = NULL;
	lpGrid = NULL;
}

frameGrid::~frameGrid()
{
	if(lpGrid != NULL)
		delete lpGrid;
	if(lpFont != NULL)
		delete lpFont;
}

BOOL frameGrid::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
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
	if((lpGrid = new CGridCtrl) == NULL)
	{
		sError.Format("Memory Error - frameGrid::OnClientCreate() [lpGrid] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return -1;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(!lpGrid->Create(rect,this,100))
	{
		sError.Format("CGridCtrl::Create Failed With %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return -1;
	}

EXCEPTION_BOOKMARK(__LINE__)
	lpFont = new CFont;
	LOGFONT lf;

EXCEPTION_BOOKMARK(__LINE__)
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 0xfffffff5;
	lf.lfWeight = FW_REGULAR;
	lf.lfOutPrecision = OUT_STROKE_PRECIS;
	lf.lfClipPrecision = CLIP_STROKE_PRECIS;
	lf.lfQuality = DRAFT_QUALITY;
	lf.lfPitchAndFamily = 0x22;
	lf.lfCharSet = 0;
	strcpy(lf.lfFaceName,"Tahoma");

EXCEPTION_BOOKMARK(__LINE__)
	lpFont->CreateFontIndirect(&lf);
	lpGrid->SetFont(lpFont,TRUE);

CATCHCATCH("frameGrid::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameGrid::OnSize(UINT nType,int cx,int cy)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
	lpGrid->MoveWindow(rect.left,rect.top,rect.right,rect.bottom,TRUE);

CATCHCATCH("frameGrid::OnSize()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

long frameGrid::AddCSVRow(LPCTSTR szData,BOOL bHeader)
{
	CString sError;
	CString sColumn;
	int iPos = 0;
	int iCol = -1;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(bHeader)
	{
		lpGrid->SetRowCount(1);
		lpGrid->SetFixedRowCount(1);
	}
	else
		lpGrid->SetRowCount(lpGrid->GetRowCount()+1);

EXCEPTION_BOOKMARK(__LINE__)
	sError = szData;
	sError.Remove(0xA);
	sError.Remove(0xD);
	if(!bHeader) sError.Remove(0x20);
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

EXCEPTION_BOOKMARK(__LINE__)
		if(bHeader)
		{
			lpGrid->SetColumnCount(lpGrid->GetColumnCount()+1);
			iCol = lpGrid->GetColumnCount()-1;
		}
		else
			iCol++;

EXCEPTION_BOOKMARK(__LINE__)
		lpGrid->SetItemText(lpGrid->GetRowCount()-1,iCol,sColumn);
		lpGrid->SetItemState(lpGrid->GetRowCount()-1,iCol,lpGrid->GetItemState(lpGrid->GetRowCount()-1,iCol) | GVIS_READONLY);

EXCEPTION_BOOKMARK(__LINE__)
		sColumn += ",";
		sError.Delete(0,sColumn.GetLength());
	}

CATCHCATCH("frameGrid::AddRow()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long frameGrid::SetColumnState(LPCTSTR szColumn,BOOL bShow)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	for(int i = 0;i < lpGrid->GetRowCount();i++)
	{
		if(lpGrid->GetItemText(0,i) == szColumn)
		{
			if(bShow == FALSE)
				lpGrid->SetColumnWidth(i,0);
			else
				lpGrid->SetColumnWidth(i,lpGrid->m_nDefCellWidth);

			lpGrid->RedrawWindow();
			break;
		}
	}

CATCHCATCH("frameGrid::SetColumnState()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long frameGrid::BuildGrid(unsigned short usX,unsigned short usY)
{
	CString sError;
	int iLoop = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpGrid->SetRowCount(usY+1);
	lpGrid->SetColumnCount(usX+1);

	lpGrid->SetFixedRowCount(1);
	lpGrid->SetFixedColumnCount(1);

	for(iLoop=0;iLoop < usY+1;iLoop++)
		lpGrid->SetRowHeight(iLoop,17);
	for(iLoop=0;iLoop < usX+1;iLoop++)
		lpGrid->SetColumnWidth(iLoop,40);

	lpGrid->SetItemBkColour(0,0,RGB(0,0,0));
	lpGrid->SetItemFgColour(0,0,RGB(255,255,255));

	lpGrid->RedrawWindow();

CATCHCATCH("frameGrid::BuildGrid()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long frameGrid::SetData_xAxis(unsigned char* cBuffer,unsigned short usLength,unsigned char cType,unsigned char cDecimals)
{
	CString sError;
	unsigned short usBufferPointer = 0;
	unsigned short usRead = 0;
	float fValue = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer == NULL || usLength == 0)
		return F_INVALID_POINTER;

EXCEPTION_BOOKMARK(__LINE__)
	while(usBufferPointer < usLength)
	{
		switch(cType)
		{
		case MAP_3D_y16x8d8:
		case MAP_3D_y16x8d16:
			fValue = (float)cBuffer[usBufferPointer++];
			break;
		case MAP_3D_y16x16d8:
		case MAP_3D_y16x16d16:
			memcpy(&usRead,&cBuffer[usBufferPointer*2],2);
			usBufferPointer++;
			fValue = (float)htons(usRead);
			break;
		case MAP_2D_y8d8:
		case MAP_2D_y16d8:
		case MAP_2D_y8d16:
		case MAP_2D_y16d16:
			return F_INVALID_ROM_FORMAT;
		}

		switch(cDecimals)
		{
		case 1:
			sError.Format("%.1f",fValue);
			break;
		case 2:
			sError.Format("%.2f",fValue);
			break;
		case 3:
			sError.Format("%.3f",fValue);
			break;
		default:
			sError.Format("%.0f",fValue);
			break;
		}
		lpGrid->SetItemText(0,usBufferPointer,sError);
	}

CATCHCATCH("frameGrid::SetData_xAxis()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long frameGrid::SetData_yAxis(unsigned char* cBuffer,unsigned short usLength,unsigned char cType,unsigned char cDecimals)
{
	CString sError;
	unsigned short usBufferPointer = 0;
	unsigned short usRead = 0;
	float fValue = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer == NULL || usLength == 0)
		return F_INVALID_POINTER;

EXCEPTION_BOOKMARK(__LINE__)
	while(usBufferPointer < usLength)
	{
		switch(cType)
		{
		case MAP_3D_y16x8d8:
		case MAP_3D_y16x8d16:
		case MAP_3D_y16x16d8:
		case MAP_3D_y16x16d16:
		case MAP_2D_y16d8:
		case MAP_2D_y16d16:
			memcpy(&usRead,&cBuffer[usBufferPointer*2],2);
			usBufferPointer++;
			fValue = (float)usRead;
			break;
		case MAP_2D_y8d8:
		case MAP_2D_y8d16:
			fValue = (float)cBuffer[usBufferPointer++];
			break;
		}

		switch(cDecimals)
		{
		case 1:
			sError.Format("%.1f",fValue);
			break;
		case 2:
			sError.Format("%.2f",fValue);
			break;
		case 3:
			sError.Format("%.3f",fValue);
			break;
		default:
			sError.Format("%.0f",fValue);
			break;
		}
		lpGrid->SetItemText(usBufferPointer,0,sError);
	}

CATCHCATCH("frameGrid::SetData_yAxis()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long frameGrid::SetData_zAxis(unsigned char* cBuffer,unsigned short usLength,unsigned char cType,unsigned char cDecimals)
{
	CString sError;
	unsigned short usBufferPointer = 0;
	unsigned short usRead = 0;
	float fValue = 0;
	int iRow = 1;
	int iCol = 1;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer == NULL || usLength == 0)
		return F_INVALID_POINTER;

EXCEPTION_BOOKMARK(__LINE__)
	while(usBufferPointer < usLength)
	{
		switch(cType)
		{
		case MAP_3D_y16x8d8:
		case MAP_3D_y16x16d8:
		case MAP_2D_y16d8:
		case MAP_2D_y8d8:
			fValue = (float)cBuffer[usBufferPointer++];
			break;
		case MAP_3D_y16x8d16:
		case MAP_3D_y16x16d16:
		case MAP_2D_y16d16:
		case MAP_2D_y8d16:
		case MAP_DATA_d16:
			memcpy(&usRead,&cBuffer[usBufferPointer*2],2);
			usBufferPointer++;
			fValue = (float)usRead;
			break;
		}

		switch(cDecimals)
		{
		case 1:
			sError.Format("%.1f",fValue);
			break;
		case 2:
			sError.Format("%.2f",fValue);
			break;
		case 3:
			sError.Format("%.3f",fValue);
			break;
		default:
			sError.Format("%.0f",fValue);
			break;
		}
		lpGrid->SetItemText(iRow,iCol++,sError);

		if(iCol >= lpGrid->GetColumnCount())
		{
			iCol = 1;
			iRow++;
		}
	}

CATCHCATCH("frameGrid::SetData_zAxis()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}