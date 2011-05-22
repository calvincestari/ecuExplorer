#ifndef SOCKET_CLASS_HEADER

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

#define SOCKET_CLASS_HEADER

class CSimpleSocket : public CDummyFile
{
   private:

      void m_Initialize( void );

   protected:

      CString m_PortName;

      short m_PortNumberInNetworkByteOrder;

      SOCKET m_SocketID; 

   public:

      CSimpleSocket();
      CSimpleSocket( SOCKET client_id );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CSimpleSocket();

      CString     Address; 
      CString     Name;
      CStringList AliasList; // Pointers to CString's

      static void __stdcall StartWindowsSockets( void );
      static void __stdcall StopWindowsSockets( void );
      static void __stdcall TranslateErrorCode( DWORD error_code, LPSTR destination_string, DWORD size_of_destination_string );

      /*
      ** Getting data from the object
      */

      virtual void   GetAddress( CString& a ) const;
      virtual SOCKET GetID( void ) const;
      virtual void   GetName( CString& a ) const;
      virtual void   GetPort( short& p ) const;
      virtual void   GetPort( CString& p ) const;

      /*
      ** Putting data into the object
      */

      virtual void SetAddress( const char *a ); // Fills in Address, Name and AliasList
      virtual void SetID( const SOCKET id );
      virtual void SetName( const char *a );
      virtual void SetPort( const char *p );
      virtual void SetPort( const short p );

      /*
      ** Misc Functions
      */

      virtual BOOL Open( void ) = 0;
      virtual BOOL Open( const char *channel_name, UINT port_number = 23, CFileException* pError = NULL ) = 0;
      virtual void Close( void );
      virtual BOOL IsDataWaiting( void );
      virtual void Write( const CString& data_to_write );
      virtual void Write( const VOID *buffer, const long number_of_bytes_to_write );

      virtual UINT Read( VOID *buffer, const int max_bytes );
      virtual void Read( CString& line_to_read );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dc ) const;

#endif // _DEBUG
};

#endif // SOCKET_CLASS_HEADER
