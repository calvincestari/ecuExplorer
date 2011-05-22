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

CSimpleSocket::CSimpleSocket()
{
   m_Initialize();
}

CSimpleSocket::~CSimpleSocket()
{
   TRACE( "Destroying a CSimpleSocket object\n" );
   Close();
}

void CSimpleSocket::Close( void )
{
   ASSERT_VALID( this );

   if ( m_SocketID != INVALID_SOCKET )
   {
      ::closesocket( m_SocketID );
      m_SocketID = INVALID_SOCKET;
      m_hFile    = CFile::hFileNull;
   }

   Name.Empty();
   Address.Empty();
   AliasList.RemoveAll();
}

#if defined( _DEBUG )

void CSimpleSocket::Dump( CDumpContext &dump_context ) const
{
   CDummyFile::Dump( dump_context );

   dump_context << "Address = \""    << Address    << "\"\n";
   dump_context << "Name = \""       << Name       << "\"\n";
   dump_context << "m_PortName = \"" << m_PortName << "\"\n";
   dump_context << "m_PortNumberInNetworkByteOrder ="  << ::ntohs( m_PortNumberInNetworkByteOrder ) << "\n";
   dump_context << "m_SocketID = " << m_SocketID << "\n";

   int index = 0;
   int number_of_aliases = AliasList.GetCount();

   dump_context << "AliasList conains " << number_of_aliases << " aliases.\n";

   CString string;

   POSITION pos = AliasList.GetHeadPosition();

   while( pos != NULL )
   {
      string = AliasList.GetNext( pos );
      dump_context << index << ". \"" << string << "\"\n";

      index++;
   }
}

#endif // _DEBUG

void CSimpleSocket::GetAddress( CString& _address ) const
{
   ASSERT_VALID( this );

   _address = Address;
}

SOCKET CSimpleSocket::GetID( void ) const
{
   ASSERT_VALID( this );

   return( m_SocketID );
}

void CSimpleSocket::GetName( CString& _host_name ) const
{
   ASSERT_VALID( this );

   _host_name = Name;
}

void CSimpleSocket::GetPort( short& _port_number ) const
{
   ASSERT_VALID( this );

   _port_number = ::ntohs( m_PortNumberInNetworkByteOrder );
}

void CSimpleSocket::GetPort( CString& _port_name ) const
{
   ASSERT_VALID( this );

   _port_name = m_PortName;
}

BOOL CSimpleSocket::IsDataWaiting( void )
{
   ASSERT_VALID( this );

   if ( m_SocketID == INVALID_SOCKET )
   {
      return( FALSE );
   }

   int bytes_for_this_socket = 0;
   int socket_status         = 0;

   FD_SET socket_in;

   TIMEVAL time_out;

   /*
   ** Initialize the data structures
   */

   FD_ZERO( (LPFD_SET) &socket_in );

   time_out.tv_sec  = 0;
   time_out.tv_usec = 0;

   /*
   ** Set socket_in to specify that we are looking for data on socket port_id
   */

   FD_SET( m_SocketID, (LPFD_SET) &socket_in );

   /*
   ** See if data is waiting
   */

   socket_status = ::select( 0, &socket_in, NULL, NULL, &time_out );

   if ( socket_status == SOCKET_ERROR )
   {
      TRACE2( "CSimpleSocket::IsDataWaiting(), Can't select() at line %d of %s\n", __LINE__, __FILE__ );

      m_ErrorCode = ::WSAGetLastError();
      return( FALSE );
   }

   if ( socket_status == 0 )
   {
      /*
      ** No Data is waiting on any socket in the OS
      */

      return( FALSE );
   }

   /*
   ** Welp, data is waiting for *A* socket. It may not be *our* socket. A socket in the
   ** operating system has data waiting in it. Let's see if it happens to be *our* socket.
   */

   bytes_for_this_socket = ::FD_ISSET( m_SocketID, &socket_in );
   
   if ( bytes_for_this_socket == 0 )
   {
      /*
      ** There is data for a socket somewhere in the system but not for our socket
      */

      return( FALSE );
   }

   return( TRUE );
}

