/*/
	frameCommSerial.cpp (2005.06.27)
/*/

#include <wfc.h>
#include <afxcview.h>
#include <setupapi.h>

#include "frameCommSerial.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"

typedef HKEY (__stdcall SETUPDIOPENDEVREGKEY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, DWORD, DWORD, REGSAM);
typedef BOOL (__stdcall SETUPDICLASSGUIDSFROMNAME)(LPCTSTR, LPGUID, DWORD, PDWORD);
typedef BOOL (__stdcall SETUPDIDESTROYDEVICEINFOLIST)(HDEVINFO);
typedef BOOL (__stdcall SETUPDIENUMDEVICEINFO)(HDEVINFO, DWORD, PSP_DEVINFO_DATA);
typedef HDEVINFO (__stdcall SETUPDIGETCLASSDEVS)(LPGUID, LPCTSTR, HWND, DWORD);
typedef BOOL (__stdcall SETUPDIGETDEVICEREGISTRYPROPERTY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, PDWORD, PBYTE, DWORD, PDWORD);

BOOL IsNumeric(LPCTSTR lpszString,BOOL bIgnoreColon);

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

IMPLEMENT_DYNCREATE(frameCommSerial, CFrameWnd)

BEGIN_MESSAGE_MAP(frameCommSerial, CFrameWnd)
	//{{AFX_MSG_MAP(frameCommSerial
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_POPUP_SSMDIAGNOSTICS,OnSSMDiagnostics)
END_MESSAGE_MAP()

frameCommSerial::frameCommSerial()
{
	lpList = NULL;
	lpWinApp = NULL;
	lpFont = NULL;
}

frameCommSerial::~frameCommSerial()
{
	if(lpList != NULL)
		delete lpList;
	if(lpFont != NULL)
		delete lpFont;
}

