#include <wfc.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
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
#endif
#if defined Inc_CObject
IMPLEMENT_DYNAMIC( CRegistry, CObject );
#endif

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

HKEY CRegistry::keyLocalMachine = HKEY_LOCAL_MACHINE;
HKEY CRegistry::keyClassesRoot  = HKEY_CLASSES_ROOT;
HKEY CRegistry::keyUsers        = HKEY_USERS;
HKEY CRegistry::keyCurrentUser  = HKEY_CURRENT_USER;


CRegistry::CRegistry()
{
	
   m_Initialize();
   m_fpLogFile = NULL;
}

CRegistry::~CRegistry()
{
   if ( m_RegistryHandle != (HKEY) NULL )
   {
      Close();
   }

   m_Initialize();
}

void CRegistry::m_Initialize( void )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif

   /*
   ** Make sure everything is zeroed out
   */

   m_ClassName.Empty();
   m_ComputerName.Empty();
   m_KeyName.Empty();
   m_RegistryName.Empty();

   m_KeyHandle                    = (HKEY) NULL;
   m_ErrorCode                    = 0L;
   m_NumberOfSubkeys              = 0;
   m_LongestSubkeyNameLength      = 0;
   m_LongestClassNameLength       = 0;
   m_NumberOfValues               = 0;
   m_LongestValueNameLength       = 0;
   m_LongestValueDataLength       = 0;
   m_SecurityDescriptorLength     = 0;
   m_LastWriteTime.dwLowDateTime  = 0;
   m_LastWriteTime.dwHighDateTime = 0;
}

BOOL CRegistry::Close( void )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif

   if (m_fpLogFile != NULL) // ElectroDynamics 20/9/98
		_close(m_fpLogFile);

   if ( m_RegistryHandle == (HKEY) NULL )
   {
      return( TRUE );
   }

   m_ErrorCode = ::RegCloseKey( m_RegistryHandle );
   // ElectroDynamics
   //CloseHandle(m_RegistryHandle);

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      m_RegistryHandle = (HKEY) NULL;
      m_Initialize();

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::Connect( HKEY key_to_open, LPCTSTR name_of_computer )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif

   /*
   ** name_of_computer can be NULL
   */

   if ( key_to_open == keyClassesRoot || key_to_open == keyCurrentUser )
   {
      if ( name_of_computer == NULL )
      {
         /*
         ** NT won't allow you to connect to these hives via RegConnectRegistry so we'll just skip that step
         */

         m_RegistryHandle = key_to_open;
         m_ErrorCode      = ERROR_SUCCESS;
      }
      else
      {
         m_ErrorCode = ERROR_INVALID_HANDLE;
      }
   }
   else
   {
      m_ErrorCode = ::RegConnectRegistry( (char *) name_of_computer, key_to_open, &m_RegistryHandle );
   }

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      if ( name_of_computer == NULL )
      {
         TCHAR computer_name[ MAX_PATH ] = "";
         DWORD size = MAX_PATH;

         if ( ::GetComputerName( computer_name, &size ) == TRUE )
         {
            m_ComputerName = computer_name;
         }
         else
         {
            m_ComputerName.Empty();
         }
      }
      else
      {
         m_ComputerName = name_of_computer;
      }

      /*
      ** It would be nice to use a switch statement here but I get a "not integral" error!
      */

      if ( key_to_open == HKEY_LOCAL_MACHINE )
      {
         m_RegistryName = "HKEY_LOCAL_MACHINE";
      }
      else if ( key_to_open == HKEY_CLASSES_ROOT )
      {
         m_RegistryName = "HKEY_CLASSES_ROOT";
      }
      else if ( key_to_open == HKEY_USERS )
      {
         m_RegistryName = "HKEY_USERS";
      }
      else if ( key_to_open == HKEY_CURRENT_USER )
      {
         m_RegistryName = "HKEY_CURRENT_USER";
      }
      else
      {
         m_RegistryName = "Unknown";
      }

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::Create( LPCTSTR               name_of_subkey, 
                        LPCTSTR               name_of_class,
                        CreateOptions         options, 
                        CreatePermissions     permissions, 
                        LPSECURITY_ATTRIBUTES security_attributes_p,
                        CreationDisposition * disposition_p )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_subkey != NULL );

   if ( name_of_subkey == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   DWORD disposition = 0;

   if ( name_of_class == NULL )
   {
      name_of_class = "";
   }

   m_ErrorCode = ::RegCreateKeyEx( m_RegistryHandle,
                                   name_of_subkey,
                           (DWORD) 0,
                          (char *) name_of_class,
                                   options,
                                   permissions,
                                   security_attributes_p,
                                  &m_KeyHandle,
                                  &disposition );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      if ( disposition_p != NULL )
      {
         *disposition_p = (CreationDisposition) disposition;
      }

      m_KeyName = name_of_subkey;

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::DeleteKey( LPCTSTR name_of_key_to_delete )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_key_to_delete != NULL );

   if ( name_of_key_to_delete == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   /*
   ** You can't delete a key given a full path. What you have to do is back up one level and then do a delete
   */

   CString full_key_name = name_of_key_to_delete;

   if ( full_key_name.Find( '\\' ) == (-1) )
   {
      /*
      ** User had not given us a full path so assume the name of the key he passed us
      ** is a key off of the current key m_RegistryHandle
      */
	  // ElectroDynamics 10-11-96
      m_ErrorCode = ::RegDeleteKey( m_KeyHandle, name_of_key_to_delete );
	  // ElectroDynamics 10-11-96
	  //m_ErrorCode = ::RegDeleteKey( m_RegistryHandle, name_of_key_to_delete );
   }
   else
   {
      int last_back_slash_location = full_key_name.GetLength() - 1;

      /*
      ** We know this loop will succeed because a back slash was found in the above if statement
      */

      while( full_key_name[ last_back_slash_location ] != '\\' )
      {
         last_back_slash_location--;
      }

      CString currently_opened_key_name = m_KeyName;

      CString parent_key_name = full_key_name.Left( last_back_slash_location );
      CString child_key_name  = full_key_name.Right( ( full_key_name.GetLength() - last_back_slash_location ) - 1 );

      /*
      ** Now we open the parent key and delete the child
      */

      if ( Open( parent_key_name ) == TRUE )
      {
         m_ErrorCode = ::RegDeleteKey( m_KeyHandle, child_key_name );
      }
      else
      {
         m_KeyName = currently_opened_key_name;
         return( FALSE );
      }
   }

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}                  