void CSimpleSocket::m_Initialize( void )
{
   ASSERT_VALID( this ) ;

   TRACE( "CsimpleSocket::m_Initialize()\n" );

   /*
   ** Make sure everything is empty
   */

   m_PortNumberInNetworkByteOrder = 0;
   m_SocketID                     = INVALID_SOCKET;
   m_hFile                        = CFile::hFileNull;

   Address.Empty();
   Name.Empty();
   m_PortName.Empty();
   AliasList.RemoveAll();
}

UINT CSimpleSocket::Read( VOID *buffer, const int size_of_buffer )
{
   ASSERT_VALID( this );
   ASSERT( buffer != NULL );
   ASSERT( size_of_buffer > 0 );

   if ( buffer == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( 0 );
   }

   if ( m_SocketID != INVALID_SOCKET )
   {
      int number_of_bytes_read = 0;

      ::ZeroMemory( buffer, size_of_buffer );
   
      number_of_bytes_read = ::recv( m_SocketID, (char *) buffer, size_of_buffer, 0 );
   
      if ( number_of_bytes_read == SOCKET_ERROR )
      {
         TRACE( "CSimpleSocket::Read(), Error in recv() at line %d of %s\n", __LINE__, __FILE__ );

         m_ErrorCode = ::WSAGetLastError();
         return( 0 );
      }
      
      return( number_of_bytes_read );   
   }
   else
   {
      return( 0 );
   }
}

void CSimpleSocket::Read( CString& line_to_read )
{
   ASSERT_VALID( this );

   if ( m_SocketID != INVALID_SOCKET )
   {
      char character = 0x00;

      int number_of_bytes_read = 0;

      /*
      ** Need to add CRLF checking!!!!
      */

      while( character != LINE_FEED )
      {
         number_of_bytes_read = ::recv( m_SocketID, &character, 1, 0 );

         if ( number_of_bytes_read == SOCKET_ERROR )
         {
            TRACE( "CSimpleSocket::Read(), Error in recv() at line %d of %s\n", __LINE__, __FILE__ );

            m_ErrorCode = ::WSAGetLastError();
            return;
         }

         line_to_read += character;
      }
   }
}

void CSimpleSocket::SetAddress( const char *address_string )
{
   /*
   ** Called when a client connects to us. This function also fills in the
   ** client's name for security checking.
   */

   ASSERT_VALID( this );
   ASSERT( address_string != NULL );
   ASSERT( AfxIsValidString( address_string ) );

   if ( address_string == NULL )
   {
      Address.Empty();
      return;
   }

   CString address;

   address = address_string;

   /*
   ** Address may contain "131.26.31.92" or "cheetah.sed.csc.com"
   ** Given either, we must fill in host_name and IP_address
   */

   LPHOSTENT host_entry_p = (LPHOSTENT) NULL;

   /*
   ** See if it is in xxx.xxx.xxx.xxx form
   */

   int index = 0;

   BOOL exit_loop = FALSE;

   while( index < address.GetLength() && exit_loop == FALSE )
   {
      if ( address[ index ] != '.' && ! isdigit( address[ index ] ) )
      {
         /*
         ** The character is not a period and not a digit so it cannot meet the requirements
         ** of xxx.xxx.xxx.xxx type address and therefore must be a host name
         */

         exit_loop = TRUE;
      }
      else
      {
         index++;
      }
   }

   if ( exit_loop == TRUE )
   {
      /*
      ** Tech Note: You must cast the CString object to a const char * when using things like printf()
      */

      host_entry_p = ::gethostbyname( (const char *) address );
   }
   else
   {
      ULONG internet_address = 0L;

      internet_address = ::inet_addr( address );
      host_entry_p = ::gethostbyaddr( (const char *) &internet_address, 4, PF_INET );
   }

   if ( host_entry_p == (LPHOSTENT) NULL )
   {
      TRACE( "CSimpleSocket::set_address(), gethostby???() failed at line %d of %s\n", __LINE__, __FILE__ );

      m_ErrorCode = ::WSAGetLastError();
      return;
   }

   if ( exit_loop != TRUE )
   {
      Address = address;
   }
   else
   {
      LPSTR dotted_ip_address = (LPSTR) NULL;

      /*
      ** You just gotta love the way Unix people thought . . . NOT!
      */

      struct in_addr internet_address;

      internet_address.S_un.S_un_b.s_b1 = host_entry_p->h_addr_list[ 0 ][ 0 ];
      internet_address.S_un.S_un_b.s_b2 = host_entry_p->h_addr_list[ 0 ][ 1 ];
      internet_address.S_un.S_un_b.s_b3 = host_entry_p->h_addr_list[ 0 ][ 2 ];
      internet_address.S_un.S_un_b.s_b4 = host_entry_p->h_addr_list[ 0 ][ 3 ];

      dotted_ip_address = ::inet_ntoa( internet_address );

	  if ( dotted_ip_address == (LPSTR) NULL )
	  {
	     return;
	  }

      Address = dotted_ip_address;

      TRACE( "ip_address == \"%s\"\n", (const char *) Address );
   }

   /*
   ** We don't call set_name() because that function will call this function and we'll go into an endless loop
   */

   Name = host_entry_p->h_name;

   /*
   ** Now lets get the aliases for this fella
   */

   AliasList.RemoveAll();

   index = 0;

   CString *new_string_p = NULL;

   while( host_entry_p->h_aliases[ index ] != (char *) NULL )
   {
      AliasList.AddTail( (const char *) host_entry_p->h_aliases[ index ] );
      index++;
   }
}

