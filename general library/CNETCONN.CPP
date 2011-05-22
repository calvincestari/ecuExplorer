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

IMPLEMENT_SERIAL( CNetworkConnectionInformation, CObject, 1 )
IMPLEMENT_SERIAL( CNetworkConnections, CNetwork, 1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

/*
** CNetworkConnectionInformation stuff
*/

CNetworkConnectionInformation::CNetworkConnectionInformation()
{
   m_Initialize();
}

CNetworkConnectionInformation::CNetworkConnectionInformation( CONNECTION_INFO_1 *source )
{
   Copy( source );
}

CNetworkConnectionInformation::CNetworkConnectionInformation( const CNetworkConnectionInformation& source )
{
   Copy( source );
}

CNetworkConnectionInformation::~CNetworkConnectionInformation()
{
   m_Initialize();
}

void CNetworkConnectionInformation::Copy( CONNECTION_INFO_1 *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

#if ! defined( UNICODE )
   ::UNICODE_to_ASCII( (LPCWSTR) source->coni1_username, source->coni1_username );
   ::UNICODE_to_ASCII( (LPCWSTR) source->coni1_netname,  source->coni1_netname  );
#endif

   ID            = source->coni1_id;
   Type          = source->coni1_type;
   NumberOfOpens = source->coni1_num_opens;
   NumberOfUsers = source->coni1_num_users;
   Time          = source->coni1_time;
   UserName      = source->coni1_username;
   NetName       = source->coni1_netname;

#if ! defined( UNICODE )
   ::ASCII_to_UNICODE( source->coni1_username, (LPWSTR) source->coni1_username );
   ::ASCII_to_UNICODE( source->coni1_netname,  (LPWSTR) source->coni1_netname  );
#endif
}

void CNetworkConnectionInformation::Copy( const CNetworkConnectionInformation& source )
{
   ASSERT( this != &source );

   /*
   ** Check to make sure we ain't copying ourselves
   */

   if ( this == &source )
   {
      return;
   }

   ID            = source.ID;
   Type          = source.Type;
   NumberOfOpens = source.NumberOfOpens;
   NumberOfUsers = source.NumberOfUsers;
   Time          = source.Time;
   UserName      = source.UserName;
   NetName       = source.NetName;
}

#if defined( _DEBUG )

void CNetworkConnectionInformation::Dump( CDumpContext& dump_context ) const
{
   CObject::Dump( dump_context );

   dump_context << "ID = "            << ID            << "\n";
   dump_context << "Type = "          << Type          << "\n";
   dump_context << "NumberOfOpens = " << NumberOfOpens << "\n";
   dump_context << "NumberOfUsers = " << NumberOfUsers << "\n";
   dump_context << "Time = "          << Time          << "\n";
   dump_context << "UserName = \""    << UserName      << "\"\n";
   dump_context << "NetName = \""     << NetName       << "\"\n";
}

#endif // _DEBUG

void CNetworkConnectionInformation::Empty( void )
{
   m_Initialize();
}

void CNetworkConnectionInformation::m_Initialize( void )
{
   UserName.Empty();
   NetName.Empty();
   ID            = 0;
   Type          = 0;
   NumberOfUsers = 0;
   NumberOfOpens = 0;
   Time          = 0;
}

void CNetworkConnectionInformation::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << ID;
      archive << Type;
      archive << NumberOfOpens;
      archive << NumberOfUsers;
      archive << UserName;
      archive << NetName;
   }
   else
   {
      archive >> ID;
      archive >> Type;
      archive >> NumberOfOpens;
      archive >> NumberOfUsers;
      archive >> UserName;
      archive >> NetName;
   }
}

const CNetworkConnectionInformation& CNetworkConnectionInformation::operator=( const CNetworkConnectionInformation& source )
{
   ASSERT( this != &source );

   if ( this != &source )
   {
      Copy( source );
   }

   return( *this );
}

/*
** CNetworkConnections Stuff
*/

CNetworkConnections::CNetworkConnections()
{
   m_Initialize();
}

CNetworkConnections::CNetworkConnections( LPCTSTR machine_name )
{
   m_Initialize();
   Open( machine_name );
}

CNetworkConnections::~CNetworkConnections()
{
   Close();
   m_Initialize();
}

void CNetworkConnections::Close( void )
{
   CNetwork::Close();

   if ( m_1InformationBuffer != NULL )
   {
      ::NetApiBufferFree( m_1InformationBuffer );
      m_1InformationBuffer = NULL;
   }
}

#if defined( _DEBUG )

void CNetworkConnections::Dump( CDumpContext& dump_context ) const
{
   CNetwork::Dump( dump_context );

   dump_context << "m_1ResumeHandle = "         << m_1ResumeHandle         << "\n";
   dump_context << "m_1CurrentEntryNumber = "   << m_1CurrentEntryNumber   << "\n";
   dump_context << "m_1NumberOfEntriesRead = "  << m_1NumberOfEntriesRead  << "\n";
   dump_context << "m_1TotalNumberOfEntries = " << m_1TotalNumberOfEntries << "\n";
}

#endif // _DEBUG

BOOL CNetworkConnections::Enumerate( LPCTSTR share_or_computer_name )
{
   if ( m_1InformationBuffer != NULL )
   {
      ::NetApiBufferFree( m_1InformationBuffer );
      m_1InformationBuffer = NULL;
   }

   if ( share_or_computer_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   LPWSTR wide_share_or_computer_name[ MAX_PATH ];

#if ! defined( UNICODE )
   ASCII_to_UNICODE( share_or_computer_name, (LPWSTR) wide_share_or_computer_name );
#else
   strcpy( wide_share_or_computer_name, share_or_computer_name );
#endif // UNICODE

   m_1CurrentEntryNumber   = 0;
   m_1NumberOfEntriesRead  = 0;
   m_1ResumeHandle         = 0;
   m_1TotalNumberOfEntries = 0;

   m_ErrorCode = ::NetConnectionEnum( (LPTSTR) m_WideMachineName,
                                      (LPTSTR) wide_share_or_computer_name, 
                                               1, 
                                   (LPBYTE *) &m_1InformationBuffer,
                                               65535,
                                              &m_1NumberOfEntriesRead,
                                              &m_1TotalNumberOfEntries,
                                              &m_1ResumeHandle );

   if ( m_ErrorCode != NERR_Success || m_1InformationBuffer == NULL )
   {
      return( FALSE );
   }

   return( TRUE );
}

BOOL CNetworkConnections::GetNext( CNetworkConnectionInformation& information )
{
   if ( m_1CurrentEntryNumber < m_1TotalNumberOfEntries )
   {
      information.Copy( &m_1InformationBuffer[ m_1CurrentEntryNumber ] );
      m_1CurrentEntryNumber++;
      return( TRUE );
   }

   information.Empty();
   return( FALSE );
}

void CNetworkConnections::m_Initialize( void )
{
   m_ErrorCode               = 0;
   m_1InformationBuffer    = NULL;
   m_1ResumeHandle         = 0;
   m_1CurrentEntryNumber   = 0;
   m_1NumberOfEntriesRead  = 0;
   m_1TotalNumberOfEntries = 0;
}

void CNetworkConnections::Serialize( CArchive& archive )
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
