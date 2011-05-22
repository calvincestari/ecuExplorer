#if ! defined ( UNC_CLASS_HEADER )

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

#define UNC_CLASS_HEADER

class CUniformResourceLocator;

class CUniversalNamingConvention : public CObject
{
   DECLARE_SERIAL( CUniversalNamingConvention )

   public:

      CUniversalNamingConvention();
      CUniversalNamingConvention( const CUniversalNamingConvention& source );
      CUniversalNamingConvention( const CUniformResourceLocator& source );
      CUniversalNamingConvention( LPCTSTR source );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CUniversalNamingConvention();

      /*
      ** Parse stuff like:
      ** \\buckaroo\public\neato\readme.txt
      */

      /*
      ** Data
      */

      CString ServerName;
      CString ShareName;
      CString PathName;
      CString UNC;

      /*
      ** Methods
      */

      virtual int  Compare( const CUniversalNamingConvention& source );
      virtual void Copy( LPCTSTR source );
      virtual void Copy( const CUniversalNamingConvention& source );
      virtual void Copy( const CUniformResourceLocator& source );
      virtual void Empty( void );
      virtual void Make( void );
      virtual void Serialize( CArchive& archive );

      virtual BOOL operator == ( const CUniversalNamingConvention& right_unc );
      virtual CUniversalNamingConvention& operator = ( const CUniversalNamingConvention& source );
      virtual CUniversalNamingConvention& operator = ( const CUniformResourceLocator& source );
      virtual CUniversalNamingConvention& operator = ( LPCTSTR source );

      operator LPCTSTR() { return( (LPCTSTR) UNC ); }

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};   

#endif // UNC_CLASS_HEADER