void CSimpleSocket::SetID( const SOCKET id )
{
   ASSERT_VALID( this );

   m_SocketID = id;
   m_hFile    = (UINT) id;
}

void CSimpleSocket::SetName( const char *host_string )
{
   ASSERT_VALID( this );
   ASSERT( host_string != NULL );

   if ( host_string == NULL )
   {
      Name.Empty();
   }
   else
   {
      Name = host_string;
   }
}

void CSimpleSocket::SetPort( const char *name_string )
{
   ASSERT_VALID( this );
   ASSERT( name_string != NULL );

   if ( name_string == NULL )
   {
      m_PortName.Empty();
      m_PortNumberInNetworkByteOrder = 0;
      return;
   }

   CString name;

   name = name_string;

   /*
   ** Although not documented anywhere, the name of the protocol must be in lower case
   ** If you look in the data file for getservbyname() [winnt/system32/drivers/etc/services]
   ** you will notice that everything is in lower case. Gotta love Unix . . .
   */

   name.MakeLower();

   /*
   ** This routine sets port_name and m_PortNumberInNetworkByteOrder
   */

   LPSERVENT service_entry_p = (LPSERVENT) NULL;

   service_entry_p = ::getservbyname( name, NULL );

   if ( service_entry_p == (LPSERVENT) NULL )
   {
      m_ErrorCode = ::WSAGetLastError();

      TRACE( "CSimpleSocket::set_server_port(), getservbyname() failed at line %d of %s, Last Error is %d\n", __LINE__, __FILE__, m_ErrorCode );

      return;
   }

   m_PortName = name;
   m_PortNumberInNetworkByteOrder = service_entry_p->s_port;
}

void CSimpleSocket::SetPort( const short p )
{
   ASSERT_VALID( this );

   /*
   ** This routine sets port_name and m_PortNumberInNetworkByteOrder
   */

   m_PortNumberInNetworkByteOrder = ::htons( p );

   /*
   ** Now go find a name for this port . . .
   */

   LPSERVENT service_entry_p = (LPSERVENT) NULL;

   service_entry_p = ::getservbyport( m_PortNumberInNetworkByteOrder, NULL );

   if ( service_entry_p == (LPSERVENT) NULL )
   {
      TRACE( "CSimpleSocket::set_port(), getservbyport() failed at line %d of %s\n", __LINE__, __FILE__ );

      m_ErrorCode = ::WSAGetLastError();
      m_PortName.Empty();
      return;
   }

   m_PortName = service_entry_p->s_name;
}

