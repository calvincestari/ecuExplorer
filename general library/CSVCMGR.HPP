#if ! defined( SERVICE_MANAGER_CLASS_HEADER )

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

#define SERVICE_MANAGER_CLASS_HEADER

class CServiceNameAndStatusA : public _ENUM_SERVICE_STATUSA
{
   public:

      CServiceNameAndStatusA();
      virtual ~CServiceNameAndStatusA();
      virtual void Copy( const _ENUM_SERVICE_STATUSA *source );
      virtual void Empty( void );
};

class CServiceNameAndStatusW : public _ENUM_SERVICE_STATUSW
{
   public:

      CServiceNameAndStatusW();
      virtual ~CServiceNameAndStatusW();
      virtual void Copy( const _ENUM_SERVICE_STATUSW *source );
      virtual void Empty( void );
};

#if defined( UNICODE )
#define CServiceNameAndStatus CServiceNameAndStatusW
#else
#define CServiceNameAndStatus CServiceNameAndStatusA
#endif // UNICODE

class CServiceControlManager : public CObject
{
   private:

      void m_Initialize( void );

   protected:

      SC_HANDLE m_ManagerHandle;

      ENUM_SERVICE_STATUS *m_Buffer;

      DWORD m_BufferSize;
      DWORD m_CurrentEntryNumber;
      DWORD m_ErrorCode;
      DWORD m_NumberOfEntries;

      CString m_MachineName;

   public:

      CServiceControlManager();
      virtual ~CServiceControlManager();

      virtual void  Close( void );
      virtual BOOL  Continue( LPCTSTR service_name );
      virtual BOOL  EnumerateStatus( DWORD state = SERVICE_ACTIVE,
                                     DWORD type  = SERVICE_WIN32 );
      virtual DWORD GetErrorCode( void ) const;
      virtual BOOL  GetNext( CServiceNameAndStatus& status );
      virtual BOOL  Install( LPCTSTR service_name, LPCTSTR friendly_name, LPCTSTR name_of_executable_file, DWORD category_items = 0, DWORD start_type = SERVICE_DEMAND_START, LPCTSTR dependency_services = NULL);
      virtual BOOL  Open( DWORD   what_to_open  = SC_MANAGER_ALL_ACCESS,
                          LPCTSTR database_name = NULL,
                          LPCTSTR machine_name  = NULL );
      virtual BOOL  Pause( LPCTSTR service_name );
      virtual BOOL  Remove( LPCTSTR service_name );
      virtual BOOL  Start( LPCTSTR service_name, DWORD service_argc = 0, LPCTSTR *service_argv = NULL );
      virtual BOOL  Stop( LPCTSTR service_name );
};

#endif // SERVICE_MANAGER_CLASS_HEADER
