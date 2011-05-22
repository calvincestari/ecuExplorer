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

/*
** CNetworkConnectionInformation stuff
*/

IMPLEMENT_SERIAL( CNetworkShareInformation, CObject, 1 )
IMPLEMENT_SERIAL( CNetworkShares, CNetwork, 1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

CNetworkShareInformation::CNetworkShareInformation()
{
   m_Initialize();
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

CNetworkShareInformation::CNetworkShareInformation( SHARE_INFO_2 *source )
{
   Copy( source );
}

CNetworkShareInformation::CNetworkShareInformation( const CNetworkShareInformation& source )
{
   Copy( source );
}

CNetworkShareInformation::~CNetworkShareInformation()
{
   m_Initialize();
}

/*
** Can't make Copy take a const pointer because Microsoft screwed up the 
** net API header files...
*/

void CNetworkShareInformation::Copy( SHARE_INFO_2 *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

#if ! defined( UNICODE )
   ::UNICODE_to_ASCII( (LPCWSTR) source->shi2_netname, source->shi2_netname );
   ::UNICODE_to_ASCII( (LPCWSTR) source->shi2_remark,  source->shi2_remark  );
   ::UNICODE_to_ASCII( (LPCWSTR) source->shi2_path,    source->shi2_path    );
   ::UNICODE_to_ASCII( (LPCWSTR) source->shi2_passwd,  source->shi2_passwd  );
#endif

   NetworkName         = source->shi2_netname;
   Type                = source->shi2_type;
   Remark              = source->shi2_remark;
   Permissions         = source->shi2_permissions;
   MaximumNumberOfUses = source->shi2_max_uses;
   CurrentNumberOfUses = source->shi2_current_uses;
   PathName            = source->shi2_path;
   Password            = source->shi2_passwd;

#if ! defined( UNICODE )
   ::ASCII_to_UNICODE( source->shi2_netname, (LPWSTR) source->shi2_netname );
   ::ASCII_to_UNICODE( source->shi2_remark,  (LPWSTR) source->shi2_remark  );
   ::ASCII_to_UNICODE( source->shi2_path,    (LPWSTR) source->shi2_path    );
   ::ASCII_to_UNICODE( source->shi2_passwd,  (LPWSTR) source->shi2_passwd  );
#endif
}

void CNetworkShareInformation::Copy( const CNetworkShareInformation& source )
{
   ASSERT( this != &source );

   /*
   ** Make sure we ain't copying ourselves
   */

   if ( this == &source )
   {
      return;
   }

   NetworkName         = source.NetworkName;
   Type                = source.Type;
   Remark              = source.Remark;
   Permissions         = source.Permissions;
   MaximumNumberOfUses = source.MaximumNumberOfUses;
   CurrentNumberOfUses = source.CurrentNumberOfUses;
   PathName            = source.PathName;
   Password            = source.Password;
}

void CNetworkShareInformation::Empty( void )
{
   m_Initialize();
}

void CNetworkShareInformation::m_Initialize( void )
{
   NetworkName.Empty();
   Type                = 0;
   Remark.Empty();
   Permissions         = 0;
   MaximumNumberOfUses = 0;
   CurrentNumberOfUses = 0;
   PathName.Empty();
   Password.Empty();
}

void CNetworkShareInformation::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << NetworkName;
      archive << Type;
      archive << Remark;
      archive << Permissions;
      archive << MaximumNumberOfUses;
      archive << CurrentNumberOfUses;
      archive << PathName;
      archive << Password;
   }
   else
   {
      archive >> NetworkName;
      archive >> Type;
      archive >> Remark;
      archive >> Permissions;
      archive >> MaximumNumberOfUses;
      archive >> CurrentNumberOfUses;
      archive >> PathName;
      archive >> Password;
   }
}

/*
** CNetworkConnections Stuff
*/

CNetworkShares::CNetworkShares()
{
   m_Initialize();
}

CNetworkShares::CNetworkShares( LPCTSTR machine_name )
{
   m_Initialize();
   Open( machine_name );
}

CNetworkShares::~CNetworkShares()
{
   Close();
   m_Initialize();
}

BOOL CNetworkShares::Add( CNetworkShareInformation& share_to_add )
{
   // NetShareAdd

   //m_ErrorCode = ::NetShareAdd( (LPTSTR) m_WideMachineName,

   if ( m_ErrorCode == NERR_Success )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

void CNetworkShares::Close( void )
{
   CNetwork::Close();

   if ( m_2InformationBuffer != NULL )
   {
      ::NetApiBufferFree( m_2InformationBuffer );
      m_2InformationBuffer = NULL;
   }
}

BOOL CNetworkShares::Delete( CNetworkShareInformation& share_to_delete )
{
   // NetShareDel

   //m_ErrorCode = ::NetShareDel( (LPTSTR) m_WideMachineName,

   if ( m_ErrorCode == NERR_Success )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

void CNetworkShares::m_Initialize( void )
{
   m_ErrorCode               = 0;
   m_2InformationBuffer    = NULL;
   m_2ResumeHandle         = 0;
   m_2CurrentEntryNumber   = 0;
   m_2NumberOfEntriesRead  = 0;
   m_2TotalNumberOfEntries = 0;
}

BOOL CNetworkShares::Enumerate( void )
{
   if ( m_2InformationBuffer != NULL )
   {
      ::NetApiBufferFree( m_2InformationBuffer );
      m_2InformationBuffer = NULL;
   }

   m_2CurrentEntryNumber   = 0;
   m_2NumberOfEntriesRead  = 0;
   m_2ResumeHandle         = 0;
   m_2TotalNumberOfEntries = 0;

   m_ErrorCode = ::NetShareEnum( (LPTSTR) m_WideMachineName,
                                          2, 
                              (LPBYTE *) &m_2InformationBuffer,
                                          65535,
                                         &m_2NumberOfEntriesRead,
                                         &m_2TotalNumberOfEntries,
                                         &m_2ResumeHandle );

   if ( m_ErrorCode != NERR_Success || m_2InformationBuffer == NULL )
   {
      return( FALSE );
   }

   return( TRUE );
}

BOOL CNetworkShares::GetNext( CNetworkShareInformation& information )
{
   if ( m_2CurrentEntryNumber < m_2TotalNumberOfEntries )
   {
      information.Copy( &m_2InformationBuffer[ m_2CurrentEntryNumber ] );
      m_2CurrentEntryNumber++;
      return( TRUE );
   }

   information.Empty();
   return( FALSE );
}

void CNetworkShares::Serialize( CArchive& archive )
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
