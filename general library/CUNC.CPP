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

IMPLEMENT_SERIAL( CUniversalNamingConvention, CObject, 1 );

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

CUniversalNamingConvention::CUniversalNamingConvention()
{
   Empty();
}

CUniversalNamingConvention::CUniversalNamingConvention( const CUniversalNamingConvention& source )
{
   Copy( source );
}

CUniversalNamingConvention::CUniversalNamingConvention( const CUniformResourceLocator& source )
{
   Copy( source );
}

CUniversalNamingConvention::CUniversalNamingConvention( LPCTSTR source )
{
   Copy( source );
}

CUniversalNamingConvention::~CUniversalNamingConvention()
{
   Empty();
}

int CUniversalNamingConvention::Compare( const CUniversalNamingConvention& source )
{
   ASSERT_VALID( this );

   if ( this == &source )
   {
      return( 0 );
   }

   return( UNC.CompareNoCase( source.UNC ) );
}

void CUniversalNamingConvention::Copy( const CUniversalNamingConvention& source )
{
   ASSERT_VALID( this );
   ASSERT( this != &source );

   /*
   ** Make sure we ain't copying ourselves
   */

   if ( this == &source )
   {
      return;
   }

   ServerName = source.ServerName;
   ShareName  = source.ShareName;
   PathName   = source.PathName;

   /*
   ** Its safer is we construct the UNC
   */

   Make();
}

void CUniversalNamingConvention::Copy( const CUniformResourceLocator& source )
{
   ASSERT_VALID( this );

   Empty();

   ServerName = source.MachineName;
   PathName   = source.PathName;

   int location_of_slash = PathName.Find( '/' );

   if ( location_of_slash != (-1) )
   {
      ShareName = PathName.Left( location_of_slash );
      PathName  = PathName.Right( ( PathName.GetLength() - location_of_slash ) - 1 );
   }

   /*
   ** Now go through the Path and convert /'s to \'s
   */

   location_of_slash = 0;

   while( location_of_slash < PathName.GetLength() )
   {
      if ( PathName[ location_of_slash ] == '/' )
      {
         PathName.SetAt( location_of_slash, '\\' );
      }

      location_of_slash++;
   }

   Make();
}

void CUniversalNamingConvention::Copy( LPCTSTR source )
{
   ASSERT_VALID( this );
   ASSERT( source != NULL );

   TRACE( "CUniversalNamingConvention::Copy( \"%s\" );\n", source );

   /*
   ** Input should be "\\server_name\share_name\directory\filename.ext"
   */

   /*
   ** Make sure we start out with a virgin object
   */

   Empty();

   if ( source == NULL )
   {
      return;
   }

   CString temp_string( source );

   if ( temp_string.GetLength() < 2 || temp_string.Left( 2 ) != "\\\\" )
   {
      PathName = temp_string;
      Make();
      return;
   }

   /*
   ** See if the first two characters are back slashes, if so, rip them off
   */

   while( temp_string[ 0 ] == '\\' )
   {
      temp_string = temp_string.Right( temp_string.GetLength() - 1 );
   }

   int location_of_back_slash = temp_string.Find( '\\' );

   /*
   ** First field is ServerName
   */

   if ( location_of_back_slash == (-1) )
   {
      /*
      ** The user sent us something like "\\dodah.txt"
      */

      PathName = temp_string;
      Make();
      return;
   }

   /*
   ** First field is server name
   */

   ServerName = temp_string.Left( location_of_back_slash );

   temp_string = temp_string.Right( ( temp_string.GetLength() - location_of_back_slash ) - 1 );

   /*
   ** Second field is ShareName
   */

   location_of_back_slash = temp_string.Find( '\\' );

   if ( location_of_back_slash == (-1) )
   {
      /*
      ** User sent us something like "\\server_name\"
      */

      Make();
      return;
   }

   ShareName = temp_string.Left( location_of_back_slash );
   PathName  = temp_string.Right( ( temp_string.GetLength() - location_of_back_slash ) - 1 );

   Make();
}

void CUniversalNamingConvention::Empty( void )
{
   ASSERT_VALID( this );

   ServerName.Empty();
   ShareName.Empty();
   PathName.Empty();
   UNC.Empty();
}

void CUniversalNamingConvention::Make( void )
{
   ASSERT_VALID( this );

   TRACE( "CUniversalNamingConvention::Make() : Adding \"\\\\\" + \"%s\" + \"\\\" + \"%s\" + \"\\\" + \"%s\"\n",
          (LPCTSTR) ServerName,
          (LPCTSTR) ShareName,
          (LPCTSTR) PathName );

   UNC  = "\\\\";
   UNC += ServerName;
   UNC += "\\";
   UNC += ShareName;
   UNC += "\\";
   UNC += PathName;
}

void CUniversalNamingConvention::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << ServerName;
      archive << ShareName;
      archive << PathName;
      archive << UNC;
   }
   else
   {
      archive >> ServerName;
      archive >> ShareName;
      archive >> PathName;
      archive >> UNC;
   }
}

CUniversalNamingConvention& CUniversalNamingConvention::operator = ( const CUniversalNamingConvention& source )
{
   ASSERT_VALID( this );
   Copy( source );
   return( *this );
}

CUniversalNamingConvention& CUniversalNamingConvention::operator = ( const CUniformResourceLocator& source )
{
   ASSERT_VALID( this );
   Copy( source );
   return( *this );
}

CUniversalNamingConvention& CUniversalNamingConvention::operator = ( LPCTSTR source )
{
   ASSERT_VALID( this );
   Copy( source );
   return( *this );
}

BOOL CUniversalNamingConvention::operator == ( const CUniversalNamingConvention& right_unc )
{
   ASSERT_VALID( this );

   if ( Compare( right_unc ) == 0 )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

#if defined( _DEBUG )

void CUniversalNamingConvention::Dump( CDumpContext& dump_context ) const
{
   CObject::Dump( dump_context );

   dump_context << "ServerName = \"" << ServerName << "\"\n";
   dump_context << "ShareName = \"" << ShareName << "\"\n";
   dump_context << "PathName = \"" << PathName << "\"\n";
   dump_context << "UNC = \"" << UNC << "\"\n";
}

#endif // _DEBUG
