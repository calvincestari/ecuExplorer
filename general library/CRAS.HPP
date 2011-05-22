#ifndef REMOTE_ACCESS_SERVICE_CLASS_HEADER

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

#define REMOTE_ACCESS_SERVICE_CLASS_HEADER

#pragma warning ( disable : 4097 )

/*
** We have to declare a typedef because of the funcky way Microsoft declared RASAMB in RAS.H
*/

typedef RASAMB _rasamb;

class CRemoteAccessServiceAuthenticationMessageBlock : public _rasamb
{
   private:

      void m_Initialize( void );

   public:

      CRemoteAccessServiceAuthenticationMessageBlock();
      CRemoteAccessServiceAuthenticationMessageBlock( const RASAMB *source );
      CRemoteAccessServiceAuthenticationMessageBlock( const CRemoteAccessServiceAuthenticationMessageBlock& source );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CRemoteAccessServiceAuthenticationMessageBlock();

      void  Copy( const RASAMB *source );
      void  Copy( const CRemoteAccessServiceAuthenticationMessageBlock& source );
      BYTE  GetLana( void ) const;
      DWORD GetErrorCode( void ) const;
      void  GetErrorString( CString& return_string ) const;
};

/*
** We have to declare a typedef because of the funcky way Microsoft declared RASAMB in RAS.H
*/

typedef RASPPPNBF _raspppnbf;

class CRemoteAccessServiceNetBEUIFramer : public _raspppnbf
{
   private:

      void m_Initialize( void );

   public:

      CRemoteAccessServiceNetBEUIFramer();
      CRemoteAccessServiceNetBEUIFramer( const RASPPPNBF *source );
      CRemoteAccessServiceNetBEUIFramer( const CRemoteAccessServiceNetBEUIFramer& source );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CRemoteAccessServiceNetBEUIFramer();

      void  Copy( const RASPPPNBF* source );
      void  Copy( const CRemoteAccessServiceNetBEUIFramer& source );
      BYTE  GetLana( void ) const;
      DWORD GetErrorCode( void ) const;
      DWORD GetNetBiosErrorCode( void ) const;
      void  GetErrorString( CString& return_string ) const;
      void  GetWorkstationName( CString& return_string ) const;
};

/*
** We have to declare a typedef because of the funcky way Microsoft declared RASAMB in RAS.H
*/

typedef RASPPPIPX _raspppipx;

class CRemoteAccessServiceInternetworkPacketExchange : public _raspppipx
{
   private:

      void m_Initialize( void );

   public:

      CRemoteAccessServiceInternetworkPacketExchange();
      CRemoteAccessServiceInternetworkPacketExchange( const RASPPPIPX * source );
      CRemoteAccessServiceInternetworkPacketExchange( const CRemoteAccessServiceInternetworkPacketExchange& source );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CRemoteAccessServiceInternetworkPacketExchange();

      void   Copy( const RASPPPIPX * source );
      void   Copy( const CRemoteAccessServiceInternetworkPacketExchange& source );
      DWORD  GetErrorCode( void ) const;
      void   GetIPXAddress( CString& return_string ) const;
};

/*
** We have to declare a typedef because of the funcky way Microsoft declared RASAMB in RAS.H
*/

typedef RASPPPIP _raspppip;

class CRemoteAccessServiceInternetProtocol : public _raspppip
{
   private:

      void m_Initialize( void );

   public:

      CRemoteAccessServiceInternetProtocol();
      CRemoteAccessServiceInternetProtocol( const RASPPPIP * source );
      CRemoteAccessServiceInternetProtocol( const CRemoteAccessServiceInternetProtocol& source );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CRemoteAccessServiceInternetProtocol();

      void   Copy( const RASPPPIP * source );
      void   Copy( const CRemoteAccessServiceInternetProtocol& source );
      DWORD  GetErrorCode( void ) const;
      void   GetIPAddress( CString& return_string ) const;
};

#pragma warning ( default : 4097 )

class CRemoteAccessService : public CObject
{
   private:

      void m_Initialize( void );
      void m_GetConnectionsIntoMemory( LPRASCONN& return_value, DWORD& number_of_entries );

   protected:

      LONG m_ErrorCode;

      HRASCONN m_ConnectionHandle;

      RASCONNSTATUS m_ConnectionStatus;

   public:

      enum Protocols
      {
         protocolAuthenticationMessageBlock = RASP_Amb,
         protocolNetBEUIFramer              = RASP_PppNbf,
         protocolInternetworkPacketExchange = RASP_PppIpx,
         protocolInternetProtocol           = RASP_PppIp
      };

      CRemoteAccessService();
      CRemoteAccessService( DWORD input_buffer_size );
      CRemoteAccessService( DWORD input_buffer_size, DWORD output_buffer_size );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CRemoteAccessService();

      /*
      ** The Win32 API
      */

      virtual BOOL     Dial( LPCTSTR who_to_call ); // RasDial
      virtual HRASCONN GetConnection( LPCTSTR name_of_connection );
      virtual BOOL     GetConnections( CStringArray& connections ); // RasEnumConnections
      virtual BOOL     GetConnectionStatus( void ); // RasGetConnectStatus
      virtual BOOL     GetErrorCode( void ) const;
      virtual BOOL     GetErrorString( CString& error_string ); // RasGetErrorString
      virtual BOOL     GetPhoneBookEntries( CStringArray& phone_book_entries );
      virtual BOOL     GetProtocolInformation( CRemoteAccessServiceAuthenticationMessageBlock& data_to_get );
      virtual BOOL     GetProtocolInformation( CRemoteAccessServiceNetBEUIFramer& data_to_get );
      virtual BOOL     GetProtocolInformation( CRemoteAccessServiceInternetworkPacketExchange& data_to_get );
      virtual BOOL     GetProtocolInformation( CRemoteAccessServiceInternetProtocol& data_to_get );
      virtual BOOL     HangUp( LPCTSTR name_of_connection = NULL );
      virtual BOOL     IsConnected( void );

      /*
      ** API's to make life a little easier 
      */

      virtual BOOL Open( LPCTSTR who_to_call ); // Dial()
      virtual BOOL Close( LPCTSTR name_of_connection = NULL ); // HangUp();
};

#endif // REMOTE_ACCESS_SERVICE_CLASS_HEADER
