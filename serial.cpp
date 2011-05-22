#include "StdAfx.h"
#include ".\serial.h"
#include "error.h"

serial::serial(void)
{
	defaulttimeout = 2000;
	baudrate=38400;
	portname="COM1:";
	portnum=1;
	Error.bDisplayError = false; //don't turn this on until we've got a good port
	hCom = NULL;
}

serial::~serial(void)
{
	if (hCom)
	{
		// get rid of any pending tx

		if(!FlushFileBuffers(hCom))
		{
			OnPortError();	
			Error.ReportSysError();
		}

		// close the port
		
		if (!CloseHandle(hCom)) 
		{
			OnPortError();
			Error.ReportSysError();
		}
	}
}

bool serial::is_active()
{
	return (hCom != NULL);
}

void serial::set_port(string port)
{
	portname = port;
}

void serial::set_baud(int baud)
{
	baudrate = baud;
	if (hCom)
	{
		// need to re-init at new baud rate
		flush();
		init();
	}
}

void serial::set_default_timeout(int to)
{
	defaulttimeout = to;
}

bool serial::write(void *d,int n)
{
	unsigned long nBytesWritten;

	if (!hCom)
		return false;

	if(!WriteFile(hCom,d,n,&nBytesWritten,NULL)) 
	{
		Error.ReportSysError();
		OnPortError();
		return false;
	}
	return true;
}

unsigned int serial::get_status()
{
	DWORD ModemStat=0;
	GetCommModemStatus(hCom,&ModemStat);
	return ModemStat;
}

bool serial::set_status(unsigned int fn)
{
	if(!hCom) return false;

	return (TRUE == EscapeCommFunction(hCom,fn));
}

bool serial::read(void *rdata,int datalen)
{
	return read_timeout(rdata,datalen,defaulttimeout);
}

// waits patiently to receive datalen chars 
// if it doesn't get one every timeout milliseconds, it times out and returns FALSE
bool serial::read_timeout(void *rdata,int datalen,int timeout) 
{
	int curlen=0,wr;
	unsigned long nBytesRead;
	BOOL waiting = FALSE;
	int to = timeout;
	string t;

	while (curlen < datalen) 
	{
		if((wr = ReadFile(hCom,(unsigned char*)rdata+curlen,datalen-curlen,&nBytesRead,NULL)) == 0) 
		{
			Error.ReportSysError();
			OnPortError();
			return false; // error!
		}

		if (nBytesRead == 0) 
		{
		}
		else 
		{
			to = timeout; // reset timout period
			waiting = FALSE;
			curlen += nBytesRead;
		}

		if (curlen < datalen)
		{
			if (to > 0)
			{
				Sleep(10); // wait timeout msec
				to -= 10;
			}
			else
			{
				// printf("timeout (rx: %d of %d)\n",curlen,datalen);
				return false; // timed out
			}
		}
	}
	return true;
}

bool serial::read_and_echo(void *rdata,int datalen)
{
	return read_timeout_and_echo(rdata,datalen,defaulttimeout);
}


