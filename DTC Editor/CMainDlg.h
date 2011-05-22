/*/
	CMainDlg.h (2005.08.02)
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
	CString sCode;
	CString sDescription;
	CString sChecksum;
	CPtrList listDTC;
	POSITION pos;
	unsigned short usIndex;
	CString sCurrent;
	CString sHistoric;
	CString sBit;

private:
	long BrowseForFile(CString* szReturn);
	void GenerateChecksum(LPSTRUCT_DTCITEM lpDTCItem);
	unsigned char GenerateChecksum(LPCTSTR szBuffer,unsigned char ucBufferLength);
	unsigned char GenerateChecksum(unsigned long ulValue);
	void LoadList();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();

	afx_msg void OnBack();
	afx_msg void OnNext();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnNew();
	afx_msg void OnDelete();

	DECLARE_MESSAGE_MAP()
};