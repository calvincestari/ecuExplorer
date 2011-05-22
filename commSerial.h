/*/
	commSerial.h (2005.06.06)
/*/
#pragma once

#include <wfc.h>

class commSerial
{
public:
	CString sCommPort;
	int	iCommPort;
	HANDLE hCommPort;
	_DCB commDCB;
	_COMMTIMEOUTS commTIMEOUTS;
	int iBaudRate;
	int iXonLim;
	int iXoffLim;
	int iReadIntervalTimeout;
	int iReadTotalTimeoutConstant;
	int iWriteTotalTimeoutConstant;

public:
	commSerial();
	~commSerial();

	long DumpHex(unsigned char* cBuffer,unsigned short usBufferLength,LPCTSTR szDirection);
	unsigned short Read(unsigned char* cBuffer,unsigned short usBytesToRead);
	void Purge();
	long Start();
	long Stop();
	long Write(unsigned char* cBuffer,unsigned short usMessageLength);
};