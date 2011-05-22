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
**
** Revision History
** 20/01/97 - ElectroDynamics Allowed service to be interactive 
**						SERVICE_INTERACTIVE_PROCESS 
**
** 21/01/02 - ElectroDynamics (Calvin Cestari)
**				1) Changed 'Install' to accept 2 more parameters, type of service start and list of dependencies.
**				   Both these new parameters have defaults which would resort to normal (demand) start and no dependencies.
*/

#if defined( _DEBUG )
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CServiceControlManager::CServiceControlManager()
{
   m_Initialize();
}

CServiceControlManager::~CServiceControlManager()
{
   Close();

   if ( m_Buffer != NULL )
   {
      delete [] m_Buffer;
      m_Buffer     = NULL;
      m_BufferSize = 0;
   }
}

void CServiceControlManager::Close( void )
{
   if ( m_ManagerHandle != NULL )
   {
      if ( ::CloseServiceHandle( m_ManagerHandle ) == FALSE )
      {
         m_ErrorCode = ::GetLastError();
      }

      m_ManagerHandle = NULL;
   }
}

BOOL CServiceControlManager::Continue( LPCTSTR service_name )
{
   ASSERT( service_name != NULL );

   if ( m_ManagerHandle == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
	  return( FALSE );
   }

   if ( service_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
	  return( FALSE );
   }

   SC_HANDLE service_handle = ::OpenService( m_ManagerHandle, service_name, SERVICE_PAUSE_CONTINUE );

   if ( service_handle == (SC_HANDLE) NULL )
   {
      m_ErrorCode = ::GetLastError();
	  return( FALSE );
   }

   SERVICE_STATUS service_status;

   ::ZeroMemory( &service_status, sizeof( service_status ) );

   BOOL return_value = ::ControlService( service_handle, SERVICE_CONTROL_CONTINUE, &service_status );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   ::CloseServiceHandle( service_handle );

   return( return_value );
}

BOOL CServiceControlManager::EnumerateStatus( DWORD state, DWORD type )
{
   /*
   ** For GetNext() calls
   */

   if ( m_ManagerHandle == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
      return( FALSE );
   }

   if ( m_Buffer == NULL )
   {
      m_BufferSize = 128 * sizeof( ENUM_SERVICE_STATUS );

      m_Buffer = (ENUM_SERVICE_STATUS *) new BYTE[ m_BufferSize ];

      if ( m_Buffer == NULL )
      {
         m_BufferSize = 0;
         m_ErrorCode = ::GetLastError();
         return( FALSE );
      }
   }

   DWORD number_of_bytes_needed      = 0;
   DWORD number_of_services_returned = 0;
   DWORD resume_handle               = 0;

   if ( ::EnumServicesStatus( m_ManagerHandle,
                              type,
                              state,
                              m_Buffer,
                              m_BufferSize,
                              &number_of_bytes_needed,
                              &number_of_services_returned,
                              &resume_handle ) == TRUE )
   {
      m_CurrentEntryNumber = 0;
      m_NumberOfEntries    = number_of_services_returned;

      return( TRUE );
   }

   m_ErrorCode = ::GetLastError();

   if ( m_ErrorCode == ERROR_MORE_DATA )
   {
      delete [] m_Buffer;

      m_Buffer = (ENUM_SERVICE_STATUS *) new BYTE[ number_of_bytes_needed ];

      if ( m_Buffer != NULL )
      {
         m_BufferSize = number_of_bytes_needed;
      }
      else
      {
         m_ErrorCode = ::GetLastError();
         return( FALSE );
      }

      number_of_bytes_needed      = 0;
      number_of_services_returned = 0;
      resume_handle               = 0;

      if ( ::EnumServicesStatus( m_ManagerHandle,
                                 type,
                                 state,
                                 m_Buffer,
                                 m_BufferSize,
                                &number_of_bytes_needed,
                                &number_of_services_returned,
                                &resume_handle ) == TRUE )
      {
         m_CurrentEntryNumber = 0;
         m_NumberOfEntries    = number_of_services_returned;

         return( TRUE );
      }
      else
      {
         m_CurrentEntryNumber = 0;
         m_NumberOfEntries    = 0;

         m_ErrorCode = ::GetLastError();

         return( FALSE );
      }
   }

   return( FALSE );
}

DWORD CServiceControlManager::GetErrorCode( void ) const
{
   return( m_ErrorCode );
}

