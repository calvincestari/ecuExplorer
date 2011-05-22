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

CListeningSocket::CListeningSocket()
{
   TRACE( "CListeningSocket::CListeningSocket()\n" );
   m_Initialize();
}

CListeningSocket::~CListeningSocket()
{
   TRACE( "Destroying a CListeningSocket object\n" );
}

#if defined( _DEBUG )

void CListeningSocket::Dump( CDumpContext &dump_context ) const
{
   CSimpleSocket::Dump( dump_context );
   dump_context << "{\n";
   dump_context << "   m_ServerSocketID = " << m_ServerSocketID << "\n";
   dump_context << "   m_NumberOfSimultaneousSocketsToAllow = " << m_NumberOfSimultaneousSocketsToAllow << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CListeningSocket::m_Initialize( void )
{
   ASSERT_VALID( this );

   TRACE( "CListeningSocket::m_Initialize(), Initializing a CListeningSocket class\n" );

   /*
   ** Make sure that everything is empty
   */

   m_ServerSocketID                     = INVALID_SOCKET;
   m_NumberOfSimultaneousSocketsToAllow = 10;
}

BOOL CListeningSocket::Open( void )
{
   ASSERT_VALID( this );

   if ( m_ServerSocketID != INVALID_SOCKET )
   {
      TRACE( "CListeningSocket::Open(), server socket already created at line %d of %s\n", __LINE__, __FILE__ );
      return( TRUE );
   }

   if ( m_PortNumberInNetworkByteOrder == 0 )
   {
      TRACE( "CListeningSocket::open(), can't server socket without a port number at line %d of %s\n", __LINE__, __FILE__ );
      return( FALSE );
   }

   /*
   ** Create the server (or listening) socket thingy
   */

   m_ServerSocketID = ::socket( AF_INET, SOCK_STREAM, 0 );

   if ( m_ServerSocketID == INVALID_SOCKET )
   {
      TRACE( "CListeningSocket::open(), socket() failed at line %d of %s\n", __LINE__, __FILE__ );

      m_ErrorCode = ::WSAGetLastError();
      return( FALSE );
   }

   /*
   ** We've got a socket thingy but its useless. It doesn't have an address. Let's give it one.
   ** We do this by bind'ing an address and port number to it.
   */

   SOCKADDR_IN socket_address;

   socket_address.sin_family      = AF_INET;
   socket_address.sin_port        = m_PortNumberInNetworkByteOrder;
   socket_address.sin_addr.s_addr = ::htonl( INADDR_ANY ); // We'll let anybody connect to us

   if ( ::bind( m_ServerSocketID, (LPSOCKADDR) &socket_address, sizeof( socket_address ) ) == SOCKET_ERROR )
   {
      TRACE( "CListeningSocket::open(), bind() failed at line %d of %s\n", __LINE__, __FILE__ );

      m_ErrorCode = ::WSAGetLastError();
      return( FALSE );
   }

   /*
   ** Now the socket thingy has an address and port number.
   */

   /*
   ** Now we make it a listening socket and start listening, program execution halts here
   */
   
   if ( ::listen( m_ServerSocketID, m_NumberOfSimultaneousSocketsToAllow ) == SOCKET_ERROR )
   {
      TRACE( "CListeningSocket::Open(), Can't listen() at line %d of %s\n", __LINE__, __FILE__ );

      m_ErrorCode = ::WSAGetLastError();
      return( FALSE );
   }

   return( TRUE );
}

#pragma warning( disable : 4100 )

BOOL CListeningSocket::Open( const char *ChannelName, UINT port_number, CFileException *pError )
{
   ASSERT_VALID( this );

   SetPort( (short) port_number );

   return( Open() );
}

#pragma warning( default : 4100 )

BOOL CListeningSocket::WaitForConnection( void )
{
   ASSERT_VALID( this );

   if ( m_PortNumberInNetworkByteOrder == 0 )
   {
      return( FALSE );
   }

   if ( m_ServerSocketID == INVALID_SOCKET )
   {
      if ( Open() == FALSE )
      {
         TRACE( "CListeningSocket::WaitForConnection(), Can't open() at line %d of %s\n", __LINE__, __FILE__ );
         return( FALSE );
      }
   }

   SOCKADDR incoming_socket_address;

   int byte_count = 0;

   byte_count = sizeof( incoming_socket_address );

   ::ZeroMemory( &incoming_socket_address, byte_count );

   /*
   ** PROGRAM EXECUTION STOPS HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   **
   ** accept() is a blocking call meaning that the this thread of execution is paused
   ** (ie goes to sleep) until someone on the network connects to us. We will "wake up"
   ** when that happens and continue along our merry way.
   */

   m_SocketID = ::accept( m_ServerSocketID, &incoming_socket_address, &byte_count );

   if ( m_SocketID == INVALID_SOCKET )
   {
      m_ErrorCode = ::WSAGetLastError();
	  return( FALSE );
   }

   /*
   ** The method for actually converting the incoming address to something that is human
   ** readable is either undocumented or extremely poorly documented. Not suprising since
   ** the idea of sockets came out of the Unix world...
   */

   LPSTR dotted_ip_address = (LPSTR) NULL;
    
   struct in_addr internet_address;

   /*
   ** Aren't these structure member names intuitively obvious??
   */

   internet_address.S_un.S_un_b.s_b1 = incoming_socket_address.sa_data[ 2 ];
   internet_address.S_un.S_un_b.s_b2 = incoming_socket_address.sa_data[ 3 ];
   internet_address.S_un.S_un_b.s_b3 = incoming_socket_address.sa_data[ 4 ];
   internet_address.S_un.S_un_b.s_b4 = incoming_socket_address.sa_data[ 5 ];

   dotted_ip_address = ::inet_ntoa( internet_address );

   if ( dotted_ip_address == (LPSTR) NULL )
   {
      m_ErrorCode = WSAEINVAL;
	  return( FALSE );
   }

   ULONG temp_long = 0L;

   temp_long = ::inet_addr( (LPCSTR) dotted_ip_address );

   if ( temp_long == INADDR_NONE )
   {
      m_ErrorCode = WSAEINVAL;
	  return( FALSE );
   }

   LPHOSTENT host_entry_p = (LPHOSTENT) NULL;

   host_entry_p = ::gethostbyaddr( (const char *) &temp_long, 4, PF_INET );

   if ( host_entry_p == (LPHOSTENT) NULL )
   {
      m_ErrorCode = ::WSAGetLastError();
	  return( FALSE );
   }

   TRACE( "%s (%s) just connected to us\n", host_entry_p->h_name, (const char *) dotted_ip_address );

   SetAddress( dotted_ip_address );

   OnNewConnection( m_SocketID, Name, dotted_ip_address );

   return( TRUE );
}

BOOL CListeningSocket::WaitForConnection( const int port_number )
{
   ASSERT_VALID( this );
   ASSERT( port_number > 0 );

   SetPort( (short) port_number );

   return( WaitForConnection() );
}

BOOL CListeningSocket::WaitForConnection( const char *p_name )
{
   ASSERT_VALID( this );
   ASSERT( p_name != NULL );

   if ( p_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   SetPort( p_name );

   return( WaitForConnection() );
}
