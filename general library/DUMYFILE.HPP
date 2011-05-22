#ifndef DUMMY_FILE_CLASS_HEADER

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

#define DUMMY_FILE_CLASS_HEADER

class CDummyFile : public CFile
{
   DECLARE_SERIAL( CDummyFile )

   protected:

      DWORD m_ErrorCode;

   public:

      CDummyFile();

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CDummyFile();

      /*
      ** Interception functions (would blow up if we let CFile get them)
      */

      virtual void  Abort( void );
      virtual void  Close( void );
      virtual void  Flush( void );
      virtual DWORD GetError( void ) const;
      virtual DWORD GetLength( void ) const;
      virtual DWORD GetPosition( void ) const;
      virtual BOOL  GetStatus( CFileStatus& status );
      virtual BOOL  GetStatus( LPCTSTR name, CFileStatus& status );
      virtual void  LockRange( DWORD position, DWORD count );
      virtual void  Remove( LPCTSTR name );
      virtual void  Rename( LPCTSTR OldName, LPCTSTR NewName );
      virtual LONG  Seek( LONG offset, UINT from );
      virtual void  Serialize( CArchive& archive );
      virtual void  SetLength( DWORD length );
      virtual void  SetStatus( LPCTSTR name, const CFileStatus& status );
      virtual void  UnlockRange( DWORD position, DWORD count );

      /*
      ** A couple of utility functions to make life easier
      */

      virtual void Write( const CString& string_to_write  );
      virtual void Write( const CByteArray& data_to_write );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

#endif // DUMMY_FILE_CLASS_HEADER
