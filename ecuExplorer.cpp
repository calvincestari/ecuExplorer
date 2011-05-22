/*/
	ecuExplorer.cpp (2005.06.16)
/*/

#include <wfc.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys\timeb.h>

#include "ecuExplorer.h"
#include "frameMain.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "definitionError.h"
#include "frameCommSerial.h"
#include "frameResourceTree.h"
#include "frameTroubleCodes.h"
#include "frameDataItems.h"
#include "frameRealtime.h"
#include "frameDTCReader.h"
#include "dialogCOMM.h"
#include "ecutools.h"
#include "log.h"
#include "dialogEcuFlash.h"
#include "dialogEcuQuery.h"
#include "dialogEditor_label.h"
#include "dialogEditor_dataitem.h"

ecuExplorer theApp;
BOOL bCaptureErrorLog;
FILE* fpErrorLog;
FILE* fpDebugLog;

void nuke();
void OutputString(CString sMessage,unsigned short usMessageType = 0);

DWORD WINAPI threadStartup(ecuExplorer* lpParent);
DWORD WINAPI threadLiveQuery(ecuExplorer* lpParent);
DWORD WINAPI threadDTCQuery(ecuExplorer* lpParent);
DWORD WINAPI threadEcuFlash(LPSTRUCT_ECUFLASHTHREAD lpBlock);

void nuke()
{
	exit(-1);
}

void OutputString(CString sMessage,unsigned short usMessageType)
{
	CString sOutput;
	_timeb tbTime;

	try
	{
		_ftime(&tbTime);
		CTime ctTime(tbTime.time);

		sMessage.Remove('\n');
		sOutput.Format("%02i:%02i:%02i.%03i - %s\n",ctTime.GetHour(),ctTime.GetMinute(),ctTime.GetSecond(),tbTime.millitm,sMessage);
		sOutput.Replace("%","%%");
		printf(sOutput);

		if(bCaptureErrorLog == TRUE && fpErrorLog != NULL && usMessageType > 0)
		{
			fprintf(fpErrorLog,sOutput);
			fflush(fpErrorLog);
		}
		if(fpDebugLog != NULL)
		{
			fprintf(fpDebugLog,sOutput);
			fflush(fpDebugLog);
		}
	}
	catch(...)
	{
		return;
	}
}

BEGIN_MESSAGE_MAP(ecuExplorer, CWinApp)
	ON_COMMAND(ID_MAINFRAME_OPENLOGFILE, OpenLogFile)
	ON_COMMAND(ID_MAINFRAME_EXIT, OnExit)
	ON_COMMAND(ID_MAINFRAME_ABOUT, ShowAbout)
	ON_COMMAND(ID_MAINFRAME_SHOWDEBUGCONSOLE, menuChangeOption_DebugConsole)
	ON_COMMAND(ID_MAINFRAME_CAPTUREERRORLOG, menuChangeOption_CaptureErrorLog)
	ON_COMMAND(ID_MAINFRAME_CAPTUREPROTOCOLTRACEFILE, menuChangeOption_CaptureProtocolTraceFile)
	ON_COMMAND(ID_MAINFRAME_RESETECU, menuResetECU)
	ON_COMMAND(ID_TOOLS_ECUQUERY, menuEcuQuery)
	ON_COMMAND(ID_ADJUSTIGNITIONRETARD_QUERYCURRENT, menuRetardIgnitionTiming_query)
	ON_COMMAND(ID_ADJUSTIGNITIONRETARD_INCREASE1, menuRetardIgnitionTiming_increase)
	ON_COMMAND(ID_ADJUSTIGNITIONRETARD_DECREASE1, menuRetardIgnitionTiming_decrease)
	ON_COMMAND(ID_ADJUSTIGNITIONRETARD_RESET, menuRetardIgnitionTiming_reset)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_NORMAL_QUERYCURRENT, menuSetIdleSpeed_Normal_query)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_NORMAL_INCREASE25, menuSetIdleSpeed_Normal_increase25)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_NORMAL_INCREASE50, menuSetIdleSpeed_Normal_increase50)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_NORMAL_INCREASE100, menuSetIdleSpeed_Normal_increase100)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_NORMAL_DECREASE25, menuSetIdleSpeed_Normal_decrease25)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_NORMAL_DECREASE50, menuSetIdleSpeed_Normal_decrease50)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_NORMAL_DECREASE100, menuSetIdleSpeed_Normal_decrease100)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_NORMAL_RESET, menuSetIdleSpeed_Normal_reset)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_AIRCON_QUERYCURRENT, menuSetIdleSpeed_Aircon_query)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_AIRCON_INCREASE100, menuSetIdleSpeed_Aircon_increase100)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_AIRCON_INCREASE50, menuSetIdleSpeed_Aircon_increase50)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_AIRCON_INCREASE25, menuSetIdleSpeed_Aircon_increase25)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_AIRCON_DECREASE25, menuSetIdleSpeed_Aircon_decrease25)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_AIRCON_DECREASE50, menuSetIdleSpeed_Aircon_decrease50)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_AIRCON_DECREASE100, menuSetIdleSpeed_Aircon_decrease100)
	ON_COMMAND(ID_ADJUSTIDLESPEEDS_AIRCON_RESET, menuSetIdleSpeed_Aircon_reset)
	ON_COMMAND(ID_POPUP_STARTFILECAPTURE,OnStartFileCapture)
	ON_COMMAND(ID_POPUP_TRIGGERLOGGINGONDEFOGSWITCH,OnTriggerLoggingOnDefogSwitch)
	ON_COMMAND(ID_POPUP_PAUSEDATACAPTURE,OnPauseDataCapture)
	ON_COMMAND(ID_POPUP_RESETMINMAXVALUES,OnResetMinMaxValues)
	ON_COMMAND(ID_POPUP_SELECTALLANALOGUEPARAMETERS,OnSelectAllAnalogueParameters)
	ON_COMMAND(ID_POPUP_DESELECTALLANALOGUEPARAMETERS,OnDeselectAllAnalogueParameters)
	ON_COMMAND(ID_POPUP_SELECTALLDIGITALPARAMETERS,OnSelectAllDigitalParameters)
	ON_COMMAND(ID_POPUP_DESELECTALLDIGITALPARAMETERS,OnDeselectAllDigitalParameters)
	ON_COMMAND(ID_MAINFRAME_SETCOMM,menuSetupComm)
	ON_COMMAND(ID_TOOLS_ROADDYNO,OnRoadDyno)
#if defined(ENABLE_EDIT_ROM)
	ON_COMMAND(ID_MAINFRAME_OPENROMFILE, OpenROMFile)
#endif
	ON_COMMAND(ID_MAINFRAME_ECUFLASHREADECUFLASHMEMORY,menuReadECU)
	ON_COMMAND(ID_TOOLS_WRITEROMIMAGETOECUFLASHMEMORY,menuWriteROM)
	ON_COMMAND(ID_TOOLS_TESTWRITETOECUFLASHMEMORY,menuTestWrite)
	ON_COMMAND(ID_TOOLS_COMPAREROMIMAGETOECUFLASHMEMORY,menuCompareROM)
	ON_COMMAND(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,OnUSDMWRX0203)
	ON_COMMAND(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20042005,OnUSDMWRX0405)
	ON_COMMAND(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXINTERNATIONAL20012005,OnIntlWRX0105)
	ON_COMMAND(ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,OnIntlSTI0105)
	ON_COMMAND(ID_MAINFRAME_EDITORS_LABELDATAFILE, Editor_label)
	ON_COMMAND(ID_MAINFRAME_EDITORS_DATAITEMDATAFILE, Editor_dataitem)
END_MESSAGE_MAP()

ecuExplorer::ecuExplorer()
{
	lpMainFrame = NULL;
	lpProtocolSSM = NULL;
	bCaptureErrorLog = TRUE;
	hWndResourceTree = NULL;
	hWndRealtime = NULL;
	hWndDTC = NULL;
	ulConfig = 0;
	hThreadStartup = NULL;
	hThreadLiveQuery = NULL;
	fpErrorLog = NULL;
	fpDebugLog = NULL;
	ulEcuFlash = 0;

	listLabel.RemoveAll();
	listDTC_OBD.RemoveAll();
	listDTC_Subaru.RemoveAll();
	mapLiveDataItem.RemoveAll();
	mapMap.RemoveAll();
	mapRom.RemoveAll();

	memset(&cAppPath,0,sizeof(cAppPath));
}

ecuExplorer::~ecuExplorer()
{}

BOOL ecuExplorer::InitInstance()
{
	CString sError;
	CMenu* pMenu = NULL;
	long lResult = 0;
	DWORD dwThreadID = 0;
	HANDLE hInstance = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	AfxEnableControlContainer();
	Enable3dControls();

EXCEPTION_BOOKMARK(__LINE__)
	if((hInstance = OpenEvent(EVENT_MODIFY_STATE,FALSE,INSTANCE_EVENTNAME)) == NULL)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_DESCRIPTOR);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = FALSE;

		hInstance = CreateEvent(&sa,FALSE,FALSE,INSTANCE_EVENTNAME);
	}
	else
	{
		MessageBox(NULL,"An instance of ecuExplorer is already running.  This instance will close","Instance already running",MB_ICONSTOP);
		exit(2);
	}

EXCEPTION_BOOKMARK(__LINE__)
	SetRegistryKey(_T("TARI Racing Software"));
	LoadStdProfileSettings();

EXCEPTION_BOOKMARK(__LINE__)
	::GetCurrentDirectory(sizeof(cAppPath),(char*)&cAppPath[0]);
	if(cAppPath[strlen((LPCTSTR)&cAppPath[0])-1] != '\\')
		cAppPath[strlen((LPCTSTR)&cAppPath[0])] = '\\';

EXCEPTION_BOOKMARK(__LINE__)
	ReadRegistry_settings();

EXCEPTION_BOOKMARK(__LINE__)
	FreeConsole();
	if(ulConfig & CONFIG_SHOWDEBUGCONSOLE)
	{
		AllocConsole();
		SetConsoleTitle("TARI Racing Software - ecuExplorer (DEBUG CONSOLE)");
		freopen("CONOUT$","r+",stdout);

		fpDebugLog = fopen("debug log.txt","a+");

		OutputString("DEBUG CONSOLE");
		sError.Format("ecuExplorer (Version %i.%i.%i)",APPVERSION_MAJOR,APPVERSION_MINOR,APPVERSION_BUILD);
		OutputString(sError);
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(bCaptureErrorLog == TRUE)
	{
		if((fpErrorLog = fopen("error log.txt","a+")) == NULL)
		{
			bCaptureErrorLog = FALSE;
			sError.Format("cannot open error log file : %i [0x%X]",GetLastError(),GetLastError());
			MessageBox(NULL,sError,"Error",MB_ICONSTOP);
		}
		else
		{
			CTime tTime = CTime::GetCurrentTime();
			sError.Format("application started @ %s",tTime.Format("%d.%m.%Y %Hh%Mm%Ss"));
			OutputString(sError,MESSAGETYPE_INFO);
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	lpMainFrame = new frameMain;
	if(!lpMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		sError.Format("lpMainFrame->LoadFrame Failed With %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return FALSE;
	}
	m_pMainWnd = lpMainFrame;

	OutputString("main frame constructed and loaded...");

EXCEPTION_BOOKMARK(__LINE__)
	m_pMainWnd->ShowWindow(GetRegistryValue("Settings","WindowState",SW_MAXIMIZE));
	m_pMainWnd->UpdateWindow();

	RECT rect;
	GetClientRect(lpMainFrame->splitterWndMain.m_hWnd,&rect);
	lpMainFrame->splitterWndMain.SetColumnInfo(0,(rect.right * GetRegistryValue("Settings","splitterMain",25)) / 100,40);
	lpMainFrame->splitterWndMain.RecalcLayout();
	GetClientRect(lpMainFrame->splitterWndLeft.m_hWnd,&rect);
	lpMainFrame->splitterWndLeft.SetRowInfo(0,(rect.bottom * GetRegistryValue("Settings","splitterLeft",85)) / 100,40);
	lpMainFrame->splitterWndLeft.RecalcLayout();
	lpMainFrame->RedrawWindow();

EXCEPTION_BOOKMARK(__LINE__)
	if((pMenu = lpMainFrame->GetMenu()) != NULL)
	{
		if(ulConfig & CONFIG_SHOWDEBUGCONSOLE)
			pMenu->CheckMenuItem(ID_MAINFRAME_SHOWDEBUGCONSOLE,MF_CHECKED);
		if(bCaptureErrorLog)
			pMenu->CheckMenuItem(ID_MAINFRAME_CAPTUREERRORLOG,MF_CHECKED);

		HMENU hMenu = GetSubMenu(GetMenu(m_pMainWnd->m_hWnd),1);
		pMenu->EnableMenuItem(ID_MAINFRAME_RESETECU,MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu,1,MF_BYPOSITION | MF_GRAYED);
		EnableMenuItem(hMenu,2,MF_BYPOSITION | MF_GRAYED);
		pMenu->EnableMenuItem(ID_TOOLS_ECUQUERY,MF_BYCOMMAND | MF_GRAYED);

#if !defined(ENABLE_EDIT_ROM)
		pMenu->DeleteMenu(ID_MAINFRAME_OPENROMFILE,MF_BYCOMMAND);
#endif

		if(ulEcuFlash & ECUFLASH_USDM_WRX_02_03)
			pMenu->CheckMenuRadioItem(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,MF_BYCOMMAND);
		else if(ulEcuFlash & ECUFLASH_USDM_WRX_04_05)
			pMenu->CheckMenuRadioItem(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20042005,MF_BYCOMMAND);
		else if(ulEcuFlash & ECUFLASH_INTL_WRX_01_05)
			pMenu->CheckMenuRadioItem(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXINTERNATIONAL20012005,MF_BYCOMMAND);
		else if(ulEcuFlash & ECUFLASH_INTL_STI_01_05)
			pMenu->CheckMenuRadioItem(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,MF_BYCOMMAND);

		OutputString("application menu built...");
	}

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = LoadDataFile_label()) != ERR_SUCCESS)
	{
		sError.Format("corrupt or missing label data file : %i [0x%X]",lResult,lResult);
		OutputString(sError,MESSAGETYPE_ERROR);
		MessageBox(m_pMainWnd->m_hWnd,sError,"Error",MB_ICONSTOP | MB_OK);
	}
	else
		OutputString("label data loaded...");	

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = LoadDataFile_live()) != ERR_SUCCESS)
	{
		sError.Format("corrupt or missing live data file : %i [0x%X]",lResult,lResult);
		OutputString(sError,MESSAGETYPE_ERROR);
		MessageBox(m_pMainWnd->m_hWnd,sError,"Error",MB_ICONSTOP | MB_OK);
	}
	else
		OutputString("live data loaded...");

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = LoadDataFile_dtc("subaru.dtc.dat",&listDTC_Subaru)) != ERR_SUCCESS)
	{
		sError.Format("corrupt or missing subaru dtc data file : %i [0x%X]",lResult,lResult);
		OutputString(sError,MESSAGETYPE_ERROR);
		MessageBox(m_pMainWnd->m_hWnd,sError,"Error",MB_ICONSTOP | MB_OK);
	}
	else
		OutputString("subaru dtc data loaded...");

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = LoadDataFile_dtc("obd.dtc.dat",&listDTC_OBD)) != ERR_SUCCESS)
	{
		sError.Format("corrupt or missing obd dtc data file : %i [0x%X]",lResult,lResult);
		OutputString(sError,MESSAGETYPE_ERROR);
		MessageBox(m_pMainWnd->m_hWnd,sError,"Error",MB_ICONSTOP | MB_OK);
	}
	else
		OutputString("obd dtc data loaded...");
