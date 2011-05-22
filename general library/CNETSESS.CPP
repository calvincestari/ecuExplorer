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

IMPLEMENT_SERIAL( CNetworkSessions, CNetwork, 1 )
IMPLEMENT_SERIAL( CNetworkSessionInformation, CObject, 1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

CNetworkSessions::CNetworkSessions()
{
   m_Initialize();
}

CNetworkSessions::CNetworkSessions( LPCTSTR machine_name )
{
   Open( machine_name );
}

CNetworkSessions::~CNetworkSessions()
{
   Close();
}

void CNetworkSessions::Close( void )
{
   CNetwork::Close();
   m_Initialize();
}

BOOL CNetworkSessions::Delete( CNetworkSessionInformation& information )
{
   WCHAR wide_user_name[ 256 ];
   WCHAR wide_client_name[ 256 ];

   ::ZeroMemory( wide_user_name,   sizeof( wide_user_name   ) );
   ::ZeroMemory( wide_client_name, sizeof( wide_client_name ) );

   LPWSTR user   = (LPWSTR) NULL;
   LPWSTR client = (LPWSTR) NULL;

   if ( information.UserName.GetLength() > 0 )
   {
#if ! defined( UNICODE )
      ::strcpy( (LPSTR) wide_user_name, information.UserName );
      ::ASCII_to_UNICODE( (LPSTR) wide_user_name, wide_user_name );
#else
      ::strcpy( wide_user_name, information.UserName );
#endif // UNICODE

      user = wide_user_name;
   }

   if ( information.ClientName.GetLength() > 0 )
   {
#if ! defined( UNICODE )
      ::strcpy( (LPSTR) wide_client_name, information.ClientName );
      ::ASCII_to_UNICODE( (LPSTR) wide_client_name, wide_client_name );
#else
      ::strcpy( client_user_name, information.ClientName );
#endif // UNICODE

      client = wide_client_name;
   }

   m_ErrorCode = ::NetSessionDel( (LPTSTR) m_WideMachineName,
                                  (LPTSTR) client,
                                  (LPTSTR) user );
   
   if ( m_ErrorCode == NERR_Success )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CNetworkSessions::Enumerate( CNetworkSessionInformation& information )
{
   SESSION_INFO_502 *information_p = (SESSION_INFO_502 *) NULL;

   SESSION_INFO_502 s;

   ::ZeroMemory( &s, sizeof( s ) );

   information_p = &s;

   DWORD prefered_maximum_length = sizeof( SESSION_INFO_502 ) * 128;
   DWORD number_of_entries_read  = 0;
   DWORD total_number_of_entries = 0;

   m_ErrorCode = ::NetSessionEnum( (LPTSTR) m_WideMachineName, 
                                   (LPTSTR) NULL, 
                                   (LPTSTR) NULL, 
                                            502,
                                (LPBYTE *) &information_p,
                                            prefered_maximum_length,
                                           &number_of_entries_read,
                                           &total_number_of_entries,
                                           &m_ResumeHandle );

   if ( information_p != (SESSION_INFO_502 *) NULL )
   {
      information.Copy( information_p );
      return( TRUE );
   }

   return( FALSE );
}

BOOL CNetworkSessions::GetNext( CNetworkSessionInformation& information )
{
   return( Enumerate( information ) );
}

void CNetworkSessions::m_Get_0_Data( void )
{
   LPBYTE buffer = (LPBYTE) NULL;

   /*
   ** One of the children got loose in the header files again...
   **
   ** Also, we can't get 101 information because it doesn't work if you supply
   ** a machine name... Go Figure...
   */

   ::NetSessionGetInfo( (LPTSTR) m_WideMachineName, NULL, NULL, 0, &buffer );

   if ( buffer != NULL )
   {
      SESSION_INFO_0 *information_p = (SESSION_INFO_0 *) buffer;

#if ! defined( UNICODE )
      ::UNICODE_to_ASCII( (LPCWSTR) information_p->sesi0_cname, information_p->sesi0_cname );
#endif

      /*
      ** Now store the info we want...
      */

      m_ServerName = information_p->sesi0_cname;
      m_Retrieved0 = TRUE;
   }
}

void CNetworkSessions::m_Get_1_Data( void )
{
}

void CNetworkSessions::m_Get_2_Data( void )
{
}

void CNetworkSessions::m_Get_10_Data( void )
{
}

void CNetworkSessions::m_Get_502_Data( void )
{
}

void CNetworkSessions::m_Initialize( void )
{
   m_ServerName.Empty();
   m_ClientName.Empty();

   m_Retrieved0   = FALSE;
   m_Retrieved1   = FALSE;
   m_Retrieved2   = FALSE;
   m_Retrieved10  = FALSE;
   m_Retrieved502 = FALSE;

   m_ErrorCode    = 0;
   m_ResumeHandle = 0;
}

void CNetworkSessions::Serialize( CArchive& archive )
{
   CNetwork::Serialize( archive );
}

/*
** CSessionInformation stuff
*/

CNetworkSessionInformation::CNetworkSessionInformation()
{
   m_Initialize();
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

CNetworkSessionInformation::CNetworkSessionInformation( SESSION_INFO_502 *source )
{
   Copy( source );
}

CNetworkSessionInformation::CNetworkSessionInformation( const CNetworkSessionInformation& source )
{
   Copy( source );
}

CNetworkSessionInformation::~CNetworkSessionInformation()
{
   m_Initialize();
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

void CNetworkSessionInformation::Copy( SESSION_INFO_502 *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

#if ! defined( UNICODE )
   ::UNICODE_to_ASCII( (LPCWSTR) source->sesi502_cname,       source->sesi502_cname       );
   ::UNICODE_to_ASCII( (LPCWSTR) source->sesi502_username,    source->sesi502_username    );
   ::UNICODE_to_ASCII( (LPCWSTR) source->sesi502_cltype_name, source->sesi502_cltype_name );
   ::UNICODE_to_ASCII( (LPCWSTR) source->sesi502_transport,   source->sesi502_transport   );
#endif

   ClientName     = source->sesi502_cname;
   UserName       = source->sesi502_username;
   ClientTypeName = source->sesi502_cltype_name;
   Transport      = source->sesi502_transport;
   NumberOfOpens  = source->sesi502_num_opens;
   Time           = CTimeSpan( source->sesi502_time      );
   IdleTime       = CTimeSpan( source->sesi502_idle_time );
   UserFlags      = source->sesi502_user_flags;

#if ! defined( UNICODE )
   ::ASCII_to_UNICODE( source->sesi502_cname,       (LPWSTR) source->sesi502_cname       );
   ::ASCII_to_UNICODE( source->sesi502_username,    (LPWSTR) source->sesi502_username    );
   ::ASCII_to_UNICODE( source->sesi502_cltype_name, (LPWSTR) source->sesi502_cltype_name );
   ::ASCII_to_UNICODE( source->sesi502_transport,   (LPWSTR) source->sesi502_transport   );
#endif
}

void CNetworkSessionInformation::Copy( const CNetworkSessionInformation& source )
{
   ASSERT( this != &source );

   /*
   ** Make sure we ain't copying ourselves
   */

   if ( this == &source )
   {
      return;
   }

   ClientName     = source.ClientName;
   UserName       = source.UserName;
   ClientTypeName = source.ClientTypeName;
   Transport      = source.Transport;
   NumberOfOpens  = source.NumberOfOpens;
   Time           = source.Time;
   IdleTime       = source.IdleTime;
   UserFlags      = source.UserFlags;
}

void CNetworkSessionInformation::Empty( void )
{
   m_Initialize();
}

void CNetworkSessionInformation::m_Initialize( void )
{
   ClientName.Empty();
   UserName.Empty();
   ClientTypeName.Empty();
   Transport.Empty();
   NumberOfOpens = 0;
   Time          = CTimeSpan( 0 );
   IdleTime      = CTimeSpan( 0 );
   UserFlags     = 0;
}

void CNetworkSessionInformation::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << ClientName;
      archive << UserName;
      archive << NumberOfOpens;
      archive << Time;
      archive << IdleTime;
      archive << UserFlags;
      archive << ClientTypeName;
      archive << Transport;
   }
   else
   {
      archive >> ClientName;
      archive >> UserName;
      archive >> NumberOfOpens;
      archive >> Time;
      archive >> IdleTime;
      archive >> UserFlags;
      archive >> ClientTypeName;
      archive >> Transport;
   }
}


