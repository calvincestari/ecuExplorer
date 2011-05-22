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

CRITICAL_SECTION g_ServiceCriticalSection;

CService *CService::m_Service_p = 0;

CService::CService( LPTHREAD_START_ROUTINE thread_start_routine, DWORD controls_accepted, DWORD wait_hint )
{
   ASSERT( thread_start_routine != NULL );

   TRACE1( "Main tid = %#lx\n", ::GetCurrentThreadId() );

   ::InitializeCriticalSection( &g_ServiceCriticalSection );

   m_ThreadStartRoutine  = thread_start_routine;
   m_ThreadHandle        = NULL;
   m_ExitEventHandle     = NULL;
   m_ServiceStatusHandle = 0;
   m_ErrorCode           = NO_ERROR;
   m_Running             = FALSE;
   m_Paused              = FALSE;
   m_Exiting             = FALSE;
   m_Debugging           = 0;
   m_ControlsAccepted    = controls_accepted;
   m_WaitHint            = wait_hint;
   m_CurrentState        = SERVICE_START_PENDING;
   m_Service_p           = this;
}

CService::~CService( void )
{
   ::DeleteCriticalSection( &g_ServiceCriticalSection );

   if ( m_ExitEventHandle != NULL )
   {
      ::CloseHandle( m_ExitEventHandle );
      m_ExitEventHandle = NULL;
   }

   if ( m_ThreadHandle != NULL )
   {
      ::CloseHandle( m_ThreadHandle );
      m_ThreadHandle = NULL;
   }
}

BOOL CService::Initialize( LPCTSTR name_of_service )
{
   /*
   ** Thank you Rob Williams (CI$ 73740,774) for fixing this function
   */

   ASSERT( name_of_service != NULL );

   BOOL return_value = TRUE;

   // initialize m_ServiceTable

   ::strncpy( m_ServiceName, name_of_service, SERVICE_NAME_LEN );

   m_ServiceTable[ 0 ].lpServiceName = m_ServiceName;
   m_ServiceTable[ 0 ].lpServiceProc = CService::ServiceMain;
   m_ServiceTable[ 1 ].lpServiceName = 0;
   m_ServiceTable[ 1 ].lpServiceProc = 0;

   // initiate conversation with SCM

   if ( ::StartServiceCtrlDispatcher( m_ServiceTable ) == FALSE )
   {
      m_ErrorCode = ::GetLastError();
      return_value = FALSE;
      LogEvent();
   }

   return( return_value );
}

void CService::AssertValid( void ) const
{
   ASSERT( m_Exiting             != TRUE );
   ASSERT( m_ExitEventHandle     != NULL );
   ASSERT( m_ServiceStatusHandle != 0    );
   ASSERT( m_ThreadHandle        != NULL );
   ASSERT( m_Service_p           != 0    );
}

