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

IMPLEMENT_SERIAL( CSerialFile, CDummyFile, 1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

const WORD CSerialFile::flowXonXoff  = 1;
const WORD CSerialFile::flowRtsCts   = 2;
const WORD CSerialFile::flowSoftware = 1;
const WORD CSerialFile::flowHardware = 2;

CSerialFile::CSerialFile()
{
   FileHandle = INVALID_HANDLE_VALUE;
   ::ZeroMemory( &m_CommunicationsStatus, sizeof( m_CommunicationsStatus ) );
   m_IsOpen = FALSE;
   Name.Empty();
}

CSerialFile::~CSerialFile()
{
   Close();
}

void CSerialFile::Close( void )
{
   TRACE1( "CSerialFile::Close( \"%s\" )\n", (LPCTSTR) Name );

   if ( FileHandle == INVALID_HANDLE_VALUE )
   {
      m_IsOpen = FALSE;
      return;
   }

   m_ClearError( __LINE__ );

   ::EscapeCommFunction( FileHandle, CLRDTR );

   ::PurgeComm( FileHandle, PURGE_TXABORT |
                            PURGE_RXABORT |
                            PURGE_TXCLEAR |
                            PURGE_RXCLEAR );

   if ( m_hFile != (UINT) CFile::hFileNull )
   {
      CDummyFile::Close();
   }

   FileHandle = INVALID_HANDLE_VALUE;
   m_IsOpen   = FALSE;
}

#if defined( _DEBUG )

void CSerialFile::Dump( CDumpContext& dump_context ) const
{
   CDummyFile::Dump( dump_context );

   dump_context << "m_IsOpen = " << m_IsOpen << "\n";
   dump_context << "m_CommunicationErrorCodes = " << m_CommunicationErrorCodes << "\n";
   dump_context << "m_CommunicationsStatus is a COMSTAT:\n";
   dump_context << "{\n";
   dump_context << "   fCtsHold = "  << m_CommunicationsStatus.fCtsHold  << "\n";
   dump_context << "   fDsrHold = "  << m_CommunicationsStatus.fDsrHold  << "\n";
   dump_context << "   fRlsdHold = " << m_CommunicationsStatus.fRlsdHold << "\n";
   dump_context << "   fXoffHold = " << m_CommunicationsStatus.fXoffHold << "\n";
   dump_context << "   fXoffSent = " << m_CommunicationsStatus.fXoffSent << "\n";
   dump_context << "   fEof = "      << m_CommunicationsStatus.fEof      << "\n";
   dump_context << "   fTxim = "     << m_CommunicationsStatus.fTxim     << "\n";
   dump_context << "   fReserved = " << m_CommunicationsStatus.fReserved << "\n";
   dump_context << "   cbInQue = "   << m_CommunicationsStatus.cbInQue   << "\n";
   dump_context << "   cbOutQue = "  << m_CommunicationsStatus.cbOutQue  << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

DWORD CSerialFile::GetFlowControl( void )
{
   ASSERT( FileHandle != INVALID_HANDLE_VALUE );

   if ( FileHandle == INVALID_HANDLE_VALUE )
   {
      return( 0 );
   }

   DWORD flow_control = 0;

   CDeviceControlBlock device_control_block;

   if ( ::GetCommState( FileHandle, &device_control_block ) == FALSE )
   {
      m_ErrorCode = ::GetLastError();
      ASSERT( FALSE );
      return( 0 );
   }

   if ( device_control_block.fOutxDsrFlow == TRUE &&
        device_control_block.fOutxCtsFlow == TRUE )
   {

      flow_control = flowHardware;
   }

   if ( device_control_block.fInX  == TRUE &&
        device_control_block.fOutX == TRUE )
   {
      flow_control |= flowSoftware;
   }

   return( flow_control );
}

void CSerialFile::m_ClearError( int line_number )
{
   ASSERT( FileHandle != INVALID_HANDLE_VALUE );

   if ( FileHandle == INVALID_HANDLE_VALUE )
   {
      return;
   }

   if ( ::ClearCommError( FileHandle, &m_CommunicationErrorCodes, &m_CommunicationsStatus ) == FALSE )
   {
      m_ErrorCode = ::GetLastError();

      if ( m_ErrorCode == ERROR_INVALID_HANDLE )
      {
         FileHandle = INVALID_HANDLE_VALUE;
      }

      return;
   }

   if ( m_CommunicationErrorCodes != 0 )
   {
      TCHAR error_message[ 2048 ];

      ::ZeroMemory( error_message, sizeof( error_message ) );

      if ( m_CommunicationErrorCodes & CE_BREAK )
      {
         ::strcat( error_message, "\r\nThe hardware detected a break condition. (CE_BREAK);" );
      }

      if ( m_CommunicationErrorCodes & CE_FRAME )
      {
         ::strcat( error_message, "\r\nThe hardware detected a framing error. (CE_FRAME);" );
         ::PurgeComm( FileHandle, PURGE_RXCLEAR );
      }

      if ( m_CommunicationErrorCodes & CE_IOE )
      {
         ::strcat( error_message, "\r\nAn I/O error occurred during communications with the device. (CE_IOE);" );
      }

      if ( m_CommunicationErrorCodes & CE_MODE )
      {
         ::strcat( error_message, "\r\nThe requested mode is not supported or the hCommDev parameter is invalid. (CE_MODE);" );
      }

      if ( m_CommunicationErrorCodes & CE_OVERRUN )
      {
         ::strcat( error_message, "\r\nA character-buffer overrun has occurred. The next character is lost. (CE_OVERRUN);" );
      }

      if ( m_CommunicationErrorCodes & CE_RXOVER )
      {
         m_ErrorCode = CE_RXOVER;
         ::strcat( error_message, "\r\nAn input buffer overflow has occurred. There is either no room in the input buffer, or a character was received after the end-of-file (EOF) character. (CE_RXOVER);" );
      }

      if ( m_CommunicationErrorCodes & CE_RXPARITY )
      {
         ::strcat( error_message, "\r\n.The hardware detected a parity error. (CE_RXPARITY);" );
      }

      if ( m_CommunicationErrorCodes & CE_TXFULL )
      {
         ::strcat( error_message, "\r\nThe application tried to transmit a character, but the output buffer was full. (CE_TXFULL);" );
      }

      if ( m_CommunicationErrorCodes & CE_DNS )
      {
         ::strcat( error_message, "\r\nThe parallel device is not selected. (CE_DNS);" );
      }

      if ( m_CommunicationErrorCodes & CE_PTO )
      {
         ::strcat( error_message, "\r\nA time-out occurred on the parallel device. (CE_PTO);" );
      }

      if ( m_CommunicationErrorCodes & CE_OOP )
      {
         ::strcat( error_message, "\r\nThe parallel device signaled that it is out of paper. (CE_OOP);" );
      }

      TCHAR temp_string[ 100 ];

      ::sprintf( temp_string, "%d (%d in input_queue)", line_number, m_CommunicationsStatus.cbInQue );

      TRACE1( "%s", error_message );
      TRACE1( "%s\n", temp_string );
   }
}

BOOL CSerialFile::IsDataWaiting( void )
{
   ASSERT( FileHandle != INVALID_HANDLE_VALUE );

   if ( FileHandle == INVALID_HANDLE_VALUE )
   {
      return( FALSE );
   }

   m_ClearError( __LINE__ );

   if ( m_CommunicationsStatus.cbInQue > 0 )
   {
      return( TRUE );
   }
   else
   {
      return( FALSE );
   }
}

DWORD CSerialFile::NumberOfBytesWaitingToBeRead( void )
{
   ASSERT( FileHandle != INVALID_HANDLE_VALUE );

   if ( FileHandle == INVALID_HANDLE_VALUE )
   {
      return( 0 );
   }

   m_ClearError( __LINE__ );

   return( m_CommunicationsStatus.cbInQue );
}

DWORD CSerialFile::NumberOfBytesWaitingToBeWritten( void )
{
   ASSERT( FileHandle != INVALID_HANDLE_VALUE );

   if ( FileHandle == INVALID_HANDLE_VALUE )
   {
      return( 0 );
   }

   m_ClearError( __LINE__ );

   return( m_CommunicationsStatus.cbOutQue );
}

#pragma warning( disable : 4100 )

BOOL CSerialFile::Open( LPCTSTR channel_name, UINT open_flags, CFileException *exception_p )
{
   ASSERT( channel_name != NULL );

   if ( channel_name == NULL )
   {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
   }

   if ( m_IsOpen == TRUE )
   {
      Close();
   }

   Name = channel_name;

   return( Open() );
}

#pragma warning( default : 4100 )

BOOL CSerialFile::Open( void )
{
   if ( m_IsOpen == TRUE )
   {
      Close();
   }

   CString com_port_name( "" );

   int index = 0;
   int string_length = Name.GetLength();

   while( index < string_length && Name[ index ] != ':' )
   {
      com_port_name += Name[ index ];
      index++;
   }

   com_port_name += ':';

   CString settings_string( "" );

   settings_string = Name.Right( ( string_length - index ) - 1 );

   FileHandle = ::CreateFile( (LPCTSTR) com_port_name,
                              GENERIC_READ | GENERIC_WRITE,
                              0,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL );

   if ( FileHandle == INVALID_HANDLE_VALUE )
   {
      m_ErrorCode = ::GetLastError();

      TRACE1( "CSerialFile::Open(), %d\n", __LINE__ );
      return( FALSE );
   }

   if ( ::PurgeComm( FileHandle, PURGE_TXABORT |
                                 PURGE_RXABORT |
                                 PURGE_TXCLEAR |
                                 PURGE_RXCLEAR ) != TRUE )
   {
   }

   m_hFile = (HFILE) FileHandle;

   CString baud_string( "" );

   index = 0;
   string_length = settings_string.GetLength();

   while( index < string_length && settings_string[ index ] != ',' )
   {
      baud_string += settings_string[ index ];
      index++;
   }

   index++; // Skip over the comma

   CString parity_string( "" );

   while( index < string_length && settings_string[ index ] != ',' )
   {
      parity_string += settings_string[ index ];
      index++;
   }

   index++; // skip the comma

   CString data_bits_string( "" );

   while( index < string_length && settings_string[ index ] != ',' )
   {
      data_bits_string += settings_string[ index ];
      index++;
   }

   index++; // another dang comma

   CString stop_bits_string( "" );

   while( index < string_length )
   {
      stop_bits_string += settings_string[ index ];
      index++;
   }

   CString dcb_string( "" );

   dcb_string  = com_port_name;
   dcb_string += " baud=";
   dcb_string += baud_string;
   dcb_string += " parity=";
   dcb_string += parity_string;
   dcb_string += " data=";
   dcb_string += data_bits_string;
   dcb_string += " stop=";
   dcb_string += stop_bits_string;

   CDeviceControlBlock device_control_block;

   if ( ::GetCommState( FileHandle, &device_control_block ) == FALSE )
   {
      m_ErrorCode = ::GetLastError();
      TRACE1( "CSerialFile::Open() %d\n", __LINE__ );
      Close();
      return( FALSE );
   }

   if ( ::BuildCommDCB( dcb_string, &device_control_block ) == FALSE )
   {
      m_ErrorCode = ::GetLastError();
      TRACE1( "CSerialFile::Open() %d\n", __LINE__ );
      Close();
      return( FALSE );
   }

   device_control_block.fAbortOnError = FALSE; // Terminate Reads & Writes if there's an error
   device_control_block.fErrorChar    = TRUE;  // Replace any garbled bytes with ErrorChar
   device_control_block.ErrorChar     = ' ';   // Garbage bytes are spaces
   device_control_block.fBinary       = TRUE;  // Ignore EOF

   if ( ::SetCommState( FileHandle, &device_control_block ) == FALSE )
   {
      m_ErrorCode = ::GetLastError();
      TRACE1( "CSerialFile::Open() %d\n", __LINE__ );
      Close();
      return( FALSE );
   }

   if ( ::SetupComm( FileHandle, 4096, 1024 ) != TRUE )
   {
   }

   // assert DTR

   if ( ::EscapeCommFunction( FileHandle, SETDTR ) != TRUE )
   {
   }

   m_IsOpen = TRUE;

   return( TRUE );
}

UINT CSerialFile::Read( void *buffer, UINT length )
{
   ASSERT( FileHandle != INVALID_HANDLE_VALUE );
   ASSERT( buffer != NULL );

   if ( buffer == NULL || FileHandle == INVALID_HANDLE_VALUE )
   {
      return( 0 );
   }

   m_ClearError( __LINE__ );

   return( CDummyFile::Read( buffer, length ) );
}

void CSerialFile::Serialize( CArchive& archive )
{
   CDummyFile::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << (DWORD) m_IsOpen;
      archive << Name;
   }
   else
   {
      DWORD temp_dword = 0;

      archive >> temp_dword;
      archive >> Name;

      m_IsOpen = temp_dword;

      if ( m_IsOpen == TRUE )
      {
         Open();
      }
   }
}

void CSerialFile::SetFlowControl( DWORD flow_control )
{
   if ( flow_control == 0 )
   {
      return;
   }

   ASSERT( FileHandle != INVALID_HANDLE_VALUE );

   if ( FileHandle == INVALID_HANDLE_VALUE )
   {
      return;
   }

   CDeviceControlBlock device_control_block;

   if ( ::GetCommState( FileHandle, &device_control_block ) == FALSE )
   {
      m_ErrorCode = ::GetLastError();
      ASSERT( FALSE );
      return;
   }

   if ( flow_control & flowHardware )
   {
      device_control_block.fOutxDsrFlow = TRUE;
      device_control_block.fOutxCtsFlow = TRUE;
   }
   else
   {
      device_control_block.fOutxDsrFlow = FALSE;
      device_control_block.fOutxCtsFlow = FALSE;
   }

   if ( flow_control & flowSoftware )
   {
      device_control_block.fInX     = TRUE;
      device_control_block.fOutX    = TRUE;
      device_control_block.XonChar  = 0x11;
      device_control_block.XoffChar = 0x13;
      device_control_block.XonLim   = 100;
      device_control_block.XoffLim  = 100;
   }
   else
   {
      device_control_block.fInX     = FALSE;
      device_control_block.fOutX    = FALSE;
      device_control_block.XonChar  = 0;
      device_control_block.XoffChar = 0;
      device_control_block.XonLim   = 0;
      device_control_block.XoffLim  = 0;
   }

   if ( ::SetCommState( FileHandle, &device_control_block ) != TRUE )
   {
      m_ErrorCode = ::GetLastError();
      ASSERT( FALSE );
      return;
   }
}

void CSerialFile::Write( const void *buffer, UINT length )
{
   ASSERT( FileHandle != INVALID_HANDLE_VALUE );
   ASSERT( buffer != NULL );

   if ( buffer == NULL || FileHandle == INVALID_HANDLE_VALUE )
   {
      return;
   }

   m_ClearError( __LINE__ );

   CFile::Write( buffer, length );
}

void CSerialFile::Write( CString& string_to_write )
{
   ASSERT( FileHandle != INVALID_HANDLE_VALUE );

   if ( FileHandle == INVALID_HANDLE_VALUE )
   {
      return;
   }

   LPTSTR buffer_p = string_to_write.GetBuffer( 1 );

   UINT number_of_bytes = string_to_write.GetLength();

   Write( buffer_p, number_of_bytes );

   string_to_write.ReleaseBuffer();
}
