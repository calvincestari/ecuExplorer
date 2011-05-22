#include <wfc.h>
#pragma hdrstop

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

#if defined( _DEBUG )
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

COverlapped::COverlapped()
{
   m_Overlapped.Internal     = 0;
   m_Overlapped.InternalHigh = 0;
   m_Overlapped.Offset       = 0;
   m_Overlapped.OffsetHigh   = 0;;
   m_Overlapped.hEvent       = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

COverlapped::COverlapped( const OVERLAPPED * source )
{
   m_Overlapped.Internal     = source->Internal;
   m_Overlapped.InternalHigh = source->InternalHigh;
   m_Overlapped.Offset       = source->Offset;
   m_Overlapped.OffsetHigh   = source->OffsetHigh;
   m_Overlapped.hEvent       = source->hEvent;
}

COverlapped::~COverlapped()
{
   if ( m_Overlapped.hEvent != NULL )
   {
      ::CloseHandle( m_Overlapped.hEvent );
   }

   m_Overlapped.Internal     = 0;
   m_Overlapped.InternalHigh = 0;
   m_Overlapped.Offset       = 0;
   m_Overlapped.OffsetHigh   = 0;;
   m_Overlapped.hEvent       = NULL;
}

void COverlapped::SetEvent( HANDLE new_event_handle )
{
   if ( m_Overlapped.hEvent != NULL )
   {
      ::CloseHandle( m_Overlapped.hEvent );
   }

   m_Overlapped.hEvent = new_event_handle;
}
