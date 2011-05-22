#if ! defined ( DRAWING_OBJECTS_CLASS_HEADER )

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

#define DRAWING_OBJECTS_CLASS_HEADER

class CRectangle : public CObject
{
   DECLARE_SERIAL( CRectangle )

   protected:

      void m_Initialize( void );
      void m_SetRectangles( void );

      DWORD m_Height;
      DWORD m_Width;

      COLORREF m_LineColor;
      COLORREF m_FillColor;

      CPoint m_Location;

      CRect m_LineRectangle;
      CRect m_FillRectangle;

   public:

      CRectangle();
      CRectangle( DWORD height, DWORD width, const CPoint& location, COLORREF fill_color = WHITE, COLORREF line_color = BLACK );

      virtual ~CRectangle();

      virtual void     Copy( const CRectangle& source );
      virtual void     Copy( const CRectangle *source_p );
      virtual void     Draw( CDC& device_context );
      virtual COLORREF GetFillColor( void ) const;
      virtual DWORD    GetHeight( void ) const;
      virtual COLORREF GetLineColor( void ) const;
      virtual void     GetRectangle( CRect& destination ) const;
      virtual DWORD    GetWidth( void ) const;
      virtual void     OnClick( void );
      virtual void     Serialize( CArchive& archive );
      virtual void     SetFillColor( COLORREF color );
      virtual void     SetHeight( DWORD height );
      virtual void     SetLineColor( COLORREF color );
      virtual void     SetLocation( const CPoint& size );
      virtual void     SetSize( const CSize& size );
      virtual void     SetWidth( DWORD width );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

class CRoundedRectangle : public CRectangle
{
   DECLARE_SERIAL( CRoundedRectangle )

   protected:

      CPoint m_RoundingPoint;

   public:

      CRoundedRectangle();

      virtual ~CRoundedRectangle();

      virtual void  Copy( const CRoundedRectangle& source );
      virtual void  Copy( const CRoundedRectangle * source_p );
      virtual void  Draw( CDC& device_context );
      virtual DWORD GetRoundingSize( void ) const;
      virtual void  Serialize( CArchive& archive );
      virtual void  SetRoundingSize( DWORD rounding_size );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

class CSquare : public CRectangle
{
   DECLARE_SERIAL( CSquare )

   public:

      CSquare();
      CSquare( DWORD size, const CPoint& location, COLORREF fill_color = WHITE, COLORREF line_color = BLACK );
     
      virtual ~CSquare();

      virtual void Serialize( CArchive& archive );
      virtual void SetHeight( DWORD height );
      virtual void SetSize( const CSize& size );
      virtual void SetWidth( DWORD width );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

class CEllipse : public CRectangle
{
   DECLARE_SERIAL( CEllipse )

   public:

      CEllipse();
      CEllipse( DWORD height, DWORD width, const CPoint& location, COLORREF fill_color = WHITE, COLORREF line_color = BLACK );
      
      virtual ~CEllipse();

      virtual void Draw( CDC& device_context );
      virtual void Serialize( CArchive& archive );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

class CCircle : public CSquare
{
   DECLARE_SERIAL( CCircle )

   public:

      CCircle();
      CCircle( DWORD size, const CPoint& location, COLORREF fill_color = WHITE, COLORREF line_color = BLACK );
      
      virtual ~CCircle();

      virtual void Draw( CDC& device_context );
      virtual void Serialize( CArchive& archive );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

#endif // DRAWING_OBJECTS_CLASS_HEADER