bool serial::read_timeout_and_echo(void *rdata,int datalen,int timeout) 
{
	int curlen=0,wr;
	unsigned long nBytesRead;
	BOOL waiting = FALSE;
	int to = timeout;
	string t;

	while (curlen < datalen) 
	{
		if((wr = ReadFile(hCom,(unsigned char*)rdata+curlen,1,&nBytesRead,NULL)) == 0) 
		{
			Error.ReportSysError();
			OnPortError();
			return false; // error!
		}

		if (nBytesRead == 0) 
		{
		}
		else 
		{
			to = timeout; // reset timout period
			waiting = FALSE;
			write((unsigned char*)rdata+curlen,nBytesRead); // echo
			curlen += nBytesRead;
		}

		if (curlen < datalen)
		{
			if (to > 0)
			{
				Sleep(10); // wait timeout msec
				to -= 10;
			}
			else
			{
				return false; // timed out
			}
		}
	}
	return true;
}
vector <string> serial::port_names()
{
	vector <string> portnames;
	HKEY hKey;
	int index = 0;
	char ValueName[256];
	unsigned long szValueName;
	unsigned char Data[256];
	unsigned long szData;
	long r;

	if (RegOpenKey(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\SERIALCOMM",&hKey) == ERROR_SUCCESS)
	{
		szValueName = 256;
		szData = 256;
		while ((r = RegEnumValue(hKey,index,&ValueName[0],&szValueName,NULL,NULL,&Data[0],&szData)) == ERROR_SUCCESS)
		{
			portnames.push_back((char*)Data);
			index++;
			szValueName = 256;
			szData = 256;
		}
	}
	return portnames;
}

void serial::OnPortError()
{
	COMSTAT cs;
	DWORD errors;
	string t;

	if (!hCom)
		return;

	if (!ClearCommError(hCom,&errors,&cs))
	{
		LPTSTR lpMsgBuf;
 
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			::GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);

		t = "CCE error [";
		t += lpMsgBuf;
		t += "]\n";
		OutputDebugString(t.c_str());
	}
}

void serial::close()
{
	if (hCom)
	{
		if (!CloseHandle(hCom))
		{
			Error.ReportSysError();
			return; // can't reconfig until we can close...
		}
		hCom = NULL;
	}
}

bool serial::open() 
{
	hCom = CreateFile(portname.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,    /* comm devices must be opened w/exclusive-access */
		NULL, /* no security attrs */
		OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
		0,    /* not overlapped I/O */
		NULL  /* hTemplate must be NULL for comm devices */
		);
	
	if (hCom == INVALID_HANDLE_VALUE) 
	{
		Error.ReportSysError();
		hCom = NULL;
		return false;
	}
	else 
		return init();
}

bool serial::init() 
{	
	DCB dcb;

	if (!hCom)
		return false;

	if (!SetupComm(hCom,16384,16384)) // set i and o buff sz
	{
		OnPortError();
		Error.ReportSysError();
		return false;
	}
	
	if (!GetCommState(hCom, &dcb))
	{
		OnPortError();
		Error.ReportSysError();
		return false;
	}
	
	/* Fill in the DCB*/
	/*
	typedef struct _DCB { // dcb 
	DWORD DCBlength;           // sizeof(DCB) 
	DWORD BaudRate;            // current baud rate 
	DWORD fBinary: 1;          // binary mode, no EOF check 
	DWORD fParity: 1;          // enable parity checking 
	DWORD fOutxCtsFlow:1;      // CTS output flow control 
	DWORD fOutxDsrFlow:1;      // DSR output flow control 
	DWORD fDtrControl:2;       // DTR flow control type 
	DWORD fDsrSensitivity:1;   // DSR sensitivity 
	DWORD fTXContinueOnXoff:1; // XOFF continues Tx 
	DWORD fOutX: 1;            // XON/XOFF out flow control 
	DWORD fInX: 1;             // XON/XOFF in flow control 
	DWORD fErrorChar: 1;       // enable error replacement 
	DWORD fNull: 1;            // enable null stripping 
	DWORD fRtsControl:2;       // RTS flow control 
	DWORD fAbortOnError:1;     // abort reads/writes on error 
	DWORD fDummy2:17;          // reserved 
	WORD wReserved;            // not currently used 
	WORD XonLim;               // transmit XON threshold 
	WORD XoffLim;              // transmit XOFF threshold 
	BYTE ByteSize;             // number of bits/byte, 4-8 
	BYTE Parity;               // 0-4=no,odd,even,mark,space 
	BYTE StopBits;             // 0,1,2 = 1, 1.5, 2 
	char XonChar;              // Tx and Rx XON character 
	char XoffChar;             // Tx and Rx XOFF character 
	char ErrorChar;            // error replacement character 
	char EofChar;              // end of input character 
	char EvtChar;              // received event character 
	WORD wReserved1;           // reserved; do not use 
	} DCB; 
	*/
	
	dcb.BaudRate = baudrate;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE; // turn on initially
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = TRUE;
	dcb.fOutX = FALSE; // disable other flow control methods
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = FALSE;
	dcb.XonLim = 2048;
	dcb.XoffLim = 512;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	
	
	if (!SetCommState(hCom, &dcb))
	{
		OnPortError();
		Error.ReportSysError();
		return false;
	}
		
	COMMTIMEOUTS cto;
	
	// this is a "secret code" to tell the system
	// to return immediately with what it has on reads
	
	cto.ReadIntervalTimeout = MAXDWORD;
	cto.ReadTotalTimeoutConstant = 0;
	cto.ReadTotalTimeoutMultiplier = 0;
	
	/*
	// this "code" tells the system to not use total timeouts
	// on writes
	cto.WriteTotalTimeoutConstant = 0;
	cto.WriteTotalTimeoutMultiplier = 0;
	*/
	
	// use some timeout so that we don't get hung up talking to empty serial ports
	cto.WriteTotalTimeoutConstant = 0;
	cto.WriteTotalTimeoutMultiplier = 50;
	
	if (!SetCommTimeouts(hCom, &cto))
	{
		OnPortError();
		Error.ReportSysError();
		return false;
	}
	
	if (!SetCommMask(hCom,EV_CTS | EV_DSR)) 
	{
		OnPortError();
		Error.ReportSysError();
		return false;
	}
	
	if(!FlushFileBuffers(hCom)) // get rid of any pending tx
	{
		OnPortError();
		Error.ReportSysError();
		return false;
	}
	return true;
}

bool serial::flush() 
{ 
	char rx[100];
	unsigned long nBytesRead=1;
	int wr;

	if (!hCom)
		return false;
	
	while (nBytesRead != 0) 
	{
		if((wr = ReadFile(hCom,rx,100,&nBytesRead,NULL)) == 0) 
		{
			OnPortError();
			Error.ReportError();
			return false; // error!
		}
	}
	return true;
}
