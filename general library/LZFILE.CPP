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

CLZFile::CLZFile()
{
   m_Initialize();
}

CLZFile::~CLZFile()
{
   TRACE( "Destroying a CLZFile object\n" );
   Close();
}

void CLZFile::Close( void )
{
   ASSERT_VALID( this );

   if ( m_LZFileHandle >= 0 )
   {
      ::LZClose( m_LZFileHandle );
      m_LZFileHandle = LZERROR_BADINHANDLE;
      m_hFile = CFile::hFileNull;
   }

   m_Initialize();
}

BOOL CLZFile::Copy( const CLZFile * source_p )
{
   return( Copy( *source_p ) );
}

BOOL CLZFile::Copy( const CLZFile& source )
{
   LONG size_of_destination_file = 0;

   size_of_destination_file = ::LZCopy( source.m_LZFileHandle, m_LZFileHandle );

   if ( size_of_destination_file < 0 )
   {
      m_ErrorCode = size_of_destination_file;
      return( FALSE );
   }

   return( TRUE );
}

#if defined( _DEBUG )

void CLZFile::Dump( CDumpContext& dump_context ) const
{
   CDummyFile::Dump( dump_context );

   dump_context << "m_LZFileHandle = " << m_LZFileHandle << "\n";
   dump_context << "m_OpenFileStructure is ";
   m_OpenFileStructure.Dump( dump_context );
}

#endif // _DEBUG

BOOL CLZFile::GetExpandedName( LPTSTR name_of_compressed_file, CString& original_file_name )
{
   TCHAR file_name[ _MAX_PATH ];

   ::ZeroMemory( file_name, sizeof( file_name ) );

   if ( ::GetExpandedName( name_of_compressed_file, file_name ) == 1 )
   {
      original_file_name = file_name;
      return( TRUE );
   }
   else
   {
      original_file_name.Empty();
      return( FALSE );
   }
}

void CLZFile::m_Initialize( void )
{
   m_LZFileHandle = 0;
   m_OpenFileStructure.Empty();
}

BOOL CLZFile::Open( const char *file_name, UINT style, CFileException * error_p )
{
   m_LZFileHandle = ::LZOpenFile( (char *) file_name, &m_OpenFileStructure, style );

   if ( m_LZFileHandle < 0 )
   {
      m_ErrorCode = m_LZFileHandle;

      return( FALSE );
   }

   return( TRUE );
}

UINT CLZFile::Read( void *buffer, UINT size_of_buffer )
{
   INT number_of_bytes_read = 0;

   number_of_bytes_read = ::LZRead( m_LZFileHandle, (char *) buffer, size_of_buffer );

   if ( number_of_bytes_read < 0 )
   {
      m_ErrorCode = number_of_bytes_read;
      return( 0 );
   }

   return( (UINT) number_of_bytes_read );
}

LONG CLZFile::Seek( LONG offset, UINT from )
{
	ASSERT_VALID( this );
	ASSERT( m_hFile != (UINT) CFile::hFileNull );
	ASSERT( from == begin || from == end || from == current);
	ASSERT( begin == FILE_BEGIN && end == FILE_END && current == FILE_CURRENT );

	LONG offset_from_beginning_of_file = ::LZSeek( m_LZFileHandle, offset, from );

   if ( offset_from_beginning_of_file < 0 )
   {
      m_ErrorCode = offset_from_beginning_of_file;
   }

	return( offset_from_beginning_of_file );
}

void CLZFile::TranslateErrorCode( int error_code, CString& error_message )
{
   switch( error_code )
   {
      case LZERROR_BADINHANDLE:

         error_message = "Invalid input handle";
         return;

      case LZERROR_BADOUTHANDLE:

         error_message = "Invalid output handle";
         return;

      case LZERROR_READ:

         error_message = "Corrupt compressed file format";
         return;

      case LZERROR_WRITE:

         error_message = "Out of space for output file";
         return;

      case LZERROR_GLOBALLOC:

         error_message = "Insufficient memory for LZFile struct";
         return;

      case LZERROR_GLOBLOCK:

         error_message = "Bad global handle";
         return;

      case LZERROR_BADVALUE:

         error_message = "Input parameter out of acceptable range";
         return;

      case LZERROR_UNKNOWNALG:

         error_message = "Compression algorithm not recognized";
         return;

      default:

         error_message.Format( "Unknown error number %d", error_code );
         return;
   }
}

