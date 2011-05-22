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

IMPLEMENT_SERIAL( CDummyFile, CFile, 1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

CDummyFile::CDummyFile()
{
}

CDummyFile::~CDummyFile()
{
   TRACE( "Destroying a CDummyFile object\n" );

   Close();
}

void CDummyFile::Abort( void )
{
   Close();
}

void CDummyFile::Close( void )
{
   /*
   ** This is here to trap calls that attempt to close an already closed file.
   ** I don't consider this an error but MFC does...
   */

   if ( m_hFile != (UINT) hFileNull )
   {
      CFile::Close();
   }
}

#if defined( _DEBUG )

void CDummyFile::Dump( CDumpContext& dump_context ) const
{
   CFile::Dump( dump_context );

   dump_context << "m_ErrorCode = " << m_ErrorCode << "\n";
}

#endif // _DEBUG

void CDummyFile::Flush( void )
{
}

DWORD CDummyFile::GetError( void ) const
{
   return( m_ErrorCode );
}

DWORD CDummyFile::GetLength( void ) const
{
   return( 0L );
}

DWORD CDummyFile::GetPosition( void ) const
{
   ASSERT( FALSE ); // Unsupported function
   return( 0L );
}

#pragma warning( disable : 4100 )

BOOL CDummyFile::GetStatus( CFileStatus& status )
{
   return( FALSE );
}

BOOL CDummyFile::GetStatus( LPCTSTR Name, CFileStatus& Status )
{
   return( FALSE );
}

void CDummyFile::LockRange( DWORD Position, DWORD Count )
{
   ASSERT( FALSE ); // Unsupported function
}

void CDummyFile::Remove( LPCTSTR name )
{
   ASSERT( FALSE ); // Unsupported function
}

void CDummyFile::Rename( LPCTSTR OldName, LPCTSTR NewName )
{
   ASSERT( FALSE ); // Unsupported function
}

#pragma warning( disable : 4100 )

LONG CDummyFile::Seek( LONG Offset, UINT From )
{
   ASSERT( FALSE ); // Unsupported function
   return( 0L );
}

#pragma warning( default : 4100 )

void CDummyFile::Serialize( CArchive& archive )
{
   CFile::Serialize( archive );
}

#pragma warning( disable : 4100 )

void CDummyFile::SetLength( DWORD length )
{
   ASSERT( FALSE ); // Unsupported function
}

void CDummyFile::SetStatus( LPCTSTR name, const CFileStatus& status )
{
}

void CDummyFile::UnlockRange( DWORD Position, DWORD Count )
{
   ASSERT( FALSE ); // Unsupported function
}

void CDummyFile::Write( const CString& string_to_write )
{
   CFile::Write( (const void *) (LPCTSTR) string_to_write, (UINT) string_to_write.GetLength() );
}

void CDummyFile::Write( const CByteArray& data_to_write )
{
   UINT number_of_bytes_to_write = data_to_write.GetSize();

   BYTE *buffer = new BYTE[ number_of_bytes_to_write ];

   ASSERT( buffer != NULL );

   if ( buffer == NULL )
   {
      return;
   }

   UINT index = 0;

   while( index < number_of_bytes_to_write )
   {
      buffer[ index ] = data_to_write.GetAt( index );
      index++;
   }

   CFile::Write( (const void *) buffer, number_of_bytes_to_write );

   delete [] buffer;
}

#pragma warning( default : 4100 )
