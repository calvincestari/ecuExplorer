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

IMPLEMENT_SERIAL( CWorkstationUser, CObject, 1 )
IMPLEMENT_SERIAL( CWorkstationInformation, CObject, 1 )
IMPLEMENT_SERIAL( CWorkstationTransport, CObject, 1 )
IMPLEMENT_SERIAL( CNetWorkstation, CNetwork, 1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

/*
** CWorkstationUser stuff
*/

CWorkstationUser::CWorkstationUser()
{
   m_Initialize();
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

CWorkstationUser::CWorkstationUser( WKSTA_USER_INFO_1 *source )
{
   Copy( source );
}

CWorkstationUser::CWorkstationUser( const CWorkstationUser& source )
{
   Copy( source );
}

CWorkstationUser::~CWorkstationUser()
{
   m_Initialize();
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

void CWorkstationUser::Copy( WKSTA_USER_INFO_1 *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

#if ! defined( UNICODE )
   ::UNICODE_to_ASCII( (LPCWSTR) source->wkui1_username,     source->wkui1_username     );
   ::UNICODE_to_ASCII( (LPCWSTR) source->wkui1_logon_domain, source->wkui1_logon_domain );
   ::UNICODE_to_ASCII( (LPCWSTR) source->wkui1_oth_domains,  source->wkui1_oth_domains  );
   ::UNICODE_to_ASCII( (LPCWSTR) source->wkui1_logon_server, source->wkui1_logon_server );
#endif

   UserName     = source->wkui1_username;
   LogonDomain  = source->wkui1_logon_domain;
   OtherDomains = source->wkui1_oth_domains;
   LogonServer  = source->wkui1_logon_server;

#if ! defined( UNICODE )
   ::ASCII_to_UNICODE( source->wkui1_username,     (LPWSTR) source->wkui1_username     );
   ::ASCII_to_UNICODE( source->wkui1_logon_domain, (LPWSTR) source->wkui1_logon_domain );
   ::ASCII_to_UNICODE( source->wkui1_oth_domains,  (LPWSTR) source->wkui1_oth_domains  );
   ::ASCII_to_UNICODE( source->wkui1_logon_server, (LPWSTR) source->wkui1_logon_server );
#endif
}

void CWorkstationUser::Copy( const CWorkstationUser& source )
{
   ASSERT( this != &source );

   /*
   ** Make sure we ain't copying ourselves
   */

   if ( this == &source )
   {
      return;
   }

   UserName     = source.UserName;
   LogonDomain  = source.LogonDomain;
   OtherDomains = source.OtherDomains;
   LogonServer  = source.LogonServer;
}

void CWorkstationUser::Empty( void )
{
   m_Initialize();
}

void CWorkstationUser::m_Initialize( void )
{
   UserName.Empty();
   LogonDomain.Empty();
   OtherDomains.Empty();
   LogonServer.Empty();
}

void CWorkstationUser::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << UserName;
      archive << LogonDomain;
      archive << OtherDomains;
      archive << LogonServer;
   }
   else
   {
      archive >> UserName;
      archive >> LogonDomain;
      archive >> OtherDomains;
      archive >> LogonServer;
   }
}

/*
** CWorkstationInformation stuff
*/

CWorkstationInformation::CWorkstationInformation()
{
   m_Initialize();
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

CWorkstationInformation::CWorkstationInformation( WKSTA_INFO_100 *source )
{
   Copy( source );
}

CWorkstationInformation::CWorkstationInformation( WKSTA_INFO_101 *source )
{
   Copy( source );
}

CWorkstationInformation::CWorkstationInformation( const CWorkstationInformation& source )
{
   Copy( source );
}

CWorkstationInformation::~CWorkstationInformation()
{
   m_Initialize();
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

void CWorkstationInformation::Copy( WKSTA_INFO_100 *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

#if ! defined( UNICODE )
   ::UNICODE_to_ASCII( (LPCWSTR) source->wki100_computername, source->wki100_computername );
   ::UNICODE_to_ASCII( (LPCWSTR) source->wki100_langroup,     source->wki100_langroup     );
#endif

   PlatformID            = source->wki100_platform_id;
   ComputerName          = source->wki100_computername;
   LANGroup              = source->wki100_langroup;
   MajorVersion          = source->wki100_ver_major;
   MinorVersion          = source->wki100_ver_minor;
   LANRoot.Empty();;
   NumberOfLoggedOnUsers = 0;

#if ! defined( UNICODE )
   ::ASCII_to_UNICODE( source->wki100_computername, (LPWSTR) source->wki100_computername );
   ::ASCII_to_UNICODE( source->wki100_langroup,     (LPWSTR) source->wki100_langroup     );
#endif
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

void CWorkstationInformation::Copy( WKSTA_INFO_101 *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

#if ! defined( UNICODE )
   ::UNICODE_to_ASCII( (LPCWSTR) source->wki101_computername, source->wki101_computername );
   ::UNICODE_to_ASCII( (LPCWSTR) source->wki101_langroup,     source->wki101_langroup     );
   ::UNICODE_to_ASCII( (LPCWSTR) source->wki101_lanroot,      source->wki101_lanroot      );
#endif

   PlatformID            = source->wki101_platform_id;
   ComputerName          = source->wki101_computername;
   LANGroup              = source->wki101_langroup;
   MajorVersion          = source->wki101_ver_major;
   MinorVersion          = source->wki101_ver_minor;
   LANRoot               = source->wki101_lanroot;
   NumberOfLoggedOnUsers = 0;

#if ! defined( UNICODE )
   ::ASCII_to_UNICODE( source->wki101_computername, (LPWSTR) source->wki101_computername );
   ::ASCII_to_UNICODE( source->wki101_langroup,     (LPWSTR) source->wki101_langroup     );
   ::ASCII_to_UNICODE( source->wki101_lanroot,      (LPWSTR) source->wki101_lanroot      );
#endif
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

void CWorkstationInformation::Copy( WKSTA_INFO_102 *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

#if ! defined( UNICODE )
   ::UNICODE_to_ASCII( (LPCWSTR) source->wki102_computername, source->wki102_computername );
   ::UNICODE_to_ASCII( (LPCWSTR) source->wki102_langroup,     source->wki102_langroup     );
   ::UNICODE_to_ASCII( (LPCWSTR) source->wki102_lanroot,      source->wki102_lanroot      );
#endif

   PlatformID            = source->wki102_platform_id;
   ComputerName          = source->wki102_computername;
   LANGroup              = source->wki102_langroup;
   MajorVersion          = source->wki102_ver_major;
   MinorVersion          = source->wki102_ver_minor;
   LANRoot               = source->wki102_lanroot;
   NumberOfLoggedOnUsers = source->wki102_logged_on_users;

#if ! defined( UNICODE )
   ::ASCII_to_UNICODE( source->wki102_computername, (LPWSTR) source->wki102_computername );
   ::ASCII_to_UNICODE( source->wki102_langroup,     (LPWSTR) source->wki102_langroup     );
   ::ASCII_to_UNICODE( source->wki102_lanroot,      (LPWSTR) source->wki102_lanroot      );
#endif
}

void CWorkstationInformation::Copy( const CWorkstationInformation& source )
{
   ASSERT( this != &source );

   /*
   ** Make sure we ain't copying ourselves
   */

   if ( this == &source )
   {
      return;
   }

   PlatformID            = source.PlatformID;
   ComputerName          = source.ComputerName;
   LANGroup              = source.LANGroup;
   MajorVersion          = source.MajorVersion;
   MinorVersion          = source.MinorVersion;
   LANRoot               = source.LANRoot;
   NumberOfLoggedOnUsers = source.NumberOfLoggedOnUsers;
}

void CWorkstationInformation::Empty( void )
{
   m_Initialize();
}

void CWorkstationInformation::m_Initialize( void )
{
   PlatformID            = 0;
   ComputerName.Empty();
   LANGroup.Empty();
   MajorVersion          = 0;
   MinorVersion          = 0;
   LANRoot.Empty();
   NumberOfLoggedOnUsers = 0;
}

void CWorkstationInformation::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << PlatformID;
      archive << ComputerName;
      archive << LANGroup;
      archive << MajorVersion;
      archive << MinorVersion;
      archive << LANRoot;
      archive << NumberOfLoggedOnUsers;
   }
   else
   {
      archive >> PlatformID;
      archive >> ComputerName;
      archive >> LANGroup;
      archive >> MajorVersion;
      archive >> MinorVersion;
      archive >> LANRoot;
      archive >> NumberOfLoggedOnUsers;
   }
}

/*
** CWorkstationTransport stuff
*/

CWorkstationTransport::CWorkstationTransport()
{
   m_Initialize();
}

CWorkstationTransport::CWorkstationTransport( WKSTA_TRANSPORT_INFO_0 *source )
{
   Copy( source );
}

CWorkstationTransport::CWorkstationTransport( const CWorkstationTransport& source )
{
   Copy( source );
}

CWorkstationTransport::~CWorkstationTransport()
{
   m_Initialize();
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

void CWorkstationTransport::Copy( WKSTA_TRANSPORT_INFO_0 *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

#if ! defined( UNICODE )
   ::UNICODE_to_ASCII( (LPCWSTR) source->wkti0_transport_name,    source->wkti0_transport_name    );
   ::UNICODE_to_ASCII( (LPCWSTR) source->wkti0_transport_address, source->wkti0_transport_address );
#endif

   QualityOfService        = source->wkti0_quality_of_service;
   NumberOfVirtualCircuits = source->wkti0_number_of_vcs;
   Name                    = source->wkti0_transport_name;
   Address                 = source->wkti0_transport_address;
   WANish                  = source->wkti0_wan_ish;

#if ! defined( UNICODE )
   ::ASCII_to_UNICODE( source->wkti0_transport_name,    (LPWSTR) source->wkti0_transport_name    );
   ::ASCII_to_UNICODE( source->wkti0_transport_address, (LPWSTR) source->wkti0_transport_address );
#endif
}

void CWorkstationTransport::Copy( const CWorkstationTransport& source )
{
   ASSERT( this != &source );

   /*
   ** Make sure we ain't copying ourselves
   */

   if ( this == &source )
   {
      return;
   }

   QualityOfService        = source.QualityOfService;
   NumberOfVirtualCircuits = source.NumberOfVirtualCircuits;
   Name                    = source.Name;
   Address                 = source.Address;
   WANish                  = source.WANish;
}

void CWorkstationTransport::Empty( void )
{
   m_Initialize();
}

void CWorkstationTransport::m_Initialize( void )
{
   QualityOfService        = 0;
   NumberOfVirtualCircuits = 0;
   Name.Empty();
   Address.Empty();
   WANish = FALSE;
}

void CWorkstationTransport::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   DWORD temp_value = 0;

   if ( archive.IsStoring() )
   {
      archive << QualityOfService;
      archive << NumberOfVirtualCircuits;
      archive << Name;
      archive << Address;

      temp_value = WANish;
      archive << temp_value;
   }
   else
   {
      archive >> QualityOfService;
      archive >> NumberOfVirtualCircuits;
      archive >> Name;
      archive >> Address;
      archive >> temp_value;
      WANish = (BOOL) temp_value;
   }
}

/*
** CNetWorkstation Stuff
*/

CNetWorkstation::CNetWorkstation()
{
   m_Initialize();
}

CNetWorkstation::CNetWorkstation( LPCTSTR machine_name )
{
   m_Initialize();
   Open( machine_name );
}

CNetWorkstation::~CNetWorkstation()
{
   Close();
}

void CNetWorkstation::Close( void )
{
   CNetwork::Close();

   if ( m_InformationBuffer100 != NULL )
   {
      ::NetApiBufferFree( m_InformationBuffer100 );
      m_InformationBuffer100 = NULL;
   }

   if ( m_InformationBuffer101 != NULL )
   {
      ::NetApiBufferFree( m_InformationBuffer101 );
      m_InformationBuffer101 = NULL;
   }

   if ( m_InformationBuffer102 != NULL )
   {
      ::NetApiBufferFree( m_InformationBuffer102 );
      m_InformationBuffer102 = NULL;
   }

   if ( m_TransportBuffer != NULL )
   {
      ::NetApiBufferFree( m_TransportBuffer );
      m_TransportBuffer = NULL;
   }

   if ( m_UserBuffer != NULL )
   {
      ::NetApiBufferFree( m_UserBuffer );
      m_UserBuffer = NULL;
   }

   m_TransportBuffer      = NULL;
   m_UserBuffer           = NULL;
}

void CNetWorkstation::m_Initialize( void )
{
   m_ErrorCode                     = 0;
   m_InformationBuffer100          = NULL;
   m_InformationBuffer101          = NULL;
   m_InformationBuffer102          = NULL;
   m_TransportBuffer               = NULL;
   m_TransportResumeHandle         = 0;
   m_TransportCurrentEntryNumber   = 0;
   m_TransportNumberOfEntriesRead  = 0;
   m_TransportTotalNumberOfEntries = 0;
   m_UserBuffer                    = NULL;
   m_UserResumeHandle              = NULL;
   m_UserCurrentEntryNumber        = NULL;
   m_UserNumberOfEntriesRead       = NULL;
   m_UserTotalNumberOfEntries      = NULL;
}

BOOL CNetWorkstation::EnumerateInformation( void )
{
   if ( m_InformationBuffer100 != NULL )
   {
      ::NetApiBufferFree( m_InformationBuffer100 );
      m_InformationBuffer100 = NULL;
   }

   if ( m_InformationBuffer101 != NULL )
   {
      ::NetApiBufferFree( m_InformationBuffer101 );
      m_InformationBuffer101 = NULL;
   }

   if ( m_InformationBuffer102 != NULL )
   {
      ::NetApiBufferFree( m_InformationBuffer102 );
      m_InformationBuffer102 = NULL;
   }

   m_ErrorCode = ::NetWkstaGetInfo( (LPTSTR) m_WideMachineName, 102, (LPBYTE *) &m_InformationBuffer102 );

   if ( m_ErrorCode == ERROR_ACCESS_DENIED )
   {
      m_ErrorCode = ::NetWkstaGetInfo( (LPTSTR) m_WideMachineName, 101, (LPBYTE *) &m_InformationBuffer101 );

      if ( m_ErrorCode == ERROR_ACCESS_DENIED )
      {
         m_ErrorCode = ::NetWkstaGetInfo( (LPTSTR) m_WideMachineName, 100, (LPBYTE *) &m_InformationBuffer100 );

         if ( m_ErrorCode != NERR_Success || m_InformationBuffer100 == NULL )
         {
            return( FALSE );
         }
      }
      else if ( m_ErrorCode != NERR_Success || m_InformationBuffer101 == NULL )
      {
         return( FALSE );
      }
   }
   else if ( m_ErrorCode != NERR_Success || m_InformationBuffer102 == NULL )
   {
      return( FALSE );
   }

   return( TRUE );
}

BOOL CNetWorkstation::EnumerateTransports( void )
{
   ::NetApiBufferFree( m_TransportBuffer );
   m_TransportBuffer = NULL;

   m_TransportCurrentEntryNumber   = 0;
   m_TransportNumberOfEntriesRead  = 0;
   m_TransportResumeHandle         = 0;
   m_TransportTotalNumberOfEntries = 0;

   m_ErrorCode = ::NetWkstaTransportEnum( (LPTSTR) m_WideMachineName, 
                                                   0, 
                                       (LPBYTE *) &m_TransportBuffer,
                                                   65536,
                                                  &m_TransportNumberOfEntriesRead,
                                                  &m_TransportTotalNumberOfEntries,
                                                  &m_TransportResumeHandle );

   if ( m_ErrorCode != NERR_Success || m_TransportBuffer == NULL || m_TransportNumberOfEntriesRead == 0 )
   {
      return( FALSE );
   }

   return( TRUE );
}

BOOL CNetWorkstation::EnumerateUsers( void )
{
   ::NetApiBufferFree( m_UserBuffer );
   m_UserBuffer = NULL;

   m_UserCurrentEntryNumber   = 0;
   m_UserNumberOfEntriesRead  = 0;
   m_UserTotalNumberOfEntries = 0;
   m_UserResumeHandle         = 0;

   m_ErrorCode = ::NetWkstaUserEnum( (LPTSTR) m_WideMachineName, 
                                     1, 
                         (LPBYTE *) &m_UserBuffer,
                                     65536,
                                    &m_UserNumberOfEntriesRead,
                                    &m_UserTotalNumberOfEntries,
                                    &m_UserResumeHandle );

   if ( m_ErrorCode != NERR_Success || m_UserBuffer == NULL || m_UserNumberOfEntriesRead == 0 )
   {
      return( FALSE );
   }

   return( TRUE );
}

BOOL CNetWorkstation::GetCurrentUser( CWorkstationUser& information )
{
   WKSTA_USER_INFO_1 *buffer = NULL;

   m_ErrorCode = ::NetWkstaUserGetInfo( NULL, 1, (LPBYTE *) &buffer );

   if ( m_ErrorCode != NERR_Success || buffer == NULL )
   {
      information.Empty();
      return( FALSE );
   }

   information.Copy( buffer );

   ::NetApiBufferFree( buffer );

   return( TRUE );
}

BOOL CNetWorkstation::GetNext( CWorkstationInformation& information )
{
   if ( m_InformationBuffer100 != NULL )
   {
      information.Copy( m_InformationBuffer100 );
      ::NetApiBufferFree( m_InformationBuffer100 );
      m_InformationBuffer100 = NULL;
      return( TRUE );
   }

   if ( m_InformationBuffer101 != NULL )
   {
      information.Copy( m_InformationBuffer101 );
      ::NetApiBufferFree( m_InformationBuffer101 );
      m_InformationBuffer101 = NULL;
      return( TRUE );
   }

   if ( m_InformationBuffer102 != NULL )
   {
      information.Copy( m_InformationBuffer102 );
      ::NetApiBufferFree( m_InformationBuffer102 );
      m_InformationBuffer102 = NULL;
      return( TRUE );
   }

   information.Empty();
   return( FALSE );
}

BOOL CNetWorkstation::GetNext( CWorkstationTransport& information )
{
   if ( m_TransportCurrentEntryNumber < m_TransportTotalNumberOfEntries )
   {
      information.Copy( &m_TransportBuffer[ m_TransportCurrentEntryNumber ] );
      m_TransportCurrentEntryNumber++;
      return( TRUE );
   }

   information.Empty();
   return( FALSE );
}

BOOL CNetWorkstation::GetNext( CWorkstationUser& information )
{
   if ( m_UserCurrentEntryNumber < m_UserTotalNumberOfEntries )
   {
      information.Copy( &m_UserBuffer[ m_UserCurrentEntryNumber ] );
      m_UserCurrentEntryNumber++;
      return( TRUE );
   }

   information.Empty();
   return( FALSE );
}

void CNetWorkstation::Serialize( CArchive& archive )
{
   CNetwork::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << m_ErrorCode;
   }
   else
   {
      archive >> m_ErrorCode;
   }
}
