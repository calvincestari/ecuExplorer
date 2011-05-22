/*/
	CMainDlg.h (2005.08.11)
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
	CString sByte;
	CString sBit;
	CString sName;
	CString sAddress_high;
	CString sAddress_low;
	CString sType;
	CString sOperand_addition;
	CString sOperand_subtract;
	CString sOperand_multiplier;
	CString sOperand_divisor;
	CString sDecimals;
	CString sUnit;
	CString sDescription;
	CString sChecksum;
	CPtrList listDataItem;
	POSITION pos;
	unsigned short usIndex;

private:
	long BrowseForFile(CString* szReturn);
	void GenerateChecksum(LPSTRUCT_DATAITEM lpDataItem);
	unsigned char GenerateChecksum(LPCTSTR szBuffer,unsigned short usBufferLength);
	unsigned char GenerateChecksum(unsigned long ulValue);
	unsigned char GenerateChecksum(unsigned short usValue);

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