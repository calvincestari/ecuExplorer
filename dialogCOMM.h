/*/
	dialogCOMM.h (2005.11.08)
/*/
#pragma once

#include <wfc.h>

#include "stdafx.h"
#include "resource.h"

class dialogCOMM : public CDialog
{
public:
	dialogCOMM();

	enum {IDD = IDD_DIALOG_COMM};

	int iXonLim, iXoffLim;
	int iReadIntervalTimeout, iReadTotalTimeoutConstant, iWriteTotalTimeoutConstant;
	void* lpParent;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()
};