#ifndef LISTENING_SOCKET_CLASS_HEADER

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

#define LISTENING_SOCKET_CLASS_HEADER

class CListeningSocket : public CSimpleSocket
{
   private:

      SOCKET m_ServerSocketID; // Used in listen()

      int m_NumberOfSimultaneousSocketsToAllow;

      void m_Initialize( void );

   public:

      CListeningSocket();
      CListeningSocket( SOCKET client_id );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CListeningSocket();

      virtual void OnNewConnection( SOCKET socket_id, const char *host_name, const char *dotted_ip_address_string ) = 0; // Called by WaitForConnection
      virtual BOOL Open( void );
      virtual BOOL Open( const char *channel_name, UINT port_number = 23, CFileException * perror = NULL );
      virtual BOOL WaitForConnection( const char *p_name );
      virtual BOOL WaitForConnection( const int p_number );
      virtual BOOL WaitForConnection( void );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dc ) const;

#endif // _DEBUG
};

#endif // LISTENING_SOCKET_CLASS_HEADER
