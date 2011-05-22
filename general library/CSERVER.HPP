#if ! defined( SERVER_CLASS_HEADER )

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

#define SERVER_CLASS_HEADER

class CServer : public CNetwork
{
   DECLARE_SERIAL( CServer )

   private:

      void m_Initialize( void );

   protected:

      CString m_Comment;
      CString m_UserPath;
      CString m_Domain;

      BOOL m_Retrieved102;
      BOOL m_Retrieved503;
      BOOL m_Retrieved599;

      DWORD m_MajorVersion;
      DWORD m_MinorVersion;
      DWORD m_NumberOfUsers;
      DWORD m_NumberOfOpens;
      DWORD m_Platform;
      DWORD m_Type;
      DWORD m_Users;

      virtual void m_Get_102_Data( void );
      virtual void m_Get_503_Data( void );
      virtual void m_Get_599_Data( void );

   public:

      CServer();
      CServer( LPCTSTR machine_name );
      virtual ~CServer();

      virtual void  Close( void );
      virtual void  GetComment( CString& comment );
      virtual void  GetDomain( CString& domain );
      virtual void  GetName( CString& name );
      virtual DWORD GetMajorVersion( void );
      virtual DWORD GetMinorVersion( void );
      virtual DWORD GetNumberOfOpens( void );
      virtual DWORD GetNumberOfUsers( void );
      virtual DWORD GetPlatform( void );
      virtual void  GetPlatformName( CString& name );
      virtual void  GetPlatformNameAndVersion( CString& name );
      virtual DWORD GetType( void );
      virtual void  GetUserPath( CString& path );
      virtual DWORD GetUsers( void );
      virtual void  Serialize( CArchive& archive );
};

#endif // SERVER_CLASS_HEADER