#if defined(ENABLE_EDIT_ROM)
EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = LoadDataFile_map()) != ERR_SUCCESS)
	{
		sError.Format("corrupt or missing map data file : %i [0x%X]",lResult,lResult);
		OutputString(sError,MESSAGETYPE_ERROR);
		MessageBox(m_pMainWnd->m_hWnd,sError,"Error",MB_ICONSTOP | MB_OK);
	}
	else
		OutputString("map data loaded...");

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = LoadDataFile_rom()) != ERR_SUCCESS)
	{
		sError.Format("corrupt or missing rom data file : %i [0x%X]",lResult,lResult);
		OutputString(sError,MESSAGETYPE_ERROR);
		MessageBox(m_pMainWnd->m_hWnd,sError,"Error",MB_ICONSTOP | MB_OK);
	}
	else
		OutputString("rom data loaded...");
#endif
EXCEPTION_BOOKMARK(__LINE__)
	if((lpProtocolSSM = new protocolSSM) == NULL)
	{
		sError.Format("Memory Error - ecuExplorer::InitInstance [lpProtocolSSM] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return F_NO_MEMORY;
	}
	lpProtocolSSM->lpParent = (void*)this;

EXCEPTION_BOOKMARK(__LINE__)
	ReadRegistry_comm();

	if(ulConfig & CONFIG_CAPTURESSMTRACEFILE)
		pMenu->CheckMenuItem(ID_MAINFRAME_CAPTUREPROTOCOLTRACEFILE,MF_CHECKED);

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	lpFrameResourceTree->lpParent = this;
	lpFrameResourceTree->BuildResourceTree();

	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
	lpFrameRealtime->lpParent = this;
	lpFrameRealtime->ResizeSplitter();

	frameDTCReader* lpFrameDTCReader = (frameDTCReader*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUTROUBLECODEREADER));
	lpFrameDTCReader->lpParent = this;
	lpFrameDTCReader->ResizeSplitter();

	frameDataItems* lpFrameDataItems = (frameDataItems*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_DATAITEMS));
	lpFrameDataItems->lpParent = this;
	lpFrameDataItems->LoadDataItems(&mapLiveDataItem);

	frameTroubleCodes* lpFrameTroubleCodes = (frameTroubleCodes*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_TROUBLECODES));
	lpFrameTroubleCodes->lpParent = this;
	lpFrameTroubleCodes->LoadDTC_Subaru(&listDTC_Subaru);
	lpFrameTroubleCodes->LoadDTC_OBD(&listDTC_OBD);
	lpFrameTroubleCodes->ResizeSplitter();

	hWndResourceTree = lpFrameResourceTree->GetSafeHwnd();
	lpProtocolSSM->hWndRealtime = hWndRealtime = lpFrameRealtime->GetSafeHwnd();
	lpProtocolSSM->hWndDTC = hWndDTC = lpFrameDTCReader->GetSafeHwnd();

	OutputString("resource tree build complete...");

	lpMainFrame->splitterWndLeft.RedrawWindow();
	lpMainFrame->lpParent = (void*)this;

EXCEPTION_BOOKMARK(__LINE__)
	frameCommSerial* lpFrameCommSerial = (frameCommSerial*)lpMainFrame->splitterWndLeft.GetPane(1,0);
	lpFrameCommSerial->lpWinApp = this;
	lpFrameCommSerial->lpProtocolSSM = lpProtocolSSM;
	lpFrameCommSerial->EnumerateSerialPorts();

	OutputString("serial view build complete...");

EXCEPTION_BOOKMARK(__LINE__)
	if(lpProtocolSSM->iCommPort > 0)
	{
		if(lpProtocolSSM->Start() != ERR_SUCCESS)
		{
			sError.Format("The com port [COM%i] used for SSM diagnostics is no longer available.\nPlease check that this is the correct port.",lpProtocolSSM->iCommPort);
			MessageBox(m_pMainWnd->m_hWnd,sError,"Error",MB_ICONSTOP | MB_OK);
		}
		else
			StartStartupThread();
	}

	OutputString("comms modules started...");

EXCEPTION_BOOKMARK(__LINE__)
	LoadExternalSensorInput();

EXCEPTION_BOOKMARK(__LINE__)
	lpFrameResourceTree->lpTree->SelectItem((HTREEITEM)lpFrameResourceTree->GetNodeData_node(NODE_DATAITEMS));

