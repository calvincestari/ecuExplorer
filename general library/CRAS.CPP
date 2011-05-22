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

/*
** CRASAuthenticationMessageBlock
*/

CRemoteAccessServiceAuthenticationMessageBlock::CRemoteAccessServiceAuthenticationMessageBlock()
{
   m_Initialize();
}

CRemoteAccessServiceAuthenticationMessageBlock::CRemoteAccessServiceAuthenticationMessageBlock( const RASAMB *source )
{
   m_Initialize();
   Copy( source );
}

CRemoteAccessServiceAuthenticationMessageBlock::CRemoteAccessServiceAuthenticationMessageBlock( const CRemoteAccessServiceAuthenticationMessageBlock& source )
{
   m_Initialize();
   Copy( source );
}

CRemoteAccessServiceAuthenticationMessageBlock::~CRemoteAccessServiceAuthenticationMessageBlock()
{
   m_Initialize();
}

void CRemoteAccessServiceAuthenticationMessageBlock::m_Initialize( void )
{
   ::ZeroMemory( (RASAMB *) this, sizeof( RASAMB ) );
   dwSize = sizeof( RASAMB );
}

void CRemoteAccessServiceAuthenticationMessageBlock::Copy( const RASAMB *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

   dwError = source->dwError;
   bLana   = source->bLana;
   ::strcpy( szNetBiosError, source->szNetBiosError );
}

void CRemoteAccessServiceAuthenticationMessageBlock::Copy( const CRemoteAccessServiceAuthenticationMessageBlock& source )
{
   dwError = source.dwError;
   bLana   = source.bLana;
   ::strcpy( szNetBiosError, source.szNetBiosError );
}

BYTE CRemoteAccessServiceAuthenticationMessageBlock::GetLana( void ) const
{
   return( bLana );
}

DWORD CRemoteAccessServiceAuthenticationMessageBlock::GetErrorCode( void ) const
{
   return( dwError );
}

void CRemoteAccessServiceAuthenticationMessageBlock::GetErrorString( CString &return_string ) const
{
   return_string = szNetBiosError;
}

/*
** CRemoteAccessServiceNetBEUIFramer
*/

CRemoteAccessServiceNetBEUIFramer::CRemoteAccessServiceNetBEUIFramer()
{
   m_Initialize();
}

CRemoteAccessServiceNetBEUIFramer::CRemoteAccessServiceNetBEUIFramer( const RASPPPNBF *source )
{
   m_Initialize();
   Copy( source );
}

CRemoteAccessServiceNetBEUIFramer::CRemoteAccessServiceNetBEUIFramer( const CRemoteAccessServiceNetBEUIFramer& source )
{
   m_Initialize();
   Copy( source );
}

CRemoteAccessServiceNetBEUIFramer::~CRemoteAccessServiceNetBEUIFramer()
{
   m_Initialize();
}

void CRemoteAccessServiceNetBEUIFramer::m_Initialize( void )
{
   ::ZeroMemory( (RASPPPNBF *) this, sizeof( RASPPPNBF ) );
   dwSize = sizeof( RASPPPNBF );
}

void CRemoteAccessServiceNetBEUIFramer::Copy( const RASPPPNBF *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

   dwError        = source->dwError;
   dwNetBiosError = source->dwNetBiosError;
   bLana          = source->bLana;
   ::strcpy( szNetBiosError,    source->szNetBiosError    );
   ::strcpy( szWorkstationName, source->szWorkstationName );
}

void CRemoteAccessServiceNetBEUIFramer::Copy( const CRemoteAccessServiceNetBEUIFramer& source )
{
   dwError        = source.dwError;
   dwNetBiosError = source.dwNetBiosError;
   bLana          = source.bLana;                      
   ::strcpy( szNetBiosError,    source.szNetBiosError    );
   ::strcpy( szWorkstationName, source.szWorkstationName );
}

BYTE CRemoteAccessServiceNetBEUIFramer::GetLana( void ) const
{
   return( bLana );
}

DWORD CRemoteAccessServiceNetBEUIFramer::GetErrorCode( void ) const
{
   return( dwError );
}

DWORD CRemoteAccessServiceNetBEUIFramer::GetNetBiosErrorCode( void ) const
{
   return( dwNetBiosError );
}

void CRemoteAccessServiceNetBEUIFramer::GetErrorString( CString& return_string ) const
{
   return_string = szNetBiosError;
}

void CRemoteAccessServiceNetBEUIFramer::GetWorkstationName( CString& return_string ) const
{
   return_string = szWorkstationName;
}

/*
** CRemoteAccessServiceInternetworkPacketExchange
*/

