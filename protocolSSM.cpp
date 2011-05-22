/*/
	protocolSSM.cpp (2005.06.28)
/*/

#include <wfc.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "protocolSSM.h"
#include "handlerError.h"
#include "definitionError.h"
#include "definitionLocal.h"
#include "ecuExplorer.h"

#define MAXSIZE_MESSAGE 1024

DWORD WINAPI threadReceive(protocolSSM* lpParent);

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

protocolSSM::protocolSSM()
{
	bThreadPoll = FALSE;
	hThreadReceive = NULL;
	iTraceFile = 0;
	listLiveQuery.RemoveAll();
	hWndRealtime = NULL;
	hWndDTC = NULL;
	lpParent = NULL;
	iBaudRate = CBR_4800;

	semaphoreRead = CreateSemaphore(NULL,0,1,NULL);
	semaphoreWrite = CreateSemaphore(NULL,0,1,NULL);
	semaphoreLive = CreateSemaphore(NULL,1,1,NULL);

	InitializeCriticalSection(&csGlobal_LiveQuery);
}

protocolSSM::~protocolSSM()
{
	if(semaphoreRead != NULL)
		CloseHandle(semaphoreRead);
	if(semaphoreWrite != NULL)
		CloseHandle(semaphoreWrite);

	DeleteCriticalSection(&csGlobal_LiveQuery);

	listCSVCapture.RemoveAll();
}

long protocolSSM::Start()
{
	CString sError;
	CString sTraceFile;
	DWORD dwThreadID = 0;
	long lResult = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	sSupported.Empty();
	sECU.Empty();

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = commSerial::Start()) != ERR_SUCCESS)
		return lResult;

