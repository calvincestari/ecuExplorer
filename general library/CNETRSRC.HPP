#if ! defined( NET_RESOURCE_CLASS_HEADER )

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

#define NET_RESOURCE_CLASS_HEADER

class CNetworkResourceInformation : public CObject
{
   DECLARE_SERIAL( CNetworkResourceInformation )

   private:

      void m_Initialize( void );

   public:

      CNetworkResourceInformation();
      CNetworkResourceInformation( const NETRESOURCE *source );
      CNetworkResourceInformation( const CNetworkResourceInformation& source );

      virtual ~CNetworkResourceInformation();

      /*
      ** Patterned after NETRESOURCE
      */

      CString   LocalName;
      CString   RemoteName;
      CString   Comment;
      CString   Provider;
      DWORD     Scope;
      DWORD     Type;
      DWORD     DisplayType;
      DWORD     Usage;

      virtual void Copy( const NETRESOURCE *source );
      virtual void Copy( const CNetworkResourceInformation& source );
      virtual void Empty( void );
      virtual void Serialize( CArchive& archive );
};

class CNetworkResources : public CNetwork
{
   DECLARE_DYNAMIC( CNetworkResources )

   private:

      void m_Initialize( void );

   protected:

      HANDLE m_ResumeHandle;

      NETRESOURCE m_NetResource;

   public:

      enum Scope {
                    scopeConnected  = RESOURCE_CONNECTED,
                    scopeAll        = RESOURCE_GLOBALNET,
                    scopePersistent = RESOURCE_REMEMBERED
                 };

      enum Type  {
                    typeAny   = RESOURCETYPE_ANY,
                    typeDisk  = RESOURCETYPE_DISK,
                    typePrint = RESOURCETYPE_PRINT
                 };

      enum Usage {
                    usageAll         = 0,
                    usageConnectable = RESOURCEUSAGE_CONNECTABLE,
                    usageContainer   = RESOURCEUSAGE_CONTAINER
                 };

      CNetworkResources();
      CNetworkResources( LPCTSTR machine_name );
      virtual ~CNetworkResources();

      virtual BOOL Enumerate( CNetworkResourceInformation& information );
      virtual BOOL GetNext( CNetworkResourceInformation& information );
};

#endif // NET_RESOURCE_CLASS_HEADER
