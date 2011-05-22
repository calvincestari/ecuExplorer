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
#define new DEBUG_NEW
#endif

CString CBubble::m_ClassName = "";

CFont CBubble::m_Font;

DWORD CBubble::m_Height = 0;

IMPLEMENT_DYNAMIC( CBubble, CWnd );

CBubble::CBubble()
{
   m_FontSize = (-8);
   m_Created  = FALSE;
}

CBubble::CBubble( int font_size )
{
   m_Created  = FALSE;
   m_FontSize = font_size;
}

CBubble::~CBubble()
{
}

BOOL CBubble::Create()
{
   if ( m_ClassName == "" )
   {
      CBrush brush;

      TRY
      {
         brush.CreateSolidBrush( LIGHT_YELLOW );
      }
      CATCH( CResourceException, e )
      {
         return( FALSE );
      }
      END_CATCH

      m_ClassName = ::AfxRegisterWndClass( CS_BYTEALIGNCLIENT | CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH) brush.Detach() );

      if ( m_ClassName == "" )
      {
         TRACE( "CBubble, AfxRegisterWndClass() failed\n" );
         return( FALSE );
      }
   }

   CRect rectangle;

   rectangle.SetRectEmpty();

   m_Created = CreateEx( 0,
                         m_ClassName,
                         "",
                         WS_POPUP | WS_BORDER,
                         rectangle.left,
                         rectangle.top,
                         rectangle.right,
                         rectangle.bottom,
                         NULL,
                 (HMENU) NULL );

   if ( m_Font.GetSafeHandle() == NULL )
   {
      m_SetFont();
   }

   return( m_Created );
}

BOOL CBubble::DestroyWindow()
{
   m_Created = FALSE;
   return( CWnd::DestroyWindow() );
}

DWORD CBubble::GetHeight( void ) const
{
   return( m_Height );
}

void CBubble::Hide( void )
{
   ShowWindow( SW_HIDE );
}

BOOL CBubble::IsCreated( void ) const
{
   return( m_Created );
}

void CBubble::m_SetFont( void )
{
   CClientDC device_context( this );

   TRY
   {
      LOGFONT font_attributes;

      ::ZeroMemory( &font_attributes, sizeof( font_attributes ) );

      font_attributes.lfHeight         = -::MulDiv( m_FontSize, device_context.GetDeviceCaps( LOGPIXELSY ), 72 );
      font_attributes.lfCharSet        = DEFAULT_CHARSET;
      font_attributes.lfQuality        = DEFAULT_QUALITY;
      font_attributes.lfClipPrecision  = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS;
      font_attributes.lfPitchAndFamily = FF_SWISS;

      m_Font.CreateFontIndirect( &font_attributes );
   }
   CATCH( CResourceException, e )
   {
      TRACE( "CBubble, font creation failed\n" );
      return;
   }
   END_CATCH

   CFont *old_font = device_context.SelectObject( &m_Font );

   TEXTMETRIC text_metrics;
   device_context.GetTextMetrics( &text_metrics );
   device_context.SelectObject( old_font );
   m_Height = text_metrics.tmHeight + text_metrics.tmExternalLeading + ( 6 * GetSystemMetrics( SM_CYBORDER ) );
}

void CBubble::SetFontSize( int font_size )
{
   m_FontSize = font_size;
}

void CBubble::Show( CPoint point, const CString& string )
{
   Hide();
   UpdateWindow();

   SetWindowText( string );

   CRect bubble;
   GetWindowRect( &bubble );
   CRect screen;

   ::GetWindowRect( ::GetDesktopWindow(), &screen );

   /*
   ** Let's make sure the window is always visible (on screen)
   */

   if ( point.y < 0 )
   {
      point.y = 0;
   }

   if ( ( point.y + bubble.Height() ) > screen.bottom )
   {
      point.y = screen.bottom - ( bubble.Height() + 1 );
   }

   if ( point.x < 0 )
   {
      point.x = abs( point.x ) - ( bubble.Width() / 2 );
   }

   if ( point.x > screen.right )
   {
      point.x = screen.right - ( bubble.Width() + 1 );
   }

   SetWindowPos( &wndTop, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE );

   ShowWindow( SW_SHOWNOACTIVATE );
}

BEGIN_MESSAGE_MAP( CBubble, CWnd )
   ON_WM_PAINT()
   ON_MESSAGE( WM_SETTEXT, OnSetText )
END_MESSAGE_MAP()

void CBubble::OnPaint()
{
   CPaintDC device_context( this );

   CRect rectangle;

   GetClientRect( rectangle );

   CFont *old_font = device_context.SelectObject( &m_Font );
   device_context.SetTextAlign( TA_CENTER );
   device_context.SetBkMode( TRANSPARENT );
   device_context.SetTextColor( BLACK );

   CString text;
   GetWindowText( text );
   CSize size = device_context.GetTextExtent( text, text.GetLength() );
   device_context.TextOut( rectangle.right / 2, ( rectangle.bottom - size.cy ) / 2, text );
   device_context.SelectObject( old_font );
}

afx_msg LONG CBubble::OnSetText( UINT, LONG lParam )
{
   CRect rectangle;
   GetWindowRect( rectangle );
   
   CClientDC device_context( this );

   CFont *old_font = device_context.SelectObject( &m_Font );
   CSize size = device_context.GetTextExtent( (LPCSTR) lParam, lstrlen( (LPCSTR) lParam ) );

   rectangle.right = rectangle.left + size.cx + ( 6 * GetSystemMetrics( SM_CXBORDER ) );

   rectangle.bottom = rectangle.top + m_Height;
   MoveWindow( &rectangle );
   device_context.SelectObject( old_font );

   return( CWnd::Default() );
}
