#pragma once

#include "stdafx.h"
#include "resource.h"
#include "definitionLocal.h"

class dialogEditor_label : public CDialog
{
public:
	dialogEditor_label();

private:
	void BrowseForFile(CString* pszReturn,LPCTSTR szTitle,DWORD dwFlags = 0);
	void LoadDataFile();
	void OnBack();
	void OnNext();
	void GetData();
	void SaveChanges();
	void OnSave();
	void OnAddNew();
	void OnDelete();
	unsigned char GenerateChecksum(LPSTRUCT_LABELITEM lpLabelItem);
	unsigned char GenerateChecksum(LPCTSTR szBuffer,unsigned char ucBufferLength);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();

private:
	CPtrList listLabel;
	POSITION pos;
	int iIndex;

	CString sVersion;
	CString sCode;
	CString sDescription;

	CFont* pFont;

	enum {IDD = IDD_DIALOG_EDITOR_LABEL};
	DECLARE_MESSAGE_MAP()
};