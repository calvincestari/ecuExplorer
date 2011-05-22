/*/
	commSerial.cpp (2005.06.06)
/*/

#include "commSerial.h"
#include "handlerError.h"
#include "definitionError.h"
#include "definitionLocal.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

commSerial::commSerial()
{
	hCommPort = NULL;
	iCommPort = 0;
	sCommPort.Empty();
	iBaudRate = CBR_4800;
	iXonLim = 2048;
	iXoffLim = 512;
	iReadIntervalTimeout = MAXDWORD;
	iReadTotalTimeoutConstant = 0;
	iWriteTotalTimeoutConstant = 50;
}

commSerial::~commSerial()
{
	if(hCommPort != NULL)
		CloseHandle(hCommPort);
}

long commSerial::Start()
{
	CString sError;
	DWORD dwThreadID = 0;
	DWORD dwModemState = 0;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(iCommPort > 0)
	{
		sCommPort.Format("\\\\.\\COM%i",iCommPort);

		sError.Format("using comm port %i [%s]",iCommPort,sCommPort);
		OutputString(sError);

EXCEPTION_BOOKMARK(__LINE__)
		if((hCommPort = CreateFile(sCommPort,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL)) == INVALID_HANDLE_VALUE)
		{
			sError.Format("Comms Error - Start [CreateFile] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			return F_COMMS_ERROR;
		}

EXCEPTION_BOOKMARK(__LINE__)
		if(GetCommState(hCommPort,&commDCB) == 0)
		{
			sError.Format("Comms Error - Start [GetCommState] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			return F_COMMS_ERROR;
		}

EXCEPTION_BOOKMARK(__LINE__)
		commDCB.BaudRate = iBaudRate;
		commDCB.ByteSize = 8;
		commDCB.Parity = NOPARITY;
		commDCB.StopBits = ONESTOPBIT;
		commDCB.fBinary = TRUE;
		commDCB.fParity = TRUE;
		commDCB.fOutxCtsFlow = FALSE;
		commDCB.fOutxDsrFlow = FALSE;
		commDCB.fDsrSensitivity = FALSE;
		commDCB.fTXContinueOnXoff = TRUE;
		commDCB.fOutX = FALSE;
		commDCB.fInX = FALSE;
		commDCB.fErrorChar = FALSE;
		commDCB.fNull = FALSE;
		commDCB.fDtrControl = DTR_CONTROL_ENABLE;
		commDCB.fRtsControl = FALSE;
		commDCB.fAbortOnError = FALSE;
//		commDCB.XonLim = 2048;
//		commDCB.XoffLim = 512;

		commDCB.XonLim = iXonLim;
		commDCB.XoffLim = iXoffLim;

/*		WORKING _DCB FOR PROBLEMATIC USERS
		commDCB.BaudRate = CBR_4800;
		commDCB.ByteSize = 8;
		commDCB.Parity = NOPARITY;
		commDCB.StopBits = ONESTOPBIT;
		commDCB.fOutX = FALSE;
		commDCB.fInX = FALSE;
		commDCB.fOutxCtsFlow = FALSE;
		commDCB.fOutxDsrFlow = FALSE;
		commDCB.fDtrControl = DTR_CONTROL_ENABLE;
		commDCB.fRtsControl = FALSE;
		commDCB.XonLim = 2048;
		commDCB.XoffLim = 2048;
		commDCB.EofChar = 0x0;*/

EXCEPTION_BOOKMARK(__LINE__)
		if(SetCommState(hCommPort,&commDCB) == 0)
		{
			sError.Format("Comms Error - Start [SetCommState] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			return F_COMMS_ERROR;
		}

EXCEPTION_BOOKMARK(__LINE__)
		if(GetCommTimeouts(hCommPort,&commTIMEOUTS) == 0)
		{
			sError.Format("Comms Error - Start [GetCommTIMEOUTS] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			return F_COMMS_ERROR;
		}

EXCEPTION_BOOKMARK(__LINE__)
//		commTIMEOUTS.ReadIntervalTimeout = MAXDWORD;
		commTIMEOUTS.ReadTotalTimeoutMultiplier = 0;
//		commTIMEOUTS.ReadTotalTimeoutConstant = 0;
		commTIMEOUTS.WriteTotalTimeoutMultiplier = 0;
//		commTIMEOUTS.WriteTotalTimeoutConstant = 50;

		commTIMEOUTS.ReadIntervalTimeout = iReadIntervalTimeout;
		commTIMEOUTS.ReadTotalTimeoutConstant = iReadTotalTimeoutConstant;
		commTIMEOUTS.WriteTotalTimeoutConstant = iWriteTotalTimeoutConstant;

/*		WORKING _DCB FOR PROBLEMATIC USERS
		commTIMEOUTS.ReadIntervalTimeout = 500;
		commTIMEOUTS.ReadTotalTimeoutMultiplier = 0;
		commTIMEOUTS.ReadTotalTimeoutConstant = 1000;
		commTIMEOUTS.WriteTotalTimeoutMultiplier = 0;
		commTIMEOUTS.WriteTotalTimeoutConstant = 5000;*/

EXCEPTION_BOOKMARK(__LINE__)
		if(SetCommTimeouts(hCommPort,&commTIMEOUTS) == 0)
		{
			sError.Format("Comms Error - Start [SetCommTIMEOUTS] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			return F_COMMS_ERROR;
		}

EXCEPTION_BOOKMARK(__LINE__)
		if(SetCommMask(hCommPort,EV_RXCHAR | EV_RXFLAG | EV_CTS | EV_DSR | EV_RLSD | EV_BREAK | EV_ERR) == 0)
		{
			sError.Format("Comms Error - Start [SetCommMask] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			return F_COMMS_ERROR;
		}

EXCEPTION_BOOKMARK(__LINE__)
		if(GetCommModemStatus(hCommPort,&dwModemState) == 0)
		{
			sError.Format("Comms Error - Start [GetCommModemStatus] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			return F_COMMS_ERROR;
		}

EXCEPTION_BOOKMARK(__LINE__)
		if(EscapeCommFunction(hCommPort,CLRRTS) == 0)
		{
			sError.Format("Comms Error - Start [EscapeCommFunction(CLRRTS)] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			return F_COMMS_ERROR;
		}

EXCEPTION_BOOKMARK(__LINE__)
		if(EscapeCommFunction(hCommPort,SETDTR) == 0)
		{
			sError.Format("Comms Error - Start [EscapeCommFunction(SETDTR)] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			return F_COMMS_ERROR;
		}
	}
	else
	{
		hCommPort = NULL;
		return F_INVALID_COM_PORT;
	}

CATCHCATCH("commSerial::Start()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

long commSerial::Stop()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(hCommPort == NULL)
		return F_NULL_POINTER;

EXCEPTION_BOOKMARK(__LINE__)
	if(EscapeCommFunction(hCommPort,CLRDTR) == 0)
	{
		sError.Format("Comms Error - Stop [EscapeCommFunction(CLRDTR)] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);

		return F_COMMS_ERROR;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(PurgeComm(hCommPort,PURGE_TXCLEAR | PURGE_RXCLEAR) == 0)
	{
		sError.Format("Comms Error - Stop [PurgeComm] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);

		return F_COMMS_ERROR;
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(hCommPort != NULL)
	{
		CloseHandle(hCommPort);
		hCommPort = NULL;
	}

CATCHCATCH("commSerial::Stop()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

unsigned short commSerial::Read(unsigned char* cBuffer,unsigned short usBytesToRead)
{
	CString sError;
	DWORD dwResult;
	COMSTAT commStat;
	int iSafetyCount = 250;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(cBuffer == NULL || usBytesToRead == 0 || hCommPort == NULL)
		return 0;

EXCEPTION_BOOKMARK(__LINE__)
	do
	{
EXCEPTION_BOOKMARK(__LINE__)
		ClearCommError(hCommPort,&dwResult,&commStat);
		if(commStat.cbInQue < usBytesToRead)
		{
			Sleep(1);
			iSafetyCount--;
		}
		if(iSafetyCount == 0) return 0;
	}while(commStat.cbInQue < usBytesToRead && iSafetyCount > 0);

EXCEPTION_BOOKMARK(__LINE__)
	if(!ReadFile(hCommPort,cBuffer,usBytesToRead,&dwResult,NULL))
	{
		sError.Format("Comms Error - Read [ReadFile] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
		dwResult = 0;
	}

CATCHCATCH("commSerial::Read()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return (unsigned short)dwResult;
}

long commSerial::Write(unsigned char* cBuffer,unsigned short usMessageLength)
{
	CString sError;
	DWORD dwBytesWritten = 0;
	DWORD dwLength = 0;
	int iSafetyCount = 250;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	DumpHex(cBuffer,usMessageLength,"-->");

EXCEPTION_BOOKMARK(__LINE__)
	while(dwLength < (DWORD)usMessageLength && iSafetyCount > 0)
	{
EXCEPTION_BOOKMARK(__LINE__)
		if(WriteFile(hCommPort,(void*)&cBuffer[dwLength],(DWORD)usMessageLength-dwBytesWritten,&dwBytesWritten,NULL) == 0)
		{
			sError.Format("Comms Error - Write [WriteFile] : %i [0x%X]",GetLastError(),GetLastError());
			OutputString(sError,MESSAGETYPE_ERROR);

			return F_COMMS_ERROR;
		}
		dwLength += dwBytesWritten;

EXCEPTION_BOOKMARK(__LINE__)
		if(dwLength < (DWORD)usMessageLength)
		{
			Sleep(1);
			iSafetyCount--;

			sError.Format("write length less than buffer size - %i vs %i",dwLength,usMessageLength);
			OutputString(sError);
		}
	}

EXCEPTION_BOOKMARK(__LINE__)
	return dwLength;

CATCHCATCH("commSerial::Write()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return 0;
}

long commSerial::DumpHex(unsigned char* cBuffer,unsigned short usBufferLength,LPCTSTR szDirection)
{
	CString sError;
	CString sOutput;
	CString sASCII;
	int iRow;
	CString sHex;
	unsigned int iLoop;
	char sLower[2];
	char sUpper[2];

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	sOutput.Empty();
	sASCII.Empty();
	iRow = 0;

EXCEPTION_BOOKMARK(__LINE__)
	strcpy(&sLower[0],"z");
	strcpy(&sUpper[0],"0");

EXCEPTION_BOOKMARK(__LINE__)
	for(iLoop = 0; iLoop < (int)usBufferLength; iLoop++)
	{
EXCEPTION_BOOKMARK(__LINE__)
		if(iRow == 8)
			sOutput += " ";

		if(iRow == 16)
		{
			sError.Format("%s %s - %s",szDirection,sOutput,sASCII);
			OutputString(sError);

			sOutput.Empty();
			sASCII.Empty();
			iRow = 0;
		}

EXCEPTION_BOOKMARK(__LINE__)
		iRow++;
		sHex.Format("%X",cBuffer[iLoop]);
		if(sHex.GetLength() < 2)
			sHex = "0" + sHex;

		sOutput += " ";
		sOutput += sHex;

EXCEPTION_BOOKMARK(__LINE__)
		if(cBuffer[iLoop] <= sLower[0] && cBuffer[iLoop] >= sUpper[0])
			sASCII += cBuffer[iLoop];
		else
			sASCII += ".";
	}

EXCEPTION_BOOKMARK(__LINE__)
	if(sOutput.GetLength() > 0)
	{
		while(sOutput.GetLength() < 50)
			sOutput += " ";

		sError.Format("%s %s - %s",szDirection,sOutput,sASCII);
		OutputString(sError);
	}

CATCHCATCH("commSerial::DumpHex()");

	if(bExceptionFlag == EXEPT_CONTINUE)
		return F_EXCEPTION;
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return ERR_SUCCESS;
}

void commSerial::Purge()
{
	CString sError;

TRYTRY

EXCEPTION_BOOKMARK(__LINE__)
	if(PurgeComm(hCommPort,PURGE_TXCLEAR | PURGE_RXCLEAR) == 0)
	{
		sError.Format("Comms Error - Purge [PurgeComm] : %i [0x%X]",GetLastError(),GetLastError());
		OutputString(sError,MESSAGETYPE_ERROR);
	}

CATCHCATCH("commSerial::Purge()");

	if(bExceptionFlag == EXEPT_CONTINUE)
	{}
	if(bExceptionFlag == EXEPT_ABORT)
		nuke();

	return;
}