BOOL frameCommSerial::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
{
	CString sError;

TRYTRY

	m_bAutoMenuEnable = FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	if(!CFrameWnd::OnCreateClient(lpcs,pContext))
		return FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
	if((lpList = new CListCtrl) == NULL)
	{
		sError.Format("Memory Error - frameCommSerial::OnClientCreate() [lpList] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return -1;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(!lpList->Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,rect,this,1))
	{
		sError.Format("CListCtrl::Create Failed With %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return -1;
	}

EXCEPTION_BOOKMARK(__LINE__)
	DWORD dwExtendedStyle = lpList->GetExtendedStyle();
	dwExtendedStyle |= LVS_EX_FULLROWSELECT;
	lpList->SetExtendedStyle(dwExtendedStyle);

EXCEPTION_BOOKMARK(__LINE__)
	lpFont = new CFont;
	LOGFONT lf;

	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 0xfffffff5;
	lf.lfWeight = FW_REGULAR;
	lf.lfOutPrecision = OUT_STROKE_PRECIS;
	lf.lfClipPrecision = CLIP_STROKE_PRECIS;
	lf.lfQuality = DRAFT_QUALITY;
	lf.lfPitchAndFamily = 0x22;
	lf.lfCharSet = 0;
	strcpy(lf.lfFaceName,"Tahoma");

	lpFont->CreateFontIndirect(&lf);
	lpList->SetFont(lpFont);

EXCEPTION_BOOKMARK(__LINE__)
	lpList->InsertColumn(0,"Port",LVCFMT_LEFT,50);
	lpList->InsertColumn(1,"Assigned To",LVCFMT_LEFT,100);
	lpList->InsertColumn(2,"Port Description",LVCFMT_LEFT,200);

CATCHCATCH("frameCommSerial::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameCommSerial::OnSize(UINT nType,int cx,int cy)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	GetClientRect(&rect);

EXCEPTION_BOOKMARK(__LINE__)
	lpList->MoveWindow(rect.left,rect.top,rect.right,rect.bottom,TRUE);

CATCHCATCH("frameCommSerial::OnSize()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

long frameCommSerial::EnumerateSerialPorts()
{
	CString sError;
	int iPort = 0;

TRYTRY

	while(lpList->GetItemCount() > 0)
		lpList->DeleteItem(0);

	/*/ This section of code was inspired by the enumser project by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com) /*/

EXCEPTION_BOOKMARK(__LINE__)
	HINSTANCE hModule = LoadLibrary(_T("SETUPAPI.DLL"));
	if(hModule == NULL)
		return F_API_ERROR;

	SETUPDIOPENDEVREGKEY* fnSETUPDIOPENDEVREGKEY = (SETUPDIOPENDEVREGKEY*)GetProcAddress(hModule,"SetupDiOpenDevRegKey");
	SETUPDICLASSGUIDSFROMNAME* fnSETUPDICLASSGUIDSFROMNAME = (SETUPDICLASSGUIDSFROMNAME*)GetProcAddress(hModule,"SetupDiClassGuidsFromNameA");
	SETUPDIGETCLASSDEVS* fnSETUPDIGETCLASSDEVS = (SETUPDIGETCLASSDEVS*)GetProcAddress(hModule,"SetupDiGetClassDevsA");
	SETUPDIGETDEVICEREGISTRYPROPERTY* fnSETUPDIGETDEVICEREGISTRYPROPERTY = (SETUPDIGETDEVICEREGISTRYPROPERTY*)GetProcAddress(hModule,"SetupDiGetDeviceRegistryPropertyA");
	SETUPDIDESTROYDEVICEINFOLIST* fnSETUPDIDESTROYDEVICEINFOLIST = (SETUPDIDESTROYDEVICEINFOLIST*)GetProcAddress(hModule,"SetupDiDestroyDeviceInfoList");
	SETUPDIENUMDEVICEINFO* fnSETUPDIENUMDEVICEINFO = (SETUPDIENUMDEVICEINFO*)GetProcAddress(hModule,"SetupDiEnumDeviceInfo");

	if(fnSETUPDIOPENDEVREGKEY == NULL || fnSETUPDICLASSGUIDSFROMNAME == NULL || fnSETUPDIDESTROYDEVICEINFOLIST == NULL || fnSETUPDIENUMDEVICEINFO == NULL || fnSETUPDIGETCLASSDEVS == NULL || fnSETUPDIGETDEVICEREGISTRYPROPERTY == NULL)
	{
		FreeLibrary(hModule);
		return F_INVALID_POINTER;
	}

	DWORD dwGuids = 0;
	fnSETUPDICLASSGUIDSFROMNAME(_T("Ports"),NULL,0,&dwGuids);
	if(dwGuids == 0)
	{
		FreeLibrary(hModule);
		return F_INVALID_POINTER;
	}

	GUID* pGuids = new GUID[dwGuids];
	if(!fnSETUPDICLASSGUIDSFROMNAME(_T("Ports"),pGuids,dwGuids,&dwGuids))
	{
		delete [] pGuids;
		FreeLibrary(hModule);
		return F_INVALID_POINTER;
	}

	HDEVINFO hDevInfo = fnSETUPDIGETCLASSDEVS(pGuids,NULL,NULL,DIGCF_PRESENT);
	if(hDevInfo == INVALID_HANDLE_VALUE)
	{
		delete [] pGuids;
		FreeLibrary(hModule);
		return F_INVALID_POINTER;
	}

	delete [] pGuids;
	pGuids = NULL;

	BOOL bIterate = TRUE;
	int iIndex = 0;
	SP_DEVINFO_DATA devInfo;
	while(bIterate)
	{
		devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
		bIterate = fnSETUPDIENUMDEVICEINFO(hDevInfo,iIndex,&devInfo);
		if(bIterate)
		{
			iPort = 0;
			BOOL bAdded = FALSE;

			HKEY hDeviceKey = fnSETUPDIOPENDEVREGKEY(hDevInfo,&devInfo,DICS_FLAG_GLOBAL,0,DIREG_DEV,KEY_QUERY_VALUE);
			if(hDeviceKey)
			{
				TCHAR pszPortName[256];
				DWORD dwSize = sizeof(pszPortName);
				DWORD dwType = 0;
  				if((RegQueryValueEx(hDeviceKey,_T("PortName"),NULL,&dwType,(LPBYTE)pszPortName,&dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
				{
					int iLength = _tcslen(pszPortName);
					if(iLength > 3)
					{
						if((_tcsnicmp(pszPortName,_T("COM"),3) == 0) && IsNumeric(&pszPortName[3],FALSE))
						{
							iPort = _ttoi(&pszPortName[3]);
							bAdded = TRUE;
						}
					}
				}
				RegCloseKey(hDeviceKey);
			}

			if(bAdded)
			{
				TCHAR szFriendlyName[256];
				DWORD dwSize = sizeof(szFriendlyName);
				DWORD dwType = 0;
				fnSETUPDIGETDEVICEREGISTRYPROPERTY(hDevInfo,&devInfo,SPDRP_DEVICEDESC,&dwType,(PBYTE)szFriendlyName,dwSize,&dwSize);
				sError.Format("COM%i",iPort);
				lpList->InsertItem(lpList->GetItemCount(),sError);
				lpList->SetItemData(lpList->GetItemCount()-1,iPort);
				if(iPort == lpProtocolSSM->iCommPort)
					lpList->SetItemText(lpList->GetItemCount()-1,1,"SSM Diagnostics");
//				else if(iPort == lpProtocolLM1->iCommPort)
//					lpList->SetItemText(lpList->GetItemCount()-1,1,"LM-1 Wideband Sensor");
				lpList->SetItemText(lpList->GetItemCount()-1,2,szFriendlyName);
			}
		}
		iIndex++;
	}

CATCHCATCH("ecuExplorer::EnumerateSerialPorts()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

BOOL IsNumeric(LPCTSTR lpszString,BOOL bIgnoreColon)
{
	CString sError;

TRYTRY

	int iLength = _tcslen(lpszString);
	if(iLength == 0)
		return FALSE;

	BOOL bNumeric = TRUE;

	for(int i = 0;i < iLength && bNumeric;i++)
	{
		bNumeric = (_istdigit(lpszString[i]) != 0);
		if(bIgnoreColon && (lpszString[i] == _T(':')))
			bNumeric = TRUE;
	}

	return bNumeric;

CATCHCATCH("IsNumeric()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return FALSE;
}

void frameCommSerial::OnContextMenu(CWnd* pWnd,CPoint ptMousePos)
{
	CString sError;
	int iPort;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	ScreenToClient(&ptMousePos);

EXCEPTION_BOOKMARK(__LINE__)
	CMenu menu;
	CMenu* pPopup = NULL;

	menu.LoadMenu(IDR_POPUP_COMMSERIAL);
	pPopup = menu.GetSubMenu(0);

	pPopup->EnableMenuItem(ID_POPUP_SSMDIAGNOSTICS,MF_BYCOMMAND | MF_GRAYED);

EXCEPTION_BOOKMARK(__LINE__)
	if(lpList->GetSelectedCount() > 0)
	{
		pPopup->EnableMenuItem(ID_POPUP_SSMDIAGNOSTICS,MF_BYCOMMAND | MF_ENABLED);

		int iIndex = -1;
		while((iIndex = lpList->GetNextItem(iIndex,LVNI_SELECTED)) != -1)
		{
			iPort = lpList->GetItemData(iIndex);
			if(iPort == lpProtocolSSM->iCommPort)
				pPopup->CheckMenuItem(ID_POPUP_SSMDIAGNOSTICS,MF_CHECKED);
			break;
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	ClientToScreen(&ptMousePos);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN,ptMousePos.x,ptMousePos.y,this);

CATCHCATCH("frameCommSerial::OnContextMenu()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameCommSerial::OnSSMDiagnostics()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	while((iIndex = lpList->GetNextItem(iIndex,LVNI_SELECTED)) != -1)
	{
		lpProtocolSSM->iCommPort = lpList->GetItemData(iIndex);
		EnumerateSerialPorts();
		lpWinApp->WriteProfileInt("Settings","COMPort",lpProtocolSSM->iCommPort);
		MessageBox("You must restart ecuExplorer for the new settings to take effect","Restart",MB_ICONINFORMATION);
		break;
	}

CATCHCATCH("frameCommSerial::OnSSMDiagnostics()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}