BOOL CRegistry::DeleteValue( LPCTSTR name_of_value_to_delete )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   /*
   ** name_of_value_to_delete can be NULL
   */

   m_ErrorCode = ::RegDeleteValue( m_KeyHandle, (LPTSTR) name_of_value_to_delete );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

#if defined( _DEBUG )

void CRegistry::Dump( CDumpContext& dump_context ) const
{
#if defined Inc_CObject
    CObject::Dump( dump_context );
#endif
   

   dump_context << "m_KeyHandle = "                << m_KeyHandle                << "\n";
   dump_context << "m_RegistryHandle = "           << m_RegistryHandle           << "\n";
   dump_context << "m_ClassName = \""              << m_ClassName                << "\"\n";
   dump_context << "m_ComputerName = \""           << m_ComputerName             << "\"\n";
   dump_context << "m_KeyName = \""                << m_KeyName                  << "\"\n";
   dump_context << "m_RegistryName = \""           << m_RegistryName             << "\"\n";
   dump_context << "m_NumberOfSubkeys = "          << m_NumberOfSubkeys          << "\n";
   dump_context << "m_NumberOfValues = "           << m_NumberOfValues           << "\n";
   dump_context << "m_LongestSubkeyNameLength = "  << m_LongestSubkeyNameLength  << "\n";
   dump_context << "m_LongestClassNameLength = "   << m_LongestClassNameLength   << "\n";
   dump_context << "m_LongestValueNameLength = "   << m_LongestValueNameLength   << "\n";
   dump_context << "m_LongestValueDataLength = "   << m_LongestValueDataLength   << "\n";
   dump_context << "m_SecurityDescriptorLength = " << m_SecurityDescriptorLength << "\n";
   dump_context << "m_LastWriteTime = "            << m_LastWriteTime            << "\n";
}

#endif // _DEBUG

