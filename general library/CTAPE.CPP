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

/*
** CTapeGetDriveParameters
*/

CTapeGetDriveParameters::CTapeGetDriveParameters()
{
   Empty();
}

CTapeGetDriveParameters::CTapeGetDriveParameters( const CTapeGetDriveParameters& source )
{
   Empty();
   Copy( source );
}

CTapeGetDriveParameters::CTapeGetDriveParameters( const TAPE_GET_DRIVE_PARAMETERS * source )
{
   Empty();
   Copy( source );
}

CTapeGetDriveParameters::~CTapeGetDriveParameters()
{
   Empty();
}

void CTapeGetDriveParameters::Copy( const CTapeGetDriveParameters& source )
{
   Copy( (const TAPE_GET_DRIVE_PARAMETERS *) &source );
}

void CTapeGetDriveParameters::Copy( const TAPE_GET_DRIVE_PARAMETERS * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   ECC                   = source->ECC;
   Compression           = source->Compression;
   DataPadding           = source->DataPadding;
   ReportSetmarks        = source->ReportSetmarks;
   DefaultBlockSize      = source->DefaultBlockSize;
   MaximumBlockSize      = source->MaximumBlockSize;
   MinimumBlockSize      = source->MinimumBlockSize;
   MaximumPartitionCount = source->MaximumPartitionCount;
   FeaturesLow           = source->FeaturesLow;
   FeaturesHigh          = source->FeaturesHigh;
   EOTWarningZoneSize    = source->EOTWarningZoneSize;
}

#if defined( _DEBUG )

void CTapeGetDriveParameters::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CTapeGetDriveParameters at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   ECC = "                   << ECC                   << "\n";
   dump_context << "   Compression = "           << Compression           << "\n";
   dump_context << "   DataPadding = "           << DataPadding           << "\n";
   dump_context << "   ReportSetmarks = "        << ReportSetmarks        << "\n";
   dump_context << "   DefaultBlockSize = "      << DefaultBlockSize      << "\n";
   dump_context << "   MaximumBlockSize = "      << MaximumBlockSize      << "\n";
   dump_context << "   MinimumBlockSize = "      << MinimumBlockSize      << "\n";
   dump_context << "   MaximumPartitionCount = " << MaximumPartitionCount << "\n";
   dump_context << "   FeaturesLow = "           << FeaturesLow           << "\n";
   dump_context << "   FeaturesHigh = "          << FeaturesHigh          << "\n";
   dump_context << "   EOTWarningZoneSize = "    << EOTWarningZoneSize    << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CTapeGetDriveParameters::Empty( void )
{
   ECC                   = 0;
   Compression           = 0;
   DataPadding           = 0;
   ReportSetmarks        = 0;
   DefaultBlockSize      = 0;
   MaximumBlockSize      = 0;
   MinimumBlockSize      = 0;
   MaximumPartitionCount = 0;
   FeaturesLow           = 0;
   FeaturesHigh          = 0;
   EOTWarningZoneSize    = 0;
}

/*
** CTapeGetMediaParameters
*/

CTapeGetMediaParameters::CTapeGetMediaParameters()
{
   Empty();
}

CTapeGetMediaParameters::CTapeGetMediaParameters( const CTapeGetMediaParameters& source )
{
   Empty();
   Copy( source );
}

CTapeGetMediaParameters::CTapeGetMediaParameters( const TAPE_GET_MEDIA_PARAMETERS * source )
{
   Empty();
   Copy( source );
}

CTapeGetMediaParameters::~CTapeGetMediaParameters()
{
   Empty();
}

void CTapeGetMediaParameters::Copy( const CTapeGetMediaParameters& source )
{
   Copy( (const TAPE_GET_MEDIA_PARAMETERS *) &source );
}

void CTapeGetMediaParameters::Copy( const TAPE_GET_MEDIA_PARAMETERS *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   Capacity.LowPart   = source->Capacity.LowPart;
   Capacity.HighPart  = source->Capacity.HighPart;
   Remaining.LowPart  = source->Remaining.LowPart;
   Remaining.HighPart = source->Remaining.HighPart;
   BlockSize          = source->BlockSize;
   PartitionCount     = source->PartitionCount;
   WriteProtected     = source->WriteProtected;
}

#if defined( _DEBUG )

void CTapeGetMediaParameters::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CTapeGetMediaParameters at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   Capacity.LowPart = "  << Capacity.LowPart   << "\n";
   dump_context << "   Capacity.HighPart = " << Capacity.HighPart  << "\n";
   dump_context << "   Remaining.LowPart = " << Remaining.LowPart  << "\n";
   dump_context << "   Remaining.HighPart = "<< Remaining.HighPart << "\n";
   dump_context << "   BlockSize = "         << BlockSize          << "\n";
   dump_context << "   PartitionCount = "    << PartitionCount     << "\n";
   dump_context << "   WriteProtected = "    << WriteProtected     << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CTapeGetMediaParameters::Empty( void )
{
   Capacity.LowPart   = 0;
   Capacity.HighPart  = 0;
   Remaining.LowPart  = 0;
   Remaining.HighPart = 0;
   BlockSize          = 0;
   PartitionCount     = 0;
   WriteProtected     = 0;
}

