/*/
	dialogEcuFlash.h (2005.11.23)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "resource.h"

class dialogEcuFlash : public CDialog
{
public:
	dialogEcuFlash();
	void UpdateProgress(LPCTSTR cBuffer);

	enum {IDD = IDD_DIALOG_ECUFLASH};

	void* lpParent;
	int iLength;
	CFont* lpFont;
	CButton* lpButton;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
};