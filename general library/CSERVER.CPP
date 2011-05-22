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

IMPLEMENT_SERIAL( CServer, CNetwork, 1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

CServer::CServer()
{
   m_Initialize();
}

CServer::CServer( LPCTSTR machine_name )
{
   Open( machine_name );
}

CServer::~CServer()
{
   Close();
}

void CServer::Close( void )
{
   CNetwork::Close();
   m_Initialize();
}

void CServer::GetComment( CString& comment )
{
   comment.Empty();

   /*
   ** Test m_ServerName of emptiness because a lot of comments are blank
   */

   if ( m_Retrieved102 != TRUE )
   {
      m_Get_102_Data();
   }

   comment = m_Comment;
}

void CServer::GetDomain( CString& name )
{
   name.Empty();

   /*
   ** Yes, domain is a member of the 503 structure, but that call doesn't work.
   ** We've got to use 599. Although for numsessions we have to use 503, go figure
   */

   if ( m_Retrieved599 != TRUE )
   {
      m_Get_599_Data();
   }

   name = m_Domain;
}

DWORD CServer::GetMajorVersion( void )
{
   if ( m_Retrieved102 != TRUE )
   {
      m_Get_102_Data();
   }

   return( m_MajorVersion );
}

DWORD CServer::GetMinorVersion( void )
{
   if ( m_Retrieved102 != TRUE )
   {
      m_Get_102_Data();
   }

   return( m_MinorVersion );
}

DWORD CServer::GetNumberOfOpens( void )
{
   if ( m_Retrieved503 != TRUE )
   {
      m_Get_503_Data();
   }

   return( m_NumberOfOpens );
}

DWORD CServer::GetNumberOfUsers( void )
{
   if ( m_Retrieved503 != TRUE )
   {
      m_Get_503_Data();
   }

   return( m_NumberOfUsers );
}

void CServer::GetName( CString& name )
{
   name.Empty();

   if ( m_Retrieved102 != TRUE )
   {
      m_Get_102_Data();
   }

   name = m_MachineName;
}

DWORD CServer::GetPlatform( void )
{
   if ( m_Retrieved102 != TRUE )
   {
      m_Get_102_Data();
   }

   return( m_Platform );
}

void CServer::GetPlatformName( CString& name )
{
   name.Empty();

   /*
   ** Test m_ServerName of emptiness because a lot of comments are blank
   */

   if ( m_Retrieved102 != TRUE )
   {
      m_Get_102_Data();
   }

   switch( m_Platform )
   {
      case SV_PLATFORM_ID_OS2:

         name = "OS/2";
         return;

      case SV_PLATFORM_ID_NT:

         name = "NT";
         return;

      default:

         CString temp_name( "" );

         temp_name.Format( "Unknown Type %d", m_Platform );
         name = temp_name;
         return;
   }
}

void CServer::GetPlatformNameAndVersion( CString& name )
{
   name.Empty();

   CString temp_name;

   GetPlatformName( temp_name );

   name.Format( "%s %d.%d", (LPCTSTR) temp_name, m_MajorVersion, m_MinorVersion );
}

DWORD CServer::GetType( void )
{
   if ( m_Retrieved102 != TRUE )
   {
      m_Get_102_Data();
   }

   return( m_Type );
}

void CServer::GetUserPath( CString& path )
{
   path.Empty();

   if ( m_Retrieved102 != TRUE )
   {
      m_Get_102_Data();
   }

   path = m_UserPath;
}

DWORD CServer::GetUsers( void )
{
   if ( m_Retrieved102 != TRUE )
   {
      m_Get_102_Data();
   }

   return( m_Users );
}

void CServer::m_Get_102_Data( void )
{
   LPBYTE buffer = (LPBYTE) NULL;

   /*
   ** One of the children got loose in the header files again...
   **
   ** Also, we can't get 101 information because it doesn't work if you supply
   ** a machine name... Go Figure...
   */

   ::NetServerGetInfo( (LPTSTR) m_WideMachineName, 102, &buffer );

   if ( buffer != NULL )
   {
      SERVER_INFO_102 *information_p = (SERVER_INFO_102 *) buffer;

#if ! defined( UNICODE )
      ::UNICODE_to_ASCII( (LPCWSTR) information_p->sv102_name,     information_p->sv102_name     );
      ::UNICODE_to_ASCII( (LPCWSTR) information_p->sv102_comment,  information_p->sv102_comment  );
      ::UNICODE_to_ASCII( (LPCWSTR) information_p->sv102_userpath, information_p->sv102_userpath );
#endif

      /*
      ** Now store the info we want...
      */

      m_MachineName  = information_p->sv102_name;
      m_UserPath     = information_p->sv102_userpath;
      m_Users        = information_p->sv102_users;
      m_Comment      = information_p->sv102_comment;
      m_Platform     = information_p->sv102_platform_id;
      m_MajorVersion = information_p->sv102_version_major;
      m_MinorVersion = information_p->sv102_version_minor;
      m_Type         = information_p->sv102_type;
      m_Retrieved102 = TRUE;
   }
}

void CServer::m_Get_503_Data( void )
{
   LPBYTE buffer = (LPBYTE) NULL;

   /*
   ** One of the children got loose in the header files again...
   */

   ::NetServerGetInfo( (LPTSTR) m_WideMachineName, 503, &buffer );

   if ( buffer != NULL )
   {
      SERVER_INFO_503 *information_p = (SERVER_INFO_503 *) buffer;

      /*
      ** Now store the info we want...
      */

      m_NumberOfUsers = information_p->sv503_sessusers;
      m_NumberOfOpens = information_p->sv503_sessopens;
      m_Retrieved503  = TRUE;
   }
}

void CServer::m_Get_599_Data( void )
{
   LPBYTE buffer = (LPBYTE) NULL;

   /*
   ** One of the children got loose in the header files again...
   */

   ::NetServerGetInfo( (LPTSTR) m_WideMachineName, 599, &buffer );

   if ( buffer != NULL )
   {
      SERVER_INFO_599 *information_p = (SERVER_INFO_599 *) buffer;

#if ! defined( UNICODE )

      if ( information_p->sv599_domain != NULL )
      {
         ::UNICODE_to_ASCII( (LPCWSTR) information_p->sv599_domain, information_p->sv599_domain );
      }
      else
      {
         information_p->sv599_domain = " ";
      }

#endif

      /*
      ** Now store the info we want...
      */

      m_Domain       = information_p->sv599_domain;
      m_Retrieved599 = TRUE;
   }
}

void CServer::m_Initialize( void )
{
   m_Comment.Empty();
   m_UserPath.Empty();

   m_Retrieved102 = FALSE;
   m_Retrieved503 = FALSE;
   m_Retrieved599 = FALSE;

   m_MajorVersion  = 0;
   m_MinorVersion  = 0;
   m_NumberOfUsers = 0;
   m_NumberOfOpens = 0;
   m_Platform      = 0;
   m_Type          = 0;
   m_Users         = 0;
}

void CServer::Serialize( CArchive& archive )
{
   CNetwork::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << m_Comment;
      archive << m_UserPath;
      archive << m_Domain;
      archive << m_MajorVersion;
      archive << m_MinorVersion;
      archive << m_NumberOfOpens;
      archive << m_NumberOfUsers;
      archive << m_Platform;
      archive << m_Type;
      archive << m_Users;
   }
   else
   {
      archive >> m_Comment;
      archive >> m_UserPath;
      archive >> m_Domain;
      archive >> m_MajorVersion;
      archive >> m_MinorVersion;
      archive >> m_NumberOfOpens;
      archive >> m_NumberOfUsers;
      archive >> m_Platform;
      archive >> m_Type;
      archive >> m_Users;
   }
}