/*
** CTapeSetDriveParameters
*/

CTapeSetDriveParameters::CTapeSetDriveParameters()
{
   Empty();
}

CTapeSetDriveParameters::CTapeSetDriveParameters( const CTapeSetDriveParameters& source )
{
   Empty();
   Copy( source );
}

CTapeSetDriveParameters::CTapeSetDriveParameters( const TAPE_SET_DRIVE_PARAMETERS * source )
{
   Empty();
   Copy( source );
}

CTapeSetDriveParameters::~CTapeSetDriveParameters()
{
   Empty();
}

void CTapeSetDriveParameters::Copy( const CTapeSetDriveParameters& source )
{
   Copy( (const TAPE_SET_DRIVE_PARAMETERS * ) &source );
}

void CTapeSetDriveParameters::Copy( const TAPE_SET_DRIVE_PARAMETERS * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   ECC                = source->ECC;
   Compression        = source->Compression;
   ReportSetmarks     = source->ReportSetmarks;
   EOTWarningZoneSize = source->EOTWarningZoneSize;
}

#if defined( _DEBUG )

void CTapeSetDriveParameters::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CTapeSetDriveParameters at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   ECC = "               << ECC                << "\n";
   dump_context << "   Compression = "       << Compression        << "\n";
   dump_context << "   ReportSetmarks = "    << ReportSetmarks     << "\n";
   dump_context << "   EOTWarningZoneSize = "<< EOTWarningZoneSize << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CTapeSetDriveParameters::Empty( void )
{
   ECC                = 0;
   Compression        = 0;
   DataPadding        = 0;
   ReportSetmarks     = 0;
   EOTWarningZoneSize = 0;
}

/*
** CTapeSetMediaParameters
*/

CTapeSetMediaParameters::CTapeSetMediaParameters()
{
   Empty();
}

CTapeSetMediaParameters::CTapeSetMediaParameters( const CTapeSetMediaParameters& source )
{
   Empty();
   Copy( source );
}

CTapeSetMediaParameters::CTapeSetMediaParameters( const TAPE_SET_MEDIA_PARAMETERS * source )
{
   Empty();
   Copy( source );
}

CTapeSetMediaParameters::~CTapeSetMediaParameters()
{
   Empty();
}

void CTapeSetMediaParameters::Copy( const CTapeSetMediaParameters& source )
{
   Copy( (const TAPE_SET_MEDIA_PARAMETERS *) &source );
}

void CTapeSetMediaParameters::Copy( const TAPE_SET_MEDIA_PARAMETERS * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   BlockSize = source->BlockSize;
}

#if defined( _DEBUG )

void CTapeSetMediaParameters::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CTapeSetMediaParameters at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   BlockSize = " << BlockSize << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CTapeSetMediaParameters::Empty( void )
{
   BlockSize = 0;
}

/*
** CTape
*/

CTape::CTape()
{
   m_Initialize();
}

CTape::~CTape()
{
   m_Initialize();
}

void CTape::m_Initialize( void )
{
   m_BackupReadContextPointer  = NULL;
   m_BackupWriteContextPointer = NULL;
}

void CTape::Close( void )
{
   Prepare( Unload );
}

