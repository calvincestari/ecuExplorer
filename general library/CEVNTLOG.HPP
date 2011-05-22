#if ! defined( EVENT_LOG_CLASS )

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

#define EVENT_LOG_CLASS
#if defined Inc_CObject
class CEventLog : public CObject
#else
class CEventLog// : public CObject
#endif
{
#if defined Inc_CObject
   DECLARE_DYNAMIC( CEventLog )
#endif

   protected:

      HANDLE m_LogHandle;
      HANDLE m_EventSourceHandle;

      DWORD m_ErrorCode;
      DWORD m_NumberOfBytesRead;
      DWORD m_NumberOfBytesInNextRecord;

      void m_Initialize( void );

   public:

      enum EventType
      {
         eventError       = EVENTLOG_ERROR_TYPE,
         eventWarning     = EVENTLOG_WARNING_TYPE,
         eventInformation = EVENTLOG_INFORMATION_TYPE,
         eventSuccess     = EVENTLOG_AUDIT_SUCCESS,
         eventFailure     = EVENTLOG_AUDIT_FAILURE
      };

      CEventLog();
      CEventLog( LPCTSTR source_name );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CEventLog();

      /*
      ** Data
      */

      CString ComputerName;
      CString LogName;

      /*
      ** Methods
      */

      virtual BOOL  Backup( LPCTSTR name_of_backup_file );
      virtual BOOL  Clear(  LPCTSTR name_of_backup_file );
      virtual BOOL  Close( void );
      virtual BOOL  CreateApplicationLog( LPCTSTR application_name,
                                          LPCTSTR file_containing_message_table_resource,
                                          DWORD   supported_types,
										  DWORD category_count = 0);
      virtual BOOL  DeleteApplicationLog( LPCTSTR application_name );
      virtual BOOL  DeregisterSource( void );
      virtual DWORD GetErrorCode( void ) const;
      virtual BOOL  GetNumberOfRecords( DWORD& number_of_records );
      virtual BOOL  NotifyChange( HANDLE event_handle, HANDLE log_handle = NULL );
      virtual BOOL  OpenBackup( LPCTSTR name_of_backup_file, LPCTSTR name_of_computer = NULL );
      virtual BOOL  Open( LPCTSTR log_name, LPCTSTR name_of_computer = NULL );
      virtual BOOL  Read( DWORD  record_number, 
                          LPVOID buffer, 
                          DWORD& number_of_bytes_to_read, 
                          DWORD  how_to_read = EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ );
      virtual BOOL  RegisterSource( LPCTSTR source_name, LPCTSTR name_of_computer = NULL );
      virtual BOOL  Report( EventType event_type,
                            WORD      category,
                            DWORD     event_id,
                            WORD      number_of_strings        = 0,
                            LPCTSTR * string_array             = NULL,
                            DWORD     number_of_raw_data_bytes = 0,
                            LPVOID    raw_data_buffer          = NULL,
                            PSID      user_sid                 = NULL );
      virtual BOOL  Report( LPCTSTR   log_name, 
                            DWORD     message_string_resource_id,
                            WORD      number_of_strings = 0,
                            LPCTSTR * string_array      = NULL );
      virtual void  ReportError( LPCTSTR string_to_report );
      virtual void  ReportInformation( LPCTSTR string_to_report );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

#endif // EVENT_LOG_CLASS
