#if ! defined( DATA_DECOMPRESSION_CLASS_HEADER )

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

#define DATA_DECOMPRESSION_CLASS_HEADER

class CLZFile : public CDummyFile
{
   private:

      void m_Initialize( void );

   protected:

      INT m_LZFileHandle;

      COFStruct m_OpenFileStructure;
      
   public:

      CLZFile();

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CLZFile();

      static void __stdcall TranslateErrorCode( int error_code, CString& error_message );

      /*
      ** The Win32 API
      */

      virtual void Close( void );
      virtual BOOL Copy( const CLZFile& source );
      virtual BOOL Copy( const CLZFile *source );
      virtual BOOL GetExpandedName( LPTSTR name_of_compressed_file, CString& original_file_name );
      virtual BOOL Open( const char *channel_name, UINT style = OF_READ, CFileException* pError = NULL );
      virtual UINT Read( void* buffer, UINT size_of_buffer );
      virtual LONG Seek( LONG offset, UINT from );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

#endif // DATA_DECOMPRESSION_CLASS_HEADER
