#if ! defined( REGISTRY_CLASS_HEADER )

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

#define REGISTRY_CLASS_HEADER

#define THIS_SUB_KEY FALSE
#define ALL_SUB_KEYS TRUE

#define SIGNAL_EVENT    TRUE
#define WAIT_FOR_CHANGE FALSE
#if defined Inc_CObject
class CRegistry : public CObject
#else
class CRegistry// : public CObject
#endif
{
#if defined Inc_CObject
   DECLARE_DYNAMIC( CRegistry )
#endif

   private:

      void m_Initialize( void );

   protected:

      HKEY m_KeyHandle;
      HKEY m_RegistryHandle;

      LONG m_ErrorCode;

      CString m_ClassName;
      CString m_ComputerName;
      CString m_KeyName;
      CString m_RegistryName;
	  CString m_sSubKeyName;	// RB 10/09/98
	  CString m_sFileName;		// RB 10/09/98
	  int  m_fpLogFile;		// RB 10/09/98
      DWORD   m_NumberOfSubkeys;
      DWORD   m_NumberOfValues;

      /*
      ** Data items filled in by QueryInfo
      */

      DWORD    m_LongestSubkeyNameLength;
      DWORD    m_LongestClassNameLength;
      DWORD    m_LongestValueNameLength;
      DWORD    m_LongestValueDataLength;
      DWORD    m_SecurityDescriptorLength;
      FILETIME m_LastWriteTime;

   public:
	  // RB - Removed static const HKEY declaration 20 Oct 96
      static HKEY keyLocalMachine;//= HKEY_LOCAL_MACHINE;
      static HKEY keyClassesRoot;//= HKEY_CLASSES_ROOT;
      static HKEY keyUsers;//= HKEY_USERS;
      static HKEY keyCurrentUser;//= HKEY_CURRENT_USER;

      enum KeyValueTypes
      {
         typeBinary                 = REG_BINARY,
         typeDoubleWord             = REG_DWORD,
         typeDoubleWordLittleEndian = REG_DWORD_LITTLE_ENDIAN,
         typeDoubleWordBigEndian    = REG_DWORD_BIG_ENDIAN,
         typeUnexpandedString       = REG_EXPAND_SZ,
         typeSymbolicLink           = REG_LINK,
         typeMultipleString         = REG_MULTI_SZ,
         typeNone                   = REG_NONE,
         typeResourceList           = REG_RESOURCE_LIST,
         typeString                 = REG_SZ
      };

      enum CreateOptions
      {
         optionsNonVolatile = REG_OPTION_NON_VOLATILE,
         optionsVolatile    = REG_OPTION_VOLATILE
      };

      enum CreatePermissions
      {
         permissionAllAccess        = KEY_ALL_ACCESS,
         permissionCreateLink       = KEY_CREATE_LINK,
         permissionCreateSubKey     = KEY_CREATE_SUB_KEY,
         permissionEnumerateSubKeys = KEY_ENUMERATE_SUB_KEYS,
         permissionExecute          = KEY_EXECUTE,
         permissionNotify           = KEY_NOTIFY,
         permissionQueryValue       = KEY_QUERY_VALUE,
         permissionRead             = KEY_READ,
         permissionSetValue         = KEY_SET_VALUE,
         permissionWrite            = KEY_WRITE
      };

      enum CreationDisposition
      {
         dispositionCreatedNewKey     = REG_CREATED_NEW_KEY,
         dispositionOpenedExistingKey = REG_OPENED_EXISTING_KEY
      };

      enum NotifyChangeFlag
      {
         changeKeyAndSubkeys    = TRUE,
         changeSpecifiedKeyOnly = FALSE
      };

      enum NotifyChangeFilter
      {
         notifyName       = REG_NOTIFY_CHANGE_NAME,
         notifyAttributes = REG_NOTIFY_CHANGE_ATTRIBUTES,
         notifyLastSet    = REG_NOTIFY_CHANGE_LAST_SET,
         notifySecurity   = REG_NOTIFY_CHANGE_SECURITY
      };

      CRegistry();

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CRegistry();

      /*
      ** Methods
      */

      virtual BOOL Close( void );
      virtual BOOL Connect( HKEY    key_to_open   = HKEY_CURRENT_USER,
                            LPCTSTR computer_name = NULL );
      virtual BOOL Create( LPCTSTR               name_of_subkey,
                           LPCTSTR               name_of_class         = NULL,
                           CreateOptions         options               = optionsNonVolatile,
                           CreatePermissions     permissions           = permissionAllAccess,
                           LPSECURITY_ATTRIBUTES security_attributes_p = NULL,
                           CreationDisposition * disposition_p         = NULL );
      virtual BOOL DeleteKey( LPCTSTR name_of_subkey_to_delete );
      virtual BOOL DeleteValue( LPCTSTR name_of_value_to_delete );
      virtual BOOL EnumerateKeys( const DWORD subkey_index,
                                  CString&    subkey_name,
                                  CString&    class_name );
      virtual BOOL EnumerateValues( const DWORD    value_index,
                                    CString&       name_of_value,
                                    KeyValueTypes& type_code,
                                    LPBYTE         data_buffer,
                                    DWORD&         size_of_data_buffer );
      virtual BOOL Flush( void );
      virtual BOOL GetBinaryValue( LPCTSTR name_of_value, CByteArray& return_array );
      virtual void GetClassName( CString& class_name ) const;
      virtual void GetComputerName( CString& computer_name ) const;
      virtual BOOL GetDoubleWordValue( LPCTSTR name_of_value, DWORD& return_value );
      virtual BOOL GetErrorCode( void ) const;
      virtual void GetKeyName( CString& key_name ) const;
      virtual DWORD GetNumberOfSubkeys( void ) const;
      virtual DWORD GetNumberOfValues( void ) const;
      virtual void GetRegistryName( CString& registry_name ) const;
      virtual BOOL GetSecurity( const SECURITY_INFORMATION what_you_want_to_know,
                                PSECURITY_DESCRIPTOR       data_buffer,
                                DWORD&                     size_of_data_buffer );
      virtual BOOL GetStringValue( LPCTSTR name_of_value, CString& return_string );
      virtual BOOL GetStringArrayValue( LPCTSTR name_of_value, CStringArray& return_array );
      virtual BOOL GetValue( LPCTSTR name_of_value, CByteArray& return_array );
      virtual BOOL GetValue( LPCTSTR name_of_value, DWORD& return_value );
      virtual BOOL GetValue( LPCTSTR name_of_value, CString& return_string );
      virtual BOOL GetValue( LPCTSTR name_of_value, CStringArray& return_array );
      virtual BOOL Load( LPCTSTR name_of_subkey,
                         LPCTSTR name_of_file_containg_information );
      virtual BOOL NotifyChange( const HANDLE event_handle                       = NULL,
                                 const NotifyChangeFilter changes_to_be_reported = notifyLastSet,
                                 const BOOL this_subkey_or_all_subkeys           = changeSpecifiedKeyOnly,
                                 const BOOL wait_for_change_or_signal_event      = WAIT_FOR_CHANGE );
      virtual BOOL Open( LPCTSTR name_of_subkey_to_open,
                         const CreatePermissions security_access_mask = permissionAllAccess );
      virtual BOOL QueryInfo( void );
      virtual BOOL QueryValue( LPCTSTR        name_of_value,
                               KeyValueTypes& value_type,
                               LPBYTE         address_of_buffer,
                               DWORD&         size_of_buffer );
      virtual BOOL Replace( LPCTSTR name_of_subkey,
                            LPCTSTR name_of_file_with_new_data,
                            LPCTSTR name_of_backup_file );
      virtual BOOL Restore( LPCTSTR name_of_file_holding_saved_tree, const DWORD volitility_flags = NULL );
      virtual BOOL Save( LPCTSTR name_of_file_to_hold_tree, LPSECURITY_ATTRIBUTES security_attributes_p = NULL );
      virtual BOOL SetBinaryValue( LPCTSTR name_of_value, const CByteArray& bytes_to_write );
      virtual BOOL SetDoubleWordValue( LPCTSTR name_of_value, DWORD value_to_write );
      virtual BOOL SetSecurity( const SECURITY_INFORMATION& security_information,
                                const PSECURITY_DESCRIPTOR security_descriptor_p );
      virtual BOOL SetStringValue( LPCTSTR name_of_value, const CString& string_value );
      virtual BOOL SetStringArrayValue( LPCTSTR name_of_value, const CStringArray& string_array );
      virtual BOOL SetValue( LPCTSTR name_of_value, const CByteArray& bytes_to_write );
      virtual BOOL SetValue( LPCTSTR name_of_value, DWORD value );
      virtual BOOL SetValue( LPCTSTR name_of_value, const CStringArray& strings_to_write );
      virtual BOOL SetValue( LPCTSTR name_of_value, const CString& string_to_write );

      virtual BOOL SetValue( LPCTSTR             name_of_subkey,
                             const KeyValueTypes type_of_value_to_set,
                             CONST PBYTE         address_of_value_data,
                             const DWORD         size_of_data );

      virtual BOOL UnLoad( LPCTSTR name_of_subkey_to_unload );
      virtual BOOL OpenWithCreate(LPCTSTR name_of_subkey_to_open);
	  virtual BOOL OpenWithCreate(LPCTSTR name_of_subkey_to_open, LPCTSTR FileName);
      virtual BOOL RegReadWithCreate(int * piMember, LPCTSTR szDefaultValue, LPCTSTR szKey);
      virtual BOOL RegReadWithCreate(unsigned long * piMember, DWORD dwDefaultValue, LPCTSTR szKey);
      virtual BOOL RegReadWithCreate(unsigned short * piMember, LPCTSTR szDefaultValue, LPCTSTR szKey); 
      virtual BOOL RegReadWithCreate(unsigned int * piMember, LPCTSTR szDefaultValue, LPCTSTR szKey); 
      virtual BOOL RegReadWithCreate(CString * psMember, LPCTSTR szDefaultValue, LPCTSTR szKey);
	  virtual BOOL RegReadWithCreateFileDefault(int * piMember, LPCTSTR szDefaultValue, LPCTSTR szKey);
	  virtual void CRegistry :: GetFileDefault( char * szDefaultValue, char * szKey);


#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

#endif // REGISTRY_CLASS_HEADER