CRemoteAccessServiceInternetworkPacketExchange::CRemoteAccessServiceInternetworkPacketExchange()
{
   m_Initialize();
}

CRemoteAccessServiceInternetworkPacketExchange::CRemoteAccessServiceInternetworkPacketExchange( const RASPPPIPX * source )
{
   m_Initialize();
   Copy( source );
}

CRemoteAccessServiceInternetworkPacketExchange::CRemoteAccessServiceInternetworkPacketExchange( const CRemoteAccessServiceInternetworkPacketExchange& source )
{
   m_Initialize();
   Copy( source );
}

CRemoteAccessServiceInternetworkPacketExchange::~CRemoteAccessServiceInternetworkPacketExchange()
{
   m_Initialize();
}

void CRemoteAccessServiceInternetworkPacketExchange::m_Initialize( void )
{
   ::ZeroMemory( (RASPPPIPX *) this, sizeof( RASPPPIPX ) );
   dwSize = sizeof( RASPPPIPX );
}

void CRemoteAccessServiceInternetworkPacketExchange::Copy( const RASPPPIPX * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

   dwError = source->dwError;
   ::strcpy( szIpxAddress, source->szIpxAddress );
}

void CRemoteAccessServiceInternetworkPacketExchange::Copy( const CRemoteAccessServiceInternetworkPacketExchange& source )
{
   dwError = source.dwError;
   ::strcpy( szIpxAddress, source.szIpxAddress );
}

DWORD CRemoteAccessServiceInternetworkPacketExchange::GetErrorCode( void ) const
{
   return( dwError );
}

void CRemoteAccessServiceInternetworkPacketExchange::GetIPXAddress( CString& return_string ) const
{
   return_string =szIpxAddress;
}

/*
** CRemoteAccessServiceInternetProtocol
*/

CRemoteAccessServiceInternetProtocol::CRemoteAccessServiceInternetProtocol()
{
   m_Initialize();
}

CRemoteAccessServiceInternetProtocol::CRemoteAccessServiceInternetProtocol( const RASPPPIP * source )
{
   m_Initialize();
   Copy( source );
}

CRemoteAccessServiceInternetProtocol::CRemoteAccessServiceInternetProtocol( const CRemoteAccessServiceInternetProtocol& source )
{
   m_Initialize();
   Copy( source );
}

CRemoteAccessServiceInternetProtocol::~CRemoteAccessServiceInternetProtocol()
{
   m_Initialize();
}

void CRemoteAccessServiceInternetProtocol::m_Initialize( void )
{
   ::ZeroMemory( (RASPPPIP *) this, sizeof( RASPPPIP ) );
   dwSize = sizeof( RASPPPIP );
}

void CRemoteAccessServiceInternetProtocol::Copy( const RASPPPIP * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

   dwError = source->dwError;
   ::strcpy( szIpAddress, source->szIpAddress );
}

void CRemoteAccessServiceInternetProtocol::Copy( const CRemoteAccessServiceInternetProtocol& source )
{
   dwError = source.dwError;
   ::strcpy( szIpAddress, source.szIpAddress );
}

DWORD CRemoteAccessServiceInternetProtocol::GetErrorCode( void ) const
{
   return( dwError );
}

void CRemoteAccessServiceInternetProtocol::GetIPAddress( CString& return_string ) const
{
   return_string = szIpAddress;
}

/*
** CRemoteAccessService
*/

CRemoteAccessService::CRemoteAccessService()
{
   m_Initialize();
}

CRemoteAccessService::~CRemoteAccessService()
{
   m_Initialize();
}

void CRemoteAccessService::m_Initialize( void )
{
   ASSERT_VALID( this );

   ::ZeroMemory( &m_ConnectionStatus, sizeof( m_ConnectionStatus ) );
   m_ConnectionStatus.dwSize = sizeof( m_ConnectionStatus );

   m_ErrorCode           = 0;
   m_ConnectionHandle = NULL;
}

BOOL CRemoteAccessService::Close( LPCTSTR name_of_connection )
{
   ASSERT_VALID( this );
   return( HangUp( name_of_connection ) );
}

