#if ! defined ( BUBBLE_CLASS_HEADER )

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

#define BUBBLE_CLASS_HEADER

class CBubble : public CWnd
{
   DECLARE_DYNAMIC( CBubble );

   private:

      static CString m_ClassName;
      static CFont   m_Font;
      static DWORD   m_Height;

      int            m_FontSize;
      BOOL           m_Created;

      void m_SetFont( void );

   public:

      CBubble();
      CBubble( int font_size );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CBubble();

      virtual BOOL  Create( void );
      virtual BOOL  DestroyWindow( void );
      virtual DWORD GetHeight( void ) const;
      virtual void  Hide( void );
      virtual BOOL  IsCreated( void ) const;
      virtual void  SetFontSize( int font_size );
      virtual void  Show( CPoint screen_coordinates, const CString& string );

   protected:

      afx_msg void OnPaint( void );
      afx_msg LONG OnSetText( UINT, LONG lParam );

      DECLARE_MESSAGE_MAP()
};

#endif // BUBBLE_CLASS_HEADER
