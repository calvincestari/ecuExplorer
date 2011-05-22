/*/
	frameResourceTree.h (2005.07.28)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "AW_CMultiViewSplitter.h"
#include "ecuExplorer.h"

class frameResourceTree : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameResourceTree)

public:
	frameResourceTree();
	virtual ~frameResourceTree();

#if defined(ENABLE_EDIT_ROM)
	long AddROMFile(LPCTSTR szLabel);
#endif
	long AddSavedLogFile(LPCTSTR szLabel);
	long BuildResourceTree();
	HTREEITEM GetNode(HTREEITEM hItem,LPCTSTR szNodeName);
	void OnNodeSelect(NMHDR* pNMHDR, LRESULT* pResult);
	long SetNodeText(DWORD dwTagData,LPCTSTR szDisplayText);
	void OnUpdateNodeText(WPARAM wParam,LPARAM lParam);
#if defined(ENABLE_EDIT_ROM)
	long AddROMMaps(LPSTRUCT_ROMITEM lpRomItem,HTREEITEM hParent,int fhROM,int iOffset_ram);
#endif
	LPSTRUCT_NODEDATA CreateNodeData(void* lpNode,int iView,int iType);
	void* GetNodeData_node(int iType);
	int GetNodeData_view(int iType);
	int GetNodeData_view(void* lpNode);
	int GetNodeData_type(void* lpNode);
	void ReleaseMemory_node();

public:
	AW_CMultiViewSplitter* lpSplitter;
	CTreeCtrl* lpTree;
	ecuExplorer* lpParent;
	CPtrList listNode;

public:
	//{{AFX_VIRTUAL(frameCommSerial)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(frameCommSerial)
	afx_msg void OnSize(UINT nType,int cx,int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};