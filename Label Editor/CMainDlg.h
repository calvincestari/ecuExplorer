/*/
	CMainDlg.h (2005.07.09)
/*/
#pragma once

#include "stdafx.h"
#include "resource.h"
#include "LocalDefinition.h"

class CMainDlg : public CDialog
{
public:
	CMainDlg();

	enum { IDD = IDD_MAIN };

	CString sVersion;
	CString sID;
	CString sLabel;
	CString sChecksum;
	CString sType;
	CPtrList listLabel;
	POSITION pos;
	unsigned short usIndex;

private:
	long BrowseForFile(CString* szReturn);
	void GenerateChecksum(LPSTRUCT_LABELITEM lpLabelItem);
	unsigned char GenerateChecksum(LPCTSTR szBuffer,unsigned char ucBufferLength);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();

	afx_msg void OnBack();
	afx_msg void OnNext();
	afx_msg void OnLoad();
	afx_msg void OnSave();

	DECLARE_MESSAGE_MAP()
};