void __stdcall CSimpleSocket::StartWindowsSockets( void )
{
   /*
   ** Start WINSOCK
   */

   WSADATA winsock_data;

   int socket_error = 0;

   WORD desired_winsock_version = 0x0101; // We'd like WINSOCK v1.1 at least

   BYTE major_version_required = 0;
   BYTE minor_version_required = 0;

   ::ZeroMemory( &winsock_data, sizeof( winsock_data ) );

   socket_error = ::WSAStartup( desired_winsock_version, (LPWSADATA) &winsock_data );

   if ( socket_error != 0 )
   {
      TRACE( "WSAStartup failed with an error code of %d at line %d of %s\n", socket_error, __LINE__, __FILE__ );
      AfxAbort();
   }

   major_version_required = HIBYTE( desired_winsock_version );
   minor_version_required = LOBYTE( desired_winsock_version );

   if ( (   LOBYTE( winsock_data.wVersion ) <  major_version_required ) ||
        (   LOBYTE( winsock_data.wVersion ) == major_version_required ) &&
        ( ( HIBYTE( winsock_data.wVersion ) <  minor_version_required ) ) )
   {
      TRACE( "Need a later version of Winsock\n" );
   }
}

void __stdcall CSimpleSocket::StopWindowsSockets( void )
{
   ::WSACleanup();
}

