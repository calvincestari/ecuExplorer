// Error.cpp: implementation of the CError class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Error.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CError::CError()
{
	msg = "No Error";
	bDisplayError = true;
	errorcount = 0;
}

CError::~CError()
{
}

void CError::ReportError()
{
	if (bDisplayError)
	{
		printf(msg.c_str());
//		MessageBox(NULL,msg,NULL,MB_OK);
	}
	else
		OutputDebugString(msg.c_str());

	errorcount++;
}

void CError::ReportSysError()
{
	LPTSTR lpMsgBuf;
 
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	msg = lpMsgBuf;
	if (bDisplayError)
	{
		printf(msg.c_str());
//		MessageBox(NULL,msg,"Error",MB_OK);
	}

	errorcount++;
	OutputDebugString((msg+"\n").c_str());
}
