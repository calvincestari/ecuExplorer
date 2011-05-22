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
#endif

IMPLEMENT_SERIAL( CNetwork, CObject, 1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

/*
** CNetResourceInformation stuff
*/

CNetwork::CNetwork( LPCTSTR machine_name )
{
   m_WideMachineName = NULL;
   m_MachineName.Empty();

   Open( machine_name );
}

CNetwork::~CNetwork()
{
   Close();
}

void CNetwork::Close( void )
{
   if ( m_WideMachineName != NULL )
   {
      delete [] m_WideMachineName;
      m_WideMachineName = NULL;
   }

   m_MachineName.Empty();
}

#if defined( _DEBUG )

void CNetwork::Dump( CDumpContext& dump_context ) const
{
   CObject::Dump( dump_context );

   dump_context << "m_MachineName = \"" << m_MachineName << "\"\n";
   dump_context << "m_ErrorCode = "     << m_ErrorCode   << "\n";
}

#endif // _DEBUG

DWORD CNetwork::GetErrorCode( void ) const
{
   return( m_ErrorCode );
}

LPCTSTR CNetwork::GetMachineName( void )
{
   return( (LPCTSTR) m_MachineName );
}

void CNetwork::Open( LPCTSTR machine_name )
{
   Close();

   if ( machine_name != NULL )
   {
      m_WideMachineName = new WCHAR[ ::strlen( machine_name ) + 1 ];

#if defined( UNICODE )
      ::strcpy( m_WideMachineName, machine_name );
#else
      ::ASCII_to_UNICODE( machine_name, m_WideMachineName );
#endif

      m_MachineName = machine_name;
   }
}

void CNetwork::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << m_MachineName;
   }
   else
   {
      CString temp_string;
      archive >> temp_string;
      Open( temp_string );
   }
}