BOOL CRegistry::EnumerateKeys( const DWORD subkey_index, CString& subkey_name, CString& class_name )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   TCHAR subkey_name_string[ 2048 ];
   TCHAR class_name_string[ 2048 ];

   DWORD size_of_subkey_name_string = sizeof( subkey_name_string ) - 1;
   DWORD size_of_class_name_string  = sizeof( class_name_string  ) - 1;

   ::ZeroMemory( subkey_name_string, sizeof( subkey_name_string ) );
   ::ZeroMemory( class_name_string,  sizeof( class_name_string  ) );

   m_ErrorCode = ::RegEnumKeyEx( m_KeyHandle, 
                                 subkey_index, 
                                 subkey_name_string, 
                                &size_of_subkey_name_string,
                                 NULL,
                                 class_name_string,
                                &size_of_class_name_string,
                                &m_LastWriteTime );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      subkey_name = subkey_name_string;
      class_name  = class_name_string;

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::EnumerateValues( const DWORD    value_index, 
                                 CString&       name_of_value, 
                                 KeyValueTypes& type_code,
                                 LPBYTE         data_buffer,
                                 DWORD&         size_of_data_buffer )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   /*
   ** data_buffer and size_of_data_buffer can be NULL
   */

   DWORD temp_type_code = type_code;

   TCHAR temp_name[ 2048 ];

   ::ZeroMemory( temp_name, sizeof( temp_name ) );

   DWORD temp_name_size = sizeof( temp_name );
   
   m_ErrorCode = ::RegEnumValue( m_KeyHandle,
                                 value_index,
                                 temp_name,
                                &temp_name_size,
                                 NULL,
                                &temp_type_code,
                                 data_buffer,
                                &size_of_data_buffer );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      type_code     = (KeyValueTypes) temp_type_code;
      name_of_value = temp_name;

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::Flush( void )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   m_ErrorCode = ::RegFlushKey( m_KeyHandle );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::GetBinaryValue( LPCTSTR name_of_value, CByteArray& return_array )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   DWORD size_of_buffer = m_LongestValueDataLength;

   LPBYTE memory_buffer = (LPBYTE) new BYTE[ size_of_buffer ];

   if ( memory_buffer == NULL )
   {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
   }

   BOOL return_value = TRUE;

   KeyValueTypes type = typeBinary;

   if ( QueryValue( name_of_value, type, memory_buffer, size_of_buffer ) == TRUE )
   {
      /*
      ** We've got data so give it back to the caller
      */

      return_array.RemoveAll();

      DWORD index = 0;

      while( index < size_of_buffer )
      {
         return_array.Add( memory_buffer[ index ] );
         index++;
      }

      return_value = TRUE;
   }
   else
   {
      return_value = FALSE;
   }

   delete [] memory_buffer;

   return( return_value );
}

void CRegistry::GetClassName( CString& class_name ) const
{
   class_name = m_ClassName;
}

void CRegistry::GetComputerName( CString& computer_name ) const
{
   computer_name = m_ComputerName;
}

BOOL CRegistry::GetDoubleWordValue( LPCTSTR name_of_value, DWORD& return_value )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   DWORD size_of_buffer = sizeof( DWORD );

   KeyValueTypes type = typeDoubleWord;

   return( QueryValue( name_of_value, type, (LPBYTE) &return_value, size_of_buffer ) );
}

BOOL CRegistry::GetErrorCode( void ) const
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif   
   return( m_ErrorCode );
}

void CRegistry::GetKeyName( CString& key_name ) const
{
   key_name = m_KeyName;
}

DWORD CRegistry::GetNumberOfSubkeys( void ) const
{
   return( m_NumberOfSubkeys );
}

DWORD CRegistry::GetNumberOfValues( void ) const
{
   return( m_NumberOfValues );
}

void CRegistry::GetRegistryName( CString& registry_name ) const
{
   registry_name = m_RegistryName;
}

BOOL CRegistry::GetSecurity( const SECURITY_INFORMATION what_you_want_to_know,
                             PSECURITY_DESCRIPTOR       data_buffer,
                             DWORD&                     size_of_data_buffer )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif   ASSERT( data_buffer != NULL );

   if ( data_buffer == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   m_ErrorCode = ::RegGetKeySecurity( m_KeyHandle,
                                      what_you_want_to_know,
                                      data_buffer,
                                     &size_of_data_buffer );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::GetStringValue( LPCTSTR name_of_value, CString& return_string )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif   
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   TCHAR temp_string[ 2048 ];
   DWORD size_of_buffer = 2048;

   ::ZeroMemory( temp_string, sizeof( temp_string ) );

   KeyValueTypes type = typeString;

   if ( QueryValue( name_of_value, type, (LPBYTE) temp_string, size_of_buffer ) == TRUE )
   {
      return_string = temp_string;
      return( TRUE );
   }
   else
   {
      return_string.Empty();
      return( FALSE );
   }
}

