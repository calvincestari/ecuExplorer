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
** 10-11-96 - R Blain Modified to handle Categories
** 29-04-97 - R Blain Conditionally removed CObject for inheratance
*/

#if defined( _DEBUG )
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#if defined Inc_CObject
IMPLEMENT_DYNAMIC( CEventLog, CObject );
#endif

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

CEventLog::CEventLog()
{
   m_Initialize();
}

CEventLog::CEventLog( LPCTSTR source_name )
{
   m_Initialize();
   RegisterSource( source_name );
}

CEventLog::~CEventLog()
{
   if ( m_EventSourceHandle != INVALID_HANDLE_VALUE )
   {
      DeregisterSource();
   }

   if ( m_LogHandle != INVALID_HANDLE_VALUE )
   {
      Close();
   }

   m_Initialize();
}

void CEventLog::m_Initialize( void )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif

   ComputerName.Empty();
   LogName.Empty();

   m_LogHandle                 = INVALID_HANDLE_VALUE;
   m_EventSourceHandle         = INVALID_HANDLE_VALUE;
   m_ErrorCode                 = 0;
   m_NumberOfBytesRead         = 0;
   m_NumberOfBytesInNextRecord = 0;
}

BOOL CEventLog::Backup( LPCTSTR name_of_backup_file )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_backup_file != NULL );

   if ( name_of_backup_file == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   BOOL return_value = ::BackupEventLog( m_LogHandle, name_of_backup_file );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   return( return_value );
}

BOOL CEventLog::Clear( LPCTSTR name_of_backup_file )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif

   /*
   ** name_of_backup_file can be NULL
   */

   BOOL return_value = ::ClearEventLog( m_LogHandle, name_of_backup_file );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   return( return_value );
}

BOOL CEventLog::Close( void )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif

   BOOL return_value = ::CloseEventLog( m_LogHandle );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   m_LogHandle = INVALID_HANDLE_VALUE;

   return( return_value );
}

BOOL CEventLog::CreateApplicationLog( LPCTSTR application_name, LPCTSTR message_resource_file, DWORD supported_types, DWORD category_count )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( application_name      != NULL );
   ASSERT( message_resource_file != NULL );

   if ( application_name == NULL || message_resource_file == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   if ( application_name[ 0 ] == 0x00 || message_resource_file[ 0 ] == 0x00 )
   {
      return( FALSE );
   }

   CRegistry registry;

   if ( registry.Connect( CRegistry::keyLocalMachine ) != TRUE )
   {
      m_ErrorCode = registry.GetErrorCode();
      return( FALSE );
   }

   CString log_key_name( "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" );

   log_key_name += application_name;

   if ( registry.Create( log_key_name ) != TRUE )
   {
      m_ErrorCode = registry.GetErrorCode();
      return( FALSE );
   }

   if ( registry.SetValue( "EventMessageFile", CRegistry::typeUnexpandedString, (LPBYTE) message_resource_file, strlen( message_resource_file ) + 1 ) != TRUE )
   {
      m_ErrorCode = registry.GetErrorCode();
      return( FALSE );
   }

   if ( registry.SetValue( "TypesSupported", supported_types ) != TRUE )
   {
      m_ErrorCode = registry.GetErrorCode();
      return( FALSE );
   }
	// ElectroDynamics Added to support Event Log Categories
   if ( registry.SetValue( "CategoryMessageFile", CRegistry::typeUnexpandedString, (LPBYTE) message_resource_file, strlen( message_resource_file ) + 1 ) != TRUE )
   {
      m_ErrorCode = registry.GetErrorCode();
      return( FALSE );
   }

   if ( registry.SetValue( "CategoryCount", category_count ) != TRUE )
   {
      m_ErrorCode = registry.GetErrorCode();
      return( FALSE );
   }

// ElectroDynamics Update Sources with our application name
if ( registry.Open( "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" ) == TRUE )
   {
      CStringArray sources;

      if ( registry.GetValue( "Sources", sources ) == TRUE )
      {
			sources.Add(application_name);
            registry.SetValue( "Sources", sources );
      }
   }

   return( TRUE );
}