BOOL CServiceControlManager::GetNext( CServiceNameAndStatus& status )
{
   if ( m_CurrentEntryNumber < m_NumberOfEntries )
   {
      status.Copy( &m_Buffer[ m_CurrentEntryNumber ] );
      m_CurrentEntryNumber++;
      return( TRUE );
   }

   return( FALSE );
}

BOOL CServiceControlManager::Install( LPCTSTR service_name, LPCTSTR friendly_name, LPCTSTR name_of_executable_file, DWORD category_items, DWORD start_type, LPCTSTR dependency_services)
{
   ASSERT( service_name            != NULL );
   ASSERT( friendly_name           != NULL );
   ASSERT( name_of_executable_file != NULL );

   if ( service_name            == (LPCTSTR) NULL ||
        friendly_name           == (LPCTSTR) NULL ||
        name_of_executable_file == (LPCTSTR) NULL ||
        m_ManagerHandle         == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
      return( FALSE );
   }

   TRACE( "CServiceControlManager::Install()\n" );

   DWORD supported_types = EVENTLOG_ERROR_TYPE       | 
                           EVENTLOG_WARNING_TYPE     |
                           EVENTLOG_INFORMATION_TYPE |
                           EVENTLOG_AUDIT_SUCCESS    |
                           EVENTLOG_AUDIT_FAILURE;

   CEventLog event_log;

   TRACE( "CServiceControlManager::Install(), Creating Application log\n" );

   if ( event_log.CreateApplicationLog( service_name, name_of_executable_file, supported_types, category_items ) != TRUE )
   {
      return( FALSE );
   }

   TRACE( "CServiceControlManager::Install(), Registering Source\n" );

   if ( event_log.RegisterSource( service_name ) != TRUE )
   {
      return( FALSE );
   }

   SC_HANDLE service_handle = (SC_HANDLE) NULL;
   
   TRACE( "CServiceControlManager::Install(), Creating Service\n" );

   service_handle = ::CreateService( m_ManagerHandle,
                                     service_name,
                                     friendly_name,
                                     SERVICE_ALL_ACCESS,
                                     SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                                     start_type,
                                     SERVICE_ERROR_NORMAL,
                                     name_of_executable_file,
                                     NULL,
                                     NULL,
                                     dependency_services,
                                     NULL,
                                     NULL );

   TRACE( "CServiceControlManager::Install(), CreateService() returned\n" );

   if ( service_handle == (SC_HANDLE) NULL )
   {
      TRACE( "CServiceControlManager::Install(), service_handle == NULL\n" );

      m_ErrorCode = ::GetLastError();

      LPVOID message_buffer = (LPVOID) NULL;

      TRACE( "CServiceControlManager::Install(), Calling FormatMessage()\n" );

      ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL,
                       m_ErrorCode,
                       MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
             (LPTSTR) &message_buffer,
                       0,
                       NULL );

      TRACE( "CServiceControlManager::Install(), FormatMessage() returned\n" );

      if ( message_buffer != NULL )
      {
         TCHAR temp_string[ 255 ];

         TRACE( "CServiceControlManager::Install(), Calling wsprintf()\n" );
         ::wsprintf( temp_string, "Can't create service because %s at line %d of CSvcMgr.cpp", message_buffer, __LINE__ );
         TRACE( "CServiceControlManager::Install(), Calling ReportError()\n" );

         event_log.ReportError( temp_string );

      TRACE( "CServiceControlManager::Install(), Calling LocalFree()\n" );
         ::LocalFree( message_buffer );
      TRACE( "CServiceControlManager::Install(), LocalFree returned()\n" );
      }

      TRACE( "CServiceControlManager::Install(), returning FALSE\n" );
      return( FALSE );
   }

   TRACE( "CServiceControlManager::Install(), Closeing service handle\n" );

   ::CloseServiceHandle( service_handle );
   service_handle = (SC_HANDLE) NULL;

   /*
   ** We successfully installed a new service, this is something we should log
   */

   TCHAR user_name[ 2048 ];
   TCHAR temp_string[ 2100 ];

   DWORD size_of_user_name = sizeof( user_name );

   ::ZeroMemory( user_name,   size_of_user_name     );
   ::ZeroMemory( temp_string, sizeof( temp_string ) );

   TRACE( "CServiceControlManager::Install(), Getting User Name\n" );

   ::GetUserName( user_name, &size_of_user_name );

   TRACE( "CServiceControlManager::Install(), wsprintf()\n" );

   ::wsprintf( temp_string, "Service successfully installed by %s", user_name );

   TRACE( "CServiceControlManager::Install(), reporting\n" );

   event_log.ReportInformation( temp_string );

   TRACE( "CServiceControlManager::Install(), returning\n" );
   return( TRUE );
}