BOOL CRegistry::GetStringArrayValue( LPCTSTR name_of_value, CStringArray& return_array )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   DWORD size_of_buffer = m_LongestValueDataLength;

   LPBYTE memory_buffer = (LPBYTE) new BYTE[ size_of_buffer ];

   if ( memory_buffer == NULL )
   {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
   }

   BOOL return_value = TRUE;

   KeyValueTypes type = typeMultipleString; // A double NULL terminated string

   if ( QueryValue( name_of_value, type, memory_buffer, size_of_buffer ) == TRUE )
   {
      /*
      ** We've got data so give it back to the caller
      */

      LPTSTR strings = (LPTSTR) memory_buffer;

      return_array.RemoveAll();

      while( strings[ 0 ] != 0x00 )
      {
         return_array.Add( (LPCTSTR) strings );
         strings += ( ::strlen( (LPCTSTR) strings ) + 1 );
      }

      return_value = TRUE;
   }
   else
   {
      return_value = FALSE;
   }

   delete [] memory_buffer;
   
   return( return_value );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, CByteArray& return_array )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif   
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( GetBinaryValue( name_of_value, return_array ) );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, DWORD& return_value )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif  
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( GetDoubleWordValue( name_of_value, return_value ) );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, CString& return_string )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( GetStringValue( name_of_value, return_string ) );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, CStringArray& return_array )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( GetStringArrayValue( name_of_value, return_array ) );
}

BOOL CRegistry::Load( LPCTSTR name_of_subkey, LPCTSTR name_of_file_containing_information )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_subkey != NULL );
   ASSERT( name_of_file_containing_information != NULL );

   if ( name_of_subkey == NULL || name_of_file_containing_information == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   m_ErrorCode = ::RegLoadKey( m_RegistryHandle, name_of_subkey, name_of_file_containing_information );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::NotifyChange( const HANDLE             event_handle, 
                              const NotifyChangeFilter changes_to_be_reported,
                              const BOOL               this_subkey_or_all_subkeys,
                              const BOOL               wait_for_change_or_signal_event )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   m_ErrorCode = ::RegNotifyChangeKeyValue( m_KeyHandle,
                                            this_subkey_or_all_subkeys,
                                            changes_to_be_reported,
                                            event_handle,
                                            wait_for_change_or_signal_event );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::Open( LPCTSTR name_of_subkey_to_open, const CreatePermissions security_access_mask )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   /*
   ** name_of_subkey_to_open can be NULL
   */

   m_ErrorCode = ::RegOpenKeyEx( m_RegistryHandle, name_of_subkey_to_open, NULL, security_access_mask, &m_KeyHandle );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      QueryInfo();
      m_KeyName = name_of_subkey_to_open;

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::QueryInfo( void )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   TCHAR class_name[ 2048 ];

   DWORD size_of_class_name = sizeof( class_name ) - 1;

   ::ZeroMemory( class_name, sizeof( class_name ) );

   m_ErrorCode = ::RegQueryInfoKey( m_KeyHandle,
                                    class_name,
                                   &size_of_class_name,
                          (LPDWORD) NULL,
                                   &m_NumberOfSubkeys,
                                   &m_LongestSubkeyNameLength,
                                   &m_LongestClassNameLength,
                                   &m_NumberOfValues,
                                   &m_LongestValueNameLength,
                                   &m_LongestValueDataLength,
                                   &m_SecurityDescriptorLength,
                                   &m_LastWriteTime );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      m_ClassName = class_name;

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::QueryValue( LPCTSTR        name_of_value, 
                            KeyValueTypes& value_type, 
                            LPBYTE         address_of_buffer, 
                            DWORD&         size_of_buffer )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif  
   ASSERT( name_of_value != NULL );

   /*
   ** address_of_buffer and size_of_buffer can be NULL
   */

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   DWORD temp_data_type = (DWORD) value_type;

   m_ErrorCode = ::RegQueryValueEx( m_KeyHandle,
                           (char *) name_of_value,
                                    NULL,
                                   &temp_data_type,
                                    address_of_buffer,
                                   &size_of_buffer );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      value_type = (KeyValueTypes) temp_data_type;
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::Replace( LPCTSTR name_of_subkey,
                         LPCTSTR name_of_file_with_new_data,
                         LPCTSTR name_of_backup_file )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_subkey             != NULL );
   ASSERT( name_of_file_with_new_data != NULL );
   ASSERT( name_of_backup_file        != NULL );

   if ( name_of_subkey             == NULL ||
        name_of_file_with_new_data == NULL ||
        name_of_backup_file        == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   m_ErrorCode = ::RegReplaceKey( m_KeyHandle, 
                                  name_of_subkey,
                                  name_of_file_with_new_data,
                                  name_of_backup_file );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::Restore( LPCTSTR name_of_file_holding_saved_tree, const DWORD volatility_flags )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_file_holding_saved_tree != NULL );

   if ( name_of_file_holding_saved_tree == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   m_ErrorCode = ::RegRestoreKey( m_KeyHandle,
                                  name_of_file_holding_saved_tree,
                                  volatility_flags );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::Save( LPCTSTR name_of_file_to_hold_tree, LPSECURITY_ATTRIBUTES security_attributes_p )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_file_to_hold_tree != NULL );

   if ( name_of_file_to_hold_tree == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   m_ErrorCode = ::RegSaveKey( m_KeyHandle, name_of_file_to_hold_tree, security_attributes_p );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::SetBinaryValue( LPCTSTR name_of_value, const CByteArray& bytes_to_write )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   DWORD size_of_buffer = bytes_to_write.GetSize();

   LPBYTE memory_buffer = new BYTE[ size_of_buffer ];

   if ( memory_buffer == NULL )
   {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
   }

   DWORD index = 0;

   while( index < size_of_buffer )
   {
      memory_buffer[ index ] = bytes_to_write[ index ];
      index++;
   }

   BOOL return_value = SetValue( name_of_value, typeBinary, memory_buffer, size_of_buffer );

   delete [] memory_buffer;

   return( return_value );
}

