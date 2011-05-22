/*/
	frameRealtime.h (2005.07.31)
/*/
#pragma once

#include <wfc.h>
#include <sys/timeb.h>

#include "stdafx.h"
#include "ecuExplorer.h"

class frameRealtime : public CFrameWnd
{
protected:
	DECLARE_DYNCREATE(frameRealtime)

public:
	frameRealtime();
	virtual ~frameRealtime();

	void OnParseSupportList(WPARAM wParam,LPARAM lParam);
	void OnUpdateListItem(WPARAM wParam,LPARAM lParam);
	void OnUpdateCSVFile(WPARAM wParam,LPARAM lParam);
	void OnContextMenu(CWnd* pWnd,CPoint ptMousePos);
	void OnStartFileCapture();
	void OnChooseLoggingDirectory();
	void OnTriggerLoggingOnDefogSwitch();
	void OnConvert_injector();
	void OnConvert_temperature();
	void OnConvert_speed();
	void OnConvert_pressure();
	void OnCovert_af();
	void OnPauseDataCapture();
	void OnResetMinMaxValues();
	void OnSelectAllAnalogueParameters();
	void OnDeselectAllAnalogueParameters();
	void OnSelectAllDigitalParameters();
	void OnDeselectAllDigitalParameters();
	void OnViewItemDescription();
	void OnAutoAddNewLogFile();
	void ResizeSplitter();
	void OnUseAbsoluteTime();

public:
	CFont* lpFont;
	CSplitterWnd* lpSplitter;
	ecuExplorer* lpParent;
	BOOL bFlag;
	CString sAbsoluteCSVFile;
	FILE* fpCSVCapture;
	_timeb tbCSVCapture;

public:
	//{{AFX_VIRTUAL(frameRealtime)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext);
	//}}AFX_VIRTUAL

protected:
	DECLARE_MESSAGE_MAP()
};