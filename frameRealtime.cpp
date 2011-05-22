/*/
	frameRealtime.cpp (2005.07.31)
/*/

#include <wfc.h>

#include "frameRealtime.h"
#include "handlerError.h"
#include "definitionLocal.h"
#include "resource.h"
#include "definitionError.h"
#include "frameList.h"
#include "frameResourceTree.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

DWORD WINAPI callbackRealtime(void* lpClass,CListCtrl* lpList,int iIndex,BOOL bState);

IMPLEMENT_DYNCREATE(frameRealtime, CFrameWnd)

BEGIN_MESSAGE_MAP(frameRealtime, CFrameWnd)
	//{{AFX_MSG_MAP(frameRealtime
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_MESSAGE(MESSAGE_PARSESUPPORTLIST,OnParseSupportList)
	ON_MESSAGE(MESSAGE_UPDATELISTITEM,OnUpdateListItem)
	ON_MESSAGE(MESSAGE_UPDATECSVFILE,OnUpdateCSVFile)
	ON_COMMAND(ID_POPUP_STARTFILECAPTURE,OnStartFileCapture)
	ON_COMMAND(ID_POPUP_CHOOSELOGGINGDIRECTORY,OnChooseLoggingDirectory)
	ON_COMMAND(ID_POPUP_TRIGGERLOGGINGONDEFOGSWITCH,OnTriggerLoggingOnDefogSwitch)
	ON_COMMAND(ID_POPUP_CONVERT_INJECTOR,OnConvert_injector)
	ON_COMMAND(ID_POPUP_CONVERT_TEMPERATURE,OnConvert_temperature)
	ON_COMMAND(ID_POPUP_CONVERT_SPEED,OnConvert_speed)
	ON_COMMAND(ID_POPUP_CONVERT_PRESSURE,OnConvert_pressure)
	ON_COMMAND(ID_POPUP_CONVERT_AF,OnCovert_af)
	ON_COMMAND(ID_POPUP_PAUSEDATACAPTURE,OnPauseDataCapture)
	ON_COMMAND(ID_POPUP_RESETMINMAXVALUES,OnResetMinMaxValues)
	ON_COMMAND(ID_POPUP_SELECTALLANALOGUEPARAMETERS,OnSelectAllAnalogueParameters)
	ON_COMMAND(ID_POPUP_DESELECTALLANALOGUEPARAMETERS,OnDeselectAllAnalogueParameters)
	ON_COMMAND(ID_POPUP_SELECTALLDIGITALPARAMETERS,OnSelectAllDigitalParameters)
	ON_COMMAND(ID_POPUP_DESELECTALLDIGITALPARAMETERS,OnDeselectAllDigitalParameters)
	ON_COMMAND(ID_POPUP_VIEWITEMDESCRIPTION,OnViewItemDescription)
	ON_COMMAND(IE_POPUP_AUTOMATICALLYADDNEWLOGFILESTOSAVEDLIST,OnAutoAddNewLogFile)
	ON_COMMAND(IE_POPUP_USEABSOLUTETIME,OnUseAbsoluteTime)
END_MESSAGE_MAP()

frameRealtime::frameRealtime()
{
	lpFont = NULL;
	lpSplitter = NULL;
	lpParent = NULL;
	fpCSVCapture = NULL;
	bFlag = FALSE;
}

frameRealtime::~frameRealtime()
{
	if(lpSplitter != NULL)
		delete lpSplitter;
	if(lpFont != NULL)
		delete lpFont;
}

BOOL frameRealtime::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* pContext)
{
	CString sError;

TRYTRY

	m_bAutoMenuEnable = FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	if(!CFrameWnd::OnCreateClient(lpcs,pContext))
		return FALSE;

EXCEPTION_BOOKMARK(__LINE__)
	if((lpSplitter = new CSplitterWnd) == NULL)
	{
		sError.Format("Memory Error - frameTroubleCodes::OnClientCreate() [lpSplitter] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
	}

	lpSplitter->CreateStatic(this,2,1);
	lpSplitter->CreateView(0,0,RUNTIME_CLASS(frameList),CSize(100,100),pContext);
	lpSplitter->CreateView(1,0,RUNTIME_CLASS(frameList),CSize(100,100),pContext);

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

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);
	lpList_analogue->RegisterCallback(callbackRealtime);
	lpList_analogue->SetFont(lpFont);
	lpList_analogue->SetViewStyle(LVS_REPORT,LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	lpList_analogue->AddColumn("Analogue Data Item Name",280);
	lpList_analogue->AddColumn("Minimum",100);
	lpList_analogue->AddColumn("Current",100);
	lpList_analogue->AddColumn("Maximum",100);
	lpList_analogue->AddColumn("Unit",100);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpSplitter->GetPane(1,0);
	lpList_digital->RegisterCallback(callbackRealtime);
	lpList_digital->SetFont(lpFont);
	lpList_digital->SetViewStyle(LVS_REPORT,LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	lpList_digital->AddColumn("Digital Data Item Name",280);
	lpList_digital->AddColumn("Minimum",100);
	lpList_digital->AddColumn("Current",100);
	lpList_digital->AddColumn("Maximum",100);
	lpList_digital->AddColumn("Unit",100);

CATCHCATCH("frameRealtime::OnCreateClient()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return FALSE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return TRUE;
}

void frameRealtime::OnParseSupportList(WPARAM wParam,LPARAM lParam)
{
	CString sError;
	unsigned short usBufferPointer = 0;
	POSITION posBit = NULL;
	LPSTRUCT_LIVEBYTEITEM lpLiveByteItem = NULL;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;
	unsigned short usCapture = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpSplitter->GetPane(1,0);

EXCEPTION_BOOKMARK(__LINE__)
	while(usBufferPointer < ((CString*)lParam)->GetLength())
	{
		if(((CMapWordToPtr*)wParam)->Lookup(usBufferPointer+9,(void*&)lpLiveByteItem) != 0)
		{
			posBit = lpLiveByteItem->listBitItems.GetHeadPosition();
			while(posBit != NULL)
			{
				lpLiveBitItem = (LPSTRUCT_LIVEBITITEM)lpLiveByteItem->listBitItems.GetNext(posBit);
				if(((CString*)lParam)->GetAt(usBufferPointer) & (char)lpLiveBitItem->cBit)
				{
					if(lpLiveBitItem->cType == TYPE_SWITCH)
					{
						lpList_digital->lpList->InsertItem(lpList_digital->lpList->GetItemCount(),lpLiveBitItem->szName);
						lpList_digital->lpList->SetItemText(lpList_digital->lpList->GetItemCount()-1,4,lpLiveBitItem->szUnit);
						lpList_digital->lpList->SetItemData(lpList_digital->lpList->GetItemCount()-1,lpLiveBitItem->cType);

						if(lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,99) == 99)
							lpParent->WriteProfileInt("Capture",lpLiveBitItem->szName,0);
						usCapture = lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,usCapture);
						lpList_digital->lpList->SetCheck(lpList_digital->lpList->GetItemCount()-1,(BOOL)usCapture);
						
						lpLiveBitItem->iListIndex = lpList_digital->lpList->GetItemCount()-1;

						if((lpLiveBitItem->bDisplay = (BOOL)usCapture) == TRUE)
						{
							lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem);
							lpParent->lpProtocolSSM->AddCSVItem(lpLiveBitItem);
						}
					}
					else
					{
						lpList_analogue->lpList->InsertItem(lpList_analogue->lpList->GetItemCount(),lpLiveBitItem->szName);
						lpList_analogue->lpList->SetItemText(lpList_analogue->lpList->GetItemCount()-1,4,lpLiveBitItem->szUnit);
						lpList_analogue->lpList->SetItemData(lpList_analogue->lpList->GetItemCount()-1,lpLiveBitItem->cType);

						if(lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,99) == 99)
							lpParent->WriteProfileInt("Capture",lpLiveBitItem->szName,0);
						usCapture = lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,usCapture);
						lpList_analogue->lpList->SetCheck(lpList_analogue->lpList->GetItemCount()-1,(BOOL)usCapture);

						lpLiveBitItem->iListIndex = lpList_analogue->lpList->GetItemCount()-1;

						if((lpLiveBitItem->bDisplay = (BOOL)usCapture) == TRUE)
						{
							lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem);
							lpParent->lpProtocolSSM->AddCSVItem(lpLiveBitItem);
						}
					}
				}
			}
		}
		usBufferPointer++;
	}