EXCEPTION_BOOKMARK(__LINE__)
	if(((ecuExplorer*)lpParent)->ulConfig & CONFIG_CAPTURESSMTRACEFILE)
	{
		CTime tTime = CTime::GetCurrentTime();
		sTraceFile.Format("%s",tTime.Format("%d-%m-%Y %Hh%Mm.ssm.ptf"));

		sError.Format("using SSM trace file [%s]",sTraceFile);
		OutputString(sError,MESSAGETYPE_INFO);

		if((iTraceFile = _open((LPCSTR)sTraceFile,_O_RDWR | _O_CREAT | _O_APPEND | _O_BINARY,_S_IREAD | _S_IWRITE)) == -1)
		{
			sError.Format("File Error - Start [_open] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			((ecuExplorer*)lpParent)->ulConfig &= ~CONFIG_CAPTURESSMTRACEFILE;
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	bThreadPoll = TRUE;
	if((hThreadReceive = CreateThread((LPSECURITY_ATTRIBUTES)NULL,0,(LPTHREAD_START_ROUTINE)threadReceive,(LPVOID)this,0,&dwThreadID)) == NULL)
	{
		sError.Format("Thread Error - Start [threadReceive] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		return F_THREAD_ERROR;
	}
	else
		SetThreadPriority(hThreadReceive,THREAD_PRIORITY_HIGHEST);

CATCHCATCH("protocolSSM::Start()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return EXIT_FAILURE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return EXIT_SUCCESS;
}

long protocolSSM::Stop()
{
	CString sError;
	DWORD dwExitCode = 0;
	LPSTRUCT_LIVEQUERYITEM lpLiveQueryItem = NULL;
	long lResult;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	bThreadPoll = FALSE;
	ReleaseSemaphore(semaphoreRead,1,NULL);

EXCEPTION_BOOKMARK(__LINE__)
	do
	{
		::GetExitCodeThread(hThreadReceive,&dwExitCode);
		if(dwExitCode == STILL_ACTIVE)
			Sleep(10);
	}while(dwExitCode == STILL_ACTIVE);

EXCEPTION_BOOKMARK(__LINE__)
	if(hThreadReceive != NULL)
	{
		CloseHandle(hThreadReceive);
		hThreadReceive = NULL;
	}

EXCEPTION_BOOKMARK(__LINE__)
	while(listLiveQuery.GetCount() > 0)
	{
		lpLiveQueryItem = (LPSTRUCT_LIVEQUERYITEM)listLiveQuery.RemoveHead();
		delete lpLiveQueryItem;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(((ecuExplorer*)lpParent)->ulConfig & CONFIG_CAPTURESSMTRACEFILE && iTraceFile > 0)
		_close(iTraceFile);

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = commSerial::Stop()) != ERR_SUCCESS)
		return lResult;

CATCHCATCH("protocolSSM::Stop()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return EXIT_FAILURE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return EXIT_SUCCESS;
}

DWORD WINAPI threadReceive(protocolSSM* lpParent)
{
	CString sError;
	unsigned char cRx;
	unsigned char* cBuffer = NULL;
	unsigned short usBufferPointer = 0;
	unsigned short usMessageLength = 0;
	unsigned short usOutstanding = 0;
	unsigned short usProtection = 0;
	long lResult = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpParent == NULL)
		return EXIT_FAILURE;

EXCEPTION_BOOKMARK(__LINE__)
	if((cBuffer = (unsigned char*)malloc((size_t)MAXSIZE_MESSAGE)) == NULL)
	{
		sError.Format("Memory Error - protocolSSM::threadReceive [cBuffer] : %i [0x%X]",GetLastError());
		OutputString(sError,TRUE);

		return F_NO_MEMORY;
	}

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)cBuffer,0,(size_t)MAXSIZE_MESSAGE);
	usBufferPointer = 0;

EXCEPTION_BOOKMARK(__LINE__)
	while(lpParent->bThreadPoll)
	{
EXCEPTION_BOOKMARK(__LINE__)
		WaitForSingleObject(lpParent->semaphoreRead,INFINITE);

LOOP_RECURSIVE:
		if(!lpParent->bThreadPoll)
			break;

EXCEPTION_BOOKMARK(__LINE__)
		usMessageLength = usBufferPointer = cRx = 0;
		lpParent->Read(&cRx,1);

		if(cRx == IE_SSM_HEADER)
		{
EXCEPTION_BOOKMARK(__LINE__)
			cBuffer[usBufferPointer++] = cRx;

EXCEPTION_BOOKMARK(__LINE__)
			if(lpParent->Read(&cRx,1) > 0) cBuffer[usBufferPointer++] = cRx;
			if(lpParent->Read(&cRx,1) > 0) cBuffer[usBufferPointer++] = cRx;
			if(lpParent->Read(&cRx,1) > 0) usMessageLength = cBuffer[usBufferPointer++] = cRx;

EXCEPTION_BOOKMARK(__LINE__)
			if(usMessageLength > 0)
			{
				lResult = 0;
				usMessageLength++;

EXCEPTION_BOOKMARK(__LINE__)
				if((lResult = lpParent->Read(&cBuffer[usBufferPointer],usMessageLength)) < usMessageLength)
				{
					sError.Format("read [%i] returned less than message length [%i]",lResult,usMessageLength);
					OutputString(sError);

					usBufferPointer += (unsigned short)lResult;
					usOutstanding = usMessageLength - (unsigned short)lResult;
					usProtection = 1024;
					while(usOutstanding > 0)
					{
						if((lResult = lpParent->Read(&cBuffer[usBufferPointer],1)) > 0)
						{
							usOutstanding -= (unsigned short)lResult;
							usBufferPointer += (unsigned short)lResult;
						}
						else if(usProtection-- <= 0)
							break;
					}

					if(usProtection <= 0)
						goto LOOP_EXIT;
				}
				else
					usBufferPointer += (unsigned short)lResult;

EXCEPTION_BOOKMARK(__LINE__)
				if(((ecuExplorer*)lpParent->lpParent)->ulConfig & CONFIG_CAPTURESSMTRACEFILE && lpParent->iTraceFile > 0)
					if(_write(lpParent->iTraceFile,&cBuffer[0],usBufferPointer) == -1)
					{
						((ecuExplorer*)lpParent->lpParent)->ulConfig |= ~CONFIG_CAPTURESSMTRACEFILE;

						sError.Format("File Error - threadReceive [_write] : %i [0x%X]",GetLastError(),GetLastError());
						OutputString(sError,TRUE);

						_close(lpParent->iTraceFile);
						lpParent->iTraceFile = 0;
					}

EXCEPTION_BOOKMARK(__LINE__)
				lpParent->DumpHex(cBuffer,usBufferPointer,"<--");

EXCEPTION_BOOKMARK(__LINE__)
				switch(cBuffer[4])
				{
				case MSG_SSM_QueryECUType:
					goto LOOP_RECURSIVE;
					break;
				case MSG_SSM_QueryECUType_Response:
					lpParent->ProcessMessage((LPSTRUCT_MSG_SSM_QUERYRESPONSE)cBuffer,usBufferPointer);
					break;
				case MSG_SSM_ReadAddressSingle:
					goto LOOP_RECURSIVE;
					break;
				case MSG_SSM_ReadAddressSingle_Response:
					if(((ecuExplorer*)lpParent->lpParent)->hThreadLiveQuery != NULL)
						lpParent->ProcessMessage((LPSTRUCT_MSG_SSM_LIVEQUERYRESPONSE)cBuffer,usBufferPointer);
					else if(((ecuExplorer*)lpParent->lpParent)->hThreadDTCQuery != NULL)
						lpParent->ProcessMessage((LPSTRUCT_MSG_SSM_DTCQUERYRESPONSE)cBuffer,usBufferPointer);
					else
						lpParent->ProcessMessage((LPSTRUCT_MSG_SSM_READADDRESSSINGLERESPONSE)cBuffer,usBufferPointer);
					break;
				case MSG_SSM_ReadAddressBlock:
					goto LOOP_RECURSIVE;
					break;
				case MSG_SSM_ReadAddressBlock_Response:
					break;
				case MSG_SSM_WriteAddressSingle:
					goto LOOP_RECURSIVE;
					break;
				case MSG_SSM_WriteAddressSingle_Response:
					lpParent->ProcessMessage((LPSTRUCT_MSG_SSM_WRITEADDRESSSINGLERESPONSE)cBuffer,usBufferPointer);
					break;
				case MSG_SSM_WriteAddressBlock:
					goto LOOP_RECURSIVE;
					break;
				default:
					sError.Format("unknown message type %i [0x%X]",cBuffer[3],cBuffer[3]);
					OutputString(sError,MESSAGETYPE_WARNING);
					break;
				}

EXCEPTION_BOOKMARK(__LINE__)
				ReleaseSemaphore(lpParent->semaphoreWrite,1,NULL);
			}
		}

LOOP_EXIT:
		Sleep(1);
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer != NULL)
		free(cBuffer);

CATCHCATCH("threadReceive(protocolSSM)");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return EXIT_FAILURE;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return EXIT_SUCCESS;
}

unsigned char protocolSSM::GenerateChecksum(unsigned char* cBuffer,unsigned short usBufferLength)
{
	CString sError;
	unsigned short usIndex;
	unsigned short usChecksum = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)

	if(cBuffer == NULL)
		return 0;

EXCEPTION_BOOKMARK(__LINE__)

	for(usIndex = 0;usIndex < usBufferLength;usIndex++)
		usChecksum += cBuffer[usIndex];

EXCEPTION_BOOKMARK(__LINE__)

	return (unsigned char)LOBYTE(usChecksum);

CATCHCATCH("protocolSSM::GenerateChecksum()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

long protocolSSM::ResetECU()
{
	CString sError;
	long lResult = 0;
	BOOL bRestart = FALSE;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = MessageBox(NULL,"Are you sure you want to reset your ECU?","Reset ECU?",MB_ICONQUESTION | MB_YESNO)) == IDNO)
		return ERR_SUCCESS;

EXCEPTION_BOOKMARK(__LINE__)
	if(((ecuExplorer*)lpParent)->hThreadLiveQuery != NULL)
		switch(MessageBox(NULL,"Data capture will be paused during this process.\nDo you want to continue?","Pause Data Capture",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			((ecuExplorer*)lpParent)->OnPauseDataCapture();
			bRestart = TRUE;
			break;
		case IDNO:
			return ERR_SUCCESS;
	}
	else if(((ecuExplorer*)lpParent)->hThreadDTCQuery != NULL)
		switch(MessageBox(NULL,"Trouble Code reading will be paused during this process.\nDo you want to continue?","Pause Trouble Code Reading",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			((ecuExplorer*)lpParent)->OnPauseDTC();
			bRestart = TRUE;
			break;
		case IDNO:
			return ERR_SUCCESS;
	}

EXCEPTION_BOOKMARK(__LINE__)
	cReturn_WriteSingleAddress = 0;

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = WriteAddressSingle(ADDRESS_RESET_ECU,0x40)) != ERR_SUCCESS)
	{
		sError.Format("Write request failed with %i [0x%X]",lResult,lResult);
		MessageBox(NULL,sError,"Reset ECU - Failed",MB_ICONSTOP | MB_OK);
		return lResult;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(cReturn_WriteSingleAddress != 0x40)
	{
		sError.Format("Invalid write check value %i [0x%X]",cReturn_WriteSingleAddress,cReturn_WriteSingleAddress);
		MessageBox(NULL,sError,"Reset ECU - Failed",MB_ICONSTOP | MB_OK);
		return F_INVALID_WRITE_CHECK_VALUE;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(bRestart == TRUE)
		((ecuExplorer*)lpParent)->StartLiveQueryThread();

EXCEPTION_BOOKMARK(__LINE__)
	MessageBox(NULL,"Your ECU is now reset.\nPlease turn the ignition OFF and then ON to complete the process.","Reset ECU",MB_ICONINFORMATION | MB_OK);

CATCHCATCH("protocolSSM::ResetECU()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

long protocolSSM::QueryIgnitionTiming()
{
	CString sError;
	long lResult = 0;
	BOOL bRestart = FALSE;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(((ecuExplorer*)lpParent)->hThreadLiveQuery != NULL)
		switch(MessageBox(NULL,"Data capture will be paused during this process.\nDo you want to continue?","Pause Data Capture",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			((ecuExplorer*)lpParent)->OnPauseDataCapture();
			bRestart = TRUE;
			break;
		case IDNO:
			return ERR_SUCCESS;
	}
	else if(((ecuExplorer*)lpParent)->hThreadDTCQuery != NULL)
		switch(MessageBox(NULL,"Trouble Code reading will be paused during this process.\nDo you want to continue?","Pause Trouble Code Reading",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			((ecuExplorer*)lpParent)->OnPauseDTC();
			bRestart = TRUE;
			break;
		case IDNO:
			return ERR_SUCCESS;
	}

EXCEPTION_BOOKMARK(__LINE__)
	cReturn_ReadSingleAddress = 0;

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = ReadAddressSingle(ADDRESS_IGNITION_RETARD)) != ERR_SUCCESS)
	{
		sError.Format("Read request failed with %i [0x%X]",lResult,lResult);
		MessageBox(NULL,sError,"Query Ignition Timing - Failed",MB_ICONSTOP | MB_OK);
		return lResult;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(bRestart == TRUE)
		((ecuExplorer*)lpParent)->StartLiveQueryThread();

EXCEPTION_BOOKMARK(__LINE__)
	sError.Format("Your ignition timing adjustment is set to a total value of %i°",(char)cReturn_ReadSingleAddress * -1);
	MessageBox(NULL,sError,"Query Ignition Timing",MB_ICONINFORMATION | MB_OK);

CATCHCATCH("protocolSSM::QueryIgnitionTiming()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

long protocolSSM::RetardIgnitionTiming(char cValue)
{
	CString sError;
	long lResult = 0;
	unsigned char cCurrent = 0;
	BOOL bRestart = FALSE;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(((ecuExplorer*)lpParent)->hThreadLiveQuery != NULL)
		switch(MessageBox(NULL,"Data capture will be paused during this process.\nDo you want to continue?","Pause Data Capture",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			((ecuExplorer*)lpParent)->OnPauseDataCapture();
			bRestart = TRUE;
			break;
		case IDNO:
			return ERR_SUCCESS;
	}
	else if(((ecuExplorer*)lpParent)->hThreadDTCQuery != NULL)
		switch(MessageBox(NULL,"Trouble Code reading will be paused during this process.\nDo you want to continue?","Pause Trouble Code Reading",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			((ecuExplorer*)lpParent)->OnPauseDTC();
			bRestart = TRUE;
			break;
		case IDNO:
			return ERR_SUCCESS;
	}

EXCEPTION_BOOKMARK(__LINE__)
	cReturn_WriteSingleAddress = 0;
	cReturn_ReadSingleAddress = 0;

	if(cValue != 0)
	{
EXCEPTION_BOOKMARK(__LINE__)
		if((lResult = ReadAddressSingle(ADDRESS_IGNITION_RETARD)) != ERR_SUCCESS)
		{
			sError.Format("Read request failed with %i [0x%X]",lResult,lResult);
			MessageBox(NULL,sError,"Retard Ignition Timing - Failed",MB_ICONSTOP | MB_OK);
			return lResult;
		}
		cCurrent = cReturn_ReadSingleAddress;

EXCEPTION_BOOKMARK(__LINE__)
		if(((char)cCurrent + cValue) > 5 || ((char)cCurrent + cValue) < -5)
		{
			sError.Format("The ignition timing value can only be adjusted by a 5° increase/decrease.\n\nIt is currently set at %i°",(char)cCurrent * -1);
			MessageBox(NULL,sError,"Retard Ignition Timing",MB_ICONWARNING | MB_OK);
			return ERR_SUCCESS;
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = WriteAddressSingle(ADDRESS_IGNITION_RETARD,cCurrent + cValue)) != ERR_SUCCESS)
	{
		sError.Format("Write request failed with %i [0x%X]",lResult,lResult);
		MessageBox(NULL,sError,"Retard Ignition Timing - Failed",MB_ICONSTOP | MB_OK);
		return lResult;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(cReturn_WriteSingleAddress != (unsigned char)((char)cCurrent + cValue))
	{
		sError.Format("Invalid write check value %i [0x%X]",cReturn_WriteSingleAddress,cReturn_WriteSingleAddress);
		MessageBox(NULL,sError,"Retard Ignition Timing - Failed",MB_ICONSTOP | MB_OK);
		return F_INVALID_WRITE_CHECK_VALUE;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(bRestart == TRUE)
		((ecuExplorer*)lpParent)->StartLiveQueryThread();

EXCEPTION_BOOKMARK(__LINE__)
	sError.Format("New value applied.\nYour ignition timing adjustment is now set to a total value of %i°",(char)cReturn_WriteSingleAddress * -1);
	MessageBox(NULL,sError,"Retard Ignition Timing",MB_ICONINFORMATION | MB_OK);

CATCHCATCH("protocolSSM::RetardIgnitionTiming()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

long protocolSSM::QueryIdleSpeed(unsigned long ulAddress)
{
	CString sError;
	CString sCaption;
	CString sType;
	long lResult = 0;
	BOOL bRestart = FALSE;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(((ecuExplorer*)lpParent)->hThreadLiveQuery != NULL)
		switch(MessageBox(NULL,"Data capture will be paused during this process.\nDo you want to continue?","Pause Data Capture",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			((ecuExplorer*)lpParent)->OnPauseDataCapture();
			bRestart = TRUE;
			break;
		case IDNO:
			return ERR_SUCCESS;
	}
	else if(((ecuExplorer*)lpParent)->hThreadDTCQuery != NULL)
		switch(MessageBox(NULL,"Trouble Code reading will be paused during this process.\nDo you want to continue?","Pause Trouble Code Reading",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			((ecuExplorer*)lpParent)->OnPauseDTC();
			bRestart = TRUE;
			break;
		case IDNO:
			return ERR_SUCCESS;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(ulAddress == ADDRESS_IDLE_ADJUST_NORMAL)
		sType = "(no aircon)";
	else
		sType = "(with aircon)";

EXCEPTION_BOOKMARK(__LINE__)
	cReturn_ReadSingleAddress = 0;

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = ReadAddressSingle(ulAddress)) != ERR_SUCCESS)
	{
		sError.Format("Read request failed with %i [0x%X]",lResult,lResult);
		sCaption.Format("Query Idle Speed %s - Failed",sType);
		MessageBox(NULL,sError,sCaption,MB_ICONSTOP | MB_OK);
		return lResult;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(bRestart == TRUE)
		((ecuExplorer*)lpParent)->StartLiveQueryThread();

EXCEPTION_BOOKMARK(__LINE__)
	sError.Format("Your idle speed adjustment %s is set to total value of %iRPM",sType,((cReturn_ReadSingleAddress - 128) * 25));
	sCaption.Format("Query Idle Speed %s",sCaption);
	MessageBox(NULL,sError,sCaption,MB_ICONINFORMATION | MB_OK);

CATCHCATCH("protocolSSM::QueryIdleSpeed()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

long protocolSSM::SetIdleSpeed(unsigned long ulAddress,char cValue)
{
	CString sError;
	CString sCaption;
	CString sType;
	long lResult = 0;
	unsigned char cCurrent = 0;
	BOOL bRestart = FALSE;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(((ecuExplorer*)lpParent)->hThreadLiveQuery != NULL)
		switch(MessageBox(NULL,"Data capture will be paused during this process.\nDo you want to continue?","Pause Data Capture",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			((ecuExplorer*)lpParent)->OnPauseDataCapture();
			bRestart = TRUE;
			break;
		case IDNO:
			return ERR_SUCCESS;
	}
	else if(((ecuExplorer*)lpParent)->hThreadDTCQuery != NULL)
		switch(MessageBox(NULL,"Trouble Code reading will be paused during this process.\nDo you want to continue?","Pause Trouble Code Reading",MB_ICONQUESTION | MB_YESNO))
		{
		case IDYES:
			((ecuExplorer*)lpParent)->OnPauseDTC();
			bRestart = TRUE;
			break;
		case IDNO:
			return ERR_SUCCESS;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(ulAddress == ADDRESS_IDLE_ADJUST_NORMAL)
		sType = "(no aircon)";
	else
		sType = "(with aircon)";

	if(cValue != (char)0x80)
	{
EXCEPTION_BOOKMARK(__LINE__)
		cReturn_WriteSingleAddress = 0;
		cReturn_ReadSingleAddress = 0;

EXCEPTION_BOOKMARK(__LINE__)
		if((lResult = ReadAddressSingle(ulAddress)) != ERR_SUCCESS)
		{
			sError.Format("Read request failed with %i [0x%X]",lResult,lResult);
			sCaption.Format("Set Idle Speed %s - Failed",sType);
			MessageBox(NULL,sError,sCaption,MB_ICONSTOP | MB_OK);
			return lResult;
		}
		cCurrent = cReturn_ReadSingleAddress;

		if(((((cCurrent + cValue) - 128) * 25) > 300) || ((((cCurrent + cValue) - 128) * 25) < -300))
		{
			sError.Format("The idle speed can only be adjusted by a 300RPM increase/decrease.\n\nIt is currently set at %iRPM",((cCurrent - 128) * 25));
			sCaption.Format("Set Idle Speed %s",sType);
			MessageBox(NULL,sError,sCaption,MB_ICONWARNING | MB_OK);
			return ERR_SUCCESS;
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = WriteAddressSingle(ulAddress,cCurrent + cValue)) != ERR_SUCCESS)
	{
		sError.Format("Write request failed with %i [0x%X]",lResult,lResult);
		sCaption.Format("Set Idle Speed %s - Failed",sType);
		MessageBox(NULL,sError,sCaption,MB_ICONSTOP | MB_OK);
		return lResult;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(cReturn_WriteSingleAddress != (unsigned char)(cCurrent + cValue))
	{
		sError.Format("Invalid write check value %i [0x%X]",cReturn_WriteSingleAddress,cReturn_WriteSingleAddress);
		sCaption.Format("Set Idle Speed %s - Failed",sType);
		MessageBox(NULL,sError,sCaption,MB_ICONSTOP | MB_OK);
		return F_INVALID_WRITE_CHECK_VALUE;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(bRestart == TRUE)
		((ecuExplorer*)lpParent)->StartLiveQueryThread();

EXCEPTION_BOOKMARK(__LINE__)
	sError.Format("New value applied.\nYour idle speed adjustment %s is now set to total value of %iRPM",sType,((cReturn_WriteSingleAddress - 128) * 25));
	sCaption.Format("Set Idle Speed %s",sCaption);
	MessageBox(NULL,sError,sCaption,MB_ICONINFORMATION | MB_OK);

CATCHCATCH("protocolSSM::SetIdleSpeed()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

long protocolSSM::QueryECUType()
{
	CString sError;
	unsigned char* cBuffer = NULL;
	unsigned short usBufferPointer = 0;
	LPSTRUCT_MSG_SSM_QUERY lpMessage = NULL;
	long lResult = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((cBuffer = (unsigned char*)malloc((size_t)MAXSIZE_MESSAGE)) == NULL)
	{
		sError.Format("Memory Error - QueryECUType [cBuffer] : %i [0x%X]",GetLastError());
		OutputString(sError,TRUE);

		return F_NO_MEMORY;
	}

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)cBuffer,0,(size_t)MAXSIZE_MESSAGE);
	usBufferPointer = 0;

EXCEPTION_BOOKMARK(__LINE__)
	lpMessage = (LPSTRUCT_MSG_SSM_QUERY)&cBuffer[usBufferPointer];
	lpMessage->Header.cHeaderIdentifier = IE_SSM_HEADER;
	lpMessage->Header.cSource = IE_SSM_SSM;
	lpMessage->Header.cDestination = IE_SSM_ECU;
	lpMessage->Header.cDataLength = 1;
	lpMessage->cMessageType = MSG_SSM_QueryECUType;
	lpMessage->cChecksum = GenerateChecksum(&cBuffer[0],5);
	usBufferPointer = sizeof(STRUCT_MSG_SSM_QUERY);

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = Write(cBuffer,usBufferPointer)) != usBufferPointer)
	{
		sError.Format("[QueryECUType] write [%i] returned less than buffer length [%i]",lResult,usBufferPointer);
		OutputString(sError,MESSAGETYPE_WARNING);

		free(cBuffer);
		return F_INVALID_WRITE_LENGTH;
	}
	else
		OutputString("to ECU : QueryECUType");

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer != NULL)
		free(cBuffer);

EXCEPTION_BOOKMARK(__LINE__)
	ReleaseSemaphore(semaphoreRead,1,NULL);

EXCEPTION_BOOKMARK(__LINE__)
	switch(WaitForSingleObject(semaphoreWrite,SEMAPHORE_TIMEOUT))
	{
	case WAIT_ABANDONED:
	case WAIT_TIMEOUT:
		Purge();
		sError.Format("Response Timeout - QueryECUType");
		OutputString(sError,MESSAGETYPE_ERROR);
		return F_MUTEX_TIMEOUT;
	}

	OutputString("semaphoreWrite returned...");

CATCHCATCH("protocolSSM::QueryECUType()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::ProcessMessage(LPSTRUCT_MSG_SSM_QUERYRESPONSE lpMessage,unsigned short usMessageLength)
{
	CString sError;
	unsigned short usBufferPointer = 0;
	unsigned char* pBuffer = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMessage == NULL)
		return F_NULL_POINTER;

	OutputString("from ECU : QueryECUTypeResponse");

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMessage->Header.cSource != IE_SSM_ECU)
		return F_INVALID_SOURCE;

	if(lpMessage->Header.cDestination != IE_SSM_SSM)
		return F_INVALID_DESTINATION;

EXCEPTION_BOOKMARK(__LINE__)
	memcpy(sECU.GetBuffer(sizeof(lpMessage->cID)),(LPCTSTR)&lpMessage->cID[0],sizeof(lpMessage->cID));
	sECU.ReleaseBuffer(sizeof(lpMessage->cID));
	usBufferPointer = sizeof(STRUCT_MSG_SSM_QUERYRESPONSE);

EXCEPTION_BOOKMARK(__LINE__)
	memcpy(sSupported.GetBuffer(usMessageLength-1-usBufferPointer),(LPCTSTR)&lpMessage->cSupported,usMessageLength-1-usBufferPointer);
	sSupported.ReleaseBuffer(usMessageLength-1-usBufferPointer);
	usBufferPointer += sSupported.GetLength();

EXCEPTION_BOOKMARK(__LINE__)
	pBuffer = (unsigned char*)lpMessage;
	if(pBuffer[usBufferPointer] != GenerateChecksum((unsigned char*)lpMessage,usMessageLength - 1))
		OutputString("invalid checksum in return [QueryECUTypeResponse]",MESSAGETYPE_ERROR);

CATCHCATCH("protocolSSM::ProcessMessage(LPSTRUCT_MSG_SSM_QUERYRESPONSE)");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::ReadAddressSingle(unsigned long ulAddress)
{
	CString sError;
	unsigned char* cBuffer = NULL;
	unsigned short usBufferPointer = 0;
	LPSTRUCT_MSG_SSM_READADDRESSSINGLE lpMessage = NULL;
	long lResult = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((cBuffer = (unsigned char*)malloc((size_t)MAXSIZE_MESSAGE)) == NULL)
	{
		sError.Format("Memory Error - ReadAddressSingle [cBuffer] : %i [0x%X]",GetLastError()); 
		OutputString(sError,TRUE);

		return F_NO_MEMORY;
	}

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)cBuffer,0,(size_t)MAXSIZE_MESSAGE);
	usBufferPointer = 0;

EXCEPTION_BOOKMARK(__LINE__)
	lpMessage = (LPSTRUCT_MSG_SSM_READADDRESSSINGLE)&cBuffer[usBufferPointer];
	lpMessage->Header.cHeaderIdentifier = IE_SSM_HEADER;
	lpMessage->Header.cSource = IE_SSM_SSM;
	lpMessage->Header.cDestination = IE_SSM_ECU;
	lpMessage->Header.cDataLength = 5;
	lpMessage->cMessageType = MSG_SSM_ReadAddressSingle;
	lpMessage->cPad = 0;
	*((unsigned long*)&lpMessage->cAddress[0]) = ntohl(ulAddress)>>8;
	lpMessage->cChecksum = GenerateChecksum(&cBuffer[0],sizeof(STRUCT_MSG_SSM_READADDRESSSINGLE)-1);
	usBufferPointer = sizeof(STRUCT_MSG_SSM_READADDRESSSINGLE);

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = Write(cBuffer,usBufferPointer)) != usBufferPointer)
	{
		sError.Format("[ReadAddressSingle] write [%i] returned less than buffer length [%i]",lResult,usBufferPointer);
		OutputString(sError,MESSAGETYPE_WARNING);

		free(cBuffer);
		return F_INVALID_WRITE_LENGTH;
	}
	else
		OutputString("to ECU : ReadAddressSingle");

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer != NULL)
		free(cBuffer);

EXCEPTION_BOOKMARK(__LINE__)
	ReleaseSemaphore(semaphoreRead,1,NULL);

EXCEPTION_BOOKMARK(__LINE__)
	switch(WaitForSingleObject(semaphoreWrite,SEMAPHORE_TIMEOUT))
	{
	case WAIT_ABANDONED:
	case WAIT_TIMEOUT:
		Purge();
		sError.Format("Response Timeout - ReadAddressSingle");
		OutputString(sError,MESSAGETYPE_ERROR);
		return F_MUTEX_TIMEOUT;
	}

	OutputString("semaphoreWrite returned...");

CATCHCATCH("protocolSSM::ReadAddressSingle()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::WriteAddressSingle(unsigned long ulAddress,unsigned char cValue)
{
	CString sError;
	unsigned char* cBuffer = NULL;
	unsigned short usBufferPointer = 0;
	LPSTRUCT_MSG_SSM_WRITEADDRESSSINGLE lpMessage = NULL;
	long lResult = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((cBuffer = (unsigned char*)malloc((size_t)MAXSIZE_MESSAGE)) == NULL)
	{
		sError.Format("Memory Error - WriteAddressSingle [cBuffer] : %i [0x%X]",GetLastError()); 
		OutputString(sError,TRUE);

		return F_NO_MEMORY;
	}

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)cBuffer,0,(size_t)MAXSIZE_MESSAGE);
	usBufferPointer = 0;

EXCEPTION_BOOKMARK(__LINE__)
	lpMessage = (LPSTRUCT_MSG_SSM_WRITEADDRESSSINGLE)&cBuffer[usBufferPointer];
	lpMessage->Header.cHeaderIdentifier = IE_SSM_HEADER;
	lpMessage->Header.cSource = IE_SSM_SSM;
	lpMessage->Header.cDestination = IE_SSM_ECU;
	lpMessage->Header.cDataLength = 5;
	lpMessage->cMessageType = MSG_SSM_WriteAddressSingle;
	*((unsigned long*)&lpMessage->cAddress[0]) = ntohl(ulAddress)>>8;
	lpMessage->cValue = cValue;
	lpMessage->cChecksum = GenerateChecksum(&cBuffer[0],sizeof(STRUCT_MSG_SSM_WRITEADDRESSSINGLE)-1);
	usBufferPointer = sizeof(STRUCT_MSG_SSM_WRITEADDRESSSINGLE);

EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = Write(cBuffer,usBufferPointer)) != usBufferPointer)
	{
		sError.Format("[WriteAddressSingle] write [%i] returned less than buffer length [%i]",lResult,usBufferPointer);
		OutputString(sError,MESSAGETYPE_WARNING);

		free(cBuffer);
		return F_INVALID_WRITE_LENGTH;
	}
	else
		OutputString("to ECU : WriteAddressSingle");

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer != NULL)
		free(cBuffer);

EXCEPTION_BOOKMARK(__LINE__)
	ReleaseSemaphore(semaphoreRead,1,NULL);

EXCEPTION_BOOKMARK(__LINE__)
	switch(WaitForSingleObject(semaphoreWrite,SEMAPHORE_TIMEOUT))
	{
	case WAIT_ABANDONED:
	case WAIT_TIMEOUT:
		Purge();
		sError.Format("Response Timeout - WriteAddressSingle");
		OutputString(sError,MESSAGETYPE_ERROR);
		return F_MUTEX_TIMEOUT;
	}

	OutputString("semaphoreWrite returned...");

CATCHCATCH("protocolSSM::WriteAddressSingle()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::ProcessMessage(LPSTRUCT_MSG_SSM_READADDRESSSINGLERESPONSE lpMessage,unsigned short usMessageLength)
{
	CString sError;
	unsigned short usBufferPointer = 0;
	unsigned char* cBuffer = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMessage == NULL)
		return F_NULL_POINTER;

	OutputString("from ECU : ReadAddressSingleResponse");

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMessage->Header.cSource != IE_SSM_ECU)
		return F_INVALID_SOURCE;

	if(lpMessage->Header.cDestination != IE_SSM_SSM)
		return F_INVALID_DESTINATION;

EXCEPTION_BOOKMARK(__LINE__)
	usBufferPointer = sizeof(STRUCT_MSG_SSM_READADDRESSSINGLERESPONSE);
	cBuffer = (unsigned char*)lpMessage;

EXCEPTION_BOOKMARK(__LINE__)
	while(usBufferPointer < usMessageLength - 1)
		cReturn_ReadSingleAddress = cBuffer[usBufferPointer++];

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer[usBufferPointer] != GenerateChecksum((unsigned char*)lpMessage,usMessageLength - 1))
		OutputString("invalid checksum in return [ReadAddressSingleResponse]",MESSAGETYPE_ERROR);

CATCHCATCH("protocolSSM::ProcessMessage(LPSTRUCT_MSG_SSM_READADDRESSSINGLE)");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::ProcessMessage(LPSTRUCT_MSG_SSM_WRITEADDRESSSINGLERESPONSE lpMessage,unsigned short usMessageLength)
{
	CString sError;
	unsigned short usBufferPointer = 0;
	unsigned char* cBuffer = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMessage == NULL)
		return F_NULL_POINTER;

	OutputString("from ECU : WriteAddressSingleResponse");

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMessage->Header.cSource != IE_SSM_ECU)
		return F_INVALID_SOURCE;

	if(lpMessage->Header.cDestination != IE_SSM_SSM)
		return F_INVALID_DESTINATION;

EXCEPTION_BOOKMARK(__LINE__)
	usBufferPointer = sizeof(STRUCT_MSG_SSM_WRITEADDRESSSINGLERESPONSE);
	cBuffer = (unsigned char*)lpMessage;

EXCEPTION_BOOKMARK(__LINE__)
	cReturn_WriteSingleAddress = cBuffer[usBufferPointer++];

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer[usBufferPointer] != GenerateChecksum((unsigned char*)lpMessage,usMessageLength - 1))
		OutputString("invalid checksum in return [WriteAddressSingleResponse]",MESSAGETYPE_ERROR);

CATCHCATCH("protocolSSM::ProcessMessage(LPSTRUCT_MSG_SSM_WRITEADDRESSSINGLE)");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::AddLiveQueryItem(LPSTRUCT_LIVEBITITEM lpLiveBitItem,BOOL bDependency)
{
	CString sError;
	POSITION posFind = NULL;
	LPSTRUCT_LIVEQUERYITEM lpLiveQueryItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpLiveBitItem == NULL)
		return F_NULL_POINTER;

EXCEPTION_BOOKMARK(__LINE__)
	if((lpLiveQueryItem = GetLiveQueryItem(lpLiveBitItem)) != NULL)
	{
		lpLiveQueryItem->iCount++;
		return F_DUPLICATE_MEMORY_QUERY;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if((lpLiveQueryItem = new STRUCT_LIVEQUERYITEM) == NULL)
	{
		sError.Format("Memory Error - AddLiveQueryItem [lpLiveQueryItem] : %i [0x%X]",GetLastError()); 
		OutputString(sError,TRUE);

		return F_NO_MEMORY;
	}
	lpLiveQueryItem->ulAddress_high = lpLiveBitItem->ulAddress_high;
	lpLiveQueryItem->ulAddress_low = lpLiveBitItem->ulAddress_low;
	lpLiveQueryItem->iCount = 1;
	lpLiveQueryItem->fValue_raw = 0;
	lpLiveQueryItem->lpLiveBitItem = lpLiveBitItem;

EXCEPTION_BOOKMARK(__LINE__)
	WaitForSingleObject(semaphoreLive,INFINITE);
	EnterCriticalSection(&csGlobal_LiveQuery);
	if(bDependency) listLiveQuery.AddHead(lpLiveQueryItem);
	else listLiveQuery.AddTail(lpLiveQueryItem);

	sError.Format("AddLiveQueryItem [0x%X] [0x%X]",lpLiveBitItem->ulAddress_high,lpLiveBitItem->ulAddress_low);
	OutputString(sError);

	LeaveCriticalSection(&csGlobal_LiveQuery);
	ReleaseSemaphore(semaphoreLive,1,NULL);

CATCHCATCH("protocolSSM::AddLiveQueryItem");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::RemoveLiveQueryItem(LPSTRUCT_LIVEBITITEM lpLiveBitItem)
{
	CString sError;
	POSITION posFind = NULL;
	LPSTRUCT_LIVEQUERYITEM lpLiveQueryItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpLiveBitItem == NULL)
		return F_NULL_POINTER;

EXCEPTION_BOOKMARK(__LINE__)
	if((lpLiveQueryItem = GetLiveQueryItem(lpLiveBitItem)) != NULL)
	{
		if(--lpLiveQueryItem->iCount <= 0)
		{
			WaitForSingleObject(semaphoreLive,INFINITE);
			EnterCriticalSection(&csGlobal_LiveQuery);
			if((posFind = listLiveQuery.Find((void*)lpLiveQueryItem)) != NULL)
			{
				listLiveQuery.RemoveAt(posFind);
				delete lpLiveQueryItem;

				sError.Format("RemoveLiveQueryItem [0x%X] [0x%X]",lpLiveBitItem->ulAddress_high,lpLiveBitItem->ulAddress_low);
				OutputString(sError);
			}
			LeaveCriticalSection(&csGlobal_LiveQuery);
			ReleaseSemaphore(semaphoreLive,1,NULL);
		}
	}

CATCHCATCH("protocolSSM::RemoveLiveQueryItem");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

LPSTRUCT_LIVEQUERYITEM protocolSSM::GetLiveQueryItem(LPSTRUCT_LIVEBITITEM lpLiveBitItem)
{
	CString sError;
	POSITION posFind = NULL;
	LPSTRUCT_LIVEQUERYITEM lpLiveQueryItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	EnterCriticalSection(&csGlobal_LiveQuery);
	posFind = listLiveQuery.GetHeadPosition();
	while(posFind != NULL)
	{
		lpLiveQueryItem = (LPSTRUCT_LIVEQUERYITEM)listLiveQuery.GetNext(posFind);
		if(lpLiveQueryItem->lpLiveBitItem == lpLiveBitItem)
			break;
		else
			lpLiveQueryItem = NULL;
	}
	LeaveCriticalSection(&csGlobal_LiveQuery);

CATCHCATCH("protocolSSM::GetLiveQueryItem");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return NULL;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return lpLiveQueryItem;
}

long protocolSSM::LiveQuery()
{
	CString sError;
	unsigned char* cBuffer = NULL;
	unsigned short usBufferPointer = 0;
	LPSTRUCT_MSG_SSM_LIVEQUERY lpMessage = NULL;
	long lResult = 0;
	POSITION pos = NULL;
	LPSTRUCT_LIVEQUERYITEM lpLiveQueryItem = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(listLiveQuery.GetCount() == 0)
		return ERR_SUCCESS;

EXCEPTION_BOOKMARK(__LINE__)
	if((cBuffer = (unsigned char*)malloc((size_t)MAXSIZE_MESSAGE)) == NULL)
	{
		sError.Format("Memory Error - LiveQuery [cBuffer] : %i [0x%X]",GetLastError()); 
		OutputString(sError,TRUE);

		return F_NO_MEMORY;
	}

EXCEPTION_BOOKMARK(__LINE__)
	WaitForSingleObject(semaphoreLive,INFINITE);

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)cBuffer,0,(size_t)MAXSIZE_MESSAGE);
	usBufferPointer = 0;

EXCEPTION_BOOKMARK(__LINE__)
	lpMessage = (LPSTRUCT_MSG_SSM_LIVEQUERY)&cBuffer[usBufferPointer];
	lpMessage->Header.cHeaderIdentifier = IE_SSM_HEADER;
	lpMessage->Header.cSource = IE_SSM_SSM;
	lpMessage->Header.cDestination = IE_SSM_ECU;
	lpMessage->cMessageType = MSG_SSM_ReadAddressSingle;
	lpMessage->cPad = 0;
	usBufferPointer = sizeof(STRUCT_MSG_SSM_LIVEQUERY);

	EnterCriticalSection(&csGlobal_LiveQuery);
	pos = listLiveQuery.GetHeadPosition();
	while(pos != NULL)
	{
		lpLiveQueryItem = (LPSTRUCT_LIVEQUERYITEM)listLiveQuery.GetNext(pos);
		if(lpLiveQueryItem->ulAddress_high > 0 || lpLiveQueryItem->ulAddress_low > 0)
		{
			*((unsigned long*)&cBuffer[usBufferPointer]) = ntohl(lpLiveQueryItem->ulAddress_low)>>8;
			usBufferPointer+=3;
			if(lpLiveQueryItem->ulAddress_high == 0xFFFFFFFF)
			{
				*((unsigned long*)&cBuffer[usBufferPointer]) = ntohl(lpLiveQueryItem->ulAddress_low+1)>>8;
				usBufferPointer+=3;
				*((unsigned long*)&cBuffer[usBufferPointer]) = ntohl(lpLiveQueryItem->ulAddress_low+2)>>8;
				usBufferPointer+=3;
				*((unsigned long*)&cBuffer[usBufferPointer]) = ntohl(lpLiveQueryItem->ulAddress_low+3)>>8;
				usBufferPointer+=3;
			}
			else if(lpLiveQueryItem->ulAddress_high > 0)
			{
				*((unsigned long*)&cBuffer[usBufferPointer]) = ntohl(lpLiveQueryItem->ulAddress_high)>>8;
				usBufferPointer+=3;
			}
			sError.Format("LiveQuery [0x%X] [0x%X]",lpLiveQueryItem->ulAddress_high,lpLiveQueryItem->ulAddress_low);
			OutputString(sError);
		}
	}
	LeaveCriticalSection(&csGlobal_LiveQuery);

	lpMessage->Header.cDataLength = (char)(usBufferPointer - 6 + 2);
	cBuffer[usBufferPointer++] = GenerateChecksum(&cBuffer[0],usBufferPointer);
	
EXCEPTION_BOOKMARK(__LINE__)
	if((lResult = Write(cBuffer,usBufferPointer)) != usBufferPointer)
	{
		sError.Format("[LiveQuery] write [%i] returned less than buffer length [%i]",lResult,usBufferPointer);
		OutputString(sError,MESSAGETYPE_WARNING);
	}
	else
		OutputString("to ECU : LiveQuery");

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer != NULL)
		free(cBuffer);

EXCEPTION_BOOKMARK(__LINE__)
	ReleaseSemaphore(semaphoreRead,1,NULL);

EXCEPTION_BOOKMARK(__LINE__)
	switch(WaitForSingleObject(semaphoreWrite,SEMAPHORE_TIMEOUT))
	{
	case WAIT_ABANDONED:
	case WAIT_TIMEOUT:
		Purge();
		sError.Format("Response Timeout - LiveQuery");
		OutputString(sError,MESSAGETYPE_ERROR);
		ReleaseSemaphore(semaphoreLive,1,NULL);
		return F_MUTEX_TIMEOUT;
	}

	OutputString("semaphoreWrite returned...");

EXCEPTION_BOOKMARK(__LINE__)
	ReleaseSemaphore(semaphoreLive,1,NULL);

CATCHCATCH("protocolSSM::LiveQuery");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::ProcessMessage(LPSTRUCT_MSG_SSM_LIVEQUERYRESPONSE lpMessage,unsigned short usMessageLength)
{
	CString sError;
	unsigned short usBufferPointer = 0;
	unsigned char* cBuffer = NULL;
	POSITION pos = NULL;
	LPSTRUCT_LIVEQUERYITEM lpLiveQueryItem = NULL;
	BYTE byteLow;
	BYTE byteHigh;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMessage == NULL)
		return F_NULL_POINTER;

	OutputString("from ECU : LiveQuery");

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMessage->Header.cSource != IE_SSM_ECU)
		return F_INVALID_SOURCE;

	if(lpMessage->Header.cDestination != IE_SSM_SSM)
		return F_INVALID_DESTINATION;

EXCEPTION_BOOKMARK(__LINE__)
	usBufferPointer = sizeof(STRUCT_MSG_SSM_LIVEQUERY) - 1;
	cBuffer = (unsigned char*)lpMessage;

EXCEPTION_BOOKMARK(__LINE__)
	EnterCriticalSection(&csGlobal_LiveQuery);
	pos = listLiveQuery.GetHeadPosition();
	while(pos != NULL)// && usBufferPointer < usMessageLength - 1)
	{
		lpLiveQueryItem = (LPSTRUCT_LIVEQUERYITEM)listLiveQuery.GetNext(pos);
		if(lpLiveQueryItem->ulAddress_high > 0 || lpLiveQueryItem->ulAddress_low > 0)
		{
			if(lpLiveQueryItem->ulAddress_high == 0xFFFFFFFF)
			{
				unsigned char c[4];
				c[3] = cBuffer[usBufferPointer++];
				c[2] = cBuffer[usBufferPointer++];
				c[1] = cBuffer[usBufferPointer++];
				c[0] = cBuffer[usBufferPointer++];
				lpLiveQueryItem->fValue_raw = *(float*)c;
			}
			else if(lpLiveQueryItem->ulAddress_high > 0)
			{
				byteLow = cBuffer[usBufferPointer++];
				byteHigh = cBuffer[usBufferPointer++];
				lpLiveQueryItem->fValue_raw = (float)MAKEWORD(byteLow,byteHigh);
			}
			else
			{
				byteLow = cBuffer[usBufferPointer++];
				lpLiveQueryItem->fValue_raw = (float)byteLow;
			}
//			TRACE("ProcessMessage(LiveQueryResponse) pointer=%i length=%i [0x%X] [0x%X] byteLow=0x%X byteHigh=0x%X\n",usBufferPointer,usMessageLength,lpLiveQueryItem->ulAddress_high,lpLiveQueryItem->ulAddress_low,byteLow,byteHigh);
		}
		PostMessage(hWndRealtime,MESSAGE_UPDATELISTITEM,NULL,(LPARAM)lpLiveQueryItem);
	}
	LeaveCriticalSection(&csGlobal_LiveQuery);

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer[usBufferPointer] != GenerateChecksum((unsigned char*)lpMessage,usMessageLength - 1))
		OutputString("invalid checksum in return [LiveQuery]",MESSAGETYPE_ERROR);

EXCEPTION_BOOKMARK(__LINE__)
	PostMessage(hWndRealtime,MESSAGE_UPDATECSVFILE,NULL,NULL);

EXCEPTION_BOOKMARK(__LINE__)
	_timeb tbNow;
	_ftime(&tbNow);
	double dblInterval = ((double)((double)tbNow.time * 1000 + (double)tbNow.millitm) - (double)((double)tbSampleRate.time * 1000 + (double)tbSampleRate.millitm));
	_ftime(&tbSampleRate);

EXCEPTION_BOOKMARK(__LINE__)
	if(listCSVCapture.GetCount() > 0)
	{
		sTemp.Format("Realtime Data View :: %ims",(long)dblInterval);
		PostMessage(((ecuExplorer*)lpParent)->hWndResourceTree,MESSAGE_UPDATENODETEXT,(WPARAM)NODE_ECUREALTIME,(LPARAM)(LPCTSTR)sTemp);
	}

CATCHCATCH("protocolSSM::ProcessMessage(LPSTRUCT_MSG_SSM_LIVEQUERY)");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::AddCSVItem(LPSTRUCT_LIVEBITITEM lpLiveBitItem)
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpLiveBitItem == NULL)
		return F_NULL_POINTER;

EXCEPTION_BOOKMARK(__LINE__)
	if(listCSVCapture.Find(lpLiveBitItem) == NULL)
		listCSVCapture.AddTail(lpLiveBitItem);

CATCHCATCH("protocolSSM::AddCSVItem()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::RemoveCSVItem(LPSTRUCT_LIVEBITITEM lpLiveBitItem)
{
	CString sError;
	POSITION pos = NULL;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpLiveBitItem == NULL)
		return F_NULL_POINTER;

EXCEPTION_BOOKMARK(__LINE__)
	if((pos = listCSVCapture.Find(lpLiveBitItem)) != NULL)
		listCSVCapture.RemoveAt(pos);

EXCEPTION_BOOKMARK(__LINE__)
	if(listCSVCapture.GetCount() == 0)
	{
		sTemp.Format("Realtime Data View");
		PostMessage(((ecuExplorer*)lpParent)->hWndResourceTree,MESSAGE_UPDATENODETEXT,(WPARAM)NODE_ECUREALTIME,(LPARAM)(LPCTSTR)sTemp);
	}

CATCHCATCH("protocolSSM::AddCSVItem()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::DTCQuery(unsigned long ulStart,unsigned long ulEnd)
{
	CString sError;
	unsigned char* cBuffer = NULL;
	unsigned short usBufferPointer = 0;
	LPSTRUCT_MSG_SSM_DTCQUERY lpMessage = NULL;
	long lResult = 0;
	int iLength = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if((cBuffer = (unsigned char*)malloc((size_t)MAXSIZE_MESSAGE)) == NULL)
	{
		sError.Format("Memory Error - DTCQuery [cBuffer] : %i [0x%X]",GetLastError()); 
		OutputString(sError,TRUE);

		return F_NO_MEMORY;
	}

EXCEPTION_BOOKMARK(__LINE__)
	memset((void*)cBuffer,0,(size_t)MAXSIZE_MESSAGE);
	usBufferPointer = 0;

EXCEPTION_BOOKMARK(__LINE__)
	lpMessage = (LPSTRUCT_MSG_SSM_DTCQUERY)&cBuffer[usBufferPointer];
	lpMessage->Header.cHeaderIdentifier = IE_SSM_HEADER;
	lpMessage->Header.cSource = IE_SSM_SSM;
	lpMessage->Header.cDestination = IE_SSM_ECU;
	lpMessage->cMessageType = MSG_SSM_ReadAddressSingle;
	lpMessage->cPad = 0;

	for(int iPointer = ulStart;iPointer <= ulEnd;iPointer += 10)
	{
		ulDTCBase = iPointer;
		usBufferPointer = sizeof(STRUCT_MSG_SSM_DTCQUERY);
		iLength = min(10,ulEnd-iPointer+1);

		for(int iLoop = 0;iLoop < iLength;iLoop++)
		{
			*((unsigned long*)&cBuffer[usBufferPointer]) = ntohl(iPointer+iLoop)>>8;
			usBufferPointer+=3;
		}

		lpMessage->Header.cDataLength = (char)(usBufferPointer - 6 + 2);
		cBuffer[usBufferPointer++] = GenerateChecksum(&cBuffer[0],usBufferPointer);
	
EXCEPTION_BOOKMARK(__LINE__)
		if((lResult = Write(&cBuffer[0],usBufferPointer)) != usBufferPointer)
		{
			sError.Format("[DTCQuery] write [%i] returned less than buffer length [%i]",lResult,usBufferPointer);
			OutputString(sError,MESSAGETYPE_WARNING);
		}
		else
			OutputString("to ECU : DTCQuery");

EXCEPTION_BOOKMARK(__LINE__)
		ReleaseSemaphore(semaphoreRead,1,NULL);

EXCEPTION_BOOKMARK(__LINE__)
		switch(WaitForSingleObject(semaphoreWrite,SEMAPHORE_TIMEOUT))
		{
		case WAIT_ABANDONED:
		case WAIT_TIMEOUT:
			Purge();
			sError.Format("Response Timeout - DTCQuery");
			OutputString(sError,MESSAGETYPE_ERROR);
			return F_MUTEX_TIMEOUT;
		}

		OutputString("semaphoreWrite returned...");
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer != NULL)
		free(cBuffer);

CATCHCATCH("protocolSSM::DTCQuery");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long protocolSSM::ProcessMessage(LPSTRUCT_MSG_SSM_DTCQUERYRESPONSE lpMessage,unsigned short usMessageLength)
{
	CString sError;
	unsigned short usBufferPointer = 0;
	unsigned char* cBuffer = NULL;
	POSITION pos = NULL;
	LPSTRUCT_DTCITEM lpDTCItem = NULL;
	CPtrList* pStorage = NULL;
	unsigned char cDTC;
	unsigned int iCheck = 0;
	unsigned int iLoop = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMessage == NULL)
		return F_NULL_POINTER;

	OutputString("from ECU : DTCQuery");

EXCEPTION_BOOKMARK(__LINE__)
	if(lpMessage->Header.cSource != IE_SSM_ECU)
		return F_INVALID_SOURCE;

	if(lpMessage->Header.cDestination != IE_SSM_SSM)
		return F_INVALID_DESTINATION;

EXCEPTION_BOOKMARK(__LINE__)
	usBufferPointer = sizeof(STRUCT_MSG_SSM_DTCQUERY) - 1;
	cBuffer = (unsigned char*)lpMessage;

EXCEPTION_BOOKMARK(__LINE__)
	if(strstr(((ecuExplorer*)lpParent)->GetECUType(sECU),"MY99") || strstr(((ecuExplorer*)lpParent)->GetECUType(sECU),"MY00"))
		pStorage = &((ecuExplorer*)lpParent)->listDTC_Subaru;
	else
		pStorage = &((ecuExplorer*)lpParent)->listDTC_OBD;

EXCEPTION_BOOKMARK(__LINE__)
	while(usBufferPointer < usMessageLength - 1)
	{
		cDTC = cBuffer[usBufferPointer++];
		iCheck = 0;
		if(cDTC > 0)
		{
			pos = pStorage->GetHeadPosition();
			while(pos != NULL)
			{
				lpDTCItem = (LPSTRUCT_DTCITEM)pStorage->GetNext(pos);
				if(lpDTCItem->ulAddress_current == ulDTCBase+iLoop)
				{
					if(cDTC & lpDTCItem->cCheckBit)
						PostMessage(hWndDTC,MESSAGE_UPDATECURRENTDTC,NULL,(LPARAM)lpDTCItem);
				}
				else if(lpDTCItem->ulAddress_historic == ulDTCBase+iLoop)
				{
					if(cDTC & lpDTCItem->cCheckBit)
						PostMessage(hWndDTC,MESSAGE_UPDATEHISTORICDTC,NULL,(LPARAM)lpDTCItem);
				}
			}
		}
		iLoop++;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer[usBufferPointer] != GenerateChecksum((unsigned char*)lpMessage,usMessageLength - 1))
		OutputString("invalid checksum in return [DTCQuery]",MESSAGETYPE_ERROR);

CATCHCATCH("protocolSSM::ProcessMessage(LPSTRUCT_MSG_SSM_DTCQUERY)");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}