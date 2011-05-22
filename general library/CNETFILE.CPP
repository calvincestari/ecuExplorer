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

IMPLEMENT_SERIAL( CNetworkFileInformation, CObject, 1 )
IMPLEMENT_SERIAL( CNetworkFiles, CNetwork, 1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

/*
** CNetworkFileInformation stuff
*/

CNetworkFileInformation::CNetworkFileInformation()
{
   m_Initialize();
}

CNetworkFileInformation::CNetworkFileInformation( FILE_INFO_3 *source )
{
   Copy( source );
}

CNetworkFileInformation::CNetworkFileInformation( const CNetworkFileInformation& source )
{
   Copy( source );
}

CNetworkFileInformation::~CNetworkFileInformation()
{
   m_Initialize();
}

void CNetworkFileInformation::Copy( FILE_INFO_3 *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

#if ! defined( UNICODE )
   ::UNICODE_to_ASCII( (LPCWSTR) source->fi3_pathname, source->fi3_pathname );
   ::UNICODE_to_ASCII( (LPCWSTR) source->fi3_username, source->fi3_username );
#endif

   ID            = source->fi3_id;
   Permissions   = source->fi3_permissions;
   NumberOfLocks = source->fi3_num_locks;
   PathName      = source->fi3_pathname;
   UserName      = source->fi3_username;

#if ! defined( UNICODE )
   ::ASCII_to_UNICODE( source->fi3_pathname, (LPWSTR) source->fi3_pathname );
   ::ASCII_to_UNICODE( source->fi3_username, (LPWSTR) source->fi3_username );
#endif
}

void CNetworkFileInformation::Copy( const CNetworkFileInformation& source )
{
   ASSERT( this != &source );

   /*
   ** Make sure we ain't copying ourselves
   */

   if ( this == &source )
   {
      return;
   }

   ID            = source.ID;
   Permissions   = source.Permissions;
   NumberOfLocks = source.NumberOfLocks;
   PathName      = source.PathName;
   UserName      = source.UserName;
}

void CNetworkFileInformation::Empty( void )
{
   m_Initialize();
}

void CNetworkFileInformation::m_Initialize( void )
{
   UserName.Empty();
   PathName.Empty();
   ID            = 0;
   Permissions   = 0;
   NumberOfLocks = 0;
}

void CNetworkFileInformation::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << UserName;
      archive << PathName;
      archive << ID;
      archive << Permissions;
      archive << NumberOfLocks;
   }
   else
   {
      archive >> UserName;
      archive >> PathName;
      archive >> ID;
      archive >> Permissions;
      archive >> NumberOfLocks;
   }
}

/*
** CNetworkFiles Stuff
*/

CNetworkFiles::CNetworkFiles()
{
   m_Initialize();
}

CNetworkFiles::CNetworkFiles( LPCTSTR machine_name )
{
   m_Initialize();
   Open( machine_name );
}

CNetworkFiles::~CNetworkFiles()
{
   Close();
   m_Initialize();
}

void CNetworkFiles::Close( void )
{
   CNetwork::Close();

   if ( m_103InformationBuffer != NULL )
   {
      ::NetApiBufferFree( m_103InformationBuffer );
      m_103InformationBuffer = NULL;
   }
}

BOOL CNetworkFiles::Close( CNetworkFileInformation& information )
{
   m_ErrorCode = ::NetFileClose( (LPTSTR) m_WideMachineName, information.ID );

   if ( m_ErrorCode == NERR_Success )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

void CNetworkFiles::m_Initialize( void )
{
   m_ErrorCode               = 0;
   m_103InformationBuffer    = NULL;
   m_103ResumeHandle         = 0;
   m_103CurrentEntryNumber   = 0;
   m_103NumberOfEntriesRead  = 0;
   m_103TotalNumberOfEntries = 0;
}

BOOL CNetworkFiles::Enumerate( void )
{
   if ( m_103InformationBuffer != NULL )
   {
      ::NetApiBufferFree( m_103InformationBuffer );
      m_103InformationBuffer = NULL;
   }

   m_103CurrentEntryNumber   = 0;
   m_103NumberOfEntriesRead  = 0;
   m_103ResumeHandle         = 0;
   m_103TotalNumberOfEntries = 0;

   m_ErrorCode = ::NetFileEnum( (LPTSTR) m_WideMachineName,
                                         NULL, 
                                         NULL, 
                                         3, 
                             (LPBYTE *) &m_103InformationBuffer,
                                         65535,
                                        &m_103NumberOfEntriesRead,
                                        &m_103TotalNumberOfEntries,
                                        &m_103ResumeHandle );

   if ( m_ErrorCode != NERR_Success || m_103InformationBuffer == NULL )
   {
      return( FALSE );
   }

   return( TRUE );
}

BOOL CNetworkFiles::GetNext( CNetworkFileInformation& information )
{
   if ( m_103CurrentEntryNumber < m_103TotalNumberOfEntries )
   {
      information.Copy( &m_103InformationBuffer[ m_103CurrentEntryNumber ] );
      m_103CurrentEntryNumber++;
      return( TRUE );
   }

   information.Empty();
   return( FALSE );
}

void CNetworkFiles::Serialize( CArchive& archive )
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