void CServiceControlManager::m_Initialize( void )
{
   m_ManagerHandle      = NULL;
   m_ErrorCode          = 0;
   m_Buffer             = NULL;
   m_BufferSize         = 0;
   m_CurrentEntryNumber = 0;
   m_NumberOfEntries    = 0;
}

BOOL CServiceControlManager::Open( DWORD what_to_open, LPCTSTR database_name, LPCTSTR machine_name )
{
   /*
   ** database_name can be NULL
   */

   if ( m_ManagerHandle != NULL )
   {
      Close();
   }

   if ( machine_name == NULL )
   {
      m_MachineName.Empty(); // Should go and get our machine's name
   }
   else
   {
      m_MachineName = machine_name;
   }

   m_ManagerHandle = ::OpenSCManager( machine_name, database_name, what_to_open );

   if ( m_ManagerHandle == NULL )
   {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
   }
   else
   {
      return( TRUE );
   }
}

BOOL CServiceControlManager::Pause( LPCTSTR service_name )
{
   ASSERT( service_name != NULL );

   if ( m_ManagerHandle == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
	  return( FALSE );
   }

   if ( service_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
	  return( FALSE );
   }

   SC_HANDLE service_handle = ::OpenService( m_ManagerHandle, service_name, SERVICE_PAUSE_CONTINUE );

   if ( service_handle == (SC_HANDLE) NULL )
   {
      m_ErrorCode = ::GetLastError();
	  return( FALSE );
   }

   SERVICE_STATUS service_status;

   ::ZeroMemory( &service_status, sizeof( service_status ) );

   BOOL return_value = ::ControlService( service_handle, SERVICE_CONTROL_PAUSE, &service_status );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   ::CloseServiceHandle( service_handle );

   return( return_value );
}

BOOL CServiceControlManager::Remove( LPCTSTR service_name )
{
   ASSERT( service_name != NULL );

   if ( service_name == (LPCTSTR) NULL || m_ManagerHandle == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
      return( FALSE );
   }

   SC_HANDLE service_handle = (SC_HANDLE) NULL;

   service_handle = ::OpenService( m_ManagerHandle, service_name, SERVICE_ALL_ACCESS );

   if ( service_handle == (SC_HANDLE) NULL )
   {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
   }

   /*
   ** We're gonna delete the service, this is something we should record
   */

   {
      TCHAR user_name[ 2048 ];
      TCHAR temp_string[ 2100 ];

      DWORD size_of_user_name = sizeof( user_name );

      ::ZeroMemory( user_name,   size_of_user_name     );
      ::ZeroMemory( temp_string, sizeof( temp_string ) );

      ::GetUserName( user_name, &size_of_user_name );

      ::wsprintf( temp_string, "Service being removed by %s", user_name );

      CEventLog event_log( service_name );

      event_log.ReportInformation( temp_string );
   }

   BOOL return_value = ::DeleteService( service_handle );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();

      /*
      ** We couldn't delete the service, let's record why...
      */

      LPVOID message_buffer = (LPVOID) NULL;

      ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL,
                       m_ErrorCode,
                       MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
             (LPTSTR) &message_buffer,
                       0,
                       NULL );

      if ( message_buffer != NULL )
      {
         TCHAR temp_string[ 255 ];

         ::wsprintf( temp_string, "Can't delete service because %s", message_buffer );

         CEventLog event_log( service_name );
         event_log.ReportError( temp_string );

         ::LocalFree( message_buffer );
      }

      return( FALSE );
   }

   /*
   ** Now that we've deleted the service, we need to remove it from the event logger
   */

   CEventLog event_log;

   event_log.DeleteApplicationLog( service_name );

   return( TRUE );
}

BOOL CServiceControlManager::Start( LPCTSTR service_name, DWORD service_argc, LPCTSTR *service_argv )
{
   ASSERT( service_name != NULL );

   if ( m_ManagerHandle == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
	  return( FALSE );
   }

   if ( service_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
	  return( FALSE );
   }

   SC_HANDLE service_handle = ::OpenService( m_ManagerHandle, service_name, SERVICE_START );

   if ( service_handle == (SC_HANDLE) NULL )
   {
      m_ErrorCode = ::GetLastError();
	  return( FALSE );
   }

   BOOL return_value = ::StartService( service_handle, service_argc, service_argv );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   ::CloseServiceHandle( service_handle );

   return( return_value );
}

