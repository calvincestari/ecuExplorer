#pragma once

#include "stdafx.h"
#include "resource.h"
#include "definitionLocal.h"

class dialogEditor_dataitem : public CDialog
{
public:
	dialogEditor_dataitem();

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
	unsigned char GenerateChecksum(LPSTRUCT_LIVEBITITEM lpDataItem);
	unsigned char GenerateChecksum(LPCTSTR szBuffer,unsigned short usBufferLength);
	unsigned char GenerateChecksum(unsigned long ulValue);
	unsigned char GenerateChecksum(unsigned short usValue);
	void UpdateLabels(bool bMode);
	void UserDefinedItemChange();
	void SetByteBitState(BOOL bState);
	void QuadSpanChange();
	void SetAddressState(BOOL bState);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	virtual void OnKillFocus();

private:
	CPtrList listDataItem;
	POSITION pos;
	int iIndex;

	CString sVersion;
	BOOL bUserDefinedItem;
	CString sByte;
	CString sBit;
	CString sName;
	CString sUnit;
	BOOL bQuadSpan;
	int iAddress_high;
	int iAddress_low;
	int iIndex_type;
	CComboBox* lpComboBox_type;
	CString sOperand_addition;
	CString sOperand_subtract;
	CString sOperand_multiplier;
	CString sOperand_divisor;
	CString sDecimals;
	CString sDescription;

	CFont* pFont;

	enum {IDD = IDD_DIALOG_EDITOR_DATAITEM};
	DECLARE_MESSAGE_MAP()
};