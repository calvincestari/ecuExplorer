#ifndef SERIAL_FILE_CLASS_HEADER

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

#define SERIAL_FILE_CLASS_HEADER

class CSerialFile : public CDummyFile
{
   DECLARE_SERIAL( CSerialFile )

   private:

      void m_Initialize( void );

   protected:

      BOOL m_IsOpen;

      DWORD m_CommunicationErrorCodes;

      COMSTAT m_CommunicationsStatus;

      virtual void m_ClearError( int line_number = 0 );

   public:

      CSerialFile();

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CSerialFile();

      static const WORD flowXonXoff;
      static const WORD flowRtsCts;
      static const WORD flowSoftware;
      static const WORD flowHardware;

      CString Name; // COM1:9600,n,8,1

      HANDLE FileHandle;

      virtual BOOL  Open( void ); // Name already filled, used in re-opening an existing session
      virtual BOOL  Open( const char *channel_name, UINT open_flags = 0, CFileException* pError = NULL );
      virtual void  Close( void );
      virtual DWORD GetFlowControl( void );
      virtual DWORD NumberOfBytesWaitingToBeRead( void );
      virtual DWORD NumberOfBytesWaitingToBeWritten( void );
      virtual BOOL  IsDataWaiting( void );
      virtual UINT  Read( void *buffer, UINT length );
      virtual void  Serialize( CArchive& archive );
      virtual void  SetFlowControl( DWORD flow_control );
      virtual void  Write( CString& data_to_write );
      virtual void  Write( const void *buffer, UINT number_of_bytes );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

#endif // SERIAL_FILE_CLASS_HEADER