void CALLBACK CService::ServiceMain( DWORD argc, LPTSTR *argv )
{
   // entry point for service called by SCM when service is started

   HANDLE thread_handle = NULL;

   ASSERT( m_Service_p != NULL );

   TRACE1( "ServiceMain tid = %#lx\n", ::GetCurrentThreadId() );

   ::EnterCriticalSection( &g_ServiceCriticalSection );
   m_Service_p->m_ServiceStatusHandle = ::RegisterServiceCtrlHandler( TEXT( m_Service_p->m_ServiceName ), ServiceControlManagerHandler );
   ::LeaveCriticalSection( &g_ServiceCriticalSection );

   if ( m_Service_p->m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE) 0 )
   {
      m_Service_p->m_ErrorCode = ::GetLastError();
      m_Service_p->LogEvent();
      m_Service_p->Exit();
   }
   else
   {
      if ( ! m_Service_p->SendStatusToServiceControlManager( SERVICE_START_PENDING, NO_ERROR, 1, m_Service_p->m_WaitHint ) )
      {
         goto EXIT_GOTO;
      }

      ::EnterCriticalSection( &g_ServiceCriticalSection );
      m_Service_p->m_ExitEventHandle = ::CreateEvent( 0, TRUE, FALSE, 0 );
      ::LeaveCriticalSection( &g_ServiceCriticalSection );

      if ( m_Service_p->m_ExitEventHandle == NULL )
      {
         m_Service_p->m_ErrorCode = ::GetLastError();
         m_Service_p->LogEvent();
         m_Service_p->Exit();
      }
      else
      {
         if ( ! m_Service_p->SendStatusToServiceControlManager( SERVICE_START_PENDING, NO_ERROR, 2, m_Service_p->m_WaitHint ) )
         {
            goto EXIT_GOTO;
         }

         m_Service_p->ParseCommandLineParameters( argc, argv );

         if ( ! m_Service_p->SendStatusToServiceControlManager( SERVICE_START_PENDING, NO_ERROR, 3, m_Service_p->m_WaitHint ) )
         {
            goto EXIT_GOTO;
         }

         m_Service_p->OnPrepareServiceThread();

         thread_handle = ::CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)
                                               m_Service_p->m_ThreadStartRoutine,
                                               m_Service_p, 0,
                                               &m_Service_p->m_ThreadId );

         ::EnterCriticalSection( &g_ServiceCriticalSection );
         m_Service_p->m_ThreadHandle = thread_handle;
         ::LeaveCriticalSection( &g_ServiceCriticalSection );

         if ( m_Service_p->m_ThreadHandle == NULL )
         {
            m_Service_p->m_ErrorCode = ::GetLastError();
            m_Service_p->LogEvent();
            m_Service_p->Exit();
         }
         else
         {
            ::EnterCriticalSection( &g_ServiceCriticalSection );
            m_Service_p->m_Running = TRUE;
            ::LeaveCriticalSection( &g_ServiceCriticalSection );

            if ( m_Service_p->SendStatusToServiceControlManager( SERVICE_RUNNING ) == FALSE )
            {
               return;
            }

            ::EnterCriticalSection( &g_ServiceCriticalSection );
            m_Service_p->m_CurrentState = SERVICE_RUNNING;
            ::LeaveCriticalSection( &g_ServiceCriticalSection );

            ::WaitForSingleObject( m_Service_p->m_ExitEventHandle, INFINITE );
         }

EXIT_GOTO:

         // notify SCM that service has stopped

         ASSERT( m_Service_p != 0 );

         if ( m_Service_p->m_ServiceStatusHandle != 0 )
         {
            m_Service_p->SendStatusToServiceControlManager( SERVICE_STOPPED, m_Service_p->m_ErrorCode );
         }
      }
   }
}

void CALLBACK CService::ServiceControlManagerHandler( DWORD control_code )
{
   // entry point for service called by SCM after service is started

   ASSERT( m_Service_p != 0 );

   switch( control_code )
   {
      case SERVICE_CONTROL_STOP:

         TRACE1( "Handling SERVICE_CONTROL_STOP tid %#lx\n", ::GetCurrentThreadId() );

         m_Service_p->SendStatusToServiceControlManager( SERVICE_STOP_PENDING, NO_ERROR, 1, m_Service_p->m_WaitHint );

         ::EnterCriticalSection( &g_ServiceCriticalSection );
         m_Service_p->m_Running = FALSE;
         m_Service_p->m_CurrentState = SERVICE_STOPPED;
         ::LeaveCriticalSection( &g_ServiceCriticalSection );

         m_Service_p->OnStop();
         m_Service_p->Exit();

         return;

      case SERVICE_CONTROL_PAUSE:

         TRACE1( "Handling SERVICE_CONTROL_PAUSE tid %#lx\n", ::GetCurrentThreadId() );

         if ( m_Service_p->m_Running == TRUE && m_Service_p->m_Paused != TRUE )
         {
            if ( m_Service_p->SendStatusToServiceControlManager( SERVICE_PAUSE_PENDING, NO_ERROR, 1, m_Service_p->m_WaitHint ) == FALSE )
            {
               return;
            }

            ::EnterCriticalSection( &g_ServiceCriticalSection );
            m_Service_p->m_Paused = TRUE;
            ::LeaveCriticalSection( &g_ServiceCriticalSection );

            m_Service_p->OnPause();
            ::SuspendThread( m_Service_p->m_ThreadHandle );

            ::EnterCriticalSection( &g_ServiceCriticalSection );
            m_Service_p->m_CurrentState = SERVICE_PAUSED;
            ::LeaveCriticalSection( &g_ServiceCriticalSection );
         }

         break;

      case SERVICE_CONTROL_CONTINUE:

         TRACE1( "Handling SERVICE_CONTROL_CONTINUE tid %#lx\n", ::GetCurrentThreadId() );

         if ( m_Service_p->m_Running == TRUE && m_Service_p->m_Paused == TRUE )
         {
            if ( m_Service_p->SendStatusToServiceControlManager( SERVICE_CONTINUE_PENDING, NO_ERROR, 1, m_Service_p->m_WaitHint ) == FALSE )
            {
               return;
            }

            ::EnterCriticalSection( &g_ServiceCriticalSection );
            m_Service_p->m_Paused = FALSE;
            ::LeaveCriticalSection( &g_ServiceCriticalSection );

            ::ResumeThread( m_Service_p->m_ThreadHandle );
            m_Service_p->OnContinue();

            ::EnterCriticalSection( &g_ServiceCriticalSection );
            m_Service_p->m_CurrentState = SERVICE_RUNNING;
            ::LeaveCriticalSection( &g_ServiceCriticalSection );
         }

         break;

      case SERVICE_CONTROL_INTERROGATE:

         TRACE1( "Handling SERVICE_CONTROL_INTERROGATE tid %#lx\n", ::GetCurrentThreadId() );
         break;

      case SERVICE_CONTROL_SHUTDOWN:

         TRACE1( "Handling SERVICE_CONTROL_SHUTDOWN tid %#lx\n", ::GetCurrentThreadId() );
         break;

      default:

         TRACE3( "Handling user-defined control code %#ld (%ld) tid %#lx\n", control_code, control_code, ::GetCurrentThreadId() );
         m_Service_p->OnControlCode( control_code );

         break;
   }

   m_Service_p->SendStatusToServiceControlManager( m_Service_p->m_CurrentState );
}