BOOL CEventLog::DeleteApplicationLog( LPCTSTR application_name )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( application_name != NULL );

   if ( application_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   if ( application_name[ 0 ] == 0x00 )
   {
      return( FALSE );
   }

   CRegistry registry;

   if ( registry.Connect( CRegistry::keyLocalMachine ) != TRUE )
   {
      m_ErrorCode = registry.GetErrorCode();
      return( FALSE );
   }

   CString log_key_name( "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" );

   log_key_name += application_name;
   //log_key_name += '\\';

   if ( registry.DeleteKey( log_key_name ) != TRUE )
   {
      m_ErrorCode = registry.GetErrorCode();
      return( FALSE );
   }

   /*
   ** Microsoft has a bug in this area. Even though we deleted the application from the
   ** HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\EventLog\Application\
   ** registry area, they don't provide a way to delete the application from the 
   ** HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\EventLog\Application\Sources
   ** value. The application name is one of the strings in this REG_MULTI_SZ value. We
   ** still need to delete it from there. The names listed in this value appear in the 
   ** "Source" combobox of the Event Viewer application View->Filter Events... menu selection.
   */

   if ( registry.Open( "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" ) == TRUE )
   {
      CStringArray sources;

      if ( registry.GetValue( "Sources", sources ) == TRUE )
      {
         int index = 0;
         int number_of_sources = sources.GetSize();

         BOOL application_was_found = FALSE;

         while( index < number_of_sources )
         {
            if ( sources[ index ] == application_name )
            {
               application_was_found = TRUE;
               sources.RemoveAt( index );
               index = number_of_sources;
            }

            index++;
         }

         if ( application_was_found == TRUE )
         {
            registry.SetValue( "Sources", sources );
         }
      }
   }

   return( TRUE );
}

BOOL CEventLog::DeregisterSource( void )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif

   BOOL return_value = TRUE;

   if ( m_EventSourceHandle != INVALID_HANDLE_VALUE )
   {
      return_value = ::DeregisterEventSource( m_EventSourceHandle );

      if ( return_value != TRUE )
      {
         m_ErrorCode = ::GetLastError();
      }

      m_EventSourceHandle = INVALID_HANDLE_VALUE;
   }

   return( return_value );
}

#if defined( _DEBUG )

void CEventLog::Dump( CDumpContext& dump_context ) const
{
#if defined Inc_CObject
   CObject::Dump( dump_context );
#endif

   dump_context << "m_LogHandle = "                 << m_LogHandle                 << "\n";
   dump_context << "m_EventSourceHandle = "         << m_EventSourceHandle         << "\n";
   dump_context << "m_ErrorCode = "                 << m_ErrorCode                 << "\n";
   dump_context << "m_NumberOfBytesRead = "         << m_NumberOfBytesRead         << "\n";
   dump_context << "m_NumberOfBytesInNextRecord = " << m_NumberOfBytesInNextRecord << "\n";
   dump_context << "ComputerName = \""              << ComputerName                << "\"\n";
   dump_context << "LogName = \""                   << LogName                     << "\"\n";
}

#endif // _DEBUG

DWORD CEventLog::GetErrorCode( void ) const
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   return( m_ErrorCode );
}

BOOL CEventLog::GetNumberOfRecords( DWORD& number_of_records )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif

   BOOL return_value = ::GetNumberOfEventLogRecords( m_LogHandle, &number_of_records );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   return( return_value );
}

BOOL CEventLog::NotifyChange( HANDLE event_handle, HANDLE user_log_handle )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( event_handle != INVALID_HANDLE_VALUE );

   if ( event_handle == INVALID_HANDLE_VALUE )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   if ( user_log_handle == INVALID_HANDLE_VALUE || user_log_handle == NULL )
   {
      user_log_handle = m_LogHandle;
   }

   BOOL return_value = ::NotifyChangeEventLog( user_log_handle, event_handle );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   return( return_value );
}

BOOL CEventLog::Open( LPCTSTR log_name, LPCTSTR name_of_computer )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( log_name != NULL );

   /*
   ** name_of_computer can be NULL
   */
                 
   if ( log_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   BOOL return_value = TRUE;

   m_LogHandle = ::OpenEventLog( name_of_computer, log_name );

   if ( m_LogHandle == NULL )
   {
      m_LogHandle  = INVALID_HANDLE_VALUE;
      m_ErrorCode  = ::GetLastError();
      return_value = FALSE;
   }
   else
   {
      if ( name_of_computer == NULL )
      {
         TCHAR computer_name[ MAX_PATH ] = "";
         DWORD size = sizeof( computer_name );

         if ( ::GetComputerName( computer_name, &size ) == TRUE )
         {
            ComputerName = computer_name;
         }
         else
         {
            ComputerName.Empty();
         }
      }
      else
      {
         ComputerName = name_of_computer;
      }
   }

   return( return_value );
}

BOOL CEventLog::OpenBackup( LPCTSTR name_of_backup_file, LPCTSTR name_of_computer )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_backup_file != NULL );

   /*
   ** name_of_computer can be NULL
   */

   if ( name_of_backup_file == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   BOOL return_value = TRUE;

   m_LogHandle = ::OpenBackupEventLog( name_of_computer, name_of_backup_file );

   if ( m_LogHandle == NULL )
   {
      m_LogHandle = INVALID_HANDLE_VALUE;
      m_ErrorCode = ::GetLastError();
      return_value = FALSE;
   }
   else
   {
      if ( name_of_computer == NULL )
      {
         TCHAR computer_name[ MAX_PATH ] = "";
         DWORD size = sizeof( computer_name );

         if ( ::GetComputerName( computer_name, &size ) == TRUE )
         {
            ComputerName = computer_name;
         }
         else
         {
            ComputerName.Empty();
         }
      }
      else
      {
         ComputerName = name_of_computer;
      }
   }

   return( return_value );
}

