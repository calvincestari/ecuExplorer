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

IMPLEMENT_SERIAL( CRectangle,        CObject,    1 )
IMPLEMENT_SERIAL( CRoundedRectangle, CRectangle, 1 )
IMPLEMENT_SERIAL( CSquare,           CRectangle, 1 )
IMPLEMENT_SERIAL( CEllipse,          CRectangle, 1 )
IMPLEMENT_SERIAL( CCircle,           CSquare,    1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

/*
** A Rectangle drawing object
*/

CRectangle::CRectangle()
{
   m_Initialize();
}

CRectangle::CRectangle( DWORD height, DWORD width, const CPoint& location, COLORREF fill_color, COLORREF line_color )
{
   m_Initialize();

   SetHeight( height );
   SetWidth( width );
   SetLocation( location );
   SetFillColor( fill_color );
   SetLineColor( line_color );
}

CRectangle::~CRectangle()
{
   SetLocation( CPoint( 0, 0 ) );
}

void CRectangle::Copy( const CRectangle& source )
{
   Copy( &source );
}

void CRectangle::Copy( const CRectangle * source_p )
{
   m_Height        = source_p->m_Height;
   m_Width         = source_p->m_Width;
   m_LineColor     = source_p->m_LineColor;
   m_FillColor     = source_p->m_FillColor;
   m_Location      = source_p->m_Location;
   m_LineRectangle = source_p->m_LineRectangle;
   m_FillRectangle = source_p->m_FillRectangle;
}

void CRectangle::Draw( CDC& device_context )
{
   device_context.FrameRect( m_LineRectangle, &CBrush( m_LineColor ) );
   device_context.FillRect(  m_FillRectangle, &CBrush( m_FillColor ) );
}

#if defined( _DEBUG )

void CRectangle::Dump( CDumpContext& dump_context ) const
{
   CObject::Dump( dump_context );

   dump_context << "m_Height = "        << m_Height        << "\n";
   dump_context << "m_Width = "         << m_Width         << "\n";
   dump_context << "m_LineColor = RGB( " << GetRValue( m_LineColor ) << ", " << GetGValue( m_LineColor ) << ", " << GetBValue( m_LineColor ) << " )\n";
   dump_context << "m_FillColor = RGB( " << GetRValue( m_FillColor ) << ", " << GetGValue( m_FillColor ) << ", " << GetBValue( m_FillColor ) << " )\n";
   dump_context << "m_Location = "      << m_Location      << "\n";
   dump_context << "m_LineRectangle = " << m_LineRectangle << "\n";
   dump_context << "m_FillRectangle = " << m_FillRectangle << "\n";
}

#endif // _DEBUG

COLORREF CRectangle::GetFillColor( void ) const
{
   return( m_FillColor );
}

DWORD CRectangle::GetHeight( void ) const
{
   return( m_Height );
}

COLORREF CRectangle::GetLineColor( void ) const
{
   return( m_LineColor );
}

void CRectangle::GetRectangle( CRect& destination ) const
{
   destination = m_LineRectangle;
}

DWORD CRectangle::GetWidth( void ) const
{
   return( m_Width );
}

void CRectangle::m_Initialize( void )
{
   m_Location.x = 0;
   m_Location.y = 0;
   m_LineRectangle.SetRectEmpty();
   m_FillRectangle.SetRectEmpty();
   SetFillColor( WHITE );
   SetLineColor( BLACK );
}

void CRectangle::m_SetRectangles( void )
{
   m_LineRectangle = CRect( m_Location.x, m_Location.y, m_Location.x + m_Width, m_Location.y + m_Height );
   m_FillRectangle = m_LineRectangle;
   m_FillRectangle.InflateRect( -1, -1 );
}

void CRectangle::OnClick( void )
{
   return;
}

void CRectangle::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << m_Height;
      archive << m_Width;
      archive << m_LineColor;
      archive << m_FillColor;
      archive << m_Location;
   }
   else
   {
      archive >> m_Height;
      archive >> m_Width;
      archive >> m_LineColor;
      archive >> m_FillColor;
      archive >> m_Location;

      SetHeight( m_Height );
      SetWidth( m_Width );
      SetLineColor( m_LineColor );
      SetFillColor( m_FillColor );
      SetLocation( m_Location );
   }
}

void CRectangle::SetFillColor( COLORREF color )
{
   m_FillColor = color;
}

void CRectangle::SetHeight( DWORD height )
{
   m_Height = height;
   m_SetRectangles();
}

void CRectangle::SetLineColor( COLORREF color )
{
   m_LineColor = color;
}

void CRectangle::SetLocation( const CPoint& source )
{
   m_Location = source;
   m_SetRectangles();
}

void CRectangle::SetSize( const CSize& size )
{
   SetWidth(  size.cx );
   SetHeight( size.cy );
   m_SetRectangles();
}

void CRectangle::SetWidth( DWORD width )
{
   m_Width = width;
   m_SetRectangles();
}

/*
** Rounded Rectangle object
*/

CRoundedRectangle::CRoundedRectangle()
{
}

CRoundedRectangle::~CRoundedRectangle()
{
}

void CRoundedRectangle::Copy( const CRoundedRectangle& source )
{
   Copy( &source );
}