EXCEPTION_BOOKMARK(__LINE__)
	lpLiveByteItem = NULL;
	lpLiveBitItem = NULL;
	if(((CMapWordToPtr*)wParam)->Lookup(0,(void*&)lpLiveByteItem) != 0)
	{
		posBit = lpLiveByteItem->listBitItems.GetHeadPosition();
		while(posBit != NULL)
		{
			lpLiveBitItem = (LPSTRUCT_LIVEBITITEM)lpLiveByteItem->listBitItems.GetNext(posBit);

			usCapture = 0;
			if((usCapture = lpParent->GetProfileInt("Settings",lpLiveBitItem->szName,usCapture)) == TRUE)
			{
				if(lpLiveBitItem->cType == TYPE_SWITCH)
				{
					lpList_digital->lpList->InsertItem(lpList_digital->lpList->GetItemCount(),lpLiveBitItem->szName);
					lpList_digital->lpList->SetItemText(lpList_digital->lpList->GetItemCount()-1,4,lpLiveBitItem->szUnit);
					lpList_digital->lpList->SetItemData(lpList_digital->lpList->GetItemCount()-1,lpLiveBitItem->cType);

					if(lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,99) == 99)
						lpParent->WriteProfileInt("Capture",lpLiveBitItem->szName,0);
					usCapture = lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,usCapture);
					lpList_digital->lpList->SetCheck(lpList_digital->lpList->GetItemCount()-1,(BOOL)usCapture);

					lpLiveBitItem->iListIndex = lpList_digital->lpList->GetItemCount()-1;

					if((lpLiveBitItem->bDisplay = (BOOL)usCapture) == TRUE)
					{
						lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem);
						lpParent->lpProtocolSSM->AddCSVItem(lpLiveBitItem);
					}
				}
				else
				{
					lpList_analogue->lpList->InsertItem(lpList_analogue->lpList->GetItemCount(),lpLiveBitItem->szName);
					lpList_analogue->lpList->SetItemText(lpList_analogue->lpList->GetItemCount()-1,4,lpLiveBitItem->szUnit);
					lpList_analogue->lpList->SetItemData(lpList_analogue->lpList->GetItemCount()-1,lpLiveBitItem->cType);

					if(lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,99) == 99)
						lpParent->WriteProfileInt("Capture",lpLiveBitItem->szName,0);
					usCapture = lpParent->GetProfileInt("Capture",lpLiveBitItem->szName,usCapture);
					lpList_analogue->lpList->SetCheck(lpList_analogue->lpList->GetItemCount()-1,(BOOL)usCapture);

					lpLiveBitItem->iListIndex = lpList_analogue->lpList->GetItemCount()-1;

					if((lpLiveBitItem->bDisplay = (BOOL)usCapture) == TRUE)
					{
						if(lpLiveBitItem->cType == TYPE_CORRECTED_BOOST)
						{
							LPSTRUCT_LIVEBITITEM lpLiveBitItem_absolute = NULL;
							LPSTRUCT_LIVEBITITEM lpLiveBitItem_atmospheric = NULL;
							if((lpLiveBitItem_absolute = lpParent->GetLiveDataItem("Manifold Absolute Pressure")) != NULL)
								lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem_absolute);
							if((lpLiveBitItem_atmospheric = lpParent->GetLiveDataItem("Atmospheric Pressure")) != NULL)
								lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem_atmospheric);
						}
						lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem);
						lpParent->lpProtocolSSM->AddCSVItem(lpLiveBitItem);
					}
				}
			}
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	lpList_analogue->lpParent = (void*)this;
	lpList_analogue->lpApp = lpParent;
	lpList_digital->lpParent = (void*)this;
	lpList_digital->lpApp = lpParent;