void __stdcall CSimpleSocket::TranslateErrorCode( DWORD error_code, LPSTR destination_string, DWORD size_of_destination_string )
{
   switch( error_code )
   {
      /*
      ** Following are Windows Sockets Library errors
      */

      case WSAENOTSOCK:

        strncpy( destination_string, "WSAENOTSOCK, Socket operation on non-socket. A socket created in one process is used by another process.", size_of_destination_string );
        return;

      case WSAEDESTADDRREQ:

        strncpy( destination_string, "WSAEDESTADDRREQ, Destination address required", size_of_destination_string );
        return;

      case WSAEMSGSIZE:

        strncpy( destination_string, "WSAEMSGSIZE, Message too long", size_of_destination_string );
        return;

      case WSAEPROTOTYPE:

        strncpy( destination_string, "WSAEPROTOTYPE, Protocol wrong type for socket", size_of_destination_string );
        return;

      case WSAENOPROTOOPT:

        strncpy( destination_string, "WSAENOPROTOOPT, Protocol not available", size_of_destination_string );
        return;

      case WSAEPROTONOSUPPORT:

        strncpy( destination_string, "WSAEPROTONOSUPPORT, Protocol not supported", size_of_destination_string );
        return;

      case WSAESOCKTNOSUPPORT:

        strncpy( destination_string, "WSAESOCKTNOSUPPORT, Socket type not supported", size_of_destination_string );
        return;

      case WSAEOPNOTSUPP:

        strncpy( destination_string, "WSAEOPNOTSUPP, Operation not supported on socket", size_of_destination_string );
        return;

      case WSAEPFNOSUPPORT:

        strncpy( destination_string, "WSAEPFNOSUPPORT, Protocol family not supported", size_of_destination_string );
        return;

      case WSAEAFNOSUPPORT:

        strncpy( destination_string, "WSEAFNOSUPPORT, Address family not supported by protocol family", size_of_destination_string );
        return;

      case WSAEADDRINUSE:

        strncpy( destination_string, "WSAEADDRINUSE, Triggered by bind() because a process went down without closing a socket.", size_of_destination_string );
        return;

      case WSAEADDRNOTAVAIL:

        strncpy( destination_string, "WSAEADDRNOTAVAIL, Can't assign requested address", size_of_destination_string );
        return;

      case WSAENETDOWN:

        strncpy( destination_string, "WSAENETDOWN, Network is down", size_of_destination_string );
        return;

      case WSAENETUNREACH:

        strncpy( destination_string, "WSAENETUNREACH, Network is unreachable", size_of_destination_string );
        return;

      case WSAENETRESET:

        strncpy( destination_string, "WSAENETRESET, Network dropped connection or reset", size_of_destination_string );
        return;

      case WSAECONNABORTED:

        strncpy( destination_string, "WSAECONNABORTED, Software caused connection abort", size_of_destination_string );
        return;

      case WSAECONNRESET:

        strncpy( destination_string, "WSAECONNRESET, Connection reset by peer", size_of_destination_string );
        return;

      case WSAENOBUFS:

        strncpy( destination_string, "WSAENOBUFS, No buffer space available.", size_of_destination_string );
        return;

      case WSAEISCONN:

        strncpy( destination_string, "WSAEISCONN, Socket is already connected", size_of_destination_string );
        return;

      case WSAENOTCONN:

        strncpy( destination_string, "WSAENOTCONN, Socket is not connected", size_of_destination_string );
        return;

      case WSAESHUTDOWN:

        strncpy( destination_string, "WSAESHUTDOWN, Can't send after socket shutdown", size_of_destination_string );
        return;

      case WSAETIMEDOUT:

        strncpy( destination_string, "WSAETIMEDOUT, Connection timed out", size_of_destination_string );
        return;

      case WSAECONNREFUSED:

        strncpy( destination_string, "WSAECONNREFUSED, Connection refused", size_of_destination_string );
        return;

      case WSAEHOSTDOWN:

        strncpy( destination_string, "WSAEHOSTDOWN, Networking subsystem not started", size_of_destination_string );
        return;

      case WSAEHOSTUNREACH:

        strncpy( destination_string, "WSAEHOSTUNREACH, No route to host", size_of_destination_string );
        return;

      case WSAEWOULDBLOCK:

        strncpy( destination_string, "WSAEWOULDBLOCK, Operation would block", size_of_destination_string );
        return;

      case WSAEINPROGRESS:

        strncpy( destination_string, "WSAEINPROGRESS, Operation now in progress", size_of_destination_string );
        return;

      case WSAEALREADY:

        strncpy( destination_string, "WSAEALREADY, Operation already in progress", size_of_destination_string );
        return;

      case WSAEINTR:

        strncpy( destination_string, "WSAEALREADY, Operation was interrupted", size_of_destination_string );
        return;

      case WSAEBADF:

        strncpy( destination_string, "WSAEBADF, Bad file number", size_of_destination_string );
        return;

      case WSAEACCES:

        strncpy( destination_string, "WSAEACCES, Access is denied", size_of_destination_string );
        return;

      case WSAEFAULT:

        strncpy( destination_string, "WSAEFAULT, Bad memory address", size_of_destination_string );
        return;

      case WSAEINVAL:

        strncpy( destination_string, "WSAEINVAL, The socket has not been bound with bind() or is already connected", size_of_destination_string );
        return;

      case WSAEMFILE:

        strncpy( destination_string, "WSAEMFILE, No more file descriptors are available", size_of_destination_string );
        return;

      case WSAETOOMANYREFS:

        strncpy( destination_string, "WSAETOOMANYREFS, Undocumented WinSock error", size_of_destination_string );
        return;

      case WSAENAMETOOLONG:

        strncpy( destination_string, "WSAENAMETOOLONG, Undocumented WinSock error", size_of_destination_string );
        return;

      case WSAENOTEMPTY:

        strncpy( destination_string, "WSAENOTEMPTY, Undocumented WinSock error", size_of_destination_string );
        return;

      case WSAEPROCLIM:

        strncpy( destination_string, "WSAEPROCLIM, Undocumented WinSock error", size_of_destination_string );
        return;

      case WSAEUSERS:

        strncpy( destination_string, "WSAEUSERS, Undocumented WinSock error", size_of_destination_string );
        return;

      case WSAEDQUOT:

        strncpy( destination_string, "WSAEDQUOT, Undocumented WinSock error", size_of_destination_string );
        return;

      case WSAESTALE:

        strncpy( destination_string, "WSAESTALE, Undocumented WinSock error", size_of_destination_string );
        return;

      case WSAEREMOTE:

        strncpy( destination_string, "WSAEREMOTE, Undocumented WinSock error", size_of_destination_string );
        return;

      case WSAEDISCON:

        strncpy( destination_string, "WSAEDISCON, Circuit was gracefully terminated", size_of_destination_string );
        return;

      case WSASYSNOTREADY:

        strncpy( destination_string, "WSASYSNOTREADY, The underlying network subsystem is not ready for network communication", size_of_destination_string );
        return;

      case WSAVERNOTSUPPORTED:

        strncpy( destination_string, "WSAVERNOTSUPPORTED, The version of Windows Sockets API support requested is not provided by this particular Windows Sockets implementation", size_of_destination_string );
        return;

      case WSANOTINITIALISED:

        strncpy( destination_string, "WSANOTINITIALISED, WSAStartup() has not been called", size_of_destination_string );
        return;

      case WSAHOST_NOT_FOUND:

        strncpy( destination_string, "WSAHOST_NOT_FOUND, Authoritative answer host not found", size_of_destination_string );
        return;

      case WSATRY_AGAIN:

        strncpy( destination_string, "WSATRY_AGAIN, Non-authoritative answer host not found or SERVERFAIL", size_of_destination_string );
        return;

      case WSANO_RECOVERY:

        strncpy( destination_string, "WSANO_RECOVERY, Non recoverable errors, FORMERR, REFUSED, NOTIMP", size_of_destination_string );
        return;

      case WSANO_DATA:

        strncpy( destination_string, "WSANO_DATA or WSANO_ADDRESS, Valid name, no data record of requested type", size_of_destination_string );
        return;

      default:

         {
            TCHAR message_string[ 129 ];

            wsprintf( (LPTSTR) message_string, (LPCSTR) TEXT( "Unknown WinSock Error Number %d" ), error_code );
            strncpy( destination_string, message_string, size_of_destination_string );
         }

         return;
   }
}