void CService::ParseCommandLineParameters( DWORD argc , LPTSTR *argv )
{
   DWORD argument_number = 1;

   // default implementation
   // parse command line parameters passed via SCM through ServiceMain

   while( argument_number < argc )
   {
      if ( argv[ argument_number ][ 0 ] == '-' || argv[ argument_number][ 0 ] == '/' )
      {
         switch( argv[ argument_number ][ 1 ] )
         {
            case 'd':
            case 'D':

               ::EnterCriticalSection( &g_ServiceCriticalSection );
               m_Debugging = 1;
               ::LeaveCriticalSection( &g_ServiceCriticalSection );

               break;

            case 'i':
            case 'I':

               char message_string[ 80 ];

               ::sprintf( message_string, "pid %#lx %ld", ::GetCurrentProcessId(), ::GetCurrentProcessId() );
               ::MessageBox( NULL, message_string, m_ServiceName, MB_OK );

               break;

            default:

               break;
        }
      }

      argument_number++;
   }

}

void CService::OnControlCode( DWORD /* dwControlCode */ )
{
   // default implementation
   // handle user-defined control codes (128 - 255 inclusive)
}

void CService::OnStop( void )
{
   // default implementation
}

void CService::OnPrepareServiceThread( void )
{
   // default implementation
   // allows for initialization prior to creating service thread
}

void CService::OnPause( void )
{
   CEventLog log( m_ServiceName );
   log.ReportInformation( "Service Paused" );
}

void CService::OnContinue( void )
{
   CEventLog log( m_ServiceName );
   log.ReportInformation( "Service Resumed" );
}

BOOL CService::SendStatusToServiceControlManager( DWORD current_state, 
                                                  DWORD win32_exit_code,
                                                  DWORD check_point,
                                                  DWORD wait_hint,
                                                  DWORD service_specific_code )
{
   BOOL return_value = FALSE;

   SERVICE_STATUS service_status;

   ::ZeroMemory( &service_status, sizeof( service_status ) );

   // initialize service_status and send it to SCM

   if ( current_state == SERVICE_START_PENDING )
   {
      service_status.dwControlsAccepted = 0;
   }
   else
   {
      service_status.dwControlsAccepted = m_ControlsAccepted;
   }

   if ( service_specific_code == 0 )
   {
      service_status.dwWin32ExitCode = win32_exit_code;
   }
   else
   {
      service_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
   }

   service_status.dwServiceType             = SERVICE_WIN32_OWN_PROCESS;
   service_status.dwCurrentState            = current_state;
   service_status.dwServiceSpecificExitCode = service_specific_code;
   service_status.dwCheckPoint              = check_point;
   service_status.dwWaitHint                = wait_hint;

#if defined( _DEBUG )
   DumpStatus( &service_status );
#endif

   return_value = ::SetServiceStatus( m_ServiceStatusHandle, &service_status );

   if ( return_value == FALSE )
   {
      m_ErrorCode = ::GetLastError();
      LogEvent();
      Exit();
   }

   return( return_value );
}

