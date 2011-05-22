#ifndef TALKING_SOCKET_CLASS_HEADER

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

#define TALKING_SOCKET_CLASS_HEADER

class CTalkingSocket : public CSimpleSocket
{
   private:

      void m_Initialize( void );

   public:

      CTalkingSocket();
      CTalkingSocket( const CString& address, const short p );
      CTalkingSocket( const CString& host, const CString& p );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CTalkingSocket();

      virtual BOOL Open( void );
      virtual BOOL Open( const CString& address, const CString& port_name );
      virtual BOOL Open( const CString& address, const short p_number );
      virtual BOOL Open( const char *name, UINT port_number = 23, CFileException* perror = NULL );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dc ) const;

#endif // _DEBUG
};

#endif // TALKING_SOCKET_CLASS_HEADER
