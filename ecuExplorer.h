/*/
	ecuExplorer.h (2005.06.16)
/*/
#pragma once

#include <wfc.h>

#include "resource.h"
#include "frameMain.h"
#include "protocolSSM.h"
#include "definitionLocal.h"

class ecuExplorer : public CWinApp
{
public:
	frameMain* lpMainFrame;
	CPtrList listLabel;
	CMapWordToPtr mapLiveDataItem;
	CPtrList listDTC_Subaru;
	CPtrList listDTC_OBD;
	protocolSSM* lpProtocolSSM;
	BOOL bThreadStartup;
	BOOL bThreadLiveQuery;
	BOOL bThreadDTCQuery;
	HANDLE hThreadStartup;
	HANDLE hThreadLiveQuery;
	HANDLE hThreadDTCQuery;
	HWND hWndResourceTree;
	HWND hWndRealtime;
	HWND hWndDTC;
	unsigned long ulConfig;
	unsigned long ulEcuFlash;
	unsigned char cAppPath[1024];
	CMapStringToPtr mapRom;
	CMapWordToPtr mapMap;
	CMapStringToPtr mapExternal;

public:
	ecuExplorer();
	~ecuExplorer();

	unsigned char GenerateChecksum(LPCTSTR szBuffer,unsigned short usBufferLength);
	unsigned char GenerateChecksum(unsigned long ulValue);
	unsigned char GenerateChecksum(unsigned short usValue);
	long LoadDataFile_dtc(LPCTSTR szFile,CPtrList* pStorage);
	long LoadDataFile_label();
	long LoadDataFile_live();
	void menuChangeOption_DebugConsole();
	void menuChangeOption_CaptureErrorLog();
	void menuChangeOption_CaptureProtocolTraceFile();
	void menuConvert_injector();
	void menuConvert_temperature();
	void menuConvert_speed();
	void menuConvert_pressure();
	void menuConvert_afr();
	void menuResetECU();
	void menuRetardIgnitionTiming_query();
	void menuRetardIgnitionTiming_decrease();
	void menuRetardIgnitionTiming_increase();
	void menuRetardIgnitionTiming_reset();
	void menuSetIdleSpeed_Aircon_query();
	void menuSetIdleSpeed_Aircon_decrease25();
	void menuSetIdleSpeed_Aircon_decrease50();
	void menuSetIdleSpeed_Aircon_decrease100();
	void menuSetIdleSpeed_Aircon_increase25();
	void menuSetIdleSpeed_Aircon_increase50();
	void menuSetIdleSpeed_Aircon_increase100();
	void menuSetIdleSpeed_Aircon_reset();
	void menuSetIdleSpeed_Normal_query();
	void menuSetIdleSpeed_Normal_decrease25();
	void menuSetIdleSpeed_Normal_decrease50();
	void menuSetIdleSpeed_Normal_decrease100();
	void menuSetIdleSpeed_Normal_increase25();
	void menuSetIdleSpeed_Normal_increase50();
	void menuSetIdleSpeed_Normal_increase100();
	void menuSetIdleSpeed_Normal_reset();
	long ReadRegistry_comm();
	long ReadRegistry_settings();
	long ReleaseMemory_live();
	long ReleaseMemory_label();
	long ReleaseMemory_dtc();
	long SetSerialPort();
	void ShowAbout();
	LPCTSTR GetECUType(LPCTSTR szID);
	void OnStartFileCapture();
	void OnTriggerLoggingOnDefogSwitch();
	void OnPauseDataCapture();
	void OnResetMinMaxValues();
	void OnSelectAllAnalogueParameters();
	void OnDeselectAllAnalogueParameters();
	void OnSelectAllDigitalParameters();
	void OnDeselectAllDigitalParameters();
	LPSTRUCT_LIVEBITITEM GetLiveDataItem(LPCTSTR szText);
	LPSTRUCT_LIVEBITITEM GetLiveDataItem(int iListIndex,int iType);
	long StartLiveQueryThread();
	long StopLiveQueryThread();
	int GetRegistryValue(CString sSection,CString sEntry,int iDefault);
	int WriteRegistryValue(CString sSection,CString sEntry,int iValue);
	void OnExit();
	void menuSetupComm();
	void menuReadECU();
	void menuWriteROM();
	void menuTestWrite();
	void menuCompareROM();
	void OnUSDMWRX0203();
	void OnUSDMWRX0405();
	void OnIntlWRX0105();
	void OnIntlSTI0105();
	long BrowseForFile(CString* szReturn,DWORD dwFlags);
	void ecuFlash(int func);
	long StartDTCQueryThread();
	long StopDTCQueryThread();
	void OnRoadDyno();
	BOOL IsRomRaw(int fhROM);
	BOOL IsRomRaw(LPCTSTR szFile);
	void OnPauseDTC();
	long StartStartupThread();
	long StopStartupThread();
	void menuEcuQuery();
#if defined(ENABLE_EDIT_ROM)
	long LoadDataFile_map();
	long LoadDataFile_rom();
	long ReleaseMemory_map();
	long ReleaseMemory_rom();
#endif
	void Editor_label();
	void Editor_dataitem();
	long LoadExternalSensorInput();

public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()

private:
	void OpenLogFile();
#if defined(ENABLE_EDIT_ROM)
	void OpenROMFile();
#endif
};