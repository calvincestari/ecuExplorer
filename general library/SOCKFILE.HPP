#ifndef SIMPLE_SOCKET_FILE_CLASS_HEADER

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

#define SIMPLE_SOCKET_FILE_CLASS_HEADER

class CSimpleSocketFile : public CSimpleSocket
{
   public:

      CSimpleSocketFile( SOCKET client_id, const char *host_name, const char *dotted_ip_address_string );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CSimpleSocketFile();

      virtual BOOL Open( void );
      virtual BOOL Open( const char *channel_name, UINT port_number = 23, CFileException* pError = NULL );
};

#endif // SIMPLE_SOCKET_FILE_CLASS_HEADER