void CRoundedRectangle::Copy( const CRoundedRectangle * source_p )
{
   CRectangle::Copy( source_p );

   m_RoundingPoint = source_p->m_RoundingPoint;
}

void CRoundedRectangle::Draw( CDC& device_context )
{
   device_context.RoundRect( &m_LineRectangle, m_RoundingPoint );
}

#if defined( _DEBUG )

void CRoundedRectangle::Dump( CDumpContext& dump_context ) const
{
   CRectangle::Dump( dump_context );

   dump_context << "m_RoundingPoint = " << m_RoundingPoint << "\n";
}

#endif // _DEBUG

DWORD CRoundedRectangle::GetRoundingSize( void ) const
{
   return( m_RoundingPoint.x );
}

void CRoundedRectangle::Serialize( CArchive& archive )
{
   CRectangle::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << m_RoundingPoint.x;
   }
   else
   {
      archive >> m_RoundingPoint.x;

      SetRoundingSize( m_RoundingPoint.x );
   }
}

void CRoundedRectangle::SetRoundingSize( DWORD rounding_size )
{
   m_RoundingPoint.x = rounding_size;
   m_RoundingPoint.y = rounding_size;
}

/*
** A Square object
*/

CSquare::CSquare()
{
   m_Initialize();

   SetHeight( 10 );
}

CSquare::CSquare( DWORD size, const CPoint& location, COLORREF fill_color, COLORREF line_color )
{
   m_Initialize();

   SetHeight( size );
   SetLocation( location );
   SetFillColor( fill_color );
   SetLineColor( line_color );
}

CSquare::~CSquare()
{
   SetLocation( CPoint( 0, 0 ) );
}

#if defined( _DEBUG )

void CSquare::Dump( CDumpContext& dump_context ) const
{
   CRectangle::Dump( dump_context );
}

#endif // _DEBUG

void CSquare::Serialize( CArchive& archive )
{
   CRectangle::Serialize( archive );
}

void CSquare::SetHeight( DWORD height )
{
   m_Height = height;
   m_Width  = height;
   m_SetRectangles();
}

void CSquare::SetSize( const CSize& size )
{
   m_Height = size.cx;
   m_Width  = size.cx;
   m_SetRectangles();
}

void CSquare::SetWidth( DWORD width )
{
   m_Width  = width;
   m_Height = width;
   m_SetRectangles();
}

/*
** An ellipse...
*/

CEllipse::CEllipse()
{
   m_Initialize();
}

CEllipse::CEllipse( DWORD height, DWORD width, const CPoint& location, COLORREF fill_color, COLORREF line_color )
{
   m_Initialize();

   SetHeight( height );
   SetWidth( width );
   SetLocation( location );
   SetFillColor( fill_color );
   SetLineColor( line_color );
}

CEllipse::~CEllipse()
{
   SetLocation( CPoint( 0, 0 ) );
}

void CEllipse::Draw( CDC& device_context )
{
   CPen line_pen( PS_SOLID, 1, m_LineColor );

   CBrush fill_brush( m_FillColor );

   CPen   *old_pen   = device_context.SelectObject( &line_pen );
   CBrush *old_brush = device_context.SelectObject( &fill_brush );

   device_context.Ellipse( m_LineRectangle );

   device_context.SelectObject( old_brush );
   device_context.SelectObject( old_pen );

   fill_brush.DeleteObject();
   line_pen.DeleteObject();
}

#if defined( _DEBUG )

void CEllipse::Dump( CDumpContext& dump_context ) const
{
   CRectangle::Dump( dump_context );
}

#endif // _DEBUG

void CEllipse::Serialize( CArchive& archive )
{
   CRectangle::Serialize( archive );

   if ( archive.IsStoring() != TRUE )
   {
      SetLineColor( m_LineColor );
   }
}

/*
** A Circle
*/

CCircle::CCircle()
{
   m_Initialize();

   SetHeight( 10 );
}

CCircle::CCircle( DWORD size, const CPoint& location, COLORREF fill_color, COLORREF line_color )
{
   m_Initialize();

   SetHeight( size );
   SetLocation( location );
   SetFillColor( fill_color );
   SetLineColor( line_color );
}

CCircle::~CCircle()
{
   SetLocation( CPoint( 0, 0 ) );
}

void CCircle::Draw( CDC& device_context )
{
   CPen line_pen( PS_SOLID, 1, m_LineColor );

   CBrush fill_brush( m_FillColor );

   CPen   *old_pen   = device_context.SelectObject( &line_pen   );
   CBrush *old_brush = device_context.SelectObject( &fill_brush );

   device_context.Ellipse( m_LineRectangle );

   device_context.SelectObject( old_brush );
   device_context.SelectObject( old_pen );

   fill_brush.DeleteObject();
   line_pen.DeleteObject();
}

#if defined( _DEBUG )

void CCircle::Dump( CDumpContext& dump_context ) const
{
   CSquare::Dump( dump_context );
}

#endif // _DEBUG

void CCircle::Serialize( CArchive& archive )
{
   CSquare::Serialize( archive );

   if ( archive.IsStoring() != TRUE )
   {
      SetLineColor( m_LineColor );
   }
}
