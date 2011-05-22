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

IMPLEMENT_SERIAL( CSquiggleData, CObject, 1 )
IMPLEMENT_SERIAL( CSquiggle, CRectangle, 1 )

#if defined( _DEBUG )
#define new DEBUG_NEW
#endif

/*
** The data that will be squiggle-ized...
*/

CSquiggleData::CSquiggleData()
{
   m_Initialize();
}

CSquiggleData::~CSquiggleData()
{
   m_Initialize();
}

void CSquiggleData::Copy( const CSquiggleData *source_p )
{
   if ( source_p == (CSquiggleData *) NULL )
   {
      m_Initialize();
      return;
   }

   Start        = source_p->Start;
   Stop         = source_p->Stop;
   Time         = source_p->Time;
   MinimumValue = source_p->MinimumValue;
   MaximumValue = source_p->MaximumValue;

   /*
   ** MFC doesn't provide an equals (assignment) operator for their CWordArray class. Odd, they provide one for
   ** CString... Anyway, we've got to write a bunch of code that should have already been written
   */

   Data.RemoveAll();
   
   int number_of_elements = source_p->Data.GetSize();
   int index              = 0;
   
   Data.SetSize( number_of_elements );

   while( index < number_of_elements )
   {
      Data.Add( source_p->Data.GetAt( index ) );
      index++;
   }
}

void CSquiggleData::Empty( void )
{
   m_Initialize();
}

void CSquiggleData::m_Initialize( void )
{
   Start.Empty();
   Stop.Empty();
   Time = CTime( 0 );
   MinimumValue = 0;
   MaximumValue = 0;
   Data.RemoveAll();
}

void CSquiggleData::Serialize( CArchive& archive )
{
   CObject::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << Start;
      archive << Stop;
      archive << Time;
      archive << MinimumValue;
      archive << MaximumValue;
   }
   else
   {
      archive >> Start;
      archive >> Stop;
      archive >> Time;
      archive >> MinimumValue;
      archive >> MaximumValue;
   }

   Data.Serialize( archive );
}

WORD CSquiggleData::ValidRange( void )
{
   if ( MinimumValue > MaximumValue )
   {
      WORD temp_word = MaximumValue;
      MaximumValue = MinimumValue;
      MinimumValue = temp_word;
   }

   return( (WORD) ( MaximumValue - MinimumValue ) );
}

/*
** The Squiggle that you see on the screen
*/

CSquiggle::CSquiggle()
{
   m_Automatically_Delete = FALSE;
   m_PointArray           = (POINT *) NULL;
   m_Initialize();
}

CSquiggle::CSquiggle( DWORD height, DWORD width, const CPoint& location, COLORREF fill_color, COLORREF line_color )
          :CRectangle( height, width, location, fill_color, line_color )
{
   m_Automatically_Delete = FALSE;
   m_PointArray           = (POINT *) NULL;
   m_Initialize();
}

CSquiggle::~CSquiggle()
{
   if ( m_PointArray != (POINT *) NULL )
   {
      delete [] m_PointArray;
      m_PointArray = (POINT *) NULL;
   }

   if ( m_Automatically_Delete == TRUE )
   {
      delete m_Squiggle_Data_p;
   }

   m_Initialize();
}

void CSquiggle::Copy( const CSquiggle *source_p )
{
   m_Initialize();

   CSquiggleData *data_p = new CSquiggleData;

   data_p->Copy( source_p->m_Squiggle_Data_p );

   SetSquiggleData( data_p, TRUE );

   CRectangle::Copy( source_p );
}

void CSquiggle::Draw( CDC& device_context )
{
   if ( m_Squiggle_Data_p == (CSquiggleData *) NULL )
   {
      return;
   }

   CBitmap bitmap;

   CDC temporary_device_context;

   CBrush brush( m_FillColor );

   DWORD width  = GetWidth();
   DWORD height = GetHeight();

   BOOL result = bitmap.CreateCompatibleBitmap( &device_context, (int) width, (int) height );

   if ( result == 0 )
   {
      return;
   }

   result = temporary_device_context.CreateCompatibleDC( &device_context );

   if ( result == 0 )
   {
      return;
   }

   CBitmap *original_bitmap = temporary_device_context.SelectObject( &bitmap );

   temporary_device_context.FillRect( CRect( 0, 0, (int) width, (int) height ), &brush );

   /*
   ** Draw the grid if we need to
   */

   if ( m_NumberOfXGridLines > 0 || m_NumberOfYGridLines > 0 )
   {
      CPen grid_line_pen( (int) m_GridLineType, 1, m_GridLineColor );

      CPen *original_pen = temporary_device_context.SelectObject( &grid_line_pen );

      /*
      ** Now turn the background color into the same color as our FillRect to prevent
      ** dotted pens from being color-white-color...
      */

      COLORREF original_background_color = temporary_device_context.SetBkColor( m_FillColor );

      int index = 0;

      for( index = 1; index < m_NumberOfXGridLines; index++ )
      {
         temporary_device_context.MoveTo( (int) ( index * width / m_NumberOfXGridLines ), 0            );
         temporary_device_context.LineTo( (int) ( index * width / m_NumberOfXGridLines ), (int) height );
      }

      for( index = 1; index < m_NumberOfYGridLines; index++ )
      {
         temporary_device_context.MoveTo( 0,           (int) ( index * height / m_NumberOfYGridLines ) );
         temporary_device_context.LineTo( (int) width, (int) ( index * height / m_NumberOfYGridLines ) );
      }

      temporary_device_context.SetBkColor( original_background_color );
      temporary_device_context.SelectObject( original_pen );
      grid_line_pen.DeleteObject();
   }

   CPen line_pen( PS_SOLID, (int) m_LineThickness, m_LineColor );

   CPen *original_pen = temporary_device_context.SelectObject( &line_pen );

   temporary_device_context.Polyline( (LPPOINT) m_PointArray, m_NumberOfPoints );

   /*
   ** Copy to the screen
   */

   device_context.BitBlt( m_Location.x, m_Location.y, (int) width, (int) height, &temporary_device_context, 0, 0, SRCCOPY );

   /*
   ** Clean up
   */

   temporary_device_context.SelectObject( original_pen    );
   temporary_device_context.SelectObject( original_bitmap );
   line_pen.DeleteObject();
   bitmap.DeleteObject();
}