CATCHCATCH("ecuExplorer::InitInstance()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

BOOL ecuExplorer::ExitInstance()
{
	CString sError;
	DWORD dwExitCode = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	StopStartupThread();
	StopLiveQueryThread();
	StopDTCQueryThread();

EXCEPTION_BOOKMARK(__LINE__)
	if(lpProtocolSSM != NULL)
	{
		lpProtocolSSM->Stop();
		delete lpProtocolSSM;
		lpProtocolSSM = NULL;
	}
	OutputString("comms modules stopped...");

EXCEPTION_BOOKMARK(__LINE__)
	ReleaseMemory_label();
	OutputString("label data memory released...");

EXCEPTION_BOOKMARK(__LINE__)
	ReleaseMemory_live();
	OutputString("data item memory released...");

EXCEPTION_BOOKMARK(__LINE__)
	ReleaseMemory_dtc();
	OutputString("dtc item memory released...");
#if defined(ENABLE_EDIT_ROM)
EXCEPTION_BOOKMARK(__LINE__)
	ReleaseMemory_map();
	OutputString("map item memory released...");

EXCEPTION_BOOKMARK(__LINE__)
	ReleaseMemory_rom();
	OutputString("rom item memory released...");
#endif
EXCEPTION_BOOKMARK(__LINE__)
	if(m_pMainWnd != NULL)
	{
		delete m_pMainWnd;
		m_pMainWnd = NULL;
	}

EXCEPTION_BOOKMARK(__LINE__)
	FreeConsole();

EXCEPTION_BOOKMARK(__LINE__)
	return CWinApp::ExitInstance();

CATCHCATCH("ecuExplorer::ExitInstance()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return FALSE;
}

void ecuExplorer::OnExit()
{
	m_pMainWnd->DestroyWindow();
}

void ecuExplorer::OpenLogFile()
{
	CString sError;
	TCHAR szTitle[255];
	TCHAR szDefExt[255];
	TCHAR szFile[255] = "\0";
	OPENFILENAME ofnFileName;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	strcpy(szFile,"");
	strcpy(szTitle,"Select a saved log capture file");
	strcpy(szDefExt,".csv");

	memset((void *)&ofnFileName,0,sizeof(OPENFILENAME));
	ofnFileName.lStructSize = sizeof(OPENFILENAME);
	ofnFileName.hwndOwner = m_pMainWnd->m_hWnd;
	ofnFileName.hInstance = NULL;
	ofnFileName.lpstrFilter = "Capture Files (*.csv)\0*.csv\0All Files (*.*)\0*.*\0\0";
	ofnFileName.lpstrCustomFilter = NULL;
	ofnFileName.nMaxCustFilter = 0;
	ofnFileName.nFilterIndex = 1;
	ofnFileName.lpstrFile = szFile;
	ofnFileName.nMaxFile = sizeof(szFile);
	ofnFileName.lpstrFileTitle = NULL;
	ofnFileName.nMaxFileTitle = 0;
	ofnFileName.lpstrInitialDir = NULL;
	ofnFileName.lpstrTitle = (LPSTR)&szTitle;
	ofnFileName.nFileOffset = 0;
	ofnFileName.nFileExtension = 0;
	ofnFileName.lpstrDefExt = (LPSTR)&szDefExt;
	ofnFileName.lCustData = NULL;
	ofnFileName.lpfnHook = NULL;
	ofnFileName.lpTemplateName = NULL;
	ofnFileName.Flags = OFN_SHOWHELP | OFN_EXPLORER | OFN_HIDEREADONLY;

	if(GetOpenFileName(&ofnFileName) == 0)
		return;

	sError.Format("selected log file - %s",szFile);
	OutputString(sError);

	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	lpFrameResourceTree->AddSavedLogFile(szFile);

	OutputString("log file item added to resource tree...");

CATCHCATCH("ecuExplorer::OpenLogFile()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}
#if defined(ENABLE_EDIT_ROM)
void ecuExplorer::OpenROMFile()
{
	CString sError;
	TCHAR szTitle[255];
	TCHAR szDefExt[255];
	TCHAR szFile[255] = "\0";
	OPENFILENAME ofnFileName;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	strcpy(szFile,"");
	strcpy(szTitle,"Select a ROM image file");
	strcpy(szDefExt,".bin");

	memset((void *)&ofnFileName,0,sizeof(OPENFILENAME));
	ofnFileName.lStructSize = sizeof(OPENFILENAME);
	ofnFileName.hwndOwner = m_pMainWnd->m_hWnd;
	ofnFileName.hInstance = NULL;
	ofnFileName.lpstrFilter = "ROM Image Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0\0";
	ofnFileName.lpstrCustomFilter = NULL;
	ofnFileName.nMaxCustFilter = 0;
	ofnFileName.nFilterIndex = 1;
	ofnFileName.lpstrFile = szFile;
	ofnFileName.nMaxFile = sizeof(szFile);
	ofnFileName.lpstrFileTitle = NULL;
	ofnFileName.nMaxFileTitle = 0;
	ofnFileName.lpstrInitialDir = NULL;
	ofnFileName.lpstrTitle = (LPSTR)&szTitle;
	ofnFileName.nFileOffset = 0;
	ofnFileName.nFileExtension = 0;
	ofnFileName.lpstrDefExt = (LPSTR)&szDefExt;
	ofnFileName.lCustData = NULL;
	ofnFileName.lpfnHook = NULL;
	ofnFileName.lpTemplateName = NULL;
	ofnFileName.Flags = OFN_SHOWHELP | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if(GetOpenFileName(&ofnFileName) == 0)
		return;

	sError.Format("selected ROM file - %s",szFile);
	OutputString(sError);

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	lpFrameResourceTree->AddROMFile(szFile);

CATCHCATCH("ecuExplorer::OpenROMFile()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}
#endif
long ecuExplorer::ReadRegistry_settings()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(GetProfileInt("Settings","CaptureErrorLog",99) == 99)
		WriteProfileInt("Settings","CaptureErrorLog",(int)bCaptureErrorLog);
	bCaptureErrorLog = GetProfileInt("Settings","CaptureErrorLog",bCaptureErrorLog);

EXCEPTION_BOOKMARK(__LINE__)
	if(GetProfileInt("Settings","Config",99) == 99)
		WriteProfileInt("Settings","Config",(int)ulConfig);
	ulConfig = GetProfileInt("Settings","Config",ulConfig);

EXCEPTION_BOOKMARK(__LINE__)
	if(GetProfileInt("Settings","ecuFlash",99) == 99)
		WriteProfileInt("Settings","ecuFlash",(int)ulEcuFlash);
	ulEcuFlash = GetProfileInt("Settings","ecuFlash",ulEcuFlash);

CATCHCATCH("ecuExplorer::ReadRegistry_settings()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return ERR_FAILED;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::ReadRegistry_comm()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(GetProfileInt("Settings","COMPort",99) == 99)
		WriteProfileInt("Settings","COMPort",lpProtocolSSM->iCommPort);
	lpProtocolSSM->iCommPort = GetProfileInt("Settings","COMPort",lpProtocolSSM->iCommPort);

EXCEPTION_BOOKMARK(__LINE__)
	if(GetProfileInt("Settings","XonLim",99) == 99)
		WriteProfileInt("Settings","XonLim",lpProtocolSSM->iXonLim);
	lpProtocolSSM->iXonLim = GetProfileInt("Settings","XonLim",lpProtocolSSM->iXonLim);

EXCEPTION_BOOKMARK(__LINE__)
	if(GetProfileInt("Settings","XoffLim",99) == 99)
		WriteProfileInt("Settings","XoffLim",lpProtocolSSM->iXoffLim);
	lpProtocolSSM->iXoffLim = GetProfileInt("Settings","XoffLim",lpProtocolSSM->iXoffLim);

EXCEPTION_BOOKMARK(__LINE__)
	if(GetProfileInt("Settings","ReadIntervalTimeout",99) == 99)
		WriteProfileInt("Settings","ReadIntervalTimeout",lpProtocolSSM->iReadIntervalTimeout);
	lpProtocolSSM->iReadIntervalTimeout = GetProfileInt("Settings","ReadIntervalTimeout",lpProtocolSSM->iReadIntervalTimeout);

EXCEPTION_BOOKMARK(__LINE__)
	if(GetProfileInt("Settings","ReadTotalTimeoutConstant",99) == 99)
		WriteProfileInt("Settings","ReadTotalTimeoutConstant",lpProtocolSSM->iReadTotalTimeoutConstant);
	lpProtocolSSM->iReadTotalTimeoutConstant = GetProfileInt("Settings","ReadTotalTimeoutConstant",lpProtocolSSM->iReadTotalTimeoutConstant);

EXCEPTION_BOOKMARK(__LINE__)
	if(GetProfileInt("Settings","WriteTotalTimeoutConstant",99) == 99)
		WriteProfileInt("Settings","WriteTotalTimeoutConstant",lpProtocolSSM->iWriteTotalTimeoutConstant);
	lpProtocolSSM->iWriteTotalTimeoutConstant = GetProfileInt("Settings","WriteTotalTimeoutConstant",lpProtocolSSM->iWriteTotalTimeoutConstant);

CATCHCATCH("ecuExplorer::ReadRegistry_comm()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return ERR_FAILED;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

void ecuExplorer::ShowAbout()
{
	CString sMessage;

	sMessage.Format("ecuExplorer - Build %i.%i.%i\n",APPVERSION_MAJOR,APPVERSION_MINOR,APPVERSION_BUILD);
	sMessage += "Copyright © 2005.  TARI Racing Software.\n";
	sMessage += "Written by Calvin Cestari\n\nCredits:\n";
	sMessage += "P J Naughter (serial port enumerator)\n";
	sMessage += "Caroline Englebienne (AW_CMultiViewSplitter)\n";
	sMessage += "Chris Maunder (MFC Grid Control)\n";
	sMessage += "tinywrex (DTC check bits)\n";
	sMessage += "Colby Boles & www.OpenECU.org (ecuFlash tools)\n";
	sMessage += "Simon Parkinson (STabCtrl)";
	MessageBox(NULL,sMessage,"About",MB_ICONINFORMATION);
}

void ecuExplorer::menuChangeOption_DebugConsole()
{
	CString sError;
	CMenu* pMenu = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(ulConfig & CONFIG_SHOWDEBUGCONSOLE)
		ulConfig &= ~CONFIG_SHOWDEBUGCONSOLE;
	else
		ulConfig |= CONFIG_SHOWDEBUGCONSOLE;

	WriteProfileInt("Settings","Config",(int)ulConfig);

EXCEPTION_BOOKMARK(__LINE__)
	if((pMenu = lpMainFrame->GetMenu()) != NULL)
		if(ulConfig & CONFIG_SHOWDEBUGCONSOLE)
			pMenu->CheckMenuItem(ID_MAINFRAME_SHOWDEBUGCONSOLE,MF_CHECKED);
		else
			pMenu->CheckMenuItem(ID_MAINFRAME_SHOWDEBUGCONSOLE,MF_UNCHECKED);

EXCEPTION_BOOKMARK(__LINE__)
	MessageBox(NULL,"You must restart ecuExplorer for the changes to take effect","Restart",MB_ICONINFORMATION);

CATCHCATCH("ecuExplorer::menuChangeOption_DebugConsole()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuChangeOption_CaptureErrorLog()
{
	CString sError;
	CMenu* pMenu = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	bCaptureErrorLog = !bCaptureErrorLog;
	WriteProfileInt("Settings","CaptureErrorLog",(int)bCaptureErrorLog);

EXCEPTION_BOOKMARK(__LINE__)
	if((pMenu = lpMainFrame->GetMenu()) != NULL)
		if(bCaptureErrorLog)
			pMenu->CheckMenuItem(ID_MAINFRAME_CAPTUREERRORLOG,MF_CHECKED);
		else
			pMenu->CheckMenuItem(ID_MAINFRAME_CAPTUREERRORLOG,MF_UNCHECKED);

EXCEPTION_BOOKMARK(__LINE__)
	MessageBox(NULL,"You must restart ecuExplorer for the changes to take effect","Restart",MB_ICONINFORMATION);

CATCHCATCH("ecuExplorer::menuChangeOption_CaptureErrorLog()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuChangeOption_CaptureProtocolTraceFile()
{
	CString sError;
	CMenu* pMenu = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(ulConfig & CONFIG_CAPTURESSMTRACEFILE)
		ulConfig &= ~CONFIG_CAPTURESSMTRACEFILE;
	else
		ulConfig |= CONFIG_CAPTURESSMTRACEFILE;

	WriteProfileInt("Settings","Config",(int)ulConfig);

EXCEPTION_BOOKMARK(__LINE__)
	if((pMenu = lpMainFrame->GetMenu()) != NULL)
		if(ulConfig & CONFIG_CAPTURESSMTRACEFILE)
			pMenu->CheckMenuItem(ID_MAINFRAME_CAPTUREPROTOCOLTRACEFILE,MF_CHECKED);
		else
			pMenu->CheckMenuItem(ID_MAINFRAME_CAPTUREPROTOCOLTRACEFILE,MF_UNCHECKED);

EXCEPTION_BOOKMARK(__LINE__)
	MessageBox(NULL,"You must restart ecuExplorer for the changes to take effect","Restart",MB_ICONINFORMATION);

CATCHCATCH("ecuExplorer::menuChangeOption_CaptureProtocolTraceFile()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

long ecuExplorer::ReleaseMemory_live()
{
	CString sError;
	POSITION posByte = NULL;
	POSITION posBit = NULL;
	LPSTRUCT_LIVEBYTEITEM lpLiveByteItem = NULL;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;
	unsigned short usByteIndex = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	posByte = mapLiveDataItem.GetStartPosition();
	while(posByte != NULL)
	{
		mapLiveDataItem.GetNextAssoc(posByte,usByteIndex,(void*&)lpLiveByteItem);
		mapLiveDataItem.RemoveKey(usByteIndex);

		while(lpLiveByteItem->listBitItems.GetCount() > 0)
		{
			lpLiveBitItem = (LPSTRUCT_LIVEBITITEM)lpLiveByteItem->listBitItems.RemoveHead();
			free(lpLiveBitItem->szName);
			free(lpLiveBitItem->szUnit);
			free(lpLiveBitItem->szDescription);
			delete lpLiveBitItem;
		}
		delete lpLiveByteItem;
	}

CATCHCATCH("ecuExplorer::ReleaseMemory_live()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::ReleaseMemory_label()
{
	CString sError;
	LPSTRUCT_LABELITEM lpLabelItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	while(listLabel.GetCount() > 0)
	{
		lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.RemoveHead();
		free(lpLabelItem->szID);
		free(lpLabelItem->szLabel);
		delete lpLabelItem;
	}

CATCHCATCH("ecuExplorer::ReleaseMemory_label()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::ReleaseMemory_dtc()
{
	CString sError;
	LPSTRUCT_LABELITEM lpLabelItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	while(listDTC_Subaru.GetCount() > 0)
	{
		lpLabelItem = (LPSTRUCT_LABELITEM)listDTC_Subaru.RemoveHead();
		free(lpLabelItem->szID);
		free(lpLabelItem->szLabel);
		delete lpLabelItem;
	}

EXCEPTION_BOOKMARK(__LINE__)
	while(listDTC_OBD.GetCount() > 0)
	{
		lpLabelItem = (LPSTRUCT_LABELITEM)listDTC_OBD.RemoveHead();
		free(lpLabelItem->szID);
		free(lpLabelItem->szLabel);
		delete lpLabelItem;
	}

CATCHCATCH("ecuExplorer::ReleaseMemory_dtc()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::SetSerialPort()
{
	CString sError;
	frameCommSerial* pCommSerialFrame = NULL;
	CWnd* pCommSerialWnd = NULL;
	RECT rect;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(!GetWindowRect(m_pMainWnd->m_hWnd,&rect))
	{
		sError.Format("failed to get parent window size for comms serial frame...");
		OutputString(sError,MESSAGETYPE_ERROR);
	}

EXCEPTION_BOOKMARK(__LINE__)
	pCommSerialFrame = new frameCommSerial;
	if(!pCommSerialFrame->Create(NULL,"Select COM Port",WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX,CRect(rect.left,rect.bottom,rect.right,250),m_pMainWnd))
	{
		sError.Format("failed to create comm serial frame...");
		OutputString(sError,MESSAGETYPE_ERROR);
		return ERR_FAILED;
	}
	pCommSerialWnd = pCommSerialFrame;
	pCommSerialFrame->lpProtocolSSM = lpProtocolSSM;
	pCommSerialFrame->lpWinApp = this;

	OutputString("comm serial frame constructed and loaded...");

EXCEPTION_BOOKMARK(__LINE__)
	pCommSerialWnd->ShowWindow(SW_SHOW);
	pCommSerialWnd->UpdateWindow();

EXCEPTION_BOOKMARK(__LINE__)
	pCommSerialFrame->EnumerateSerialPorts();

CATCHCATCH("ecuExplorer::SetSerialPort()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

DWORD WINAPI threadStartup(ecuExplorer* lpParent)
{
	CString sError;
	long lResult;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpParent == NULL)
		return EXIT_FAILURE;

EXCEPTION_BOOKMARK(__LINE__)
	while(lpParent->bThreadStartup)
	{
		if((lResult = lpParent->lpProtocolSSM->QueryECUType()) != ERR_SUCCESS)
		{
			sError.Format("QueryECUType failed with %i [0x%X]",lResult,lResult);
			OutputString(sError,MESSAGETYPE_INFO);
		}
		else
			break;

		Sleep(10);
	}

	if(lpParent->bThreadStartup == FALSE)
		return EXIT_SUCCESS;

EXCEPTION_BOOKMARK(__LINE__)
	PostMessage(lpParent->hWndResourceTree,MESSAGE_UPDATENODETEXT,(WPARAM)NODE_ECUPARENT,(LPARAM)lpParent->GetECUType(lpParent->lpProtocolSSM->sECU));

EXCEPTION_BOOKMARK(__LINE__)
	PostMessage(lpParent->hWndRealtime,MESSAGE_PARSESUPPORTLIST,(WPARAM)&lpParent->mapLiveDataItem,(LPARAM)&lpParent->lpProtocolSSM->sSupported);

EXCEPTION_BOOKMARK(__LINE__)
	CMenu* pMenu = lpParent->lpMainFrame->GetMenu();
	pMenu->EnableMenuItem(ID_MAINFRAME_RESETECU,MF_BYCOMMAND | MF_ENABLED);
	HMENU hMenu = GetSubMenu(GetMenu(lpParent->m_pMainWnd->m_hWnd),1);
	EnableMenuItem(hMenu,1,MF_BYPOSITION | MF_ENABLED);
	EnableMenuItem(hMenu,2,MF_BYPOSITION | MF_ENABLED);
	pMenu->EnableMenuItem(ID_TOOLS_ECUQUERY,MF_BYCOMMAND | MF_ENABLED);

#if defined(ENABLE_EDIT_ROM)
	pMenu->EnableMenuItem(ID_MAINFRAME_OPENROMFILE,MF_BYCOMMAND | MF_ENABLED);
#endif

	CloseHandle(lpParent->hThreadStartup);
	lpParent->hThreadStartup = NULL;

CATCHCATCH("threadStartup()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return EXIT_FAILURE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return EXIT_SUCCESS;
}

unsigned char ecuExplorer::GenerateChecksum(LPCTSTR szBuffer,unsigned short usBufferLength)
{
	CString sError;
	int iLoop;
	unsigned char cReturn = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	for(iLoop = 0;iLoop < usBufferLength;iLoop++)
		cReturn += szBuffer[iLoop];

CATCHCATCH("ecuExplorer::GenerateChecksum(LPCTSTR)");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return 0;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return cReturn;
}

unsigned char ecuExplorer::GenerateChecksum(unsigned long ulValue)
{
	CString sError;
	unsigned char* szValue = NULL;
	int iLoop;
	unsigned char cReturn = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((szValue = (unsigned char*)malloc(4)) == NULL)
	{
		sError.Format("Memory Error - GenerateChecksum(unsigned long) [szValue] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return 0;
	}

	memset(szValue,0,4);
	memcpy(szValue,&ulValue,4);

EXCEPTION_BOOKMARK(__LINE__)
	for(iLoop = 0;iLoop < 4;iLoop++)
		cReturn += szValue[iLoop];

EXCEPTION_BOOKMARK(__LINE__)
	if(szValue != NULL) free(szValue);

CATCHCATCH("ecuExplorer::GenerateChecksum(unsigned long)");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return 0;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return cReturn;
}

unsigned char ecuExplorer::GenerateChecksum(unsigned short usValue)
{
	CString sError;
	unsigned char* szValue = NULL;
	int iLoop;
	unsigned char cReturn = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((szValue = (unsigned char*)malloc(2)) == NULL)
	{
		sError.Format("Memory Error - GenerateChecksum(unsigned short) [szValue] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return 0;
	}

	memset(szValue,0,2);
	memcpy(szValue,&usValue,2);

EXCEPTION_BOOKMARK(__LINE__)
	for(iLoop = 0;iLoop < 2;iLoop++)
		cReturn += szValue[iLoop];

EXCEPTION_BOOKMARK(__LINE__)
	if(szValue != NULL) free(szValue);

CATCHCATCH("ecuExplorer::GenerateChecksum(unsigned short)");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return 0;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return cReturn;
}

long ecuExplorer::LoadDataFile_label()
{
	CString sError;
	int fh = 0;
	unsigned char cRead;
	unsigned char cStorage[255];
	unsigned char cChecksum;
	LPSTRUCT_LABELITEM lpLabelItem = NULL;

TRYTRY

	// Label Data File Format
	// 1 bytes [Version Length]
	// x bytes [Version]
	// 1 bytes [Version Checksum]
	// [..]
	//		1 bytes [ECU ID Length]
	//		x bytes [ECU ID]
	//		1 bytes [Label Length]
	//		x bytes [Label]
	//		1 bytes [Label Checksum]
	// [..]

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)&cStorage,0,255);
	sError.Format("%slabel.dat",&cAppPath);
	if((fh = _open(sError,_O_BINARY | _O_RDONLY,_S_IREAD)) == -1)
		return F_FILE_ERROR;

	_lseek(fh,0,SEEK_SET);

EXCEPTION_BOOKMARK(__LINE__)
	_read(fh,&cRead,1);
	_read(fh,&cStorage[0],cRead);
	cStorage[cRead] = '\0';

EXCEPTION_BOOKMARK(__LINE__)
	_read(fh,&cRead,1);
	if((cChecksum = strlen((LPCTSTR)&cStorage[0]) + GenerateChecksum((LPCTSTR)&cStorage[0],strlen((LPCTSTR)&cStorage[0]))) != cRead)
		return F_INVALID_CHECKSUM_VERSION;

	sError.Format("[%i] %s {%i}",strlen((LPCTSTR)&cStorage[0]),cStorage,cChecksum);
	OutputString(sError);

EXCEPTION_BOOKMARK(__LINE__)
	while(!_eof(fh))
	{
EXCEPTION_BOOKMARK(__LINE__)
		if((lpLabelItem = new STRUCT_LABELITEM) == NULL)
		{
			sError.Format("Memory Error - LoadDataFile_label [lpLabelItem] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);
			return F_NO_MEMORY;
		}

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpLabelItem->cLength_id,1);
		_read(fh,&cStorage,lpLabelItem->cLength_id);
		cStorage[lpLabelItem->cLength_id] = '\0';
		lpLabelItem->szID = (LPTSTR)malloc(lpLabelItem->cLength_id + 1);
		memset(lpLabelItem->szID,0,lpLabelItem->cLength_id);
		strncpy(lpLabelItem->szID,(LPCTSTR)&cStorage[0],lpLabelItem->cLength_id + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpLabelItem->cLength_label,1);
		_read(fh,&cStorage,lpLabelItem->cLength_label);
		cStorage[lpLabelItem->cLength_label] = '\0';
		lpLabelItem->szLabel = (LPTSTR)malloc(lpLabelItem->cLength_label + 1);
		memset(lpLabelItem->szLabel,0,lpLabelItem->cLength_label);
		strncpy(lpLabelItem->szLabel,(LPCTSTR)&cStorage[0],lpLabelItem->cLength_label + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cRead,1);
		if((cChecksum = lpLabelItem->cLength_id + GenerateChecksum(lpLabelItem->szID,lpLabelItem->cLength_id) + lpLabelItem->cLength_label + GenerateChecksum(lpLabelItem->szLabel,lpLabelItem->cLength_label)) != cRead)
			return F_INVALID_CHECKSUM_ITEM;

		sError.Format("[%i] %s {%i}",lpLabelItem->cLength_label,lpLabelItem->szLabel,cRead);
		OutputString(sError);
		
		listLabel.AddTail(lpLabelItem);
	}

EXCEPTION_BOOKMARK(__LINE__)
	_close(fh);

CATCHCATCH("ecuExplorer::LoadDataFile_label()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::LoadDataFile_live()
{
	CString sError;
	int fh = 0;
	unsigned char cRead;
	unsigned char cStorage[1024];
	unsigned char cChecksum;
	LPSTRUCT_LIVEBYTEITEM lpLiveByteItem = NULL;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;

TRYTRY

	// Live Data File Format
	// 1 bytes [Version Length]
	// x bytes [Version]
	// 1 bytes [Version Checksum]
	// [..]
	//		1 bytes [Byte]
	//		1 bytes [Bit]
	//		1 bytes [Name Length]
	//		x bytes [Name]
	//		4 bytes [Address High]
	//		4 bytes [Address Low]
	//		1 bytes [Type]
	//		2 bytes [Operand Addition]
	//		2 bytes [Operand Subtract]
	//		2 bytes [Operand Multiplier]
	//		2 bytes [Operand Divisor]
	//		2 bytes [Decimals]
	//		1 bytes [Unit Length]
	//		x bytes [Unit]
	//		2 bytes [Description Length]
	//		x bytes [Description]
	//		1 bytes [Checksum]
	// [..]

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)&cStorage,0,255);
	sError.Format("%slive.dat",&cAppPath);
	if((fh = _open(sError,_O_BINARY | _O_RDONLY,_S_IREAD)) == -1)
		return F_FILE_ERROR;

	_lseek(fh,0,SEEK_SET);

EXCEPTION_BOOKMARK(__LINE__)
	_read(fh,&cRead,1);
	_read(fh,&cStorage[0],cRead);
	cStorage[cRead] = '\0';

EXCEPTION_BOOKMARK(__LINE__)
	_read(fh,&cRead,1);
	if((cChecksum = strlen((LPCTSTR)&cStorage[0]) + GenerateChecksum((LPCTSTR)&cStorage[0],strlen((LPCTSTR)&cStorage[0]))) != cRead)
		return F_INVALID_CHECKSUM_VERSION;

	sError.Format("[%i] %s {%i}",strlen((LPCTSTR)&cStorage[0]),cStorage,cChecksum);
	OutputString(sError);

EXCEPTION_BOOKMARK(__LINE__)
	while(!_eof(fh))
	{
EXCEPTION_BOOKMARK(__LINE__)
		if((lpLiveBitItem = new STRUCT_LIVEBITITEM) == NULL)
		{
			sError.Format("Memory Error - LoadDataFile_live [lpLiveBitItem] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);
			return F_NO_MEMORY;
		}
		lpLiveBitItem->iListIndex = -1;
		lpLiveBitItem->bDisplay = FALSE;
		lpLiveBitItem->fValue = 0;

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpLiveBitItem->cByte,1);
		_read(fh,&lpLiveBitItem->cBit,1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpLiveBitItem->cLength_name,1);
		_read(fh,&cStorage,lpLiveBitItem->cLength_name);
		cStorage[lpLiveBitItem->cLength_name] = '\0';
		lpLiveBitItem->szName = (LPTSTR)malloc(lpLiveBitItem->cLength_name + 1);
		memset(lpLiveBitItem->szName,0,lpLiveBitItem->cLength_name);
		strncpy(lpLiveBitItem->szName,(LPCTSTR)&cStorage[0],lpLiveBitItem->cLength_name + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cStorage,4);
		memcpy(&lpLiveBitItem->ulAddress_high,&cStorage,4);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cStorage,4);
		memcpy(&lpLiveBitItem->ulAddress_low,&cStorage,4);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpLiveBitItem->cType,1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cStorage,2);
		memcpy(&lpLiveBitItem->usOperand_addition,&cStorage,2);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cStorage,2);
		memcpy(&lpLiveBitItem->usOperand_subtract,&cStorage,2);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cStorage,2);
		memcpy(&lpLiveBitItem->usOperand_multiplier,&cStorage,2);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cStorage,2);
		memcpy(&lpLiveBitItem->usOperand_divisor,&cStorage,2);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cStorage,2);
		memcpy(&lpLiveBitItem->usDecimals,&cStorage,2);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpLiveBitItem->cLength_unit,1);
		_read(fh,&cStorage,lpLiveBitItem->cLength_unit);
		cStorage[lpLiveBitItem->cLength_unit] = '\0';
		lpLiveBitItem->szUnit = (LPTSTR)malloc(lpLiveBitItem->cLength_unit + 1);
		memset(lpLiveBitItem->szUnit,0,lpLiveBitItem->cLength_unit);
		strncpy(lpLiveBitItem->szUnit,(LPCTSTR)&cStorage[0],lpLiveBitItem->cLength_unit + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpLiveBitItem->usLength_description,2);
		_read(fh,&cStorage,lpLiveBitItem->usLength_description);
		cStorage[lpLiveBitItem->usLength_description] = '\0';
		lpLiveBitItem->szDescription = (LPTSTR)malloc(lpLiveBitItem->usLength_description + 1);
		memset(lpLiveBitItem->szDescription,0,lpLiveBitItem->usLength_description);
		strncpy(lpLiveBitItem->szDescription,(LPCTSTR)&cStorage[0],lpLiveBitItem->usLength_description + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cRead,1);
		cChecksum = lpLiveBitItem->cByte + lpLiveBitItem->cBit + lpLiveBitItem->cLength_name + GenerateChecksum(lpLiveBitItem->szName,lpLiveBitItem->cLength_name) + GenerateChecksum(lpLiveBitItem->ulAddress_high) + GenerateChecksum(lpLiveBitItem->ulAddress_low) + lpLiveBitItem->cType + GenerateChecksum(lpLiveBitItem->usOperand_addition) + GenerateChecksum(lpLiveBitItem->usOperand_subtract) + GenerateChecksum(lpLiveBitItem->usOperand_multiplier) + GenerateChecksum(lpLiveBitItem->usOperand_divisor) + GenerateChecksum(lpLiveBitItem->usDecimals) + lpLiveBitItem->cLength_unit + GenerateChecksum(lpLiveBitItem->szUnit,lpLiveBitItem->cLength_unit) + GenerateChecksum(lpLiveBitItem->usLength_description) + GenerateChecksum(lpLiveBitItem->szDescription,lpLiveBitItem->usLength_description);
		if(cChecksum != cRead)
			return F_INVALID_CHECKSUM_ITEM;

		sError.Format("[%i::%i] [%i] %s [0x%X] [0x%X] [%i] [%i] [%i] [%i] [%i] [%i] [%i] %s {%i}",lpLiveBitItem->cByte,lpLiveBitItem->cBit,lpLiveBitItem->cLength_name,lpLiveBitItem->szName,lpLiveBitItem->ulAddress_high,lpLiveBitItem->ulAddress_low,lpLiveBitItem->cType,lpLiveBitItem->usOperand_addition,lpLiveBitItem->usOperand_subtract,lpLiveBitItem->usOperand_multiplier,lpLiveBitItem->usOperand_divisor,lpLiveBitItem->usDecimals,lpLiveBitItem->cLength_unit,lpLiveBitItem->szUnit,cRead);
		OutputString(sError);

EXCEPTION_BOOKMARK(__LINE__)
		if(mapLiveDataItem.Lookup(lpLiveBitItem->cByte,(void*&)lpLiveByteItem) == 0)
		{
EXCEPTION_BOOKMARK(__LINE__)
			if((lpLiveByteItem = new STRUCT_LIVEBYTEITEM) == NULL)
			{
				sError.Format("Memory Error - LoadDataFile_live [lpLiveByteItem] : %i [0x%X]",GetLastError(),GetLastError());
				OutputString(sError,MESSAGETYPE_ERROR);
				return F_NO_MEMORY;
			}
			mapLiveDataItem.SetAt(lpLiveBitItem->cByte,(void*&)lpLiveByteItem);
		}
		lpLiveByteItem->listBitItems.AddTail(lpLiveBitItem);
	}

EXCEPTION_BOOKMARK(__LINE__)
	_close(fh);

CATCHCATCH("ecuExplorer::LoadDataFile_live()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::LoadDataFile_dtc(LPCTSTR szFile,CPtrList* pStorage)
{
	CString sError;
	int fh = 0;
	unsigned char cRead;
	unsigned char cStorage[255];
	unsigned char cChecksum;
	LPSTRUCT_DTCITEM lpDTCItem = NULL;

TRYTRY

	// DTC Data File Format
	// 1 bytes [Version Length]
	// x bytes [Version]
	// 1 bytes [Version Checksum]
	// [..]
	//		1 bytes [DTC ID Length]
	//		x bytes [DTC ID]
	//		1 bytes [DTC Label Length]
	//		x bytes [DTC Label]
	//		4 bytes [Address - Current]
	//		4 bytes [Address - Historic]
	//		1 bytes [Check Bit]
	//		1 bytes [Checksum]
	// [..]

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)&cStorage,0,255);
	sError.Format("%s%s",&cAppPath,szFile);
	if((fh = _open(szFile,_O_BINARY | _O_RDONLY,_S_IREAD)) == -1)
		return F_FILE_ERROR;

	_lseek(fh,0,SEEK_SET);

EXCEPTION_BOOKMARK(__LINE__)
	_read(fh,&cRead,1);
	_read(fh,&cStorage[0],cRead);
	cStorage[cRead] = '\0';

EXCEPTION_BOOKMARK(__LINE__)
	_read(fh,&cRead,1);
	if((cChecksum = strlen((LPCTSTR)&cStorage[0]) + GenerateChecksum((LPCTSTR)&cStorage[0],strlen((LPCTSTR)&cStorage[0]))) != cRead)
		return F_INVALID_CHECKSUM_VERSION;

	sError.Format("[%i] %s {%i}",strlen((LPCTSTR)&cStorage[0]),cStorage,cChecksum);
	OutputString(sError);

EXCEPTION_BOOKMARK(__LINE__)
	while(!_eof(fh))
	{
EXCEPTION_BOOKMARK(__LINE__)
		if((lpDTCItem = new STRUCT_DTCITEM) == NULL)
		{
			sError.Format("Memory Error - LoadDataFile_dtc [lpDTCItem] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);
			return F_NO_MEMORY;
		}

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpDTCItem->cLength_id,1);
		_read(fh,&cStorage,lpDTCItem->cLength_id);
		cStorage[lpDTCItem->cLength_id] = '\0';
		lpDTCItem->szID = (LPTSTR)malloc(lpDTCItem->cLength_id + 1);
		memset(lpDTCItem->szID,0,lpDTCItem->cLength_id);
		strncpy(lpDTCItem->szID,(LPCTSTR)&cStorage[0],lpDTCItem->cLength_id + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpDTCItem->cLength_label,1);
		_read(fh,&cStorage,lpDTCItem->cLength_label);
		cStorage[lpDTCItem->cLength_label] = '\0';
		lpDTCItem->szLabel = (LPTSTR)malloc(lpDTCItem->cLength_label + 1);
		memset(lpDTCItem->szLabel,0,lpDTCItem->cLength_label);
		strncpy(lpDTCItem->szLabel,(LPCTSTR)&cStorage[0],lpDTCItem->cLength_label + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpDTCItem->ulAddress_current,4);
		_read(fh,&lpDTCItem->ulAddress_historic,4);
		_read(fh,&lpDTCItem->cCheckBit,1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cRead,1);
		if((cChecksum = lpDTCItem->cLength_id + GenerateChecksum(lpDTCItem->szID,lpDTCItem->cLength_id) + lpDTCItem->cLength_label + GenerateChecksum(lpDTCItem->szLabel,lpDTCItem->cLength_label) + GenerateChecksum(lpDTCItem->ulAddress_current) + GenerateChecksum(lpDTCItem->ulAddress_historic) + lpDTCItem->cCheckBit) != cRead)
			return F_INVALID_CHECKSUM_ITEM;

		sError.Format("[%i] %s [%i] %s 0x%X 0x%X 0x%X {%i}",lpDTCItem->cLength_id,lpDTCItem->szID,lpDTCItem->cLength_label,lpDTCItem->szLabel,lpDTCItem->ulAddress_current,lpDTCItem->ulAddress_historic,lpDTCItem->cCheckBit,cRead);
		OutputString(sError);
		
		pStorage->AddTail(lpDTCItem);
	}

EXCEPTION_BOOKMARK(__LINE__)
	_close(fh);

CATCHCATCH("ecuExplorer::LoadDataFile_dtc()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

LPCTSTR ecuExplorer::GetECUType(LPCTSTR szID)
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_LABELITEM lpLabelItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	pos = listLabel.GetHeadPosition();
	while(pos != NULL)
	{
		lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetNext(pos);
		if(strcmp(lpLabelItem->szID,szID) == 0)
			return lpLabelItem->szLabel;
	}

CATCHCATCH("ecuExplorer::GetECUType()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return "";
}

void ecuExplorer::menuResetECU()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->ResetECU();

CATCHCATCH("ecuExplorer::menuResetECU()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuRetardIgnitionTiming_query()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->QueryIgnitionTiming();

CATCHCATCH("ecuExplorer::menuRetardIgnitionTiming_query()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuRetardIgnitionTiming_increase()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->RetardIgnitionTiming(-1);

CATCHCATCH("ecuExplorer::menuRetardIgnitionTiming_increase()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuRetardIgnitionTiming_decrease()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->RetardIgnitionTiming(1);

CATCHCATCH("ecuExplorer::menuRetardIgnitionTiming_decrease()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuRetardIgnitionTiming_reset()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->RetardIgnitionTiming(0);

CATCHCATCH("ecuExplorer::menuRetardIgnitionTiming_reset()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Aircon_query()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->QueryIdleSpeed(ADDRESS_IDLE_ADJUST_AIRCON);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Aircon_query()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Aircon_decrease25()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_AIRCON,-1);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Aircon_decrease25()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Aircon_decrease50()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_AIRCON,-2);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Aircon_decrease50()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Aircon_decrease100()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_AIRCON,-4);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Aircon_decrease100()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Aircon_increase25()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_AIRCON,1);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Aircon_increase25()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Aircon_increase50()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_AIRCON,2);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Aircon_increase50()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Aircon_increase100()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_AIRCON,4);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Aircon_increase100()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Aircon_reset()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_AIRCON,(char)0x80);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Aircon_reset()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Normal_query()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->QueryIdleSpeed(ADDRESS_IDLE_ADJUST_NORMAL);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Normal_query()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Normal_decrease25()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_NORMAL,-1);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Normal_decrease25()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Normal_decrease50()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_NORMAL,-2);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Normal_decrease50()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Normal_decrease100()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_NORMAL,-4);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Normal_decrease100()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Normal_increase25()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_NORMAL,1);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Normal_increase25()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Normal_increase50()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_NORMAL,2);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Normal_increase50()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Normal_increase100()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_NORMAL,4);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Normal_increase100()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuSetIdleSpeed_Normal_reset()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	lpProtocolSSM->SetIdleSpeed(ADDRESS_IDLE_ADJUST_NORMAL,(char)0x80);

CATCHCATCH("ecuExplorer::menuSetIdleSpeed_Normal_reset()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnStartFileCapture()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
	lpFrameRealtime->OnStartFileCapture();

CATCHCATCH("ecuExplorer::OnStartFileCapture()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnTriggerLoggingOnDefogSwitch()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
	lpFrameRealtime->OnTriggerLoggingOnDefogSwitch();

CATCHCATCH("ecuExplorer::OnTriggerLoggingOnDefogSwitch()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnPauseDataCapture()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
	lpFrameRealtime->OnPauseDataCapture();

CATCHCATCH("ecuExplorer::OnPauseDataCapture()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnPauseDTC()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameDTCReader* lpFrameDTCReader = (frameDTCReader*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUTROUBLECODEREADER));
	lpFrameDTCReader->OnPauseDTC();

CATCHCATCH("ecuExplorer::OnPauseDataCapture()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnResetMinMaxValues()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
	lpFrameRealtime->OnResetMinMaxValues();

CATCHCATCH("ecuExplorer::OnResetMinMaxValues()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnSelectAllAnalogueParameters()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
	lpFrameRealtime->OnSelectAllAnalogueParameters();

CATCHCATCH("ecuExplorer::OnSelectAllAnalogueParameters()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnDeselectAllAnalogueParameters()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
	lpFrameRealtime->OnDeselectAllAnalogueParameters();

CATCHCATCH("ecuExplorer::OnDeselectAllAnalogueParameters()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnSelectAllDigitalParameters()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
	lpFrameRealtime->OnSelectAllDigitalParameters();

CATCHCATCH("ecuExplorer::OnSelectAllDigitalParameters()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnDeselectAllDigitalParameters()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	frameRealtime* lpFrameRealtime = (frameRealtime*)lpFrameResourceTree->lpSplitter->GetView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
	lpFrameRealtime->OnDeselectAllDigitalParameters();

CATCHCATCH("ecuExplorer::OnDeselectAllDigitalParameters()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

LPSTRUCT_LIVEBITITEM ecuExplorer::GetLiveDataItem(LPCTSTR szText)
{
	CString sError;
	POSITION posByte = NULL;
	POSITION posBit = NULL;
	LPSTRUCT_LIVEBYTEITEM lpLiveByteItem = NULL;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;
	unsigned short usByteIndex = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	posByte = mapLiveDataItem.GetStartPosition();
	while(posByte != NULL)
	{
		mapLiveDataItem.GetNextAssoc(posByte,usByteIndex,(void*&)lpLiveByteItem);
		posBit = lpLiveByteItem->listBitItems.GetHeadPosition();
		while(posBit != NULL)
		{
			lpLiveBitItem = (LPSTRUCT_LIVEBITITEM)lpLiveByteItem->listBitItems.GetNext(posBit);
			if(strcmp(lpLiveBitItem->szName,szText) == 0)
				return lpLiveBitItem;
		}
	}

CATCHCATCH("ecuExplorer::GetLiveDataItem(LPCTSTR)");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return NULL;
}

LPSTRUCT_LIVEBITITEM ecuExplorer::GetLiveDataItem(int iListIndex,int iType)
{
	CString sError;
	POSITION posByte = NULL;
	POSITION posBit = NULL;
	LPSTRUCT_LIVEBYTEITEM lpLiveByteItem = NULL;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;
	unsigned short usByteIndex = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	posByte = mapLiveDataItem.GetStartPosition();
	while(posByte != NULL)
	{
		mapLiveDataItem.GetNextAssoc(posByte,usByteIndex,(void*&)lpLiveByteItem);
		posBit = lpLiveByteItem->listBitItems.GetHeadPosition();
		while(posBit != NULL)
		{
			lpLiveBitItem = (LPSTRUCT_LIVEBITITEM)lpLiveByteItem->listBitItems.GetNext(posBit);
			if(lpLiveBitItem->iListIndex == iListIndex && lpLiveBitItem->cType == (char)iType)
				return lpLiveBitItem;
		}
	}

CATCHCATCH("ecuExplorer::GetLiveDataItem(int)");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return NULL;
}

DWORD WINAPI threadLiveQuery(ecuExplorer* lpParent)
{
	CString sError;
	long lResult;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpParent == NULL)
		return EXIT_FAILURE;

EXCEPTION_BOOKMARK(__LINE__)
	while(lpParent->bThreadLiveQuery)
	{
		if(lpParent->lpProtocolSSM->sSupported.GetLength() > 0)
		{
			if((lResult = lpParent->lpProtocolSSM->LiveQuery()) != ERR_SUCCESS)
			{
				sError.Format("LiveQuery failed with %i [0x%X]",lResult,lResult);
				OutputString(sError,MESSAGETYPE_INFO);
			}
		}
		Sleep(10);
	}

CATCHCATCH("threadLiveQuery()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return EXIT_FAILURE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return EXIT_SUCCESS;
}

long ecuExplorer::StartLiveQueryThread()
{
	CString sError;
	DWORD dwThreadID = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(hThreadLiveQuery != NULL)
		return EXIT_SUCCESS;

EXCEPTION_BOOKMARK(__LINE__)
	bThreadLiveQuery = TRUE;
	if((hThreadLiveQuery = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)threadLiveQuery,(LPVOID)this,0,&dwThreadID)) == NULL)
	{
		sError.Format("Thread Error - Start [hThreadLiveQuery] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return F_THREAD_ERROR;
	}
	else
		::SetThreadPriority(hThreadLiveQuery,THREAD_PRIORITY_ABOVE_NORMAL);

CATCHCATCH("ecuExplorer::StartLiveQueryThread()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return EXIT_FAILURE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return EXIT_SUCCESS;
}

long ecuExplorer::StopLiveQueryThread()
{
	CString sError;
	DWORD dwExitCode = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	bThreadLiveQuery = FALSE;
		
EXCEPTION_BOOKMARK(__LINE__)
	do
	{
		::GetExitCodeThread(hThreadLiveQuery,&dwExitCode);
		if(dwExitCode == STILL_ACTIVE)
			Sleep(10);
	}while(dwExitCode == STILL_ACTIVE);

EXCEPTION_BOOKMARK(__LINE__)
	if(hThreadLiveQuery != NULL)
	{
		CloseHandle(hThreadLiveQuery);
		hThreadLiveQuery = NULL;
	}

CATCHCATCH("ecuExplorer::StopLiveQueryThread()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return EXIT_FAILURE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return EXIT_SUCCESS;
}

int ecuExplorer::GetRegistryValue(CString sSection,CString sEntry,int iDefault)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(GetProfileInt(sSection,sEntry,iDefault) == iDefault)
		WriteProfileInt(sSection,sEntry,iDefault);
	iDefault = GetProfileInt(sSection,sEntry,iDefault);

CATCHCATCH("ecuExplorer::GetRegistryValue()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return iDefault;
}

int ecuExplorer::WriteRegistryValue(CString sSection,CString sEntry,int iValue)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	return WriteProfileInt(sSection,sEntry,iValue);

CATCHCATCH("ecuExplorer::WriteRegistryValue()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

void ecuExplorer::menuSetupComm()
{
	CString sError;
	BOOL bRestart = FALSE;

TRYTRY

	dialogCOMM dialog;
	dialog.lpParent = (void*)this;
	dialog.DoModal();

CATCHCATCH("ecuExplorer::menuSetupComm()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnUSDMWRX0203()
{
	CString sError;
	CMenu* pMenu = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(ulEcuFlash & ECUFLASH_USDM_WRX_02_03)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	ulEcuFlash |= ECUFLASH_USDM_WRX_02_03;
	ulEcuFlash &= ~ECUFLASH_USDM_WRX_04_05 & ~ECUFLASH_INTL_WRX_01_05 & ~ECUFLASH_INTL_STI_01_05;
	WriteProfileInt("Settings","ecuFlash",(int)ulEcuFlash);

EXCEPTION_BOOKMARK(__LINE__)
	if((pMenu = lpMainFrame->GetMenu()) != NULL)
		pMenu->CheckMenuRadioItem(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,MF_BYCOMMAND);

CATCHCATCH("ecuExplorer::OnUSDMWRX0203()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnUSDMWRX0405()
{
	CString sError;
	CMenu* pMenu = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(ulEcuFlash & ECUFLASH_USDM_WRX_04_05)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	ulEcuFlash |= ECUFLASH_USDM_WRX_04_05;
	ulEcuFlash &= ~ECUFLASH_USDM_WRX_02_03 & ~ECUFLASH_INTL_WRX_01_05 & ~ECUFLASH_INTL_STI_01_05;
	WriteProfileInt("Settings","ecuFlash",(int)ulEcuFlash);

EXCEPTION_BOOKMARK(__LINE__)
	if((pMenu = lpMainFrame->GetMenu()) != NULL)
		pMenu->CheckMenuRadioItem(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20042005,MF_BYCOMMAND);

CATCHCATCH("ecuExplorer::OnUSDMWRX0405()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnIntlWRX0105()
{
	CString sError;
	CMenu* pMenu = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(ulEcuFlash & ECUFLASH_INTL_WRX_01_05)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	ulEcuFlash |= ECUFLASH_INTL_WRX_01_05;
	ulEcuFlash &= ~ECUFLASH_USDM_WRX_02_03 & ~ECUFLASH_USDM_WRX_04_05 & ~ECUFLASH_INTL_STI_01_05;
	WriteProfileInt("Settings","ecuFlash",(int)ulEcuFlash);

EXCEPTION_BOOKMARK(__LINE__)
	if((pMenu = lpMainFrame->GetMenu()) != NULL)
		pMenu->CheckMenuRadioItem(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXINTERNATIONAL20012005,MF_BYCOMMAND);

CATCHCATCH("ecuExplorer::OnIntlWRX0105()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::OnIntlSTI0105()
{
	CString sError;
	CMenu* pMenu = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(ulEcuFlash & ECUFLASH_INTL_STI_01_05)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	ulEcuFlash |= ECUFLASH_INTL_STI_01_05;
	ulEcuFlash &= ~ECUFLASH_USDM_WRX_02_03 & ~ECUFLASH_USDM_WRX_04_05 & ~ECUFLASH_INTL_WRX_01_05;
	WriteProfileInt("Settings","ecuFlash",(int)ulEcuFlash);

EXCEPTION_BOOKMARK(__LINE__)
	if((pMenu = lpMainFrame->GetMenu()) != NULL)
		pMenu->CheckMenuRadioItem(ID_OPTIONS_ECUFLASH_MODEL_SUBARUWRXUSDM20022003,ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,ID_OPTIONS_ECUFLASH_MODEL_SUBARUSTIINTERNATIONAL20012005NONUSDM,MF_BYCOMMAND);

CATCHCATCH("ecuExplorer::OnIntlSTI0105()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuReadECU()
{
	int iResult = 0;
	if((iResult = MessageBox(NULL,"The ecuFlash tools are provided in the interest of open source software.  By clicking 'Yes' you agree to indemnify TARI Racing Software and all/any of its contributors against all claims of damage of loss of through the use of the ecuFlash tools provided within.\n\nUsing these tools can cause irreparable damage to the connected ECU.  Please ensure you are familiar with the risks before proceeding.\n\nDo you wish to proceed?","Disclaimer",MB_ICONSTOP | MB_YESNO)) == IDNO)
		return;

	ecuFlash(ID_MAINFRAME_ECUFLASHREADECUFLASHMEMORY);
}

void ecuExplorer::menuEcuQuery()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(hThreadStartup != NULL)
		StopStartupThread();
	else if(hThreadLiveQuery != NULL)
		switch(MessageBox(NULL,"Data capture will be paused during this process.\nDo you want to continue?","Pause Data Capture",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			OnPauseDataCapture();
			break;
		case IDNO:
			return;
	}
	else if(hThreadDTCQuery != NULL)
		switch(MessageBox(NULL,"Trouble Code reading will be paused during this process.\nDo you want to continue?","Pause Trouble Code Reading",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			OnPauseDTC();
			break;
		case IDNO:
			return;
	}
	if(lpProtocolSSM != NULL) lpProtocolSSM->Stop();

EXCEPTION_BOOKMARK(__LINE__)
	dialogEcuQuery dialog;
	dialog.lpParent = (void*)this;

EXCEPTION_BOOKMARK(__LINE__)
	dialog.DoModal();

EXCEPTION_BOOKMARK(__LINE__)
	if(lpProtocolSSM != NULL) lpProtocolSSM->Start();
	StartStartupThread();

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	lpFrameResourceTree->lpTree->SelectItem((HTREEITEM)lpFrameResourceTree->GetNodeData_node(NODE_ECUPARENT));

CATCHCATCH("ecuExplorer::menuEcuQuery()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void ecuExplorer::menuWriteROM()
{
	int iResult = 0;
	if((iResult = MessageBox(NULL,"The ecuFlash tools are provided in the interest of open source software.  By clicking 'Yes' you agree to indemnify TARI Racing Software and all/any of its contributors against all claims of damage of loss of through the use of the ecuFlash tools provided within.\n\nUsing these tools can cause irreparable damage to the connected ECU.  Please ensure you are familiar with the risks before proceeding.\n\nDo you wish to proceed?","Disclaimer",MB_ICONSTOP | MB_YESNO)) == IDNO)
		return;

	ecuFlash(ID_TOOLS_WRITEROMIMAGETOECUFLASHMEMORY);
}

void ecuExplorer::menuTestWrite()
{
	int iResult = 0;
	if((iResult = MessageBox(NULL,"The ecuFlash tools are provided in the interest of open source software.  By clicking 'Yes' you agree to indemnify TARI Racing Software and all/any of its contributors against all claims of damage of loss of through the use of the ecuFlash tools provided within.\n\nUsing these tools can cause irreparable damage to the connected ECU.  Please ensure you are familiar with the risks before proceeding.\n\nDo you wish to proceed?","Disclaimer",MB_ICONSTOP | MB_YESNO)) == IDNO)
		return;

	ecuFlash(ID_TOOLS_TESTWRITETOECUFLASHMEMORY);
}

void ecuExplorer::menuCompareROM()
{
	int iResult = 0;
	if((iResult = MessageBox(NULL,"The ecuFlash tools are provided in the interest of open source software.  By clicking 'Yes' you agree to indemnify TARI Racing Software and all/any of its contributors against all claims of damage of loss of through the use of the ecuFlash tools provided within.\n\nUsing these tools can cause irreparable damage to the connected ECU.  Please ensure you are familiar with the risks before proceeding.\n\nDo you wish to proceed?","Disclaimer",MB_ICONSTOP | MB_YESNO)) == IDNO)
		return;

	ecuFlash(ID_TOOLS_COMPAREROMIMAGETOECUFLASHMEMORY);
}

long ecuExplorer::BrowseForFile(CString* szReturn,DWORD dwFlags)
{
	TCHAR szTitle[255];
	TCHAR szDefExt[255];
	TCHAR szFile[255] = "\0";
	OPENFILENAME ofnFileName;

	strcpy(szFile,"");
	strcpy(szTitle,"Select a file");
	strcpy(szDefExt,"");

	memset((void *)&ofnFileName,0,sizeof(OPENFILENAME));
	ofnFileName.lStructSize = sizeof(OPENFILENAME);
	ofnFileName.hwndOwner = m_pMainWnd->m_hWnd;
	ofnFileName.hInstance = NULL;
	ofnFileName.lpstrFilter = "All Files (*.*)\0*.*\0\0";
	ofnFileName.lpstrCustomFilter = NULL;
	ofnFileName.nMaxCustFilter = 0;
	ofnFileName.nFilterIndex = 1;
	ofnFileName.lpstrFile = szFile;
	ofnFileName.nMaxFile = sizeof(szFile);
	ofnFileName.lpstrFileTitle = NULL;
	ofnFileName.nMaxFileTitle = 0;
	ofnFileName.lpstrInitialDir = NULL;
	ofnFileName.lpstrTitle = (LPSTR)&szTitle;
	ofnFileName.nFileOffset = 0;
	ofnFileName.nFileExtension = 0;
	ofnFileName.lpstrDefExt = (LPSTR)&szDefExt;
	ofnFileName.lCustData = NULL;
	ofnFileName.lpfnHook = NULL;
	ofnFileName.lpTemplateName = NULL;
	ofnFileName.Flags = OFN_SHOWHELP | OFN_EXPLORER | OFN_HIDEREADONLY | dwFlags;

	if(dwFlags & OFN_OVERWRITEPROMPT)
	{
		if(GetSaveFileName(&ofnFileName) != 0)
			*szReturn = ofnFileName.lpstrFile;
		else
			return FALSE;
	}
	else
	{
		if(GetOpenFileName(&ofnFileName) != 0)
			*szReturn = ofnFileName.lpstrFile;
		else
			return FALSE;
	}

	return TRUE;
}

void ecuExplorer::ecuFlash(int func)
{
	CString sError;
	HANDLE hThread = NULL;
	DWORD dwTemp = 0;
	STRUCT_ECUFLASHTHREAD structEcuFlashThread;
	CMenu* pMenu = NULL;
	int iResult = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(!(ulEcuFlash & ECUFLASH_USDM_WRX_02_03) && !(ulEcuFlash & ECUFLASH_USDM_WRX_04_05) && !(ulEcuFlash & ECUFLASH_INTL_WRX_01_05) && !(ulEcuFlash & ECUFLASH_INTL_STI_01_05))
		MessageBox(m_pMainWnd->m_hWnd,"You must first select a model from the 'Options' -> 'ecuFlash' -> 'Model' menu","Invalid ecuFlash Model",MB_OK | MB_ICONEXCLAMATION);

EXCEPTION_BOOKMARK(__LINE__)
	if(hThreadStartup != NULL)
		StopStartupThread();
	else if(hThreadLiveQuery != NULL)
		switch(MessageBox(NULL,"Data capture will be paused during this process.\nDo you want to continue?","Pause Data Capture",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			OnPauseDataCapture();
			break;
		case IDNO:
			return;
	}
	else if(hThreadDTCQuery != NULL)
		switch(MessageBox(NULL,"Trouble Code reading will be paused during this process.\nDo you want to continue?","Pause Trouble Code Reading",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			OnPauseDTC();
			break;
		case IDNO:
			return;
	}
	if(lpProtocolSSM != NULL) lpProtocolSSM->Stop();

EXCEPTION_BOOKMARK(__LINE__)
	sError = "ECU (Disconnected)";
	PostMessage(hWndResourceTree,MESSAGE_UPDATENODETEXT,(WPARAM)NODE_ECUPARENT,(LPARAM)(LPCTSTR)sError);

	if((pMenu = lpMainFrame->GetMenu()) != NULL)
	{
		HMENU hMenu = GetSubMenu(GetMenu(m_pMainWnd->m_hWnd),1);
		pMenu->EnableMenuItem(ID_MAINFRAME_RESETECU,MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu,1,MF_BYPOSITION | MF_GRAYED);
		EnableMenuItem(hMenu,2,MF_BYPOSITION | MF_GRAYED);
		pMenu->EnableMenuItem(ID_TOOLS_ECUQUERY,MF_BYCOMMAND | MF_GRAYED);
	}

EXCEPTION_BOOKMARK(__LINE__)
	dialogEcuFlash dialog;
	dialog.lpParent = (void*)this;

	structEcuFlashThread.lpDialog = (void*)&dialog;
	structEcuFlashThread.lpParent = (void*)this;
	structEcuFlashThread.func = func;

EXCEPTION_BOOKMARK(__LINE__)
	if((hThread = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)threadEcuFlash,(LPVOID)&structEcuFlashThread,0,&dwTemp)) == NULL)
	{
		sError.Format("Thread Error - ecuFlash [hThread] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return;
	}

EXCEPTION_BOOKMARK(__LINE__)
	dialog.DoModal();

EXCEPTION_BOOKMARK(__LINE__)
	if((iResult = MessageBox(m_pMainWnd->m_hWnd,"To enable the diagnostics functions of ecuExplorer you must do the following:\n1) Turn the ignition to the 'OFF' position\n2) DISCONNECT the green test mode connector under the dash\n3) Turn the ignition to the 'ON' position\n\nClick 'YES' to enable the diagnostics functions of ecuExplorer\nClick 'NO' to continue using the ecuFlash tools\n\nIf you click 'NO' the above action is not required!","Action Required",MB_ICONINFORMATION + MB_YESNO)) == IDNO)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	if(lpProtocolSSM != NULL) lpProtocolSSM->Start();
	StartStartupThread();

EXCEPTION_BOOKMARK(__LINE__)
	frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpMainFrame->splitterWndLeft.GetPane(0,0);
	lpFrameResourceTree->lpTree->SelectItem((HTREEITEM)lpFrameResourceTree->GetNodeData_node(NODE_ECUPARENT));

CATCHCATCH("ecuExplorer::ecuFlash()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

DWORD WINAPI threadEcuFlash(LPSTRUCT_ECUFLASHTHREAD lpBlock)
{
	CString sError;
	log l;
	unsigned char cStorage[255];
	int iResult = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)&cStorage,0,255);
	sError.Format("%secuFlash Log.txt",&((ecuExplorer*)lpBlock->lpParent)->cAppPath);

EXCEPTION_BOOKMARK(__LINE__)
	l.set_dialog((dialogEcuFlash*)lpBlock->lpDialog);
	l.set_filename((LPCTSTR)sError);
	l.set_timestamp(false);
	l.open();

EXCEPTION_BOOKMARK(__LINE__)
	ecutools ecut((LPCTSTR)((ecuExplorer*)lpBlock->lpParent)->lpProtocolSSM->sCommPort);
	ecut.addlog(&l);

EXCEPTION_BOOKMARK(__LINE__)
	if(((ecuExplorer*)lpBlock->lpParent)->ulEcuFlash & ECUFLASH_USDM_WRX_02_03)
		ecut.set_model(wrx2002);
	else if(((ecuExplorer*)lpBlock->lpParent)->ulEcuFlash & ECUFLASH_USDM_WRX_04_05)
		ecut.set_model(wrx2004);
	else if(((ecuExplorer*)lpBlock->lpParent)->ulEcuFlash & ECUFLASH_INTL_WRX_01_05)
		ecut.set_model(wrx2002);
	else if(((ecuExplorer*)lpBlock->lpParent)->ulEcuFlash & ECUFLASH_INTL_STI_01_05)
		ecut.set_model(wrx2002);

EXCEPTION_BOOKMARK(__LINE__)
	if(!ecut.read_kernel_from_resource(IDR_BINARY1))
		return ERR_FAILED;
	l.print("kernel image loaded and verified.\n");

EXCEPTION_BOOKMARK(__LINE__)
	switch(lpBlock->func)
	{
	case ID_MAINFRAME_ECUFLASHREADECUFLASHMEMORY:
		if(!((ecuExplorer*)lpBlock->lpParent)->BrowseForFile(&sError,OFN_OVERWRITEPROMPT))
		{
			((dialogEcuFlash*)lpBlock->lpDialog)->UpdateProgress("!! PROCESS ABORTED !!");
			return ERR_FAILED;
		}
		break;
	case ID_TOOLS_WRITEROMIMAGETOECUFLASHMEMORY:
	case ID_TOOLS_TESTWRITETOECUFLASHMEMORY:
	case ID_TOOLS_COMPAREROMIMAGETOECUFLASHMEMORY:
		if(!((ecuExplorer*)lpBlock->lpParent)->BrowseForFile(&sError,OFN_FILEMUSTEXIST))
		{
			((dialogEcuFlash*)lpBlock->lpDialog)->UpdateProgress("!! PROCESS ABORTED !!");
			return ERR_FAILED;
		}
		if(!((ecuExplorer*)lpBlock->lpParent)->IsRomRaw(sError))
			if((iResult = MessageBox(((ecuExplorer*)lpBlock->lpParent)->m_pMainWnd->m_hWnd,"The format of the ROM image does not appear to be valid.  It must be in raw format for writing to the ECU flash memory.\n\nAre you sure the loaded ROM image is not encrypted?","Confirmation Required",MB_YESNO | MB_ICONEXCLAMATION)) == IDNO)
			{
				((dialogEcuFlash*)lpBlock->lpDialog)->UpdateProgress("!! PROCESS ABORTED !!");
				return ERR_FAILED;
			}
		if(!ecut.read_image((LPCTSTR)sError))
		{
			((dialogEcuFlash*)lpBlock->lpDialog)->UpdateProgress("!! PROCESS FAILED !!");
			return ERR_FAILED;
		}
		l.print("ecu image loaded.\n");
		break;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(!ecut.ready_port())
	{
		l.print("unable to connect to port.\n");
		((dialogEcuFlash*)lpBlock->lpDialog)->UpdateProgress("!! PROCESS FAILED !!");
		return ERR_FAILED;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(lpBlock->func == ID_TOOLS_WRITEROMIMAGETOECUFLASHMEMORY)
	{
		if((iResult = MessageBox(((ecuExplorer*)lpBlock->lpParent)->m_pMainWnd->m_hWnd,"It is important that the ROM image is compatible with the connected ECU, otherwise your ECU will be useless.\n\nAre you sure the loaded ROM image is compatible with the connected ECU?","Confirmation Required",MB_YESNO | MB_ICONEXCLAMATION)) == IDNO)
		{
			((dialogEcuFlash*)lpBlock->lpDialog)->UpdateProgress("!! PROCESS ABORTED !!");
			return ERR_FAILED;
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	MessageBox(((ecuExplorer*)lpBlock->lpParent)->m_pMainWnd->m_hWnd,"You must do the following:\n1) Connect the green test mode connector under the dash\n2) Connect the white flash block\n3) Turn the ignition to the 'ON' position\n\nClick 'OK' to Continue","Action Required",MB_ICONINFORMATION + MB_OKCANCEL);

EXCEPTION_BOOKMARK(__LINE__)
	((dialogEcuFlash*)lpBlock->lpDialog)->lpButton->EnableWindow(FALSE);

	Sleep(5000);

EXCEPTION_BOOKMARK(__LINE__)
	switch(lpBlock->func)
	{
	case ID_MAINFRAME_ECUFLASHREADECUFLASHMEMORY:
		if(ecut.load_kernel())
		{
			ecut.dump_all_memory_to_file((LPCTSTR)sError);
		}
		break;
	case ID_TOOLS_WRITEROMIMAGETOECUFLASHMEMORY:
		ecut.flash_image(true);
		break;
	case ID_TOOLS_TESTWRITETOECUFLASHMEMORY:
		ecut.flash_image(false);
		break;
	case ID_TOOLS_COMPAREROMIMAGETOECUFLASHMEMORY:
		ecut.compare_image();
		break;
	}

EXCEPTION_BOOKMARK(__LINE__)
	ecut.close_port();
	l.close();

EXCEPTION_BOOKMARK(__LINE__)
	((dialogEcuFlash*)lpBlock->lpDialog)->UpdateProgress("<< PROCESS COMPLETE >>");

EXCEPTION_BOOKMARK(__LINE__)
	((dialogEcuFlash*)lpBlock->lpDialog)->lpButton->EnableWindow(TRUE);

CATCHCATCH("threadEcuFlash()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::StartDTCQueryThread()
{
	CString sError;
	DWORD dwThreadID = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(hThreadDTCQuery != NULL)
		return EXIT_SUCCESS;

EXCEPTION_BOOKMARK(__LINE__)
	bThreadDTCQuery = TRUE;
	if((hThreadDTCQuery = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)threadDTCQuery,(LPVOID)this,0,&dwThreadID)) == NULL)
	{
		sError.Format("Thread Error - Start [hThreadDTCQuery] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return F_THREAD_ERROR;
	}
	else
		::SetThreadPriority(hThreadDTCQuery,THREAD_PRIORITY_ABOVE_NORMAL);

CATCHCATCH("ecuExplorer::StartDTCQueryThread()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return EXIT_FAILURE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return EXIT_SUCCESS;
}

long ecuExplorer::StopDTCQueryThread()
{
	CString sError;
	DWORD dwExitCode = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	bThreadDTCQuery = FALSE;
		
EXCEPTION_BOOKMARK(__LINE__)
	do
	{
		::GetExitCodeThread(hThreadDTCQuery,&dwExitCode);
		if(dwExitCode == STILL_ACTIVE)
			Sleep(10);
	}while(dwExitCode == STILL_ACTIVE);

EXCEPTION_BOOKMARK(__LINE__)
	if(hThreadDTCQuery != NULL)
	{
		CloseHandle(hThreadDTCQuery);
		hThreadDTCQuery = NULL;
	}

CATCHCATCH("ecuExplorer::StopDTCQueryThread()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return EXIT_FAILURE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return EXIT_SUCCESS;
}

DWORD WINAPI threadDTCQuery(ecuExplorer* lpParent)
{
	CString sError;
	long lResult;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpParent == NULL)
		return EXIT_FAILURE;

EXCEPTION_BOOKMARK(__LINE__)
	while(lpParent->bThreadDTCQuery)
	{
		if(lpParent->lpProtocolSSM->sSupported.GetLength() > 0)
		{
			if(lpParent->lpProtocolSSM->sSupported.GetLength() <= 31)
			{
				if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_CURRENT_99_00_START,ADDRESS_DTC_CURRENT_99_00_END)) != ERR_SUCCESS)
				{
					sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_CURRENT_99_00_START,ADDRESS_DTC_CURRENT_99_00_END,lResult,lResult);
					OutputString(sError,MESSAGETYPE_INFO);
				}
				if(!lpParent->bThreadDTCQuery) break;
				if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_HISTORIC_99_00_START,ADDRESS_DTC_HISTORIC_99_00_END)) != ERR_SUCCESS)
				{
					sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_HISTORIC_99_00_START,ADDRESS_DTC_HISTORIC_99_00_END,lResult,lResult);
					OutputString(sError,MESSAGETYPE_INFO);
				}
				if(!lpParent->bThreadDTCQuery) break;
			}
			else
			{
				if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_CURRENT_01_03_START_1,ADDRESS_DTC_CURRENT_01_03_END_1)) != ERR_SUCCESS)
				{
					sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_CURRENT_01_03_START_1,ADDRESS_DTC_CURRENT_01_03_END_1,lResult,lResult);
					OutputString(sError,MESSAGETYPE_INFO);
				}
				if(!lpParent->bThreadDTCQuery) break;
				if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_CURRENT_01_03_START_2,ADDRESS_DTC_CURRENT_01_03_END_2)) != ERR_SUCCESS)
				{
					sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_CURRENT_01_03_START_2,ADDRESS_DTC_CURRENT_01_03_END_2,lResult,lResult);
					OutputString(sError,MESSAGETYPE_INFO);
				}
				if(!lpParent->bThreadDTCQuery) break;
				if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_HISTORIC_01_03_START_1,ADDRESS_DTC_HISTORIC_01_03_END_1)) != ERR_SUCCESS)
				{
					sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_HISTORIC_01_03_START_1,ADDRESS_DTC_HISTORIC_01_03_END_1,lResult,lResult);
					OutputString(sError,MESSAGETYPE_INFO);
				}
				if(!lpParent->bThreadDTCQuery) break;
				if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_HISTORIC_01_03_START_2,ADDRESS_DTC_HISTORIC_01_03_END_2)) != ERR_SUCCESS)
				{
					sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_HISTORIC_01_03_START_2,ADDRESS_DTC_HISTORIC_01_03_END_2,lResult,lResult);
					OutputString(sError,MESSAGETYPE_INFO);
				}
				if(!lpParent->bThreadDTCQuery) break;
				if(strstr(lpParent->GetECUType(lpParent->lpProtocolSSM->sECU),"MY04") || strstr(lpParent->GetECUType(lpParent->lpProtocolSSM->sECU),"MY05"))
				{
					if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_CURRENT_04_05_START_1,ADDRESS_DTC_CURRENT_04_05_END_1)) != ERR_SUCCESS)
					{
						sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_CURRENT_04_05_START_1,ADDRESS_DTC_CURRENT_04_05_END_1,lResult,lResult);
						OutputString(sError,MESSAGETYPE_INFO);
					}
					if(!lpParent->bThreadDTCQuery) break;
					if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_CURRENT_04_05_START_2,ADDRESS_DTC_CURRENT_04_05_END_2)) != ERR_SUCCESS)
					{
						sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_CURRENT_04_05_START_2,ADDRESS_DTC_CURRENT_04_05_END_2,lResult,lResult);
						OutputString(sError,MESSAGETYPE_INFO);
					}
					if(!lpParent->bThreadDTCQuery) break;
					if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_CURRENT_04_05_START_3,ADDRESS_DTC_CURRENT_04_05_END_3)) != ERR_SUCCESS)
					{
						sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_CURRENT_04_05_START_3,ADDRESS_DTC_CURRENT_04_05_END_3,lResult,lResult);
						OutputString(sError,MESSAGETYPE_INFO);
					}
					if(!lpParent->bThreadDTCQuery) break;
					if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_HISTORIC_04_05_START_1,ADDRESS_DTC_HISTORIC_04_05_END_1)) != ERR_SUCCESS)
					{
						sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_HISTORIC_04_05_START_1,ADDRESS_DTC_HISTORIC_04_05_END_1,lResult,lResult);
						OutputString(sError,MESSAGETYPE_INFO);
					}
					if(!lpParent->bThreadDTCQuery) break;
					if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_HISTORIC_04_05_START_2,ADDRESS_DTC_HISTORIC_04_05_END_2)) != ERR_SUCCESS)
					{
						sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_HISTORIC_04_05_START_2,ADDRESS_DTC_HISTORIC_04_05_END_2,lResult,lResult);
						OutputString(sError,MESSAGETYPE_INFO);
					}
					if(!lpParent->bThreadDTCQuery) break;
					if((lResult = lpParent->lpProtocolSSM->DTCQuery(ADDRESS_DTC_HISTORIC_04_05_START_3,ADDRESS_DTC_HISTORIC_04_05_END_3)) != ERR_SUCCESS)
					{
						sError.Format("DTCQuery(0x%X,0x%X) failed with %i [0x%X]",ADDRESS_DTC_HISTORIC_04_05_START_3,ADDRESS_DTC_HISTORIC_04_05_END_3,lResult,lResult);
						OutputString(sError,MESSAGETYPE_INFO);
					}
					if(!lpParent->bThreadDTCQuery) break;
				}
			}
		}
		Sleep(10);
	}

CATCHCATCH("threadDTCQuery()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return EXIT_FAILURE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return EXIT_SUCCESS;
}

void ecuExplorer::OnRoadDyno()
{}

BOOL ecuExplorer::IsRomRaw(int fhROM)
{
	CString sError;
	unsigned char cBuffer;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	_lseek(fhROM,0,SEEK_SET);

	_read(fhROM,&cBuffer,1);
	if(cBuffer != 0x2)
		return FALSE;

	_read(fhROM,&cBuffer,1);
	if(cBuffer != 0x20)
		return FALSE;

CATCHCATCH("ecuExplorer::IsRomRaw()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

BOOL ecuExplorer::IsRomRaw(LPCTSTR szFile)
{
	CString sError;
	int fhROM = 0;
	BOOL bResult = FALSE;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((fhROM = _open(szFile,_O_BINARY | _O_RDWR,_S_IREAD | _S_IWRITE)) == -1)
	{
		sError.Format("File Error - IsRomRaw(LPCTSTR) [_open] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError);
		return FALSE;
	}

	bResult = IsRomRaw(fhROM);
	_close(fhROM);
	return bResult;

CATCHCATCH("ecuExplorer::IsRomRaw()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return FALSE;
}

long ecuExplorer::StartStartupThread()
{
	CString sError;
	DWORD dwThreadID = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(hThreadStartup != NULL)
		return EXIT_SUCCESS;

EXCEPTION_BOOKMARK(__LINE__)
	bThreadStartup = TRUE;
	if((hThreadStartup = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)threadStartup,(LPVOID)this,0,&dwThreadID)) == NULL)
	{
		sError.Format("Thread Error - Start [threadStartup] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return F_THREAD_ERROR;
	}

CATCHCATCH("ecuExplorer::StartStartupThread()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::StopStartupThread()
{
	CString sError;
	DWORD dwExitCode = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	bThreadStartup = false;

EXCEPTION_BOOKMARK(__LINE__)
	do
	{
		::GetExitCodeThread(hThreadStartup,&dwExitCode);
		if(dwExitCode == STILL_ACTIVE)
			Sleep(10);
	}while(dwExitCode == STILL_ACTIVE);

EXCEPTION_BOOKMARK(__LINE__)
	if(hThreadStartup != NULL)
	{
		CloseHandle(hThreadStartup);
		hThreadStartup = NULL;
	}

CATCHCATCH("ecuExplorer::StopStartupThread()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}
#if defined(ENABLE_EDIT_ROM)
long ecuExplorer::LoadDataFile_map()
{
	CString sError;
	int fh = 0;
	unsigned char cRead = 0;
	unsigned char cChecksum = 0;
	unsigned char cStorage[1024];
	unsigned int iLoop = 0;
	LPSTRUCT_MAPITEM lpMapItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)

	// 1 bytes [Version Length]
	// x bytes [Version]
	// 1 bytes [Version Checksum]
	// [..]
	//		1 bytes [Node Type]
	//		1 bytes [Map Label Length]
	//		x bytes [Map Label]
	//		1 bytes [Data Map Number Of Items]
	//		1 bytes [Data Map Read Block Length]
	//		[X-AXIS]
	//			1 bytes [Label Length]
	//			x bytes [Label]
	//			1 bytes [Unit Length]
	//			x bytes [Unit]
	//			8 bytes [Operand - Multiplier]
	//			8 bytes [Operand - Addition]
	//			1 bytes [Decimals]
	//		[Y-AXIS]
	//			1 bytes [Label Length]
	//			x bytes [Label]
	//			1 bytes [Unit Length]
	//			x bytes [Unit]
	//			8 bytes [Operand - Multiplier]
	//			8 bytes [Operand - Addition]
	//			1 bytes [Decimals]
	//		[Z-AXIS]
	//			1 bytes [Label Length]
	//			x bytes [Label]
	//			1 bytes [Unit Length]
	//			x bytes [Unit]
	//			8 bytes [Operand - Multiplier]
	//			8 bytes [Operand - Addition]
	//			1 bytes [Decimals]
	//		1 bytes [Help Length]
	//		x bytes [Help]
	//		1 bytes [Checksum]

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)&cStorage,0,255);
	sError.Format("%smap.dat",&cAppPath);
	if((fh = _open(sError,_O_BINARY | _O_RDONLY,_S_IREAD)) == -1)
		return F_FILE_ERROR;

	_lseek(fh,0,SEEK_SET);

EXCEPTION_BOOKMARK(__LINE__)
	_read(fh,&cRead,1);
	_read(fh,&cStorage[0],cRead);
	cStorage[cRead] = '\0';

EXCEPTION_BOOKMARK(__LINE__)
	_read(fh,&cRead,1);
//	if((cChecksum = strlen((LPCTSTR)&cStorage[0]) + GenerateChecksum((LPCTSTR)&cStorage[0],strlen((LPCTSTR)&cStorage[0]))) != cRead)
//		return F_INVALID_CHECKSUM_VERSION;

	sError.Format("[%i] %s {%i}",strlen((LPCTSTR)&cStorage[0]),cStorage,cChecksum);
	OutputString(sError);

EXCEPTION_BOOKMARK(__LINE__)
	while(!_eof(fh))
	{
EXCEPTION_BOOKMARK(__LINE__)
		if((lpMapItem = new STRUCT_MAPITEM) == NULL)
		{
			sError.Format("Memory Error - LoadDataFile_map [lpMapItem] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);
			return F_NO_MEMORY;
		}
		lpMapItem->iIndex_lookup = iLoop;

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->cType,1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->cLength_label,1);
		_read(fh,&cStorage,lpMapItem->cLength_label);
		cStorage[lpMapItem->cLength_label] = '\0';
		lpMapItem->szLabel = (LPTSTR)malloc(lpMapItem->cLength_label + 1);
		memset(lpMapItem->szLabel,0,lpMapItem->cLength_label);
		strncpy(lpMapItem->szLabel,(LPCTSTR)&cStorage[0],lpMapItem->cLength_label + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->usDataMap_NumberOfItems,2);
		_read(fh,&lpMapItem->usDataMap_BlockLength,2);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->axisX.cLength_label,1);
		_read(fh,&cStorage,lpMapItem->axisX.cLength_label);
		cStorage[lpMapItem->axisX.cLength_label] = '\0';
		lpMapItem->axisX.szLabel = (LPTSTR)malloc(lpMapItem->axisX.cLength_label + 1);
		memset(lpMapItem->axisX.szLabel,0,lpMapItem->axisX.cLength_label);
		strncpy(lpMapItem->axisX.szLabel,(LPCTSTR)&cStorage[0],lpMapItem->axisX.cLength_label + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->axisX.cLength_unit,1);
		_read(fh,&cStorage,lpMapItem->axisX.cLength_unit);
		cStorage[lpMapItem->axisX.cLength_unit] = '\0';
		lpMapItem->axisX.szUnit = (LPTSTR)malloc(lpMapItem->axisX.cLength_unit + 1);
		memset(lpMapItem->axisX.szUnit,0,lpMapItem->axisX.cLength_unit);
		strncpy(lpMapItem->axisX.szUnit,(LPCTSTR)&cStorage[0],lpMapItem->axisX.cLength_unit + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->axisX.cType_unit,1);
		_read(fh,&lpMapItem->axisX.dblOperand_addition,8);
		_read(fh,&lpMapItem->axisX.dblOperand_multiplier,8);
		_read(fh,&lpMapItem->axisX.cDecimals,1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->axisY.cLength_label,1);
		_read(fh,&cStorage,lpMapItem->axisY.cLength_label);
		cStorage[lpMapItem->axisY.cLength_label] = '\0';
		lpMapItem->axisY.szLabel = (LPTSTR)malloc(lpMapItem->axisY.cLength_label + 1);
		memset(lpMapItem->axisY.szLabel,0,lpMapItem->axisY.cLength_label);
		strncpy(lpMapItem->axisY.szLabel,(LPCTSTR)&cStorage[0],lpMapItem->axisY.cLength_label + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->axisY.cLength_unit,1);
		_read(fh,&cStorage,lpMapItem->axisY.cLength_unit);
		cStorage[lpMapItem->axisY.cLength_unit] = '\0';
		lpMapItem->axisY.szUnit = (LPTSTR)malloc(lpMapItem->axisY.cLength_unit + 1);
		memset(lpMapItem->axisY.szUnit,0,lpMapItem->axisY.cLength_unit);
		strncpy(lpMapItem->axisY.szUnit,(LPCTSTR)&cStorage[0],lpMapItem->axisY.cLength_unit + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->axisY.cType_unit,1);
		_read(fh,&lpMapItem->axisY.dblOperand_addition,8);
		_read(fh,&lpMapItem->axisY.dblOperand_multiplier,8);
		_read(fh,&lpMapItem->axisY.cDecimals,1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->axisZ.cLength_label,1);
		_read(fh,&cStorage,lpMapItem->axisZ.cLength_label);
		cStorage[lpMapItem->axisZ.cLength_label] = '\0';
		lpMapItem->axisZ.szLabel = (LPTSTR)malloc(lpMapItem->axisZ.cLength_label + 1);
		memset(lpMapItem->axisZ.szLabel,0,lpMapItem->axisZ.cLength_label);
		strncpy(lpMapItem->axisZ.szLabel,(LPCTSTR)&cStorage[0],lpMapItem->axisZ.cLength_label + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->axisZ.cLength_unit,1);
		_read(fh,&cStorage,lpMapItem->axisZ.cLength_unit);
		cStorage[lpMapItem->axisZ.cLength_unit] = '\0';
		lpMapItem->axisZ.szUnit = (LPTSTR)malloc(lpMapItem->axisZ.cLength_unit + 1);
		memset(lpMapItem->axisZ.szUnit,0,lpMapItem->axisZ.cLength_unit);
		strncpy(lpMapItem->axisZ.szUnit,(LPCTSTR)&cStorage[0],lpMapItem->axisZ.cLength_unit + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->axisZ.cType_unit,1);
		_read(fh,&lpMapItem->axisZ.dblOperand_addition,8);
		_read(fh,&lpMapItem->axisZ.dblOperand_multiplier,8);
		_read(fh,&lpMapItem->axisZ.cDecimals,1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpMapItem->cLength_help,1);
		_read(fh,&cStorage,lpMapItem->cLength_help);
		cStorage[lpMapItem->cLength_help] = '\0';
		lpMapItem->szHelp = (LPTSTR)malloc(lpMapItem->cLength_help + 1);
		memset(lpMapItem->szHelp,0,lpMapItem->cLength_help);
		strncpy(lpMapItem->szHelp,(LPCTSTR)&cStorage[0],lpMapItem->cLength_help + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&cRead,1);
//		if((cChecksum = lpLabelItem->cLength_id + GenerateChecksum(lpLabelItem->szID,lpLabelItem->cLength_id) + lpLabelItem->cLength_label + GenerateChecksum(lpLabelItem->szLabel,lpLabelItem->cLength_label)) != cRead)
//			return F_INVALID_CHECKSUM_ITEM;

		sError.Format("map[%s] axisX[%s (%s)] axisY[%s (%s)] axisY[%s (%s)] {%i}",lpMapItem->szLabel,lpMapItem->axisX.szLabel,lpMapItem->axisX.szUnit,lpMapItem->axisY.szLabel,lpMapItem->axisY.szUnit,lpMapItem->axisZ.szLabel,lpMapItem->axisZ.szUnit,cRead);
		OutputString(sError);
		
		mapMap.SetAt(iLoop++,lpMapItem);
	}

EXCEPTION_BOOKMARK(__LINE__)
	_close(fh);

CATCHCATCH("ecuExplorer::LoadDataFile_map()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::LoadDataFile_rom()
{
	CString sError;
	int fh = 0;
	unsigned char cRead = 0;
	unsigned char cChecksum = 0;
	unsigned char cStorage[1024];
	LPSTRUCT_ROMITEM lpROMItem = NULL;
	unsigned long* lpOffset = NULL;
	int iLoop = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)

	// 1 bytes [Version Length]
	// x bytes [Version]
	// 1 bytes [Version Checksum]
	// [..]
	//		1 bytes [Revision Length]
	//		x bytes [Revision]
	//		1 bytes [Description Length]
	//		x bytes [Description]
	//		4 bytes [Map Offsets] - correspond with map.dat
	//		..
	//		4 bytes [Map Offsets] - correspond with map.dat
	//		1 bytes [Checksum]

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)&cStorage,0,255);
	sError.Format("%srom.dat",&cAppPath);
	if((fh = _open(sError,_O_BINARY | _O_RDONLY,_S_IREAD)) == -1)
		return F_FILE_ERROR;

	_lseek(fh,0,SEEK_SET);

EXCEPTION_BOOKMARK(__LINE__)
	_read(fh,&cRead,1);
	_read(fh,&cStorage[0],cRead);
	cStorage[cRead] = '\0';

EXCEPTION_BOOKMARK(__LINE__)
	_read(fh,&cRead,1);
//	if((cChecksum = strlen((LPCTSTR)&cStorage[0]) + GenerateChecksum((LPCTSTR)&cStorage[0],strlen((LPCTSTR)&cStorage[0]))) != cRead)
//		return F_INVALID_CHECKSUM_VERSION;

	sError.Format("[%i] %s {%i}",strlen((LPCTSTR)&cStorage[0]),cStorage,cChecksum);
	OutputString(sError);

EXCEPTION_BOOKMARK(__LINE__)
	while(!_eof(fh))
	{
EXCEPTION_BOOKMARK(__LINE__)
		if((lpROMItem = new STRUCT_ROMITEM) == NULL)
		{
			sError.Format("Memory Error - LoadDataFile_rom [lpROMItem] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);
			return F_NO_MEMORY;
		}

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpROMItem->cLength_revision,1);
		_read(fh,&cStorage,lpROMItem->cLength_revision);
		cStorage[lpROMItem->cLength_revision] = '\0';
		lpROMItem->szRevision = (LPTSTR)malloc(lpROMItem->cLength_revision + 1);
		memset(lpROMItem->szRevision,0,lpROMItem->cLength_revision);
		strncpy(lpROMItem->szRevision,(LPCTSTR)&cStorage[0],lpROMItem->cLength_revision + 1);

EXCEPTION_BOOKMARK(__LINE__)
		_read(fh,&lpROMItem->cLength_description,1);
		_read(fh,&cStorage,lpROMItem->cLength_description);
		cStorage[lpROMItem->cLength_description] = '\0';
		lpROMItem->szDescription = (LPTSTR)malloc(lpROMItem->cLength_description + 1);
		memset(lpROMItem->szDescription,0,lpROMItem->cLength_description);
		strncpy(lpROMItem->szDescription,(LPCTSTR)&cStorage[0],lpROMItem->cLength_description + 1);

EXCEPTION_BOOKMARK(__LINE__)
		for(iLoop=0;iLoop<mapMap.GetCount();iLoop++)
		{
			if((lpOffset = new unsigned long) == NULL)
			{
				sError.Format("Memory Error - LoadDataFile_rom [lpOffset] : %i [0x%X]",GetLastError(),GetLastError());
				OutputString(sError,MESSAGETYPE_ERROR);
				return F_NO_MEMORY;
			}

			_read(fh,lpOffset,4);
			lpROMItem->mapOffset.SetAt((WORD)iLoop,lpOffset);
		}

		_read(fh,&cRead,1);
//		if((cChecksum = lpLabelItem->cLength_id + GenerateChecksum(lpLabelItem->szID,lpLabelItem->cLength_id) + lpLabelItem->cLength_label + GenerateChecksum(lpLabelItem->szLabel,lpLabelItem->cLength_label)) != cRead)
//			return F_INVALID_CHECKSUM_ITEM;

		sError.Format("revision[%s] description[%s] maps[%i] {%i}",lpROMItem->szRevision,lpROMItem->szDescription,lpROMItem->mapOffset.GetCount(),cRead);
		OutputString(sError);
		
		mapRom.SetAt(lpROMItem->szRevision,lpROMItem);
	}

EXCEPTION_BOOKMARK(__LINE__)
	_close(fh);

CATCHCATCH("ecuExplorer::LoadDataFile_rom()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::ReleaseMemory_map()
{
	CString sError;
	WORD wKey = 0;
	POSITION pos = NULL;
	LPSTRUCT_MAPITEM lpMapItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	pos = mapMap.GetStartPosition();
	while(pos != NULL)
	{
		mapMap.GetNextAssoc(pos,wKey,(void*&)lpMapItem);
		mapMap.RemoveKey(wKey);

		free(lpMapItem->szLabel);
		free(lpMapItem->szHelp);
		free(lpMapItem->axisX.szLabel);
		free(lpMapItem->axisX.szUnit);
		free(lpMapItem->axisY.szLabel);
		free(lpMapItem->axisY.szUnit);
		free(lpMapItem->axisZ.szLabel);
		free(lpMapItem->axisZ.szUnit);
		delete lpMapItem;
	}

CATCHCATCH("ecuExplorer::ReleaseMemory_map()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long ecuExplorer::ReleaseMemory_rom()
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_ROMITEM lpROMItem = NULL;
	POSITION posOffset = NULL;
	unsigned long* lpOffset = NULL;
	WORD wKey = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)

	pos = mapRom.GetStartPosition();
	while(pos != NULL)
	{
		mapRom.GetNextAssoc(pos,sError,(void*&)lpROMItem);
		mapRom.RemoveKey(sError);

		free(lpROMItem->szRevision);
		free(lpROMItem->szDescription);
		posOffset = lpROMItem->mapOffset.GetStartPosition();
		while(posOffset != NULL)
		{
			lpROMItem->mapOffset.GetNextAssoc(posOffset,wKey,(void*&)lpOffset);
			lpROMItem->mapOffset.RemoveKey(wKey);
			delete lpOffset;
		}
		delete lpROMItem;
	}

CATCHCATCH("ecuExplorer::ReleaseMemory_rom()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}
#endif

void ecuExplorer::Editor_label()
{
	dialogEditor_label dg;
	dg.DoModal();
}

void ecuExplorer::Editor_dataitem()
{
	dialogEditor_dataitem dg;
	dg.DoModal();
}

long ecuExplorer::LoadExternalSensorInput()
{
	CString sError;
	DWORD dwIndex = 0;
	DWORD dwSize = MAX_PATH;
	char cName[MAX_PATH];
	char cFile[MAX_PATH];
	FILETIME timeFile;
	LONG lResult = 0;
	LPSTRUCT_EXTERNALINPUT lpExternalInput = NULL;
	HKEY hExternal = NULL;


TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(m_pszRegistryKey != NULL)
	{
		HKEY hKey = GetSectionKey("External");
		if(hKey == NULL) return ERR_FAILED;

		while((lResult = RegEnumKeyEx(hKey,dwIndex++,&cName[0],&dwSize,NULL,NULL,NULL,&timeFile)) != ERROR_NO_MORE_ITEMS)
		{
			if((lResult = RegOpenKeyEx(hKey,cName,0,KEY_ALL_ACCESS,&hExternal)) == ERR_SUCCESS)
			{
				if((lResult = RegQueryValueEx(hExternal,"DllFile",NULL,NULL,(unsigned char*)&cFile[0],&dwSize)) == ERR_SUCCESS)
				{
					if((lpExternalInput = new STRUCT_EXTERNALINPUT) == NULL)
					{
						sError.Format("Memory Error - LoadExternalSensorInput [lpExternalInput] : %i [0x%X]",GetLastError()); 
						OutputString(sError,TRUE);
						return F_NO_MEMORY;
					}

					lpExternalInput->sName = cName;
					lpExternalInput->sDll = cFile;
				}
				RegCloseKey(hExternal);
			}
		}
		RegCloseKey(hKey);
	}

CATCHCATCH("ecuExplorer::LoadExternalSensorInput()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}