BOOL CTape::CreatePartition( const TypeOfPartition type_of_partition, 
                             const DWORD           number_of_partitions, 
                             const DWORD           number_of_megabytes_in_each_partition )
{
   m_ErrorCode = ::CreateTapePartition( (HANDLE) m_hFile, (DWORD) type_of_partition, number_of_partitions, number_of_megabytes_in_each_partition );

   if ( m_ErrorCode == NO_ERROR )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CTape::Erase( const TypeOfErasure type_of_erasure, const BOOL return_immediately )
{
   m_ErrorCode = ::EraseTape( (HANDLE) m_hFile, type_of_erasure, return_immediately );

   if ( m_ErrorCode == NO_ERROR )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CTape::GetParameters( CTapeGetDriveParameters& parameters )
{
   DWORD size = sizeof( TAPE_GET_DRIVE_PARAMETERS );

   TAPE_GET_DRIVE_PARAMETERS drive_parameters;

   ::ZeroMemory( &drive_parameters, size );

   m_ErrorCode = ::GetTapeParameters( (HANDLE) m_hFile, GET_TAPE_DRIVE_INFORMATION, &size, &drive_parameters );

   if ( m_ErrorCode == NO_ERROR )
   {
      parameters.Copy( &drive_parameters );
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CTape::GetParameters( CTapeGetMediaParameters& parameters )
{
   DWORD size = sizeof( TAPE_GET_MEDIA_PARAMETERS );

   TAPE_GET_MEDIA_PARAMETERS media_parameters;

   ::ZeroMemory( &media_parameters, size );

   m_ErrorCode = ::GetTapeParameters( (HANDLE) m_hFile, GET_TAPE_MEDIA_INFORMATION, &size, &media_parameters );

   if ( m_ErrorCode == NO_ERROR )
   {
      parameters.Copy( &media_parameters );
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CTape::GetPosition( const Position type_of_position_to_get, DWORD& partition_number, DWORD& position_low, DWORD& position_high )
{
   m_ErrorCode = ::GetTapePosition( (HANDLE) m_hFile, type_of_position_to_get, &partition_number, &position_low, &position_high );

   if ( m_ErrorCode == NO_ERROR )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

DWORD CTape::GetStatus( void ) const
{
   return( ::GetTapeStatus( (HANDLE) m_hFile ) );
}

BOOL CTape::Mark( const TypeOfMark type_of_mark, const DWORD number_of_marks, const BOOL return_immediately )
{
   m_ErrorCode = ::WriteTapemark( (HANDLE) m_hFile, type_of_mark, number_of_marks, return_immediately );

   if ( m_ErrorCode == NO_ERROR )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CTape::Open( const UINT tape_drive_number_starting_at_zero )
{
   TCHAR tape_drive_name[ MAX_PATH ];

   ::ZeroMemory( tape_drive_name, sizeof( tape_drive_name ) );

   ::wsprintf( tape_drive_name, "\\\\.\\TAPE%u", tape_drive_number_starting_at_zero );

   HANDLE file_handle = INVALID_HANDLE_VALUE;

   file_handle = ::CreateFile( tape_drive_name,
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               0,
                               OPEN_EXISTING,
                               0,
                               NULL );

   if ( file_handle == INVALID_HANDLE_VALUE )
   {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
   }

   m_hFile = (UINT) file_handle;

   return( TRUE );
}

BOOL CTape::Prepare( const Operation what_to_do, const BOOL return_immediately )
{
   m_ErrorCode = ::PrepareTape( (HANDLE) m_hFile, what_to_do, return_immediately );

   if ( m_ErrorCode == NO_ERROR )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CTape::Read( LPBYTE buffer, const DWORD number_of_bytes_to_read, LPDWORD number_of_bytes_read, BOOL abort, BOOL restore_security_data )
{
   BOOL return_value = ::BackupRead( (HANDLE) m_hFile,
                                     buffer,
                                     number_of_bytes_to_read,
                                     number_of_bytes_read,
                                     abort,
                                     restore_security_data,
                                     &m_BackupReadContextPointer );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   return( return_value );
}

BOOL CTape::Seek( const DWORD seek_low, const DWORD seek_high, LPDWORD seeked_low, LPDWORD seeked_high )
{
   BOOL return_value = ::BackupSeek( (HANDLE) m_hFile, seek_low, seek_high, seeked_low, seeked_high, &m_BackupReadContextPointer );

   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   return( return_value );
}

BOOL CTape::SetParameters( const CTapeSetMediaParameters& parameters )
{
   CTapeSetMediaParameters media_parameters( parameters );

   m_ErrorCode = ::SetTapeParameters( (HANDLE) m_hFile, SET_TAPE_MEDIA_INFORMATION, (TAPE_SET_MEDIA_PARAMETERS *) &media_parameters );

   if ( m_ErrorCode == NO_ERROR )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CTape::SetParameters( const CTapeSetDriveParameters& parameters )
{
   CTapeSetDriveParameters drive_parameters( parameters );

   m_ErrorCode = ::SetTapeParameters( (HANDLE) m_hFile, SET_TAPE_DRIVE_INFORMATION, (TAPE_SET_DRIVE_PARAMETERS *) &drive_parameters );

   if ( m_ErrorCode == NO_ERROR )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CTape::SetPosition( const PositionMethod how_to_get_there, const DWORD partition_number, const DWORD position_low, const DWORD position_high, const BOOL return_immediately )
{
   m_ErrorCode = ::SetTapePosition( (HANDLE) m_hFile, how_to_get_there, partition_number, position_low, position_high, return_immediately );

   if ( m_ErrorCode == NO_ERROR )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

BOOL CTape::Write( LPBYTE buffer, const DWORD number_of_bytes_to_write, LPDWORD number_of_bytes_written, BOOL abort, BOOL restore_security_data )
{
   BOOL return_value = ::BackupWrite( (HANDLE) m_hFile,
                                      buffer,
                                      number_of_bytes_to_write,
                                      number_of_bytes_written,
                                      abort,
                                      restore_security_data,
                                      &m_BackupWriteContextPointer );
   if ( return_value != TRUE )
   {
      m_ErrorCode = ::GetLastError();
   }

   return( return_value );
}
