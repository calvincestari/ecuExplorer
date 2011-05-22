#if ! defined( NET_SESSION_CLASS_HEADER )

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

#define NET_SESSION_CLASS_HEADER

class CNetworkSessionInformation : public CObject
{
   DECLARE_SERIAL( CNetworkSessionInformation )

   private:

      void m_Initialize( void );

   public:

      CNetworkSessionInformation();
      CNetworkSessionInformation( SESSION_INFO_502 *source );
      CNetworkSessionInformation( const CNetworkSessionInformation& source );
      virtual ~CNetworkSessionInformation();

      /*
      ** Patterned after SESSION_INFO_502
      */

      CString   ClientName;
      CString   UserName;
      DWORD     NumberOfOpens;
      CTimeSpan Time;
      CTimeSpan IdleTime;
      DWORD     UserFlags;
      CString   ClientTypeName;
      CString   Transport;

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      virtual void Copy( SESSION_INFO_502 *source );
      virtual void Copy( const CNetworkSessionInformation& source );
      virtual void Empty( void );
      virtual void Serialize( CArchive& archive );
};

class CNetworkSessions : public CNetwork
{
   DECLARE_SERIAL( CNetworkSessions )

   private:

      void m_Initialize( void );

   protected:

      CString m_ServerName;
      CString m_ClientName;

      BOOL m_Retrieved0;
      BOOL m_Retrieved1;
      BOOL m_Retrieved2;
      BOOL m_Retrieved10;
      BOOL m_Retrieved502;

      DWORD m_ResumeHandle;

      virtual void m_Get_0_Data( void );
      virtual void m_Get_1_Data( void );
      virtual void m_Get_2_Data( void );
      virtual void m_Get_10_Data( void );
      virtual void m_Get_502_Data( void );

   public:

      CNetworkSessions();
      CNetworkSessions( LPCTSTR machine_name );
      virtual ~CNetworkSessions();

      virtual void  Close( void );
      virtual BOOL  Delete( CNetworkSessionInformation& session_to_delete );
      virtual BOOL  Enumerate( CNetworkSessionInformation& information );
      virtual BOOL  GetNext( CNetworkSessionInformation& information );
      virtual void  Serialize( CArchive& archive );
};

#endif // NET_SESSION_CLASS_HEADER
