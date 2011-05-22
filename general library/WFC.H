#ifndef WINDOWS_FOUNDATION_CLASSES

/*
** Author: Samuel R. Blackburn
** CI$: 76300,326
** Internet: sammy@sed.csc.com
**
** You can use it any way you like as long as you don't try to sell it.
**
** Any attempt to sell WFC in source code form must have the permission
** of the original author. You can produce commercial executables with
** WFC but you can't sell WFC.
**
** Copyright, 1995, Samuel R. Blackburn
*/

#define WINDOWS_FOUNDATION_CLASSES

/*
** Include all the needed MFC stuff
*/

#include <afx.h>
#include <afxwin.h>
#include <afxcoll.h>
#include <winperf.h>
#include <winsock2.h>
#include <ras.h>
#include <raserror.h>
#include <winuser.h>
#include <lm.h>
#include <lzexpand.h>

#define CARRIAGE_RETURN 13
#define LINE_FEED       10

/*
** An easy way to refer to colors . . .
*/

#define BLACK     RGB( 0x00, 0x00, 0x00 )
#define RED       RGB( 0xFF, 0x00, 0x00 )
#define GREEN     RGB( 0x00, 0xFF, 0x00 )
#define YELLOW    RGB( 0xFF, 0xFF, 0x00 )
#define DKGRAY    RGB( 0x7F, 0x7F, 0x7F )
#define DARK_GRAY DKGRAY
#define LTGRAY    RGB( 0xC0, 0xC0, 0xC0 )
#define LIGHT_GRAY LTGRAY
#define DKRED     RGB( 0x7F, 0x00, 0x00 )
#define DARK_RED  DKRED
#define MAGENTA   RGB( 0xFF, 0x00, 0xFF )
#define PURPLE    RGB( 0x7F, 0x00, 0x7F )
#define BLUE      RGB( 0x00, 0x00, 0xFF )
#define DKBLUE    RGB( 0x00, 0x00, 0x7F )
#define DARK_BLUE DKBLUE
#define INDIGO    RGB( 0x00, 0xFF, 0xFF )
#define DKINDIGO  RGB( 0x00, 0x7F, 0x7F )
#define DARK_INDIGO DKINDIGO
#define WHITE     RGB( 0xFF, 0xFF, 0xFF )
#define DKGREEN   RGB( 0x00, 0x7F, 0x00 )
#define DARK_GREEN DKGREEN
#define DKTYELLOW RGB( 0x7F, 0x7F, 0x00 )
#define DARK_YELLOW DKYELLOW
#define LTYELLOW  RGB( 0xFF, 0xFF, 0x7F )
#define LIGHT_YELLOW LTYELLOW

void ASCII_to_UNICODE( LPCSTR ansi_string, LPWSTR unicode_string );
void UNICODE_to_ASCII( LPCWSTR unicode_string, LPSTR ansi_string );
void Convert_NERR_Code_to_String( DWORD error_code, CString& error_message );

/*
** Structure Helpers
*/

#include <Structs.hpp>

/*
** The Registration Database API
*/

#include <CRegisty.hpp>

/*
** Event Logging
*/

#include <CEvntLog.hpp>

#include <CUNC.hpp>
#include <CURL.hpp>

/*
** Remote Access Service
*/

#include <CRAS.hpp>

#include <DumyFile.hpp>
#include <Serial.hpp>

/*
** Simple Sockets
*/

#include <sockets.hpp>
#include <lsockets.hpp>
#include <tsockets.hpp>
#include <SockFile.hpp>

/*
** Named Pipes
*/

#include <cnmpipe.hpp>

/*
** Compressed Files
*/

#include <lzfile.hpp>

#include <CNetwork.hpp>
#include <CServer.hpp>
#include <CNetRsrc.hpp>
#include <CNetSess.hpp>
#include <CNetWkst.hpp>
#include <CNetFile.hpp>
#include <CNetConn.hpp>
#include <CNetShar.hpp>
#include <Ovrlpd.hpp>
#include <dcb.hpp>
#include <CService.hpp>
#include <CSvcMgr.hpp>
//s#include <CWait.hpp>
#include <CBubble.hpp>
#include <DrawObj.hpp>
#include <DrawGrid.hpp>
#include <CSquigl.hpp>
#include <CTape.hpp>

#pragma warning( disable : 4699 )

#endif // WINDOWS_FOUNDATION_CLASSES
