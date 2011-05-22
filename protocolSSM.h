/*/
	protocolSSM.h (2005.06.28)
/*/
#pragma once

#include <wfc.h>
#include <sys/timeb.h>

#include "commSerial.h"
#include "definitionSSM.h"
#include "definitionLocal.h"

class protocolSSM : public commSerial
{
public:
	BOOL bThreadPoll;
	HANDLE semaphoreRead;
	HANDLE semaphoreWrite;
	HANDLE hThreadReceive;
	int iTraceFile;
	CString sECU;
	CString sSupported;
	CString sTemp;
	unsigned char cReturn_WriteSingleAddress;
	unsigned char cReturn_ReadSingleAddress;
	unsigned long ulDTCBase;
	CPtrList listLiveQuery;
	CRITICAL_SECTION csGlobal_LiveQuery;
	HWND hWndRealtime;
	HWND hWndDTC;
	CPtrList listCSVCapture;
	void* lpParent;
	_timeb tbSampleRate;
	HANDLE semaphoreLive;

public:
	protocolSSM();
	~protocolSSM();

	long AddLiveQueryItem(LPSTRUCT_LIVEBITITEM lpLiveBitItem,BOOL bDependency = FALSE);
	long AddCSVItem(LPSTRUCT_LIVEBITITEM lpLiveBitItem);
	LPSTRUCT_LIVEQUERYITEM GetLiveQueryItem(LPSTRUCT_LIVEBITITEM lpLiveBitItem);
	unsigned char GenerateChecksum(unsigned char* cBuffer,unsigned short usBufferLength);
	long QueryIgnitionTiming();
	long QueryIdleSpeed(unsigned long ulAddress);
	long RemoveLiveQueryItem(LPSTRUCT_LIVEBITITEM lpLiveBitItem);
	long RemoveCSVItem(LPSTRUCT_LIVEBITITEM lpLiveBitItem);
	long ResetECU();
	long RetardIgnitionTiming(char cValue);
	long SetIdleSpeed(unsigned long ulAddress,char cValue);
	long Start();
	long Stop();

	long QueryECUType();
	long ReadAddressSingle(unsigned long ulAddress);
	long WriteAddressSingle(unsigned long ulAddress,unsigned char cValue);
	long LiveQuery();
	long DTCQuery(unsigned long ulStart,unsigned long ulEnd);

	long ProcessMessage(LPSTRUCT_MSG_SSM_QUERYRESPONSE lpMessage,unsigned short usMessageLength);
	long ProcessMessage(LPSTRUCT_MSG_SSM_READADDRESSSINGLERESPONSE lpMessage,unsigned short usMessageLength);
	long ProcessMessage(LPSTRUCT_MSG_SSM_WRITEADDRESSSINGLERESPONSE lpMessage,unsigned short usMessageLength);
	long ProcessMessage(LPSTRUCT_MSG_SSM_LIVEQUERYRESPONSE lpMessage,unsigned short usMessageLength);
	long ProcessMessage(LPSTRUCT_MSG_SSM_DTCQUERYRESPONSE lpMessage,unsigned short usMessageLength);
};