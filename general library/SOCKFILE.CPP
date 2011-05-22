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

CSimpleSocketFile::CSimpleSocketFile( SOCKET client_id, const char * name, const char *dotted_ip_address )
{
   m_SocketID = client_id;
   m_hFile    = client_id;

   if ( name == NULL )
   {
      Name.Empty();
   }
   else
   {
      Name = name;
   }

   if ( dotted_ip_address == NULL )
   {
      Address.Empty();
   }
   else
   {
      Address = dotted_ip_address;
   }
}

CSimpleSocketFile::~CSimpleSocketFile()
{
   TRACE( "Destroying a CSimpleSocketFile object\n" );
   Close();
}

BOOL CSimpleSocketFile::Open( void )
{
   ASSERT_VALID( this );
   return( TRUE );
}

#pragma warning( disable : 4100 )

BOOL CSimpleSocketFile::Open( const char *name, UINT port, CFileException *perror )
{
   ASSERT_VALID( this );
   return( TRUE );
}

#pragma warning( default : 4100 )