EXCEPTION_BOOKMARK(__LINE__)
	bFlag = TRUE;
	if(lpParent->ulConfig & CONFIG_TRIGGERONDEFOGSWITCH)
		OnTriggerLoggingOnDefogSwitch();
	if(lpParent->ulConfig & CONFIG_CONVERT_INJECTOR)
		OnConvert_injector();
	if(lpParent->ulConfig & CONFIG_CONVERT_PRESSURE)
		OnConvert_pressure();
	if(lpParent->ulConfig & CONFIG_CONVERT_SPEED)
		OnConvert_speed();
	if(lpParent->ulConfig & CONFIG_CONVERT_TEMPERATURE)
		OnConvert_temperature();
	if(lpParent->ulConfig & CONFIG_CONVERT_AIRFUEL)
		OnCovert_af();
	bFlag = FALSE;

CATCHCATCH("frameRealtime::OnParseSupportList()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnContextMenu(CWnd* pWnd,CPoint ptMousePos)
{
	CString sError;
	CWnd* pCWnd = NULL;
	int iIndex = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	ScreenToClient(&ptMousePos);

EXCEPTION_BOOKMARK(__LINE__)
	CMenu menu;
	CMenu* pPopup = NULL;

	menu.LoadMenu(IDR_POPUP_REALTIME);
	pPopup = menu.GetSubMenu(0);

	pPopup->EnableMenuItem(ID_POPUP_CHOOSELOGGINGDIRECTORY,MF_BYCOMMAND | MF_ENABLED);
	pPopup->EnableMenuItem(ID_POPUP_TRIGGERLOGGINGONDEFOGSWITCH,MF_BYCOMMAND | MF_ENABLED);
	pPopup->EnableMenuItem(IE_POPUP_AUTOMATICALLYADDNEWLOGFILESTOSAVEDLIST,MF_BYCOMMAND | MF_ENABLED);
	pPopup->EnableMenuItem(ID_POPUP_STARTFILECAPTURE,MF_BYCOMMAND | MF_GRAYED);
	pPopup->EnableMenuItem(7,MF_BYPOSITION | MF_GRAYED);
	pPopup->EnableMenuItem(8,MF_BYPOSITION | MF_GRAYED);
	pPopup->EnableMenuItem(ID_POPUP_VIEWITEMDESCRIPTION,MF_BYCOMMAND | MF_GRAYED);

	if(lpParent->ulConfig & CONFIG_TRIGGERONDEFOGSWITCH)
		pPopup->CheckMenuItem(ID_POPUP_TRIGGERLOGGINGONDEFOGSWITCH,MF_CHECKED);
	if(lpParent->ulConfig & CONFIG_AUTOADDNEWLOGFILE)
		pPopup->CheckMenuItem(IE_POPUP_AUTOMATICALLYADDNEWLOGFILESTOSAVEDLIST,MF_CHECKED);
	if(lpParent->ulConfig & CONFIG_USEABSOLUTETIME)
		pPopup->CheckMenuItem(IE_POPUP_USEABSOLUTETIME,MF_CHECKED);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpSplitter->GetPane(1,0);

	if(lpList_analogue->lpList->GetItemCount() > 0 || lpList_digital->lpList->GetItemCount() > 0)
	{
		pPopup->EnableMenuItem(ID_POPUP_STARTFILECAPTURE,MF_BYCOMMAND | MF_ENABLED);

		pPopup->EnableMenuItem(7,MF_BYPOSITION | MF_ENABLED);
		if(lpParent->ulConfig & CONFIG_CONVERT_INJECTOR)
			pPopup->CheckMenuItem(ID_POPUP_CONVERT_INJECTOR,MF_CHECKED);
		if(lpParent->ulConfig & CONFIG_CONVERT_TEMPERATURE)
			pPopup->CheckMenuItem(ID_POPUP_CONVERT_TEMPERATURE,MF_CHECKED);
		if(lpParent->ulConfig & CONFIG_CONVERT_SPEED)
			pPopup->CheckMenuItem(ID_POPUP_CONVERT_SPEED,MF_CHECKED);
		if(lpParent->ulConfig & CONFIG_CONVERT_PRESSURE)
			pPopup->CheckMenuItem(ID_POPUP_CONVERT_PRESSURE,MF_CHECKED);
		if(lpParent->ulConfig & CONFIG_CONVERT_AIRFUEL)
			pPopup->CheckMenuItem(ID_POPUP_CONVERT_AF,MF_CHECKED);

		pPopup->EnableMenuItem(8,MF_BYPOSITION | MF_ENABLED);
		if(lpParent->bThreadLiveQuery == FALSE)
			pPopup->CheckMenuItem(ID_POPUP_PAUSEDATACAPTURE,MF_CHECKED);
	}

	pWnd = lpSplitter->ChildWindowFromPoint(ptMousePos);
	if(pWnd == lpList_analogue)
	{
		if(lpList_analogue->lpList->GetSelectedCount() > 0)
			pPopup->EnableMenuItem(ID_POPUP_VIEWITEMDESCRIPTION,MF_BYCOMMAND | MF_ENABLED);

		while((iIndex = lpList_digital->lpList->GetNextItem(iIndex,LVNI_SELECTED)) != -1)
			lpList_digital->lpList->SetItemState(iIndex,~LVIS_SELECTED,LVIS_SELECTED);
	}
	else if(pWnd == lpList_digital)
	{
		if(lpList_digital->lpList->GetSelectedCount() > 0)
			pPopup->EnableMenuItem(ID_POPUP_VIEWITEMDESCRIPTION,MF_BYCOMMAND | MF_ENABLED);

		while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_SELECTED)) != -1)
			lpList_analogue->lpList->SetItemState(iIndex,~LVIS_SELECTED,LVIS_SELECTED);
	}

EXCEPTION_BOOKMARK(__LINE__)
	ClientToScreen(&ptMousePos);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN,ptMousePos.x,ptMousePos.y,this);

CATCHCATCH("frameRealtime::OnContextMenu()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnStartFileCapture()
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;
	FILE* fpTemp = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpParent->hThreadLiveQuery == NULL || lpParent->lpProtocolSSM->listLiveQuery.GetCount() == 0)
		return;

	printf("\a");

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpSplitter->GetPane(1,0);

