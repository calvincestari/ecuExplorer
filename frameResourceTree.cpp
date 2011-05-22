/*/
	frameResourceTree.cpp (2005.07.28)
/*/

#include <wfc.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "frameResourceTree.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"
#include "frameGrid.h"
#include "frameErrorLog.h"
#include "frameDataItems.h"
#include "frameTroubleCodes.h"
#include "frameRealtime.h"
#include "frameDTCReader.h"
#include "frameLogFile.h"
#include "frameList.h"
#if defined(ENABLE_EDIT_ROM)
#include "frameRomTables.h"
#endif

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameResourceTree, CFrameWnd)

BEGIN_MESSAGE_MAP(frameResourceTree, CFrameWnd)
	//{{AFX_MSG_MAP(frameResourceTree
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_SELCHANGED,ID_TREE_RESOURCE,OnNodeSelect)
	ON_MESSAGE(MESSAGE_UPDATENODETEXT,OnUpdateNodeText)
END_MESSAGE_MAP()

frameResourceTree::frameResourceTree()
{
	lpTree = NULL;
	lpParent = NULL;
	listNode.RemoveAll();
}

frameResourceTree::~frameResourceTree()
{
	ReleaseMemory_node();

	if(lpTree != NULL)
		delete lpTree;
}

BOOL frameResourceTree::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
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
	if((lpTree = new CTreeCtrl) == NULL)
	{
		sError.Format("Memory Error - frameResourceTree::OnClientCreate() [lpTree] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return -1;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(!lpTree->Create(WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,rect,this,ID_TREE_RESOURCE))
	{
		sError.Format("CTreeCtrl::Create Failed With %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return -1;
	}

CATCHCATCH("frameResourceTree::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameResourceTree::OnSize(UINT nType,int cx,int cy)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
	lpTree->MoveWindow(rect.left,rect.top,rect.right,rect.bottom,TRUE);

CATCHCATCH("frameResourceTree::OnSize()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

long frameResourceTree::BuildResourceTree()
{
	CString sError;
	HTREEITEM hParent = NULL;
	HTREEITEM hItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	hItem = lpTree->InsertItem("User-Defined Data Items",0,0);
	lpTree->SetItemData(hItem,(DWORD)CreateNodeData(hItem,lpSplitter->AddView(0,1,RUNTIME_CLASS(frameDataItems),NULL),NODE_DATAITEMS));

	OutputString("resource tree item added - user-defined items...");

EXCEPTION_BOOKMARK(__LINE__)
	hItem = lpTree->InsertItem("Known Trouble Codes",0,0);
	lpTree->SetItemData(hItem,(DWORD)CreateNodeData(hItem,lpSplitter->AddView(0,1,RUNTIME_CLASS(frameTroubleCodes),NULL),NODE_TROUBLECODES));

	OutputString("resource tree item added - known trouble codes...");

EXCEPTION_BOOKMARK(__LINE__)
	hItem = lpTree->InsertItem("Saved Log Files",0,0);
	lpTree->SetItemData(hItem,(DWORD)CreateNodeData(hItem,0,NODE_SAVEDLOGFILEPARENT));

	OutputString("resource tree item added - saved log files...");

EXCEPTION_BOOKMARK(__LINE__)
	hParent = lpTree->InsertItem("ECU (Disconnected)",0,0);
	lpTree->SetItemData(hParent,(DWORD)CreateNodeData(hParent,0,NODE_ECUPARENT));

	OutputString("resource tree item added - ecu...");

EXCEPTION_BOOKMARK(__LINE__)
	hItem = lpTree->InsertItem("Realtime Data View",hParent);
	lpTree->SetItemData(hItem,(DWORD)CreateNodeData(hItem,lpSplitter->AddView(0,1,RUNTIME_CLASS(frameRealtime),NULL),NODE_ECUREALTIME));
	
	OutputString("resource tree item added - realtime data view...");

EXCEPTION_BOOKMARK(__LINE__)
	hItem = lpTree->InsertItem("Trouble Code Reader",hParent);
	lpTree->SetItemData(hItem,(DWORD)CreateNodeData(hItem,lpSplitter->AddView(0,1,RUNTIME_CLASS(frameDTCReader),NULL),NODE_ECUTROUBLECODEREADER));

	OutputString("resource tree item added - trouble code reader...");

CATCHCATCH("frameResourceTree::BuildResourceTree()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

void frameResourceTree::OnNodeSelect(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString sError;
	CString sTemp;
	HTREEITEM hItem = NULL;
	HTREEITEM hParent = NULL;
	DWORD dwThreadID = 0;
	CPtrList* lpStorage = NULL;
	LPSTRUCT_NODEDATA lpNodeData = NULL;
#if defined(ENABLE_EDIT_ROM)
	frameRomTables* lpFrameRomTables = NULL;
#endif

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((hItem = lpTree->GetSelectedItem()) != NULL)
	{
		if(GetNodeData_type((void*)hItem) != NODE_ECUREALTIME)
			lpParent->StopLiveQueryThread();
		if(GetNodeData_type((void*)hItem) != NODE_ECUTROUBLECODEREADER)
			lpParent->StopDTCQueryThread();
		if(lpTree->GetParentItem(hItem) != NULL)
			if(GetNodeData_type((void*)lpTree->GetParentItem(hItem)) == NODE_SAVEDLOGFILEPARENT)
			{
				lpSplitter->ShowView(GetNodeData_view(hItem));
				return;
			}

		if((lpNodeData = (LPSTRUCT_NODEDATA)lpTree->GetItemData(hItem)) == NULL)
			return;

		switch(lpNodeData->iType)
		{
		case NODE_DATAITEMS:
		case NODE_TROUBLECODES:
			lpSplitter->ShowView(lpNodeData->iView);
			break;
		case NODE_ECUREALTIME:
			lpSplitter->ShowView(lpNodeData->iView);
			lpParent->StartLiveQueryThread();
			break;
		case NODE_ECUTROUBLECODEREADER:
			lpSplitter->ShowView(lpNodeData->iView);
			lpParent->StartDTCQueryThread();
			break;
		case NODE_ROMTABLES:
			lpSplitter->ShowView(lpNodeData->iView);
#if defined(ENABLE_EDIT_ROM)
			lpFrameRomTables = (frameRomTables*)lpSplitter->GetView(lpNodeData->iView);
			lpFrameRomTables->lpTab->SetCurSel(lpFrameRomTables->lpTab->GetCurSel());
#endif
			break;
		default:
			break;
		}
	}

CATCHCATCH("frameResourceTree::OnNodeSelect()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

HTREEITEM frameResourceTree::GetNode(HTREEITEM hItem,LPCTSTR szNodeName)
{
	CString sError;
	HTREEITEM hTemp = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(hItem == NULL)
		hItem = lpTree->GetRootItem();

EXCEPTION_BOOKMARK(__LINE__)
	while(hItem != NULL)
	{
		if(lpTree->GetItemText(hItem) == szNodeName)
			return hItem;

		if((hTemp = lpTree->GetChildItem(hItem)) != NULL)
		{
			if((hTemp = GetNode(hTemp,szNodeName)) != NULL)
				return hTemp;
		}
		hItem = lpTree->GetNextItem(hItem,TVGN_NEXT);
	}

CATCHCATCH("frameResourceTree::GetNode(LPCTSTR)");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return NULL;
}

long frameResourceTree::AddSavedLogFile(LPCTSTR szLabel)
{
	CString sError;
	HTREEITEM hParent = NULL;
	HTREEITEM hItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((hParent = (HTREEITEM)GetNodeData_node(NODE_SAVEDLOGFILEPARENT)) != NULL)
	{
EXCEPTION_BOOKMARK(__LINE__)
		if((hItem = lpTree->InsertItem(szLabel,0,0,hParent)) != NULL)
		{
EXCEPTION_BOOKMARK(__LINE__)
			lpTree->SetItemData(hItem,(DWORD)CreateNodeData(hItem,lpSplitter->AddView(0,1,RUNTIME_CLASS(frameLogFile),NULL),NODE_SAVEDLOGFILEPARENT));
			frameLogFile* lpFrameLogFile = (frameLogFile*)lpSplitter->GetView(GetNodeData_view(hItem));
			lpFrameLogFile->LoadLogFile(szLabel);
			
EXCEPTION_BOOKMARK(__LINE__)
			lpTree->Expand(hParent,TVE_EXPAND);
			lpTree->RedrawWindow();

			sError.Format("resource tree item added - %s...",szLabel);
			OutputString(sError);
		}
	}

CATCHCATCH("frameResourceTree::AddSavedLogFile()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;;
}
#if defined(ENABLE_EDIT_ROM)
long frameResourceTree::AddROMFile(LPCTSTR szFile)
{
	CString sError;
	CString sNode;
	int iIndex = 1;
	HTREEITEM hParent = NULL;
	HTREEITEM hItem = NULL;
	int fhROM = 0;
	unsigned char cBuffer[1024];
	long lResult = 0;
	int iOffset_ram = 0;
	LPSTRUCT_ROMITEM lpROMItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((fhROM = _open(szFile,_O_BINARY | _O_RDWR,_S_IREAD | _S_IWRITE)) == -1)
	{
		sError.Format("File Error - AddROMFile [_open] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError);
		return F_FILE_ERROR;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(!lpParent->IsRomRaw(fhROM))
	{
		sError.Format("The format of the ROM file does not appear to be valid.  It must be in raw format for editing.\nPlease ensure it is not encrypted by any 3rd party application");
		OutputString(sError,MESSAGETYPE_ERROR);
		MessageBox(sError,"Invalid ROM format",MB_ICONSTOP | MB_OK);
		_close(fhROM);
		return F_INVALID_ROM_FORMAT;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = _lseek(fhROM,0,SEEK_END)) == -1)
		return F_FILE_ERROR;

	switch(lResult)
	{
	case MAPSIZE_STANDARD:
		iOffset_ram = MAPSIZE_RAM;
		break;
	case MAPSIZE_COMPACT:
		iOffset_ram = 0;
		break;
	default:
		sError.Format("The ROM file does not have a valid length.  It must be in raw format for editing.\nPlease ensure it is not encrypted by any 3rd party application");
		OutputString(sError,MESSAGETYPE_ERROR);
		MessageBox(sError,"Invalid ROM length",MB_ICONSTOP | MB_OK);
		_close(fhROM);
		return F_INVALID_ROM_FORMAT;
		break;
	}

EXCEPTION_BOOKMARK(__LINE__)
	_lseek(fhROM,0x200,SEEK_SET);
	memset((void*)&cBuffer[0],0,1024);
	_read(fhROM,&cBuffer[0],8);
	cBuffer[8] = '\0';

	if((lpParent->mapRom.Lookup((LPCTSTR)&cBuffer[0],(void*&)lpROMItem)) == 0)
	{
		sError.Format("This ROM revision is not currently supported [%s]",cBuffer);
		OutputString(sError,MESSAGETYPE_ERROR);
		MessageBox(sError,"Not supported",MB_ICONSTOP | MB_OK);
		_close(fhROM);
		return F_ROM_INDEX_LOOKUP_FAILED;
	}

EXCEPTION_BOOKMARK(__LINE__)
	sError = szFile;
	sError.Delete(0,sError.ReverseFind('\\')+1);
	sNode.Format("%s (%s - %s)",sError,lpROMItem->szRevision,lpROMItem->szDescription);
	sError = sNode;
	while((hParent = GetNode(NULL,sError)) != NULL)
		sError.Format("%s #%i",sNode,iIndex++);
	hParent = lpTree->InsertItem(sError,0,0);
	lpTree->SetItemData(hParent,(DWORD)CreateNodeData(hParent,lpSplitter->AddView(0,1,RUNTIME_CLASS(frameRomTables),NULL),NODE_ROMTABLES));
	
	OutputString("resource tree item added - rom image...");

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = AddROMMaps(lpROMItem,hParent,fhROM,iOffset_ram)) != ERR_SUCCESS)
	{
		sError.Format("unable to add rom tables to resource tree : %i [0x%X]",lResult,lResult);
		OutputString(sError,MESSAGETYPE_ERROR);
		_close(fhROM);
		return lResult;
	}
	OutputString("ROM file item added to resource tree...");

	_close(fhROM);

CATCHCATCH("frameResourceTree::AddROMFile()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;;
}
#endif
long frameResourceTree::SetNodeText(DWORD dwTagData,LPCTSTR szDisplayText)
{
	CString sError;
	HTREEITEM hItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((hItem = (HTREEITEM)GetNodeData_node((int)dwTagData)) != NULL)
	{
EXCEPTION_BOOKMARK(__LINE__)
		lpTree->SetItemText(hItem,szDisplayText);
		lpTree->RedrawWindow();
	}

CATCHCATCH("frameResourceTree::SetNodeText()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

void frameResourceTree::OnUpdateNodeText(WPARAM wParam,LPARAM lParam)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	SetNodeText((DWORD)wParam,(LPCTSTR)lParam);

CATCHCATCH("frameResourceTree::OnUpdateNodeText()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}
#if defined(ENABLE_EDIT_ROM)
long frameResourceTree::AddROMMaps(LPSTRUCT_ROMITEM lpRomItem,HTREEITEM hParent,int fhROM,int iOffset_ram)
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_MAPITEM lpMapItem = NULL;
	WORD wKey = 0;
	unsigned long* lpOffset = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameRomTables* lpFrameRomTables = (frameRomTables*)lpSplitter->GetView(GetNodeData_view(hParent));
	lpFrameRomTables->lpParent = lpParent;

EXCEPTION_BOOKMARK(__LINE__)
	pos = lpParent->mapMap.GetStartPosition();
	while(pos != NULL)
	{
		lpParent->mapMap.GetNextAssoc(pos,wKey,(void*&)lpMapItem);
		if((lpRomItem->mapOffset.Lookup(wKey,(void*&)lpOffset)) != 0)
			if(*lpOffset > 0)
				lpFrameRomTables->AddRomTable(lpMapItem->szLabel,lpMapItem->cType,*lpOffset);
	}

CATCHCATCH("frameResourceTree::AddROMMaps()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}
#endif
LPSTRUCT_NODEDATA frameResourceTree::CreateNodeData(void* lpNode,int iView,int iType)
{
	CString sError;
	LPSTRUCT_NODEDATA lpNodeData = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((lpNodeData = new STRUCT_NODEDATA) == NULL)
	{
		sError.Format("Memory Error - CreateNodeData [lpNodeData] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return NULL;
	}

	lpNodeData->lpNode = lpNode;
	lpNodeData->iView = iView;
	lpNodeData->iType = iType;

	listNode.AddTail(lpNodeData);
	return lpNodeData;

CATCHCATCH("frameResourceTree::CreateNodeData()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return NULL;
}

void* frameResourceTree::GetNodeData_node(int iType)
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_NODEDATA lpNodeData = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	pos = listNode.GetHeadPosition();
	while(pos != NULL)
	{
		lpNodeData = (LPSTRUCT_NODEDATA)listNode.GetNext(pos);
		if(lpNodeData->iType == iType)
			return lpNodeData->lpNode;
	}

CATCHCATCH("frameResourceTree::GetNodeData_node()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return NULL;
}

int frameResourceTree::GetNodeData_view(int iType)
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_NODEDATA lpNodeData = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	pos = listNode.GetHeadPosition();
	while(pos != NULL)
	{
		lpNodeData = (LPSTRUCT_NODEDATA)listNode.GetNext(pos);
		if(lpNodeData->iType == iType)
			return lpNodeData->iView;
	}

CATCHCATCH("frameResourceTree::GetNodeData_view(int)");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

int frameResourceTree::GetNodeData_view(void* lpNode)
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_NODEDATA lpNodeData = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	pos = listNode.GetHeadPosition();
	while(pos != NULL)
	{
		lpNodeData = (LPSTRUCT_NODEDATA)listNode.GetNext(pos);
		if(lpNodeData->lpNode == lpNode)
			return lpNodeData->iView;
	}

CATCHCATCH("frameResourceTree::GetNodeData_view(void*)");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

int frameResourceTree::GetNodeData_type(void* lpNode)
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_NODEDATA lpNodeData = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	pos = listNode.GetHeadPosition();
	while(pos != NULL)
	{
		lpNodeData = (LPSTRUCT_NODEDATA)listNode.GetNext(pos);
		if(lpNodeData->lpNode == lpNode)
			return lpNodeData->iType;
	}

CATCHCATCH("frameResourceTree::GetNodeData_type()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

void frameResourceTree::ReleaseMemory_node()
{
	CString sError;
	LPSTRUCT_NODEDATA lpNodeData = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	while(listNode.GetCount() > 0)
	{
		lpNodeData = (LPSTRUCT_NODEDATA)listNode.RemoveHead();
		delete lpNodeData;
	}

CATCHCATCH("frameResourceTree::ReleaseMemory_node()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}