void CService::Exit( void )
{
   ASSERT_VALID( this );

   ::EnterCriticalSection( &g_ServiceCriticalSection );

   m_Running      = FALSE;
   m_CurrentState = SERVICE_STOPPED;
   m_Exiting      = TRUE;

   ::LeaveCriticalSection( &g_ServiceCriticalSection );

   if ( m_ExitEventHandle != NULL )
   {
      ::SetEvent( m_ExitEventHandle );
   }
}

#pragma warning( disable : 4100 )

void CService::LogEvent( WORD event_type, LPTSTR message_string, DWORD error_code )
{
   CEventLog log( m_ServiceName );

   LPTSTR strings[ 1 ];

   strings[ 0 ] = message_string;

   log.Report( (CEventLog::EventType) event_type, 0, 0, 1, (const char **) strings );
}

#pragma warning( default : 4100 )

#if defined( _DEBUG )

void CService::DumpStatus( SERVICE_STATUS *pStatus ) const
{
   TRACE( "\ncalling SetServiceStatus with:\n" );

   switch( pStatus->dwServiceType )
   {
      case SERVICE_WIN32_OWN_PROCESS:

         TRACE( "dwServiceType SERVICE_WIN32_OWN_PROCESS\n" );
         break;

      case SERVICE_WIN32_SHARE_PROCESS:

         TRACE( "dwServiceType SERVICE_WIN32_SHARE_PROCESS\n" );
         break;
   }

   TRACE1( "dwControlsAccepted %#lx:\n", pStatus->dwControlsAccepted );
   TRACE( "   SERVICE_CONTROL_INTERROGATE\n" );

   if ( pStatus->dwControlsAccepted & SERVICE_ACCEPT_STOP )
   {
      TRACE( "   SERVICE_CONTROL_STOP\n" );
   }

   if ( pStatus->dwControlsAccepted & SERVICE_ACCEPT_PAUSE_CONTINUE )
   {
      TRACE( "   SERVICE_CONTROL_PAUSE\n" );
      TRACE( "   SERVICE_CONTROL_CONTINUE\n" );
   }

   if ( pStatus->dwControlsAccepted & SERVICE_ACCEPT_SHUTDOWN )
   {
      TRACE( "   SERVICE_CONTROL_SHUTDOWN\n" );
   }

   switch( pStatus->dwCurrentState )
   {
      case SERVICE_STOPPED:

         TRACE( "dwCurrentState SERVICE_STOPPED\n" );
         break;

      case SERVICE_START_PENDING:

         TRACE( "dwCurrentState SERVICE_START_PENDING\n" );
         break;

      case SERVICE_STOP_PENDING:

         TRACE( "dwCurrentState SERVICE_STOP_PENDING\n" );
         break;

      case SERVICE_RUNNING:

         TRACE( "dwCurrentState SERVICE_RUNNING\n" );
         break;

      case SERVICE_CONTINUE_PENDING:

         TRACE( "dwCurrentState SERVICE_CONTINUE_PENDING\n" );
         break;

      case SERVICE_PAUSE_PENDING:

         TRACE( "dwCurrentState SERVICE_PAUSE_PENDING\n" );
         break;

      case SERVICE_PAUSED:

         TRACE( "dwCurrentState SERVICE_PAUSED\n" );
         break;

      default:

         TRACE2( "dwCurrentState %#lx (%ld)\n", pStatus->dwCurrentState, pStatus->dwCurrentState );
         break;
   }

   if ( pStatus->dwWin32ExitCode == ERROR_SERVICE_SPECIFIC_ERROR )
   {
      TRACE2( "dwServiceSpecificExitCode %#lx (%ld)\n", pStatus->dwServiceSpecificExitCode, pStatus->dwServiceSpecificExitCode );
   }
   else
   {
      TRACE2( "dwWin32ExitCode %#lx (%ld)\n", pStatus->dwWin32ExitCode, pStatus->dwWin32ExitCode );
   }

   TRACE1( "dwCheckPoint %ld\n", pStatus->dwCheckPoint );
   TRACE1( "dwWaitHint %ld\n\n", pStatus->dwWaitHint );
}

#endif // _DEBUG