EXCEPTION_BOOKMARK(__LINE__)
	if(fpCSVCapture == NULL)
	{
		sAbsoluteCSVFile = lpParent->GetProfileString("Capture","LogPath");
		if(sAbsoluteCSVFile.Right(1) != "\\")
			sAbsoluteCSVFile += "\\";

		CString sTemp = lpParent->GetECUType(lpParent->lpProtocolSSM->sECU);
		sTemp.Delete(0,sTemp.Find("[",0));

		sAbsoluteCSVFile += "ecuExplorer " + sTemp + " ";

		CTime ctNow = CTime::GetCurrentTime();
		sAbsoluteCSVFile += ctNow.Format("%d-%m-%Y %Hh%Mm%Ss.csv");

		if((fpTemp = fopen(sAbsoluteCSVFile,"a+")) == NULL)
		{
			OutputString("cannot open live capture file",MESSAGETYPE_ERROR);
			return;
		}

		sError.Format("Logging to %s",sAbsoluteCSVFile);
		::SetWindowText(lpParent->m_pMainWnd->m_hWnd,sError);

		fprintf(fpTemp,"Time,");
		pos = lpParent->lpProtocolSSM->listCSVCapture.GetHeadPosition();
		while(pos != NULL)
		{
			lpLiveBitItem = (LPSTRUCT_LIVEBITITEM)lpParent->lpProtocolSSM->listCSVCapture.GetNext(pos);
			if(lpLiveBitItem->cType == TYPE_SWITCH)
			{
				fprintf(fpTemp,lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,0));
				fprintf(fpTemp," (");
				fprintf(fpTemp,lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,4));
				fprintf(fpTemp,")");
			}
			else
			{
				fprintf(fpTemp,lpList_analogue->lpList->GetItemText(lpLiveBitItem->iListIndex,0));
				fprintf(fpTemp," (");
				if(lpList_analogue->lpList->GetItemText(lpLiveBitItem->iListIndex,4) == "%")
					fprintf(fpTemp,"%%");
				else
					fprintf(fpTemp,lpList_analogue->lpList->GetItemText(lpLiveBitItem->iListIndex,4));
				fprintf(fpTemp,")");
			}
			if(pos != NULL)
				fprintf(fpTemp,",");
		}
		fprintf(fpTemp,"\n");
		fflush(fpTemp);

		_ftime(&tbCSVCapture);
		fpCSVCapture = fpTemp;
	}
	else
	{
		::SetWindowText(lpParent->m_pMainWnd->m_hWnd,"TARI Racing Software - ecuExplorer");
		fclose(fpCSVCapture);
		fpCSVCapture = NULL;

		if(lpParent->ulConfig & CONFIG_AUTOADDNEWLOGFILE)
		{
			frameResourceTree* lpFrameResourceTree = (frameResourceTree*)lpParent->lpMainFrame->splitterWndLeft.GetPane(0,0);
			lpFrameResourceTree->AddSavedLogFile(sAbsoluteCSVFile);
			lpFrameResourceTree->lpSplitter->ShowView(lpFrameResourceTree->GetNodeData_view(NODE_ECUREALTIME));
		}
	}

CATCHCATCH("frameRealtime::OnStartFileCapture()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnChooseLoggingDirectory()
{
	CString sError,sPath;
	BROWSEINFO bi;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	sPath = lpParent->GetProfileString("Capture","LogPath");
	sError.Format("Current Logging Path:\n%s",sPath);

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)&bi,0,sizeof(bi));	

EXCEPTION_BOOKMARK(__LINE__)
	TCHAR szDisplayName[MAX_PATH];
	szDisplayName[0] = '\0';

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = sError;
	bi.ulFlags = BIF_EDITBOX | BIF_VALIDATE;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

EXCEPTION_BOOKMARK(__LINE__)
	szDisplayName[0] = '\0';
	if(SHGetPathFromIDList(pidl,szDisplayName))
		lpParent->WriteProfileString("Capture","LogPath",szDisplayName);

