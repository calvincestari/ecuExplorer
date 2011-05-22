#if ! defined( NET_WORKSTATION_CLASS_HEADER )

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

#define NET_WORKSTATION_CLASS_HEADER

class CWorkstationUser : public CObject
{
   DECLARE_SERIAL( CWorkstationUser )

   private:

      void m_Initialize( void );

   public:

      CWorkstationUser();

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      CWorkstationUser( WKSTA_USER_INFO_1 *source );
      CWorkstationUser( const CWorkstationUser& source );
      virtual ~CWorkstationUser();

      /*
      ** Patterned after WKSTA_USER_INFO_1
      */

      CString UserName;
      CString LogonDomain;
      CString OtherDomains;
      CString LogonServer;

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      virtual void Copy( WKSTA_USER_INFO_1 *source );
      virtual void Copy( const CWorkstationUser& source );
      virtual void Empty( void );
      virtual void Serialize( CArchive& archive );
};

class CWorkstationInformation : public CObject
{
   DECLARE_SERIAL( CWorkstationInformation )

   private:

      void m_Initialize( void );

   public:

      CWorkstationInformation();

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      CWorkstationInformation( WKSTA_INFO_100 *source );
      CWorkstationInformation( WKSTA_INFO_101 *source );
      CWorkstationInformation( WKSTA_INFO_102 *source );
      CWorkstationInformation( const CWorkstationInformation& source );
      virtual ~CWorkstationInformation();

      /*
      ** Patterned after WKSTA_INFO_102
      */

      DWORD   PlatformID;
      CString ComputerName;
      CString LANGroup;
      DWORD   MajorVersion;
      DWORD   MinorVersion;
      CString LANRoot;
      DWORD   NumberOfLoggedOnUsers;

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      virtual void Copy( WKSTA_INFO_100 *source );
      virtual void Copy( WKSTA_INFO_101 *source );
      virtual void Copy( WKSTA_INFO_102 *source );
      virtual void Copy( const CWorkstationInformation& source );
      virtual void Empty( void );
      virtual void Serialize( CArchive& archive );
};

class CWorkstationTransport : public CObject
{
   DECLARE_SERIAL( CWorkstationTransport )

   private:

      void m_Initialize( void );

   public:

      CWorkstationTransport();

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      CWorkstationTransport( WKSTA_TRANSPORT_INFO_0 *source );
      CWorkstationTransport( const CWorkstationTransport& source );
      virtual ~CWorkstationTransport();

      /*
      ** Patterned after WKSTA_USER_INFO_1
      */

      DWORD   QualityOfService;
      DWORD   NumberOfVirtualCircuits;
      CString Name;
      CString Address;
      BOOL    WANish;

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      virtual void Copy( WKSTA_TRANSPORT_INFO_0 *source );
      virtual void Copy( const CWorkstationTransport& source );
      virtual void Empty( void );
      virtual void Serialize( CArchive& archive );
};

class CNetWorkstation : public CNetwork
{
   DECLARE_SERIAL( CNetWorkstation )

   private:

      void m_Initialize( void );

   protected:

      /*
      ** Workstation information variables
      */

      WKSTA_INFO_100 *m_InformationBuffer100;
      WKSTA_INFO_101 *m_InformationBuffer101;
      WKSTA_INFO_102 *m_InformationBuffer102;

      /*
      ** Transport enumeration variables
      */

      WKSTA_TRANSPORT_INFO_0 *m_TransportBuffer;
      DWORD m_TransportResumeHandle;
      DWORD m_TransportCurrentEntryNumber;
      DWORD m_TransportNumberOfEntriesRead;
      DWORD m_TransportTotalNumberOfEntries;

      /*
      ** User enumeration variables
      */

      WKSTA_USER_INFO_1 *m_UserBuffer;
      DWORD m_UserResumeHandle;
      DWORD m_UserCurrentEntryNumber;
      DWORD m_UserNumberOfEntriesRead;
      DWORD m_UserTotalNumberOfEntries;

   public:

      CNetWorkstation();
      CNetWorkstation( LPCTSTR machine_name );
      virtual ~CNetWorkstation();

      virtual void  Close( void );
      virtual BOOL  EnumerateInformation( void );
      virtual BOOL  EnumerateTransports( void );
      virtual BOOL  EnumerateUsers( void );
      virtual BOOL  GetCurrentUser( CWorkstationUser& information );
      virtual BOOL  GetNext( CWorkstationInformation& information );
      virtual BOOL  GetNext( CWorkstationTransport& information );
      virtual BOOL  GetNext( CWorkstationUser& information );
      virtual void  Serialize( CArchive& archive );
};

#endif // NET_WORKSTATION_CLASS_HEADER
