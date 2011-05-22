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

IMPLEMENT_SERIAL( CUniformResourceLocator, CObject, 1 );

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

CUniformResourceLocator::CUniformResourceLocator()
{
   Empty();
}

CUniformResourceLocator::CUniformResourceLocator( const CUniformResourceLocator& source )
{
   Copy( source );
}

CUniformResourceLocator::CUniformResourceLocator( const CUniversalNamingConvention& source )
{
   Copy( source );
}

CUniformResourceLocator::CUniformResourceLocator( LPCTSTR source )
{
   Copy( source );
}

CUniformResourceLocator::~CUniformResourceLocator()
{
   Empty();
}

int CUniformResourceLocator::Compare( const CUniformResourceLocator& source )
{
   ASSERT_VALID( this );

   return( URL.CompareNoCase( source.URL ) );
}

void CUniformResourceLocator::Copy( const CUniformResourceLocator& source )
{
   ASSERT_VALID( this );

   ProtocolName = source.ProtocolName;
   MachineName  = source.MachineName;
   PathName     = source.PathName;
   PortName     = source.PortName;
   URL          = source.URL;
}

void CUniformResourceLocator::Copy( const CUniversalNamingConvention& source )
{
   ASSERT_VALID( this );

   Empty();

   ProtocolName = "file";
   MachineName  = source.ServerName;
   PathName     = source.ShareName;
   PathName    += "/";
   PathName    += source.PathName;

   /*
   ** Now go through PathName to make sure it has /'s instead of \'s
   */

   int location_of_slash = 0;

   while( location_of_slash < PathName.GetLength() )
   {
      if ( PathName[ location_of_slash ] == '\\' )
      {
         PathName.SetAt( location_of_slash, '/' );
      }

      location_of_slash++;
   }

   Make();
}

void CUniformResourceLocator::Copy( LPCTSTR source )
{
   ASSERT_VALID( this );
   ASSERT( source != NULL );

   Empty();

   if ( source == NULL )
   {
      return;
   }

   CString temp_string = source;

   int location = temp_string.Find( ':' );

   if ( location == (-1) )
   {
      return;
   }

   ProtocolName = temp_string.Left( location );

   temp_string = temp_string.Right( ( temp_string.GetLength() - location ) - 1 );

   while( temp_string[ 0 ] == '/' )
   {
      temp_string = temp_string.Right( temp_string.GetLength() - 1 );
   }

   location = temp_string.Find( '/' );

   if ( location == (-1) )
   {
      MachineName = temp_string;
   }
   else
   {
      MachineName = temp_string.Left( location );
      PathName    = temp_string.Right( ( temp_string.GetLength() - location ) - 1 );
   }

   /*
   ** Now see if MachineName has a PortName in it
   */

   location = MachineName.Find( ':' );

   if ( location != (-1) )
   {
      PortName    = MachineName.Right( ( MachineName.GetLength() - location ) - 1 );
      MachineName = MachineName.Left( location );
   }

   Make();
}

void CUniformResourceLocator::Empty( void )
{
   ASSERT_VALID( this );

   ProtocolName.Empty();
   MachineName.Empty();
   PathName.Empty();
   PortName.Empty();
   URL.Empty();
}

void CUniformResourceLocator::Make( void )
{
   ASSERT_VALID( this );

   URL  = ProtocolName;
   URL += "://";
   URL += MachineName;

   if ( PortName != "" )
   {
      URL += ":";
      URL += PortName;
   }

   URL += "/";
   URL += PathName;
}

void CUniformResourceLocator::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << ProtocolName;
      archive << MachineName;
      archive << PathName;
      archive << PortName;
      archive << URL;
   }
   else
   {
      archive >> ProtocolName;
      archive >> MachineName;
      archive >> PathName;
      archive >> PortName;
      archive >> URL;
   }
}

CUniformResourceLocator& CUniformResourceLocator::operator = ( const CUniformResourceLocator& source )
{
   ASSERT_VALID( this );
   Copy( source );
   return( *this );
}

CUniformResourceLocator& CUniformResourceLocator::operator = ( const CUniversalNamingConvention& source )
{
   ASSERT_VALID( this );
   Copy( source );
   return( *this );
}

CUniformResourceLocator& CUniformResourceLocator::operator = ( LPCTSTR source )
{
   ASSERT_VALID( this );
   Copy( source );
   return( *this );
}

BOOL CUniformResourceLocator::operator == ( const CUniformResourceLocator& right_url )
{
   ASSERT_VALID( this );

   if ( Compare( right_url ) == 0 )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

#if defined( _DEBUG )

void CUniformResourceLocator::Dump( CDumpContext& dump_context ) const
{
   CObject::Dump( dump_context );

   dump_context << "ProtocolName = \"" << ProtocolName << "\"\n";
   dump_context << "MachineName = \"" << MachineName << "\"\n";
   dump_context << "PathName = \"" << PathName << "\"\n";
   dump_context << "PortName = \"" << PortName << "\"\n";
   dump_context << "URL = \"" << URL << "\"\n";
}

#endif // _DEBUG
