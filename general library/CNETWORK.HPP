#if ! defined( NETWORK_CLASS_HEADER )

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

#define NETWORK_CLASS_HEADER

class CNetwork : public CObject
{
   DECLARE_SERIAL( CNetwork )

   protected:

      LPWSTR m_WideMachineName;

      CString m_MachineName;

      DWORD m_ErrorCode;

   public:

      CNetwork( LPCTSTR machine_name = NULL );
      virtual ~CNetwork();

      virtual void    Close( void );
      virtual DWORD   GetErrorCode( void ) const;
      virtual LPCTSTR GetMachineName( void );
      virtual void    Open( LPCTSTR machine_name = NULL );
      virtual void    Serialize( CArchive& archive );
      operator LPCWSTR() const { return( m_WideMachineName ); }
      operator LPWSTR()  const { return( m_WideMachineName ); }

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

#endif // NETWORK_CLASS_HEADER