void CSquiggle::Empty( void )
{
   m_Initialize();
}

DWORD CSquiggle::GetLineThickness( void ) const
{
   return( m_LineThickness );
}

void CSquiggle::m_Initialize( void )
{
   m_Squiggle_Data_p     = (CSquiggleData *) NULL;
   m_PointArray          = (POINT *) NULL;
   m_NumberOfPoints      = 0;
   m_NumberOfXGridLines  = 0;
   m_NumberOfYGridLines  = 0;
   m_GridLineColor       = DARK_GREEN;
   m_LineThickness       = 1;
   m_GridLineType        = PS_DOT;
}

void CSquiggle::Serialize( CArchive& archive )
{
   CRectangle::Serialize( archive );

   if ( archive.IsStoring() )
   {
      archive << m_NumberOfXGridLines;
      archive << m_NumberOfYGridLines;
      archive << m_GridLineColor;
      archive << m_LineThickness;
      archive << m_GridLineType;

      DWORD do_we_have_data = 0;

      if ( m_Squiggle_Data_p != (CSquiggleData *) NULL )
      {
         do_we_have_data = 1;
      }

      archive << do_we_have_data;

      if ( do_we_have_data == 1 )
      {
         m_Squiggle_Data_p->Serialize( archive );
      }
   }
   else
   {
      archive >> m_NumberOfXGridLines;
      archive >> m_NumberOfYGridLines;
      archive >> m_GridLineColor;
      archive >> m_LineThickness;
      archive >> m_GridLineType;

      DWORD is_there_data = 0;

      archive >> is_there_data;

      if ( is_there_data == 1 )
      {
         CSquiggleData *data_p = new CSquiggleData;

         data_p->Serialize( archive );

         SetSquiggleData( data_p, TRUE );
      }
   }
}

void CSquiggle::SetGridLineColor( COLORREF grid_line_color )
{
   m_GridLineColor = grid_line_color;
}

void CSquiggle::SetGridLineType( DWORD type )
{
   m_GridLineType = type;
}

void CSquiggle::SetLineThickness( DWORD thickness )
{
   m_LineThickness = thickness;
}

void CSquiggle::SetNumberOfGridLines( WORD number_of_x_lines, WORD number_of_y_lines )
{
   m_NumberOfXGridLines = number_of_x_lines;
   m_NumberOfYGridLines = number_of_y_lines;
}

void CSquiggle::SetSquiggleData( CSquiggleData *source_p, BOOL auto_delete )
{
   if ( m_PointArray != (POINT *) NULL )
   {
      delete [] m_PointArray;
      m_PointArray = (POINT *) NULL;
   }

   m_NumberOfPoints = 0;

   if ( m_Automatically_Delete == TRUE )
   {
      delete m_Squiggle_Data_p;
      m_Squiggle_Data_p = (CSquiggleData *) NULL;
      m_Automatically_Delete = FALSE;
   }

   if ( source_p != (CSquiggleData *) NULL )
   {
      m_Squiggle_Data_p      = source_p;
      m_Automatically_Delete = auto_delete;

      m_NumberOfPoints = source_p->Data.GetSize();

      m_PointArray = new POINT[ m_NumberOfPoints ];

      if ( m_PointArray == (POINT *) NULL )
      {
         /*
         ** Out of Memory
         */

         m_NumberOfPoints = 0;
         return;
      }

      int number_of_data_points = m_NumberOfPoints;

      if ( number_of_data_points == 0 )
      {
         number_of_data_points = 1;
      }

      int valid_range = m_Squiggle_Data_p->ValidRange();

      if ( valid_range == 0 )
      {
         valid_range = 1;
      }

      double x_ratio = (double) GetWidth()  / (double) number_of_data_points;
      double y_ratio = (double) GetHeight() / (double) valid_range;

      int origin_y = (int) ( (double) m_Squiggle_Data_p->MaximumValue * y_ratio );

      for ( int index = 0; index < m_NumberOfPoints; index++ )
      {
         m_PointArray[ index ].x = (int) ( (double) index * x_ratio );
         m_PointArray[ index ].y = origin_y - (int) ( (double) m_Squiggle_Data_p->Data.GetAt( index ) * y_ratio );
      }
   }
}