CATCHCATCH("frameRealtime::OnChooseLoggingDirectory()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnTriggerLoggingOnDefogSwitch()
{
	CString sError;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(bFlag == FALSE)
	{
		if(lpParent->ulConfig & CONFIG_TRIGGERONDEFOGSWITCH)
			lpParent->ulConfig &= ~CONFIG_TRIGGERONDEFOGSWITCH;
		else
			lpParent->ulConfig |= CONFIG_TRIGGERONDEFOGSWITCH;

		lpParent->WriteProfileInt("Settings","Config",(int)lpParent->ulConfig);
	}

EXCEPTION_BOOKMARK(__LINE__)
	if((lpLiveBitItem = lpParent->GetLiveDataItem("Rear Defogger Switch Signal")) != NULL)
	{
		if(lpParent->ulConfig & CONFIG_TRIGGERONDEFOGSWITCH)
			lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem);
		else
			lpParent->lpProtocolSSM->RemoveLiveQueryItem(lpLiveBitItem);
	}

CATCHCATCH("frameRealtime::OnTriggerLoggingOnDefogSwitch()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnConvert_injector()
{
	CString sError;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem_injector = NULL;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem_engine = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(bFlag == FALSE)
	{
		if(lpParent->ulConfig & CONFIG_CONVERT_INJECTOR)
			lpParent->ulConfig &= ~CONFIG_CONVERT_INJECTOR;
		else
			lpParent->ulConfig |= CONFIG_CONVERT_INJECTOR;

		lpParent->WriteProfileInt("Settings","Config",(int)lpParent->ulConfig);
	}

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		if(lpList_analogue->lpList->GetItemData(iIndex) == TYPE_INJECTOR)
		{
			sError = lpList_analogue->lpList->GetItemText(iIndex,0);
			if(lpParent->ulConfig & CONFIG_CONVERT_INJECTOR)
			{
				lpList_analogue->lpList->SetItemText(iIndex,4,"%");
				sError.Replace("Pulse Width","Duty");
			}
			else
			{
				lpList_analogue->lpList->SetItemText(iIndex,4,"ms");
				sError.Replace("Duty","Pulse Width");
			}
			lpList_analogue->lpList->SetItemText(iIndex,0,sError);

			lpList_analogue->lpList->SetItemText(iIndex,1,"");
			lpList_analogue->lpList->SetItemText(iIndex,3,"");
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(lpParent->ulConfig & CONFIG_CONVERT_INJECTOR)
	{
		int iIndex = -1;
		while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
		{
			if((lpLiveBitItem_injector = lpParent->GetLiveDataItem(iIndex,lpList_analogue->lpList->GetItemData(iIndex))) != NULL)
				if(lpLiveBitItem_injector->cType == TYPE_INJECTOR && lpLiveBitItem_injector->bDisplay == TRUE)
					if((lpLiveBitItem_engine = lpParent->GetLiveDataItem("Engine Speed")) != NULL)
						lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem_engine,TRUE);
		}
	}
	else
	{
		int iIndex = -1;
		while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
		{
			if((lpLiveBitItem_injector = lpParent->GetLiveDataItem(iIndex,lpList_analogue->lpList->GetItemData(iIndex))) != NULL)
				if(lpLiveBitItem_injector->cType == TYPE_INJECTOR && lpLiveBitItem_injector->bDisplay == TRUE)
					if((lpLiveBitItem_engine = lpParent->GetLiveDataItem("Engine Speed")) != NULL)
						lpParent->lpProtocolSSM->RemoveLiveQueryItem(lpLiveBitItem_engine);
		}
	}

CATCHCATCH("frameRealtime::OnConvert_injector()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnConvert_temperature()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(bFlag == FALSE)
	{
		if(lpParent->ulConfig & CONFIG_CONVERT_TEMPERATURE)
			lpParent->ulConfig &= ~CONFIG_CONVERT_TEMPERATURE;
		else
			lpParent->ulConfig |= CONFIG_CONVERT_TEMPERATURE;

		lpParent->WriteProfileInt("Settings","Config",(int)lpParent->ulConfig);
	}

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		if(lpList_analogue->lpList->GetItemData(iIndex) == TYPE_TEMPERATURE)
		{
			if(lpParent->ulConfig & CONFIG_CONVERT_TEMPERATURE)
				lpList_analogue->lpList->SetItemText(iIndex,4,"°F");
			else
				lpList_analogue->lpList->SetItemText(iIndex,4,"°C");

			lpList_analogue->lpList->SetItemText(iIndex,1,"");
			lpList_analogue->lpList->SetItemText(iIndex,3,"");
		}
	}

CATCHCATCH("frameRealtime::OnConvert_temperature()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnConvert_speed()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(bFlag == FALSE)
	{
		if(lpParent->ulConfig & CONFIG_CONVERT_SPEED)
			lpParent->ulConfig &= ~CONFIG_CONVERT_SPEED;
		else
			lpParent->ulConfig |= CONFIG_CONVERT_SPEED;

		lpParent->WriteProfileInt("Settings","Config",(int)lpParent->ulConfig);
	}

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		if(lpList_analogue->lpList->GetItemData(iIndex) == TYPE_SPEED)
		{
			if(lpParent->ulConfig & CONFIG_CONVERT_SPEED)
				lpList_analogue->lpList->SetItemText(iIndex,4,"MPH");
			else
				lpList_analogue->lpList->SetItemText(iIndex,4,"KPH");

			lpList_analogue->lpList->SetItemText(iIndex,1,"");
			lpList_analogue->lpList->SetItemText(iIndex,3,"");
		}
	}

CATCHCATCH("frameRealtime::OnConvert_speed()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnConvert_pressure()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(bFlag == FALSE)
	{
		if(lpParent->ulConfig & CONFIG_CONVERT_PRESSURE)
			lpParent->ulConfig &= ~CONFIG_CONVERT_PRESSURE;
		else
			lpParent->ulConfig |= CONFIG_CONVERT_PRESSURE;

		lpParent->WriteProfileInt("Settings","Config",(int)lpParent->ulConfig);
	}

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		if(lpList_analogue->lpList->GetItemData(iIndex) == TYPE_PRESSURE)
		{
			if(lpParent->ulConfig & CONFIG_CONVERT_PRESSURE)
				lpList_analogue->lpList->SetItemText(iIndex,4,"Bar");
			else
				lpList_analogue->lpList->SetItemText(iIndex,4,"PSI");

			lpList_analogue->lpList->SetItemText(iIndex,1,"");
			lpList_analogue->lpList->SetItemText(iIndex,3,"");
		}
	}

CATCHCATCH("frameRealtime::OnConvert_pressure()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnCovert_af()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(bFlag == FALSE)
	{
		if(lpParent->ulConfig & CONFIG_CONVERT_AIRFUEL)
			lpParent->ulConfig &= ~CONFIG_CONVERT_AIRFUEL;
		else
			lpParent->ulConfig |= CONFIG_CONVERT_AIRFUEL;

		lpParent->WriteProfileInt("Settings","Config",(int)lpParent->ulConfig);
	}

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		if(lpList_analogue->lpList->GetItemData(iIndex) == TYPE_AIRFUEL)
		{
			if(lpParent->ulConfig & CONFIG_CONVERT_AIRFUEL)
				lpList_analogue->lpList->SetItemText(iIndex,4,"AFR");
			else
				lpList_analogue->lpList->SetItemText(iIndex,4,"Lambda");

			lpList_analogue->lpList->SetItemText(iIndex,1,"");
			lpList_analogue->lpList->SetItemText(iIndex,3,"");
		}
	}

CATCHCATCH("frameRealtime::OnCovert_af()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnPauseDataCapture()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpParent->bThreadLiveQuery == TRUE)
		lpParent->StopLiveQueryThread();
	else
		lpParent->StartLiveQueryThread();

CATCHCATCH("frameRealtime::OnPauseDataCapture()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnResetMinMaxValues()
{
	CString sError;
	int iIndex;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	iIndex = -1;
	while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		lpList_analogue->lpList->SetItemText(iIndex,1,"");
		lpList_analogue->lpList->SetItemText(iIndex,2,"");
		lpList_analogue->lpList->SetItemText(iIndex,3,"");
	}

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpSplitter->GetPane(1,0);

EXCEPTION_BOOKMARK(__LINE__)
	iIndex = -1;
	while((iIndex = lpList_digital->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		lpList_digital->lpList->SetItemText(iIndex,1,"");
		lpList_digital->lpList->SetItemText(iIndex,2,"");
		lpList_digital->lpList->SetItemText(iIndex,3,"");
	}

CATCHCATCH("frameRealtime::OnResetMinMaxValues()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnSelectAllAnalogueParameters()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		if(lpList_analogue->lpList->GetCheck(iIndex) == FALSE)
			lpList_analogue->lpList->SetCheck(iIndex);
	}

CATCHCATCH("frameRealtime::OnSelectAllAnalogueParameters()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnDeselectAllAnalogueParameters()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		if(lpList_analogue->lpList->GetCheck(iIndex) == TRUE)
			lpList_analogue->lpList->SetCheck(iIndex,FALSE);
	}

CATCHCATCH("frameRealtime::OnDeselectAllAnalogueParameters()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnSelectAllDigitalParameters()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpSplitter->GetPane(1,0);

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	while((iIndex = lpList_digital->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		if(lpList_digital->lpList->GetCheck(iIndex) == FALSE)
			lpList_digital->lpList->SetCheck(iIndex);
	}

CATCHCATCH("frameRealtime::OnSelectAllDigitalParameters()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnDeselectAllDigitalParameters()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpSplitter->GetPane(1,0);

EXCEPTION_BOOKMARK(__LINE__)
	int iIndex = -1;
	while((iIndex = lpList_digital->lpList->GetNextItem(iIndex,LVNI_ALL)) != -1)
	{
		if(lpList_digital->lpList->GetCheck(iIndex) == TRUE)
			lpList_digital->lpList->SetCheck(iIndex,FALSE);
	}

CATCHCATCH("frameRealtime::OnDeselectAllDigitalParameters()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnViewItemDescription()
{
	CString sError;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

	if(lpList_analogue->lpList->GetSelectedCount() > 0)
	{
		int iIndex = -1;
		while((iIndex = lpList_analogue->lpList->GetNextItem(iIndex,LVNI_SELECTED)) != -1)
			if((lpLiveBitItem = lpParent->GetLiveDataItem(iIndex,lpList_analogue->lpList->GetItemData(iIndex))) != NULL)
			{
				sError.Format("%s (%s)\n\n",lpLiveBitItem->szName,lpLiveBitItem->szUnit);
				if(lpLiveBitItem->usLength_description > 0)
					sError += lpLiveBitItem->szDescription;
				else
					sError += "(no description)";
				MessageBox(sError,"Item Description",MB_ICONINFORMATION | MB_OK);
				return;
			}
	}

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpSplitter->GetPane(1,0);

	if(lpList_digital->lpList->GetSelectedCount() > 0)
	{
		int iIndex = -1;
		while((iIndex = lpList_digital->lpList->GetNextItem(iIndex,LVNI_SELECTED)) != -1)
			if((lpLiveBitItem = lpParent->GetLiveDataItem(iIndex,lpList_digital->lpList->GetItemData(iIndex))) != NULL)
			{
				sError.Format("%s (%s)\n\n",lpLiveBitItem->szName,lpLiveBitItem->szUnit);
				if(lpLiveBitItem->usLength_description > 0)
					sError += lpLiveBitItem->szDescription;
				else
					sError += "(no description)";
				MessageBox(sError,"Item Description",MB_ICONINFORMATION | MB_OK);
				return;
			}
	}

CATCHCATCH("frameRealtime::OnViewItemDescription()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnUpdateListItem(WPARAM wParam,LPARAM lParam)
{
	CString sError;
	LPSTRUCT_LIVEQUERYITEM lpLiveQueryItem = (LPSTRUCT_LIVEQUERYITEM)lParam;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = (LPSTRUCT_LIVEBITITEM)lpLiveQueryItem->lpLiveBitItem;
	char cOldValue = 0;
	char cNewValue = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpParent->hThreadLiveQuery == NULL)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_analogue = NULL;
	lpList_analogue = (frameList*)lpSplitter->GetPane(0,0);

EXCEPTION_BOOKMARK(__LINE__)
	frameList* lpList_digital = NULL;
	lpList_digital = (frameList*)lpSplitter->GetPane(1,0);

EXCEPTION_BOOKMARK(__LINE__)
	if(lpLiveBitItem->cType == TYPE_SWITCH && strstr(lpLiveBitItem->szName,"Rear Defogger Switch Signal") != NULL)
		if((char)lpLiveBitItem->fValue & lpLiveBitItem->cBit)
			cOldValue = 1;

EXCEPTION_BOOKMARK(__LINE__)
	lpLiveBitItem->fValue = lpLiveQueryItem->fValue_raw;
	lpLiveBitItem->fValue += lpLiveBitItem->usOperand_addition;
	lpLiveBitItem->fValue -= lpLiveBitItem->usOperand_subtract;
	if(lpLiveBitItem->usOperand_multiplier > 0)
		lpLiveBitItem->fValue *= lpLiveBitItem->usOperand_multiplier;
	if(lpLiveBitItem->usOperand_divisor > 0)
		lpLiveBitItem->fValue /= lpLiveBitItem->usOperand_divisor;

EXCEPTION_BOOKMARK(__LINE__)
	switch(lpLiveBitItem->cType)
	{
	case TYPE_TEMPERATURE:
		if(lpParent->ulConfig & CONFIG_CONVERT_TEMPERATURE)
			lpLiveBitItem->fValue = ((lpLiveBitItem->fValue * 9) / 5) + 32;
		break;
	case TYPE_PRESSURE:
		if(lpParent->ulConfig & CONFIG_CONVERT_PRESSURE)
			lpLiveBitItem->fValue *= (float)0.06894757;
		break;
	case TYPE_AIRFUEL:
		if(lpParent->ulConfig & CONFIG_CONVERT_AIRFUEL)
			lpLiveBitItem->fValue *= (float)14.7;
		break;
	case TYPE_SPEED:
		if(lpParent->ulConfig & CONFIG_CONVERT_SPEED)
			lpLiveBitItem->fValue *= (float)0.6213712;
		break;
	case TYPE_INJECTOR:
		if(lpParent->ulConfig & CONFIG_CONVERT_INJECTOR)
		{
			LPSTRUCT_LIVEBITITEM lpLiveBitItem_engine = NULL;
			if((lpLiveBitItem_engine = lpParent->GetLiveDataItem("Engine Speed")) != NULL)
				lpLiveBitItem->fValue = lpLiveBitItem_engine->fValue * (lpLiveBitItem->fValue / 1200);
		}
		break;
	case TYPE_CORRECTED_BOOST:
		LPSTRUCT_LIVEBITITEM lpLiveBitItem_absolute = NULL;
		LPSTRUCT_LIVEBITITEM lpLiveBitItem_atmospheric = NULL;
		if((lpLiveBitItem_absolute = lpParent->GetLiveDataItem("Manifold Absolute Pressure")) != NULL && (lpLiveBitItem_atmospheric = lpParent->GetLiveDataItem("Atmospheric Pressure")) != NULL)
				lpLiveBitItem->fValue = lpLiveBitItem_absolute->fValue - lpLiveBitItem_atmospheric->fValue;
		break;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(lpLiveBitItem->bDisplay == TRUE)
	{
		switch(lpLiveBitItem->usDecimals)
		{
		case 1:
			sError.Format("%.1f",lpLiveBitItem->fValue);
			break;
		case 2:
			sError.Format("%.2f",lpLiveBitItem->fValue);
			break;
		case 3:
			sError.Format("%.3f",lpLiveBitItem->fValue);
			break;
		default:
			sError.Format("%.0f",lpLiveBitItem->fValue);
			break;
		}

EXCEPTION_BOOKMARK(__LINE__)
		if(lpLiveBitItem->cType == TYPE_SWITCH)
		{
			if(strstr(lpLiveBitItem->szName,"Rear Defogger Switch Signal") != NULL)
				if(lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,2) != "")
					cOldValue = (char)atoi(lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,2));

			if((char)lpLiveBitItem->fValue & lpLiveBitItem->cBit)
				lpList_digital->lpList->SetItemText(lpLiveBitItem->iListIndex,2,"1");
			else
				lpList_digital->lpList->SetItemText(lpLiveBitItem->iListIndex,2,"0");
			if(strcmp(lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,2),lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,1)) < 0 || lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,1) == "")
				lpList_digital->lpList->SetItemText(lpLiveBitItem->iListIndex,1,lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,2));
			if(strcmp(lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,2),lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,3)) > 0 || lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,3) == "")
				lpList_digital->lpList->SetItemText(lpLiveBitItem->iListIndex,3,lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,2));

			if(strstr(lpLiveBitItem->szName,"Rear Defogger Switch Signal") != NULL)
			{
				cNewValue = (char)atoi(lpList_digital->lpList->GetItemText(lpLiveBitItem->iListIndex,2));
				if(cOldValue != cNewValue)
					OnStartFileCapture();
			}
		}
		else
		{
			lpList_analogue->lpList->SetItemText(lpLiveBitItem->iListIndex,2,sError);
			float fCompare = atof(lpList_analogue->lpList->GetItemText(lpLiveBitItem->iListIndex,1));
			if(lpLiveBitItem->fValue < fCompare || lpList_analogue->lpList->GetItemText(lpLiveBitItem->iListIndex,1) == "")
				lpList_analogue->lpList->SetItemText(lpLiveBitItem->iListIndex,1,sError);
			fCompare = atof(lpList_analogue->lpList->GetItemText(lpLiveBitItem->iListIndex,3));
			if(lpLiveBitItem->fValue > fCompare || lpList_analogue->lpList->GetItemText(lpLiveBitItem->iListIndex,3) == "")
				lpList_analogue->lpList->SetItemText(lpLiveBitItem->iListIndex,3,sError);

			TRACE("item [%s] new[%s] min[%s] current[%s] max[%s]\n",lpLiveBitItem->szName,sError,lpList_analogue->lpList->GetItemText(lpLiveBitItem->iListIndex,1),lpList_analogue->lpList->GetItemText(lpLiveBitItem->iListIndex,2),lpList_analogue->lpList->GetItemText(lpLiveBitItem->iListIndex,3));
		}
	}
	else if(lpLiveBitItem->cType == TYPE_SWITCH && strstr(lpLiveBitItem->szName,"Rear Defogger Switch Signal") != NULL)
	{
		if((char)lpLiveBitItem->fValue & lpLiveBitItem->cBit)
			cNewValue = 1;

		if(cOldValue != cNewValue)
			OnStartFileCapture();
	}

CATCHCATCH("frameRealtime::OnUpdateListItem()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{
		DWORD abc = 9;
	}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnUpdateCSVFile(WPARAM wParam,LPARAM lParam)
{
	CString sError;
	POSITION pos = NULL;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(fpCSVCapture == NULL)
		return;

EXCEPTION_BOOKMARK(__LINE__)
	_timeb tbNow;
	_ftime(&tbNow);
	if(lpParent->ulConfig & CONFIG_USEABSOLUTETIME)
	{
		_timeb tbNow;
		_ftime(&tbNow);
		CTime ctNow(tbNow.time);
		sError.Format("%s.%03i,",ctNow.Format("%H:%M:%S"),tbNow.millitm);
	}
	else
	{
		double dblInterval = ((double)((double)tbNow.time * 1000 + (double)tbNow.millitm) - (double)((double)tbCSVCapture.time * 1000 + (double)tbCSVCapture.millitm));
		sError.Format("%i,",(long)dblInterval);
	}
	fprintf(fpCSVCapture,sError);

EXCEPTION_BOOKMARK(__LINE__)
	pos = lpParent->lpProtocolSSM->listCSVCapture.GetHeadPosition();
	while(pos != NULL)
	{
		lpLiveBitItem = (LPSTRUCT_LIVEBITITEM)lpParent->lpProtocolSSM->listCSVCapture.GetNext(pos);

		switch(lpLiveBitItem->usDecimals)
		{
		case 1:
			sError.Format("%.1f",lpLiveBitItem->fValue);
			break;
		case 2:
			sError.Format("%.2f",lpLiveBitItem->fValue);
			break;
		case 3:
			sError.Format("%.3f",lpLiveBitItem->fValue);
			break;
		default:
			sError.Format("%.0f",lpLiveBitItem->fValue);
			break;
		}

		if(lpLiveBitItem->cType == TYPE_SWITCH)
		{
			if((char)lpLiveBitItem->fValue & lpLiveBitItem->cBit)
				fprintf(fpCSVCapture,"1");
			else
				fprintf(fpCSVCapture,"0");
		}
		else
			fprintf(fpCSVCapture,sError);

		if(pos != NULL)
			fprintf(fpCSVCapture,",");
	}
	fprintf(fpCSVCapture,"\n");
	fflush(fpCSVCapture);

CATCHCATCH("frameRealtime::OnUpdateCSVFile()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnAutoAddNewLogFile()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(bFlag == FALSE)
	{
		if(lpParent->ulConfig & CONFIG_AUTOADDNEWLOGFILE)
			lpParent->ulConfig &= ~CONFIG_AUTOADDNEWLOGFILE;
		else
			lpParent->ulConfig |= CONFIG_AUTOADDNEWLOGFILE;

		lpParent->WriteProfileInt("Settings","Config",(int)lpParent->ulConfig);
	}

CATCHCATCH("frameRealtime::OnAutoAddNewLogFile()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::ResizeSplitter()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	RECT rect;
	::GetClientRect(lpParent->lpMainFrame->splitterWndLeft.m_hWnd,&rect);
	lpSplitter->SetRowInfo(0,(rect.bottom * lpParent->GetRegistryValue("Settings","splitterRealtime",50)) / 100,40);
	lpSplitter->RecalcLayout();

CATCHCATCH("frameRealtime::ResizeSplitter()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

void frameRealtime::OnUseAbsoluteTime()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(bFlag == FALSE)
	{
		if(lpParent->ulConfig & CONFIG_USEABSOLUTETIME)
			lpParent->ulConfig &= ~CONFIG_USEABSOLUTETIME;
		else
			lpParent->ulConfig |= CONFIG_USEABSOLUTETIME;

		lpParent->WriteProfileInt("Settings","Config",(int)lpParent->ulConfig);
	}

CATCHCATCH("frameRealtime::OnUseAbsoluteTime()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}

DWORD WINAPI callbackRealtime(void* lpClass,CListCtrl* lpList,int iIndex,BOOL bState)
{
	CString sError;
	LPSTRUCT_LIVEBITITEM lpLiveBitItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpClass == NULL) return ERR_SUCCESS;

EXCEPTION_BOOKMARK(__LINE__)
	if((lpLiveBitItem = ((frameRealtime*)lpClass)->lpParent->GetLiveDataItem(iIndex,lpList->GetItemData(iIndex))) != NULL)
	{
		((frameRealtime*)lpClass)->lpParent->WriteProfileInt("Capture",lpLiveBitItem->szName,bState);
		if((lpLiveBitItem->bDisplay = bState) == TRUE)
		{
			if(lpLiveBitItem->cType == TYPE_INJECTOR && ((frameRealtime*)lpClass)->lpParent->ulConfig & CONFIG_CONVERT_INJECTOR)
			{
				LPSTRUCT_LIVEBITITEM lpLiveBitItem_engine = NULL;
				if((lpLiveBitItem_engine = ((frameRealtime*)lpClass)->lpParent->GetLiveDataItem("Engine Speed")) != NULL)
					((frameRealtime*)lpClass)->lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem_engine);
			}
			else if(lpLiveBitItem->cType == TYPE_CORRECTED_BOOST)
			{
				LPSTRUCT_LIVEBITITEM lpLiveBitItem_absolute = NULL;
				LPSTRUCT_LIVEBITITEM lpLiveBitItem_atmospheric = NULL;
				if((lpLiveBitItem_absolute = ((frameRealtime*)lpClass)->lpParent->GetLiveDataItem("Manifold Absolute Pressure")) != NULL)
					((frameRealtime*)lpClass)->lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem_absolute);
				if((lpLiveBitItem_atmospheric = ((frameRealtime*)lpClass)->lpParent->GetLiveDataItem("Atmospheric Pressure")) != NULL)
					((frameRealtime*)lpClass)->lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem_atmospheric);
			}

			((frameRealtime*)lpClass)->lpParent->lpProtocolSSM->AddLiveQueryItem(lpLiveBitItem);
			((frameRealtime*)lpClass)->lpParent->lpProtocolSSM->AddCSVItem(lpLiveBitItem);
		}
		else
		{
			((frameRealtime*)lpClass)->lpParent->lpProtocolSSM->RemoveLiveQueryItem(lpLiveBitItem);
			((frameRealtime*)lpClass)->lpParent->lpProtocolSSM->RemoveCSVItem(lpLiveBitItem);

			lpList->SetItemText(iIndex,1,"");
			lpList->SetItemText(iIndex,2,"");
			lpList->SetItemText(iIndex,3,"");

			if(lpLiveBitItem->cType == TYPE_INJECTOR && ((frameRealtime*)lpClass)->lpParent->ulConfig & CONFIG_CONVERT_INJECTOR)
			{
				LPSTRUCT_LIVEBITITEM lpLiveBitItem_engine = NULL;
				if((lpLiveBitItem_engine = ((frameRealtime*)lpClass)->lpParent->GetLiveDataItem("Engine Speed")) != NULL)
					((frameRealtime*)lpClass)->lpParent->lpProtocolSSM->RemoveLiveQueryItem(lpLiveBitItem_engine);
			}
			else if(lpLiveBitItem->cType == TYPE_CORRECTED_BOOST)
			{
				LPSTRUCT_LIVEBITITEM lpLiveBitItem_absolute = NULL;
				LPSTRUCT_LIVEBITITEM lpLiveBitItem_atmospheric = NULL;
				if((lpLiveBitItem_absolute = ((frameRealtime*)lpClass)->lpParent->GetLiveDataItem("Manifold Absolute Pressure")) != NULL)
					((frameRealtime*)lpClass)->lpParent->lpProtocolSSM->RemoveLiveQueryItem(lpLiveBitItem_absolute);
				if((lpLiveBitItem_atmospheric = ((frameRealtime*)lpClass)->lpParent->GetLiveDataItem("Atmospheric Pressure")) != NULL)
					((frameRealtime*)lpClass)->lpParent->lpProtocolSSM->RemoveLiveQueryItem(lpLiveBitItem_atmospheric);
			}
		}
	}

CATCHCATCH("callbackRealtime()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}