BOOL CEventLog::Read( DWORD record_number, LPVOID buffer, DWORD& number_of_bytes_to_read, DWORD how_to_read )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( buffer != NULL );

   if ( buffer == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   BOOL return_value = ::ReadEventLog( m_LogHandle,
                                       how_to_read,
                                       record_number,
                                       buffer,
                                       number_of_bytes_to_read,
                                      &m_NumberOfBytesRead,
                                      &m_NumberOfBytesInNextRecord );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   return( return_value );
}

BOOL CEventLog::RegisterSource( LPCTSTR source_name, LPCTSTR name_of_computer )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( source_name != NULL );

   /*
   ** name_of_computer can be NULL
   */

   if ( source_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   BOOL return_value = TRUE;

   if ( m_EventSourceHandle != INVALID_HANDLE_VALUE )
   {
      DeregisterSource();
   }
	// DeregisterEventSource
   m_EventSourceHandle = ::RegisterEventSource( name_of_computer, source_name );

   if ( m_EventSourceHandle == NULL )
   {
      TRACE( "RegisterEventSource returned NULL\n" );
      m_EventSourceHandle = INVALID_HANDLE_VALUE;
      m_ErrorCode          = ::GetLastError();
      return_value        = FALSE;
   }

   return( return_value );
}

BOOL CEventLog::Report( EventType event_type,
                        WORD      category,
                        DWORD     event_identifier,
                        WORD      number_of_strings,
                        LPCTSTR * string_array,
                        DWORD     number_of_raw_data_bytes,
                        LPVOID    raw_data_buffer,
                        PSID      user_security_identifier )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif

   BYTE security_identifier_buffer[ 4096 ];

   DWORD size_of_security_identifier_buffer = sizeof( security_identifier_buffer );

   if ( user_security_identifier == NULL )
   {
      TCHAR user_name[ 256 ];
      DWORD size_of_user_name  = sizeof( user_name );

      TCHAR domain_name[ 256 ];
      DWORD size_of_domain_name = sizeof( domain_name );

      SID_NAME_USE type_of_security_identifier;

      ::ZeroMemory( user_name, size_of_user_name );
      ::ZeroMemory( domain_name, size_of_domain_name );
      ::ZeroMemory( security_identifier_buffer, size_of_security_identifier_buffer );

      ::GetUserName( user_name, &size_of_user_name );

      if ( ::LookupAccountName( NULL,
                                user_name,
                               &security_identifier_buffer,
                               &size_of_security_identifier_buffer,
                                domain_name,
                               &size_of_domain_name,
                               &type_of_security_identifier ) == TRUE )
      {
         user_security_identifier = security_identifier_buffer;
      }
   }

   BOOL return_value = FALSE;

   if ( m_EventSourceHandle != INVALID_HANDLE_VALUE )
   {
      return_value = ::ReportEvent( m_EventSourceHandle,
                                    event_type,
                                    category,
                                    event_identifier,
                                    user_security_identifier,
                                    number_of_strings,
                                    number_of_raw_data_bytes,
                                    string_array,
                                    raw_data_buffer );

      //TRACE( "CEventLog::Report(), Calling ReportEvent() went OK\n" );

      if ( return_value != TRUE )
      {
         m_ErrorCode = ::GetLastError();
      }
   }
   else
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
   }

   return( return_value );
}

BOOL CEventLog::Report( LPCTSTR log_name, DWORD message_string_resource_identifier, WORD number_of_strings, LPCTSTR* string_array )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( log_name != NULL );

   if ( log_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   if ( string_array == (LPCTSTR *) NULL )
   {
      m_ErrorCode = ERROR_INVALID_HANDLE;
      return( FALSE );
   }

   if ( RegisterSource( log_name ) == TRUE )
   {
      if ( Report( eventError, 0, message_string_resource_identifier, number_of_strings, string_array ) != TRUE )
      {
         DeregisterSource();
         return( FALSE );
      }

      DeregisterSource();
   }
   else
   {
      return( FALSE );
   }

   return( TRUE );
}

void CEventLog::ReportError( LPCTSTR string_to_report )
{
   LPCTSTR string_array[ 1 ];

   if ( string_to_report == (LPCTSTR) NULL )
   {
      string_array[ 0 ] = "CEventLog::ReportError( NULL )";
   }
   else
   {
      string_array[ 0 ] = string_to_report;
   }

   TRACE( "CEventLog::ReportError()\n" );
   Report( eventError, 0, 0, 1, string_array );
}

void CEventLog::ReportInformation( LPCTSTR string_to_report )
{
   LPCTSTR string_array[ 1 ];

   if ( string_to_report == (LPCTSTR) NULL )
   {
      string_array[ 0 ] = "CEventLog::ReportInformation( NULL )";
   }
   else
   {
      string_array[ 0 ] = string_to_report;
   }

   Report( eventInformation, 0, 0, 1, string_array );
}
