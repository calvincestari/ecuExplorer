#if ! defined( NETWORK_FILE_CLASS_HEADER )

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

#define NETWORK_FILE_CLASS_HEADER

class CNetworkFileInformation : public CObject
{
   DECLARE_SERIAL( CNetworkFileInformation )

   private:

      void m_Initialize( void );

   public:

      CNetworkFileInformation();

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      CNetworkFileInformation( FILE_INFO_3 *information_p );
      CNetworkFileInformation( const CNetworkFileInformation& source );
      virtual ~CNetworkFileInformation();

      DWORD   ID;
      DWORD   Permissions;
      DWORD   NumberOfLocks;
      CString PathName;
      CString UserName;

      /*
      ** Can't make Copy take a const pointer because Microsoft screwed up the 
      ** net API header files...
      */

      virtual void Copy( FILE_INFO_3 *source );
      virtual void Copy( const CNetworkFileInformation& source );
      virtual void Empty( void );
      virtual void Serialize( CArchive& archive );
};

class CNetworkFiles : public CNetwork
{
   DECLARE_SERIAL( CNetworkFiles )

   private:

      void m_Initialize( void );

   protected:

      /*
      ** Workstation information variables
      */

      FILE_INFO_3 *m_103InformationBuffer;

      /*
      ** File Information enumeration variables
      */

      DWORD m_103ResumeHandle;
      DWORD m_103CurrentEntryNumber;
      DWORD m_103NumberOfEntriesRead;
      DWORD m_103TotalNumberOfEntries;

   public:

      CNetworkFiles();
      CNetworkFiles( LPCTSTR machine_name );
      virtual ~CNetworkFiles();

      virtual void  Close( void );
      virtual BOOL  Close( CNetworkFileInformation& file_to_close );
      virtual BOOL  Enumerate( void );
      virtual BOOL  GetNext( CNetworkFileInformation& information );
      virtual void  Serialize( CArchive& archive );
};

#endif // NETWORK_FILE_CLASS_HEADER