void CSimpleSocket::Write( const VOID *buffer, const long number_of_bytes_to_write )
{
   ASSERT_VALID( this );
   ASSERT( buffer != NULL );
   ASSERT( number_of_bytes_to_write > 0L );

   if ( buffer == NULL )
   {
      return;
   }

   if ( m_SocketID == INVALID_SOCKET )
   {
      return;
   }

   BYTE *byte_buffer = (BYTE *) NULL;

   byte_buffer = (BYTE *) buffer;

   if ( byte_buffer == (BYTE *) NULL )
   {
      return;
   }

   /*
   ** Loop until the bytes are sent or until we give up
   */

   BOOL bytes_were_sent = FALSE;

   int number_of_bytes_sent = 0;
   int loop_count           = 0;

   while( bytes_were_sent == FALSE && loop_count < 100 )
   {
      number_of_bytes_sent = ::send( m_SocketID, (const char *) byte_buffer, number_of_bytes_to_write, 0 );

      if ( number_of_bytes_sent == SOCKET_ERROR )
      {
         m_ErrorCode = ::WSAGetLastError();

         char temp_string[ 513 ];

         TranslateErrorCode( m_ErrorCode, temp_string, sizeof( temp_string ) );

         TRACE1( "CSimpleSocket::Write, %s\n", temp_string );

         if ( m_ErrorCode != WSAENOBUFS &&
              m_ErrorCode != WSAEINPROGRESS )
         {
            if ( m_ErrorCode == WSAENOTCONN  ||
                 m_ErrorCode == WSAENETRESET ||
                 m_ErrorCode == WSAESHUTDOWN )
            {
               /*
               ** Someone hung up on us or unplugged our lan cable
               */

               m_SocketID = INVALID_SOCKET;
               m_hFile    = CFile::hFileNull;
            }

            return;
         }
         else
         {
            loop_count++;
         }
      }
      else
      {
         bytes_were_sent = TRUE;
      }
   }
}

void CSimpleSocket::Write( const CString& string_to_write )
{
   ASSERT_VALID( this );

   CString temp_string;

   temp_string = string_to_write;

   Write( (VOID *) temp_string.GetBuffer( temp_string.GetLength() + 1 ), temp_string.GetLength() );
}

#pragma warning( default : 4100 )