BOOL CServiceControlManager::Stop( LPCTSTR service_name )
{
   ASSERT( service_name != NULL );

   if ( m_ManagerHandle == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
	  return( FALSE );
   }

   if ( service_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
	  return( FALSE );
   }

   SC_HANDLE service_handle = ::OpenService( m_ManagerHandle, service_name, SERVICE_STOP );

   if ( service_handle == (SC_HANDLE) NULL )
   {
      m_ErrorCode = ::GetLastError();
	  return( FALSE );
   }

   SERVICE_STATUS service_status;

   ::ZeroMemory( &service_status, sizeof( service_status ) );

   BOOL return_value = ::ControlService( service_handle, SERVICE_CONTROL_STOP, &service_status );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   ::CloseServiceHandle( service_handle );

   return( return_value );
}

CServiceNameAndStatusA::CServiceNameAndStatusA()
{
   Empty();
}

CServiceNameAndStatusA::~CServiceNameAndStatusA()
{
   Empty();
}

void CServiceNameAndStatusA::Copy( const _ENUM_SERVICE_STATUSA *source_p )
{
   ASSERT( source_p != NULL );

   if ( source_p == NULL )
   {
      Empty();
      return;
   }

   lpServiceName                           = source_p->lpServiceName;
   lpDisplayName                           = source_p->lpDisplayName;
   ServiceStatus.dwServiceType             = source_p->ServiceStatus.dwServiceType;
   ServiceStatus.dwCurrentState            = source_p->ServiceStatus.dwCurrentState;
   ServiceStatus.dwControlsAccepted        = source_p->ServiceStatus.dwControlsAccepted;
   ServiceStatus.dwWin32ExitCode           = source_p->ServiceStatus.dwWin32ExitCode;
   ServiceStatus.dwServiceSpecificExitCode = source_p->ServiceStatus.dwServiceSpecificExitCode;
   ServiceStatus.dwCheckPoint              = source_p->ServiceStatus.dwCheckPoint;
   ServiceStatus.dwWaitHint                = source_p->ServiceStatus.dwWaitHint;
}

void CServiceNameAndStatusA::Empty( void )
{
   lpServiceName                           = NULL;
   lpDisplayName                           = NULL;
   ServiceStatus.dwServiceType             = 0;
   ServiceStatus.dwCurrentState            = 0;
   ServiceStatus.dwControlsAccepted        = 0;
   ServiceStatus.dwWin32ExitCode           = 0;
   ServiceStatus.dwServiceSpecificExitCode = 0;
   ServiceStatus.dwCheckPoint              = 0;
   ServiceStatus.dwWaitHint                = 0;
}

CServiceNameAndStatusW::CServiceNameAndStatusW()
{
   Empty();
}

CServiceNameAndStatusW::~CServiceNameAndStatusW()
{
   Empty();
}

void CServiceNameAndStatusW::Copy( const _ENUM_SERVICE_STATUSW *source_p )
{
   ASSERT( source_p != NULL );

   if ( source_p == NULL )
   {
      Empty();
      return;
   }

   lpServiceName                           = source_p->lpServiceName;
   lpDisplayName                           = source_p->lpDisplayName;
   ServiceStatus.dwServiceType             = source_p->ServiceStatus.dwServiceType;
   ServiceStatus.dwCurrentState            = source_p->ServiceStatus.dwCurrentState;
   ServiceStatus.dwControlsAccepted        = source_p->ServiceStatus.dwControlsAccepted;
   ServiceStatus.dwWin32ExitCode           = source_p->ServiceStatus.dwWin32ExitCode;
   ServiceStatus.dwServiceSpecificExitCode = source_p->ServiceStatus.dwServiceSpecificExitCode;
   ServiceStatus.dwCheckPoint              = source_p->ServiceStatus.dwCheckPoint;
   ServiceStatus.dwWaitHint                = source_p->ServiceStatus.dwWaitHint;
}

void CServiceNameAndStatusW::Empty( void )
{
   lpServiceName                           = NULL;
   lpDisplayName                           = NULL;
   ServiceStatus.dwServiceType             = 0;
   ServiceStatus.dwCurrentState            = 0;
   ServiceStatus.dwControlsAccepted        = 0;
   ServiceStatus.dwWin32ExitCode           = 0;
   ServiceStatus.dwServiceSpecificExitCode = 0;
   ServiceStatus.dwCheckPoint              = 0;
   ServiceStatus.dwWaitHint                = 0;
}