BOOL CRegistry::SetDoubleWordValue( LPCTSTR name_of_value, DWORD value_to_write )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( SetValue( name_of_value, typeDoubleWord, (const PBYTE) &value_to_write, sizeof( DWORD ) ) );
}

BOOL CRegistry::SetSecurity( const SECURITY_INFORMATION& security_information,
                             const PSECURITY_DESCRIPTOR  security_descriptor_p )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( security_descriptor_p != NULL );

   if ( security_descriptor_p == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   m_ErrorCode = ::RegSetKeySecurity( m_KeyHandle, security_information, security_descriptor_p );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::SetStringValue( LPCTSTR name_of_value, const CString& string_value )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( SetValue( name_of_value, typeString, (const PBYTE) (const char *) string_value, string_value.GetLength() + 1 ) );
}

BOOL CRegistry::SetStringArrayValue( LPCTSTR name_of_value, const CStringArray& string_array )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   DWORD size_of_buffer = 0;

   /*
   ** Find out how big our buffer needs to be...
   */

   int index             = 0;
   int number_of_strings = string_array.GetSize();

   CString temp_string;

   while( index < number_of_strings )
   {
      temp_string = string_array[ index ];
      size_of_buffer += temp_string.GetLength() + 1;
      index++;
   }

   /*
   ** Don't forget the second NULL needed for double null terminated strings...
   */

   size_of_buffer++;

   LPBYTE memory_buffer = new BYTE[ size_of_buffer ];

   if ( memory_buffer == NULL )
   {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
   }

   ::ZeroMemory( memory_buffer, size_of_buffer );

   /*
   ** OK, now add the strings to the memory buffer
   */

   LPTSTR string = (LPTSTR) memory_buffer;

   index             = 0;
   int string_length = 0;

   while( index < number_of_strings )
   {
      temp_string = string_array[ index ];
      ::strcpy( &string[ string_length ], temp_string );
      string_length += temp_string.GetLength() + 1;

      index++;
   }

   string_length++;

   BOOL return_value = TRUE;

   KeyValueTypes type = typeMultipleString; // A double NULL terminated string

   if ( SetValue( name_of_value, type, memory_buffer, size_of_buffer ) != TRUE )
   {
      return_value = FALSE;
   }

   delete [] memory_buffer;

   return( return_value );
}

