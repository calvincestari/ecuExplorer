#if ! defined( NETWORK_CONNECTION_CLASS_HEADER )

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

#define NETWORK_CONNECTION_CLASS_HEADER

class CNetworkConnectionInformation : public CObject
{
   DECLARE_SERIAL( CNetworkConnectionInformation )

   private:

      void m_Initialize( void );

   public:

      CNetworkConnectionInformation();

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      CNetworkConnectionInformation( CONNECTION_INFO_1 *information_p );
      CNetworkConnectionInformation( const CNetworkConnectionInformation& source );
      virtual ~CNetworkConnectionInformation();

      DWORD   ID;
      DWORD   Type;
      DWORD   NumberOfUsers;
      DWORD   NumberOfOpens;
      DWORD   Time;
      CString UserName;
      CString NetName;

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      virtual void Copy( CONNECTION_INFO_1 *source );
      virtual void Copy( const CNetworkConnectionInformation& source );
      virtual void Empty( void );
      virtual void Serialize( CArchive& archive );

      virtual const CNetworkConnectionInformation& operator=( const CNetworkConnectionInformation& source );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

class CNetworkConnections : public CNetwork
{
   DECLARE_SERIAL( CNetworkConnections )

   private:

      void m_Initialize( void );

   protected:

      /*
      ** Connection information variables
      */

      CONNECTION_INFO_1 *m_1InformationBuffer;

      /*
      ** File Information enumeration variables
      */

      DWORD m_1ResumeHandle;
      DWORD m_1CurrentEntryNumber;
      DWORD m_1NumberOfEntriesRead;
      DWORD m_1TotalNumberOfEntries;

   public:

      CNetworkConnections();
      CNetworkConnections( LPCTSTR machine_name );
      virtual ~CNetworkConnections();

      virtual void  Close( void );
      virtual BOOL  Enumerate( LPCTSTR share_or_computer_name );
      virtual BOOL  GetNext( CNetworkConnectionInformation& information );
      virtual void  Serialize( CArchive& archive );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

#endif // NETWORK_CONNECTION_CLASS_HEADER
