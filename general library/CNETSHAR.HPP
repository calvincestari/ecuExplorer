#if ! defined( NETWORK_SHARE_CLASS_HEADER )

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

#define NETWORK_SHARE_CLASS_HEADER

class CNetworkShareInformation : public CObject
{
   DECLARE_SERIAL( CNetworkShareInformation )

   private:

      void m_Initialize( void );

   public:

      CNetworkShareInformation();
      CNetworkShareInformation( SHARE_INFO_2 *information_p );
      CNetworkShareInformation( const CNetworkShareInformation& source );
      virtual ~CNetworkShareInformation();

      CString NetworkName;
      DWORD   Type;
      CString Remark;
      DWORD   Permissions;
      DWORD   MaximumNumberOfUses;
      DWORD   CurrentNumberOfUses;
      CString PathName;
      CString Password;

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      virtual void Copy( SHARE_INFO_2 *source );
      virtual void Copy( const CNetworkShareInformation& source );
      virtual void Empty( void );
      virtual void Serialize( CArchive& archive );
};

class CNetworkShares : public CNetwork
{
   DECLARE_SERIAL( CNetworkShares )

   private:

      void m_Initialize( void );

   protected:

      /*
      ** Connection information variables
      */

      SHARE_INFO_2 *m_2InformationBuffer;

      /*
      ** File Information enumeration variables
      */

      DWORD m_2ResumeHandle;
      DWORD m_2CurrentEntryNumber;
      DWORD m_2NumberOfEntriesRead;
      DWORD m_2TotalNumberOfEntries;

   public:

      CNetworkShares();
      CNetworkShares( LPCTSTR machine_name );
      virtual ~CNetworkShares();

      virtual BOOL  Add( CNetworkShareInformation& share_to_add );
      virtual void  Close( void );
      virtual BOOL  Delete( CNetworkShareInformation& share_to_delete );
      virtual BOOL  Enumerate( void );
      virtual BOOL  GetNext( CNetworkShareInformation& information );
      virtual void  Serialize( CArchive& archive );
};

#endif // NETWORK_SHARE_CLASS_HEADER