BOOL CRegistry::SetValue( LPCTSTR name_of_value, const CByteArray& bytes_to_write )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( SetBinaryValue( name_of_value, bytes_to_write ) );
}

BOOL CRegistry::SetValue( LPCTSTR name_of_value, DWORD value )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( SetDoubleWordValue( name_of_value, value ) );
}

BOOL CRegistry::SetValue( LPCTSTR name_of_value, const CStringArray& strings_to_write )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( SetStringArrayValue( name_of_value, strings_to_write ) );
}

BOOL CRegistry::SetValue( LPCTSTR name_of_value, const CString& string_to_write )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value != NULL );

   if ( name_of_value == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   return( SetStringValue( name_of_value, string_to_write ) );
}

BOOL CRegistry::SetValue( LPCTSTR             name_of_value, 
                          const KeyValueTypes type_of_value_to_set, 
                          const PBYTE         address_of_value_data, 
                          const DWORD         size_of_data )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_value         != NULL );
   ASSERT( address_of_value_data != NULL );

   if ( name_of_value == NULL || address_of_value_data == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   m_ErrorCode = ::RegSetValueEx( m_KeyHandle,
                                  name_of_value,
                                  0,
                                  type_of_value_to_set,
                                  address_of_value_data,
                                  size_of_data );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRegistry::UnLoad( LPCTSTR name_of_subkey_to_unload )
{
#if defined Inc_CObject
   ASSERT_VALID( this );
#endif
   ASSERT( name_of_subkey_to_unload != NULL );

   if ( name_of_subkey_to_unload == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   m_ErrorCode = ::RegUnLoadKey( m_KeyHandle, name_of_subkey_to_unload );

   if ( m_ErrorCode == ERROR_SUCCESS )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

// Added by R Blain - 29-Mar-97
// Open with create
// Return TRUE if created for the first time
BOOL CRegistry::OpenWithCreate(LPCTSTR name_of_subkey_to_open)
{
CString sError;
CRegistry::CreationDisposition dwDisposition;

m_sSubKeyName = name_of_subkey_to_open;

    if ( Connect( keyLocalMachine, "" ) == TRUE )
        if ( Open(name_of_subkey_to_open) != TRUE )
        {
            Create(     name_of_subkey_to_open,
                        "",
                        CRegistry::optionsNonVolatile,
                        CRegistry::permissionAllAccess,
                        NULL,
                        &dwDisposition);
            printf("Creating registry key:'%s'",name_of_subkey_to_open);
            return TRUE;
        }
    return FALSE;
}

// Added by ElectroDynamics - 20-Sept-98
// Open with create
// Return TRUE if created for the first time
BOOL CRegistry::OpenWithCreate(LPCTSTR name_of_subkey_to_open, LPCTSTR FileName)
{
CString sError;
CRegistry::CreationDisposition dwDisposition;

	m_sFileName = FileName;
	m_sFileName += ".def";
	m_sSubKeyName = name_of_subkey_to_open;

	if (m_fpLogFile != NULL)
	{
		printf("Registry Default File is already open");
		return TRUE;
	}

	m_fpLogFile = _open((LPCSTR)m_sFileName,_O_RDWR | _O_TEXT | _O_CREAT  , _S_IREAD | _S_IWRITE );
	if (m_fpLogFile == -1)
	{
		printf("Unable to open Registry Default File");
		return TRUE;
	}

    if ( Connect( keyLocalMachine, "" ) == TRUE )
        if ( Open(name_of_subkey_to_open) != TRUE )
        {
            Create(     name_of_subkey_to_open,
                        "",
                        CRegistry::optionsNonVolatile,
                        CRegistry::permissionAllAccess,
                        NULL,
                        &dwDisposition);
            printf("Creating registry key:'%s'",name_of_subkey_to_open);
            return TRUE;
        }
    return FALSE;
}

// -----------------------------------------------------------------------------------
// Function:    RegReadWithCreate
// Description: Used to read a numeric value from the registry
// parameters:  CString sMember         - Destination variable
//              LPCTSTR szDefaultValue   - Default string 
//              LPCTSTR szKey            - Registry key
// return code: void
// -----------------------------------------------------------------------------------
// Reads and Creates a String Numeric Value
BOOL CRegistry :: RegReadWithCreate(int * piMember, LPCTSTR szDefaultValue, LPCTSTR szKey)
{
CString sRegTemp, sMessage;

    if(GetStringValue( szKey, sRegTemp)==FALSE)
    {
	    SetStringValue( szKey, szDefaultValue  );
        printf("Creating registry value: %s:=%s\n",szKey,szDefaultValue);
	    *piMember = atoi(szDefaultValue);
        return TRUE;
    }
    else
        *piMember = atoi(sRegTemp);
    return FALSE;
}

// -----------------------------------------------------------------------------------
// Function:    RegReadWithCreate
// Description: Used to read a numeric value from the registry
// parameters:  CString sMember         - Destination variable
//              LPCTSTR szDefaultValue   - Default string 
//              LPCTSTR szKey            - Registry key
// return code: void
// -----------------------------------------------------------------------------------
// Reads and Creates a String Numeric Value
BOOL CRegistry :: RegReadWithCreateFileDefault(int * piMember, LPCTSTR szDefaultValue, LPCTSTR szKey)
{
CString sRegTemp, sMessage,sTempOutput,sBuffer;
//char pChar[500];
DWORD dwBytesToRead = 500;
//DWORD dwErrorFlags;
//int iPos;
CString sTemp = m_sSubKeyName;
	sTemp += szKey;

    if(GetStringValue( szKey, sRegTemp)==FALSE)
    {
		// look in firl for default
		//GetFileDefault((char *)szDefaultValue, (char *)szKey);
	
	    SetStringValue( szKey, szDefaultValue  );
        printf("Creating registry value: %s:=%s\n",szKey,szDefaultValue);
	    *piMember = atoi(szDefaultValue);
        return TRUE;
    }
    else
        *piMember = atoi(sRegTemp);
    return FALSE;
}

// -----------------------------------------------------------------------------------
// Function:    GetFileDefault
// Description: Reads an 'APPNAME.DEF' file looking for a match on szKey
//              and returning a default, if none is found then initial default used
// parameters:   char * szDefaultValue, char * szKey
// return code: void
// -----------------------------------------------------------------------------------
// Reads and Creates a String Numeric Value
void CRegistry :: GetFileDefault( char * szDefaultValue, char * szKey)
{
	CString sRegTemp, sMessage,sTempOutput,sBuffer;
//	char pChar[500];
	DWORD dwBytesToRead;
	DWORD dwErrorFlags;
	int iPos;
	CString sTemp = m_sSubKeyName;
	sTemp += szKey;
	// Look up default in file if it exists
	if(m_fpLogFile != NULL)
	{
		dwBytesToRead = _filelength( m_fpLogFile );
		// Read until match on szKey
		//dwBytesToRead = 20000;
		_lseek(m_fpLogFile,0,SEEK_SET);
		dwErrorFlags = _read( m_fpLogFile,sBuffer.GetBuffer(dwBytesToRead), dwBytesToRead);
		
		if(dwErrorFlags == 0)
		{
			sBuffer.ReleaseBuffer(0);
			// Write out default value
			sTempOutput.Format("%s=%s\n",(LPCSTR)sTemp,szDefaultValue);
		_lseek(m_fpLogFile,0,SEEK_END);
			_write(m_fpLogFile,(LPCSTR)sTempOutput,sTempOutput.GetLength());
		}
		else
		{
			sBuffer.ReleaseBuffer(dwErrorFlags);
			// Read until match on szKey
			iPos = sBuffer.Find((LPCSTR)sTemp); // CString

			// FindOneOf
			if(iPos >= 0)
			{
				sTemp = sBuffer.Right(sBuffer.GetLength() - (iPos + sTemp.GetLength())-1);
				sBuffer.Format("\n");
				iPos = sTemp.FindOneOf(sBuffer);
				if(iPos >= 0)
				{
					sBuffer = sTemp.Left(iPos);
					strcpy((char *)szDefaultValue,(LPCSTR)sBuffer);
				}
			}
			else
			{
				// Write out default value
				sTempOutput.Format("%s=%s\n",(LPCSTR)sTemp,szDefaultValue);
				_lseek(m_fpLogFile,0,SEEK_END);
				_write(m_fpLogFile,(LPCSTR)sTempOutput,sTempOutput.GetLength());
			}
		}
	}
}

// -----------------------------------------------------------------------------------
// Function:    RegReadWithCreate
// Description: Used to read a numeric value from the registry
// parameters:  CString sMember         - Destination variable
//              LPCTSTR szDefaultValue   - Default string 
//              LPCTSTR szKey            - Registry key
// return code: void
// -----------------------------------------------------------------------------------
// Reads and Creates a String Numeric Value
BOOL CRegistry :: RegReadWithCreate(unsigned long * piMember, DWORD dwDefaultValue, LPCTSTR szKey)
{
	if(GetDoubleWordValue(szKey,*piMember) == FALSE)
	{
		SetDoubleWordValue(szKey,dwDefaultValue);
		printf("Creating registry value: %s=%i\n",szKey,dwDefaultValue);
		*piMember = dwDefaultValue;
		return TRUE;
	}

	return FALSE;

//    if(GetStringValue( szKey, sRegTemp)==FALSE)
//  {
//		GetFileDefault((char *)szDefaultValue, (char *)szKey);
//	    SetStringValue( szKey, szDefaultValue  );
//      printf("Creating registry value: %s:=%s\n",szKey,szDefaultValue);
//	    *piMember = atoi(szDefaultValue);
//      return TRUE;
//  }
//  else
//      *piMember = atoi(sRegTemp);
//  return FALSE;
}

// -----------------------------------------------------------------------------------
// Function:    RegReadWithCreate
// Description: Used to read a numeric value from the registry
// parameters:  CString sMember         - Destination variable
//              LPCTSTR szDefaultValue   - Default string 
//              LPCTSTR szKey            - Registry key
// return code: void
// -----------------------------------------------------------------------------------
// Reads and Creates a String Numeric Value
BOOL CRegistry :: RegReadWithCreate(unsigned short * piMember, LPCTSTR szDefaultValue, LPCTSTR szKey)
{
CString sRegTemp, sMessage;

    if(GetStringValue( szKey, sRegTemp)==FALSE)
    {
		GetFileDefault((char *)szDefaultValue, (char *)szKey);
	    SetStringValue( szKey, szDefaultValue  );
        printf("Creating registry value: %s:=%s\n",szKey,szDefaultValue);
	    *piMember = (unsigned short)atoi(szDefaultValue);
        return TRUE;
    }
    else
        *piMember = (unsigned short)atoi(sRegTemp);
    return FALSE;
}

// -----------------------------------------------------------------------------------
// Function:    RegReadWithCreate
// Description: Used to read a numeric value from the registry
// parameters:  CString sMember         - Destination variable
//              LPCTSTR szDefaultValue   - Default string 
//              LPCTSTR szKey            - Registry key
// return code: void
// -----------------------------------------------------------------------------------
// Reads and Creates a String Numeric Value
BOOL CRegistry :: RegReadWithCreate(unsigned int * piMember, LPCTSTR szDefaultValue, LPCTSTR szKey)
{
CString sRegTemp, sMessage;

    if(GetStringValue( szKey, sRegTemp)==FALSE)
    {
		GetFileDefault((char *)szDefaultValue, (char *)szKey);
	    SetStringValue( szKey, szDefaultValue  );
        printf("Creating registry value: %s:=%s\n",szKey,szDefaultValue);
	    *piMember = (unsigned int)atoi(szDefaultValue);
        return TRUE;
    }
    else
        *piMember = (unsigned int)atoi(sRegTemp);
    return FALSE;
}
		
// -----------------------------------------------------------------------------------
// Function:    RegReadWithCreate
// Description: Used to read a string value from the registry
// parameters:  CString sMember         - Destination variable
//              LPCTSTR szDefaultValue   - Default string 
//              LPCTSTR szKey            - Registry key
// return code: void
// -----------------------------------------------------------------------------------

BOOL CRegistry :: RegReadWithCreate(CString * psMember, LPCTSTR szDefaultValue, LPCTSTR szKey)
{
CString sRegTemp, sMessage;

    if(GetStringValue( szKey, sRegTemp)==FALSE)
    {
		//GetFileDefault((char *)szDefaultValue, (char *)szKey);
	    SetStringValue( szKey, szDefaultValue  );
        printf("Creating registry value: %s:=%s\n",szKey,szDefaultValue);
	    *psMember = szDefaultValue;
        return TRUE;
    }
    else
        *psMember = sRegTemp;
    return FALSE;
}