BOOL CRemoteAccessService::Dial( LPCTSTR who_to_dial )
{
   ASSERT_VALID( this );
   ASSERT( who_to_dial != NULL );

   if ( who_to_dial == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   RASDIALPARAMS dialing_parameters;

   ::ZeroMemory( &dialing_parameters, sizeof( dialing_parameters ) );

   dialing_parameters.dwSize = sizeof( dialing_parameters );
   strcpy( dialing_parameters.szEntryName, who_to_dial );

   m_ConnectionHandle = NULL;

   m_ErrorCode = ::RasDial( NULL, NULL, &dialing_parameters, 0, NULL, &m_ConnectionHandle );

   if ( m_ErrorCode == 0 )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

void CRemoteAccessService::m_GetConnectionsIntoMemory( LPRASCONN& connections, DWORD& number_of_connections )
{
   ASSERT_VALID( this );

   DWORD size_of_buffer         = 64 * sizeof( RASCONN );
   DWORD number_of_bytes_needed = size_of_buffer;
   DWORD number_of_entries      = 0;

   LPRASCONN memory_buffer = (LPRASCONN) new BYTE[ size_of_buffer ]; // Get room for 64 entries (to begin with)

   if ( memory_buffer == NULL )
   {
      m_ErrorCode = ERROR_NOT_ENOUGH_MEMORY;
      return;
   }

   ::ZeroMemory( memory_buffer, size_of_buffer );

   memory_buffer->dwSize = sizeof( RASCONN );

   m_ErrorCode = ::RasEnumConnections( memory_buffer, &number_of_bytes_needed, &number_of_entries );

   if ( m_ErrorCode != 0 )
   {
      if ( m_ErrorCode == ERROR_BUFFER_TOO_SMALL || m_ErrorCode == ERROR_NOT_ENOUGH_MEMORY )
      {
         delete [] memory_buffer;

         memory_buffer = (LPRASCONN) new BYTE[ number_of_bytes_needed ];

         if ( memory_buffer == NULL )
         {
            m_ErrorCode = ERROR_NOT_ENOUGH_MEMORY;
            return;
         }

         ::ZeroMemory( memory_buffer, number_of_bytes_needed );

         memory_buffer->dwSize = sizeof( RASCONN );
         m_ErrorCode = ::RasEnumConnections( memory_buffer, &number_of_bytes_needed, &number_of_entries );
      }
   }

   connections           = memory_buffer;
   number_of_connections = number_of_entries;
}

HRASCONN CRemoteAccessService::GetConnection( LPCTSTR name_of_connection )
{
   ASSERT_VALID( this );

   if ( name_of_connection == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( NULL );
   }

   LPRASCONN connections       = NULL;
   DWORD     number_of_entries = 0;

   m_GetConnectionsIntoMemory( connections, number_of_entries );

   HRASCONN return_value = NULL;

   if ( m_ErrorCode == 0 )
   {
      DWORD index = 0;

      while( index < number_of_entries )
      {
         if ( ::strcmpi( name_of_connection, connections[ index ].szEntryName ) == 0 )
         {
            return_value = connections[ index ].hrasconn;
            index = number_of_entries;
         }

         index++;
      }
   }

   delete [] connections;

   return( return_value );
}

BOOL CRemoteAccessService::GetConnections( CStringArray& connection_names )
{
   ASSERT_VALID( this );

   connection_names.RemoveAll();

   LPRASCONN connections       = NULL;
   DWORD     number_of_entries = 0;

   m_GetConnectionsIntoMemory( connections, number_of_entries );

   BOOL return_value = FALSE;

   if ( m_ErrorCode == 0 )
   {
      return_value = TRUE;

      DWORD index = 0;

      while( index < number_of_entries )
      {
         connection_names.Add( connections[ index ].szEntryName );
         index++;
      }
   }

   delete [] connections;

   return( return_value );
}

BOOL CRemoteAccessService::GetConnectionStatus( void )
{
   ASSERT_VALID( this );

   m_ErrorCode = ::RasGetConnectStatus( m_ConnectionHandle, &m_ConnectionStatus );

   if ( m_ErrorCode == 0 )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRemoteAccessService::GetErrorCode( void ) const
{
   ASSERT_VALID( this );

   return( m_ErrorCode );
}

BOOL CRemoteAccessService::GetErrorString( CString& return_string )
{
   ASSERT_VALID( this );

   TCHAR string[ 4096 ];

   ::ZeroMemory( string, sizeof( string ) );

   if ( ::RasGetErrorString( m_ErrorCode, string, sizeof( string ) ) == 0 )
   {
      return_string = string;
      return( TRUE );
   }
   else
   {
      return_string.Empty();
      return( FALSE );
   }
}

BOOL CRemoteAccessService::GetPhoneBookEntries( CStringArray& phone_book_entries )
{
   ASSERT_VALID( this );

   phone_book_entries.RemoveAll();

   DWORD size_of_buffer         = 64 * sizeof( RASENTRYNAME );
   DWORD number_of_bytes_needed = size_of_buffer;
   DWORD number_of_entries      = 0;

   LPRASENTRYNAME memory_buffer = (LPRASENTRYNAME) new BYTE[ size_of_buffer ]; // Get room for 64 entries (to begin with)

   if ( memory_buffer == NULL )
   {
      m_ErrorCode = ERROR_NOT_ENOUGH_MEMORY;
      return( FALSE );
   }

   ::ZeroMemory( memory_buffer, size_of_buffer );

   memory_buffer->dwSize = sizeof( RASENTRYNAME );

   m_ErrorCode = ::RasEnumEntries( NULL, NULL, memory_buffer, &number_of_bytes_needed, &number_of_entries );

   if ( m_ErrorCode != 0 )
   {
      if ( m_ErrorCode == ERROR_BUFFER_TOO_SMALL || m_ErrorCode == ERROR_NOT_ENOUGH_MEMORY )
      {
         delete [] memory_buffer;

         memory_buffer = (LPRASENTRYNAME) new BYTE[ number_of_bytes_needed ];

         if ( memory_buffer == NULL )
         {
            m_ErrorCode = ERROR_NOT_ENOUGH_MEMORY;
            return( FALSE );
         }

         ::ZeroMemory( memory_buffer, number_of_bytes_needed );

         memory_buffer->dwSize = sizeof( RASENTRYNAME );
         m_ErrorCode = ::RasEnumEntries( NULL, NULL, memory_buffer, &number_of_bytes_needed, &number_of_entries );
      }
   }

   BOOL return_value = FALSE;

   if ( m_ErrorCode == 0 )
   {
      return_value = TRUE;

      DWORD index = 0;

      while( index < number_of_entries )
      {
         phone_book_entries.Add( memory_buffer[ index ].szEntryName );
         index++;
      }
   }

   delete [] memory_buffer;

   return( return_value );
}

BOOL CRemoteAccessService::GetProtocolInformation( CRemoteAccessServiceAuthenticationMessageBlock& data_to_get )
{
   ASSERT_VALID( this );

   CRemoteAccessServiceAuthenticationMessageBlock return_data;

   if ( ::RasGetProjectionInfo( m_ConnectionHandle, RASP_Amb, (LPVOID) (RASAMB *) &return_data, &return_data.dwSize ) == 0 )
   {
      data_to_get.Copy( return_data );

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRemoteAccessService::GetProtocolInformation( CRemoteAccessServiceNetBEUIFramer& data_to_get )
{
   ASSERT_VALID( this );

   CRemoteAccessServiceNetBEUIFramer return_data;

   if ( ::RasGetProjectionInfo( m_ConnectionHandle, RASP_PppNbf, (LPVOID) (RASPPPNBF *) &return_data, &return_data.dwSize ) == 0 )
   {
      data_to_get.Copy( return_data );

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRemoteAccessService::GetProtocolInformation( CRemoteAccessServiceInternetworkPacketExchange& data_to_get )
{
   ASSERT_VALID( this );

   CRemoteAccessServiceInternetworkPacketExchange return_data;

   if ( ::RasGetProjectionInfo( m_ConnectionHandle, RASP_PppIpx, (LPVOID) (RASPPPIPX *) &return_data, &return_data.dwSize ) == 0 )
   {
      data_to_get.Copy( return_data );

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRemoteAccessService::GetProtocolInformation( CRemoteAccessServiceInternetProtocol& data_to_get )
{
   ASSERT_VALID( this );

   CRemoteAccessServiceInternetProtocol return_data;

   if ( ::RasGetProjectionInfo( m_ConnectionHandle, RASP_PppIp, (LPVOID) (RASPPPIP *) &return_data, &return_data.dwSize ) == 0 )
   {
      data_to_get.Copy( return_data );

      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRemoteAccessService::HangUp( LPCTSTR name_of_connection )
{
   ASSERT_VALID( this );

   if ( name_of_connection == NULL )
   {
      m_ErrorCode = ::RasHangUp( m_ConnectionHandle );
      m_ConnectionHandle = NULL;
   }
   else
   {
      HRASCONN temp_handle = GetConnection( name_of_connection );
      m_ErrorCode = ::RasHangUp( temp_handle );
   }

   if ( m_ErrorCode == 0 )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CRemoteAccessService::IsConnected( void )
{
   ASSERT_VALID( this );

   GetConnectionStatus();

   if ( m_ConnectionStatus.rasconnstate == RASCS_Connected )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}
                                                              
BOOL CRemoteAccessService::Open( LPCTSTR what_to_open )
{
   ASSERT_VALID( this );

   return( Dial( what_to_open ) );
}

