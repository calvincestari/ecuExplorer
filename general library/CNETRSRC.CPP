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

IMPLEMENT_SERIAL( CNetworkResourceInformation, CObject, 1 )
IMPLEMENT_DYNAMIC( CNetworkResources, CNetwork )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

/*
** CNetResourceInformation stuff
*/

CNetworkResourceInformation::CNetworkResourceInformation()
{
   m_Initialize();
}

CNetworkResourceInformation::CNetworkResourceInformation( const NETRESOURCE *source )
{
   Copy( source );
}

CNetworkResourceInformation::CNetworkResourceInformation( const CNetworkResourceInformation& source )
{
   Copy( source );
}

CNetworkResourceInformation::~CNetworkResourceInformation()
{
   m_Initialize();
}

void CNetworkResourceInformation::Copy( const NETRESOURCE *source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      m_Initialize();
      return;
   }

   if ( source->lpLocalName != (LPTSTR) NULL )
   {
      LocalName = source->lpLocalName;
   }
   else
   {
      LocalName.Empty();
   }

   if ( source->lpRemoteName != (LPTSTR) NULL )
   {
      RemoteName = source->lpRemoteName;
   }
   else
   {
      RemoteName.Empty();
   }

   if ( source->lpComment != (LPTSTR) NULL )
   {
      Comment = source->lpComment;
   }
   else
   {
      Comment.Empty();
   }

   if ( source->lpProvider != (LPTSTR) NULL )
   {
      Provider = source->lpProvider;
   }                               
   else
   {
      Provider.Empty();
   }

   Scope       = source->dwScope;
   Type        = source->dwType;
   DisplayType = source->dwDisplayType;
   Usage       = source->dwUsage;
}

void CNetworkResourceInformation::Copy( const CNetworkResourceInformation& source )
{
   ASSERT( this != &source );

   /*
   ** Make sure we ain't copying ourselves
   */

   if ( this == &source )
   {
      return;
   }

   LocalName   = source.LocalName;
   RemoteName  = source.RemoteName;
   Comment     = source.Comment;
   Provider    = source.Provider;
   Scope       = source.Scope;
   Type        = source.Type;
   DisplayType = source.DisplayType;
   Usage       = source.Usage;
}

void CNetworkResourceInformation::Empty( void )
{
   m_Initialize();
}

void CNetworkResourceInformation::m_Initialize( void )
{
   LocalName.Empty();
   RemoteName.Empty();
   Comment.Empty();
   Provider.Empty();
   Scope       = 0;
   Type        = 0;
   DisplayType = 0;
   Usage       = 0;
}

void CNetworkResourceInformation::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << LocalName;
      archive << RemoteName;
      archive << Comment;
      archive << Provider;
      archive << Scope;
      archive << Type;
      archive << DisplayType;
      archive << Usage;
   }
   else
   {
      archive >> LocalName;
      archive >> RemoteName;
      archive >> Comment;
      archive >> Provider;
      archive >> Scope;
      archive >> Type;
      archive >> DisplayType;
      archive >> Usage;
   }
}

/*
** CNetworkResources Stuff
*/

CNetworkResources::CNetworkResources()
{
   m_Initialize();
}

CNetworkResources::~CNetworkResources()
{
   if ( m_ResumeHandle != NULL )
   {
      ::WNetCloseEnum( m_ResumeHandle );
      m_ResumeHandle = NULL;
   }

   m_Initialize();
}

void CNetworkResources::m_Initialize( void )
{
   m_ErrorCode    = 0;
   m_ResumeHandle = (HANDLE) NULL;
   ::ZeroMemory( &m_NetResource, sizeof( m_NetResource ) );
}

BOOL CNetworkResources::Enumerate( CNetworkResourceInformation& information )
{
   if ( m_ResumeHandle != NULL )
   {
      ::WNetCloseEnum( m_ResumeHandle );
      m_ResumeHandle = NULL;
   }

   ::ZeroMemory( &m_NetResource, sizeof( m_NetResource ) );
   m_NetResource.dwUsage = usageContainer;

   NETRESOURCE *net_resource_parameter = (NETRESOURCE *) NULL;

   /*
   ** Let's see what we want to enumerate
   */

   switch( information.Scope )
   {
      case scopeConnected: // information.Usage is ignored

         break;
            
      case scopePersistent:  // information.Usage is ignored

         break;

      case scopeAll:

         break;

      default:

         m_ErrorCode = ERROR_INVALID_PARAMETER;
         return( FALSE );
   }

   switch( information.Usage )
   {
      case usageAll:

         net_resource_parameter = (NETRESOURCE *) NULL;
         break;

      case usageConnectable:
      case usageContainer:

         net_resource_parameter = &m_NetResource;
         break;

      default:

         m_ErrorCode = ERROR_INVALID_PARAMETER;
         return( FALSE );
   }

   m_ErrorCode = ::WNetOpenEnum( information.Scope, 
                                 information.Type, 
                                 information.Usage, 
                                 net_resource_parameter,
                                &m_ResumeHandle );

   if ( m_ErrorCode == NO_ERROR )
   {
      return( TRUE );
   }
   else if ( m_ErrorCode == ERROR_EXTENDED_ERROR )
   {
      m_ErrorCode = ::GetLastError();
   }

   return( FALSE );
}

BOOL CNetworkResources::GetNext( CNetworkResourceInformation& information )
{
   DWORD number_of_entries = 1;
   DWORD size_of_buffer    = sizeof( m_NetResource );

   m_ErrorCode = ::WNetEnumResource( m_ResumeHandle,
                                    &number_of_entries,
                                    &m_NetResource,
                                    &size_of_buffer );

   if ( m_ErrorCode == NO_ERROR )
   {
      information.Copy( &m_NetResource );
      return( TRUE );
   }
   else if ( m_ErrorCode == ERROR_EXTENDED_ERROR )
   {
      m_ErrorCode = ::GetLastError();
   }
                                        
   return( FALSE );
}
