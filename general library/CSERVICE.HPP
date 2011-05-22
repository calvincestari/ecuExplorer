#if ! defined( SERVICE_CLASS_HEADER )

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

#define SERVICE_CLASS_HEADER

extern CRITICAL_SECTION g_ServiceCriticalSection;

#define SERVICE_NAME_LEN   256
#define ACCEPT_FLAGS ( SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN )

class CService : public CObject
{
   private:

      void m_Initialize( void );

   public:

      static CService *m_Service_p;

      CService( LPTHREAD_START_ROUTINE pThread, DWORD dwControlsAccepted = ACCEPT_FLAGS, DWORD dwWaitHint = 5000 );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CService( void );

      virtual BOOL Initialize( LPCTSTR pName );
      void LogEvent( WORD EventType = EVENTLOG_ERROR_TYPE, LPTSTR pMsgStr = 0, DWORD dwError = NO_ERROR );

   protected:

      HANDLE m_ExitEventHandle;
      HANDLE m_ThreadHandle;

      DWORD m_ControlsAccepted;
      DWORD m_CurrentState;
      DWORD m_ErrorCode;
      DWORD m_ThreadId;
      DWORD m_WaitHint;

      LPTHREAD_START_ROUTINE m_ThreadStartRoutine;

      SERVICE_STATUS_HANDLE m_ServiceStatusHandle;

      BOOL m_Debugging;
      BOOL m_Running;
      BOOL m_Paused;
      BOOL m_Exiting;

      SERVICE_TABLE_ENTRY m_ServiceTable[ 2 ];

      char m_ServiceName[ SERVICE_NAME_LEN + 1 ];

      static void CALLBACK ServiceControlManagerHandler( DWORD dwControlCode );
      static void CALLBACK ServiceMain( DWORD Argc, LPTSTR *argv );

      virtual void AssertValid( void ) const;
      virtual void ParseCommandLineParameters( DWORD Argc, LPTSTR *Argv );
      virtual void OnControlCode( DWORD dwControlCode );
      virtual void OnStop( void );
      virtual void OnPrepareServiceThread( void );
      virtual void OnPause( void );
      virtual void OnContinue( void );

      void Exit( void );

      BOOL SendStatusToServiceControlManager( DWORD CurrentState,
                                              DWORD Win32ExitCode = NO_ERROR,
                                              DWORD CheckPoint = 0, 
                                              DWORD WaitHint = 0,
                                              DWORD ServiceSpecificCode = NO_ERROR );

#if defined ( _DEBUG )
   void DumpStatus( SERVICE_STATUS *pStatus ) const;
#endif

private:

   // not-implemented:
//   CNtService& operator= (const CNtService& rhs);  // assignment operator
  // CNtService (const CNtService& rhs);             // copy constructor
   //CNtService* operator& (void);                   // address-of operators
   //const CNtService* operator& (void) const;
};

#endif // SERVICE_CLASS_HEADER
