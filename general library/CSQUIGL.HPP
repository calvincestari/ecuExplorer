#if ! defined( SQUIGGLE_CLASS_HEADER )

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

#define SQUIGGLE_CLASS_HEADER

class CSquiggleData : public CObject
{
   DECLARE_SERIAL( CSquiggleData )

   private:

      void m_Initialize( void );

   public:

      CSquiggleData();
      CSquiggleData( const CSquiggleData *source_p );

      /*
      ** Data Members
      */

      CString    Start;
      CString    Stop;
      CTime      Time;
      WORD       MinimumValue;
      WORD       MaximumValue;
      CWordArray Data;

      virtual ~CSquiggleData();

      virtual void Copy( const CSquiggleData * source_p );
      virtual void Empty( void );
      virtual void Serialize( CArchive& archive );
      virtual WORD ValidRange( void );
};

class CSquiggle : public CRectangle
{
   DECLARE_SERIAL( CSquiggle )

   private:

      void m_Initialize( void );

   protected:

      BOOL m_Automatically_Delete;

      CSquiggleData *m_Squiggle_Data_p;

      int m_NumberOfPoints;

      WORD m_NumberOfXGridLines;
      WORD m_NumberOfYGridLines;

      DWORD m_LineThickness;
      DWORD m_GridLineType;

      COLORREF m_GridLineColor;

      POINT *m_PointArray;

   public:

      CSquiggle();
      CSquiggle( DWORD height, DWORD width, const CPoint& location, COLORREF fill_color, COLORREF line_color );

      virtual      ~CSquiggle();
      virtual void  Copy( const CSquiggle *source_p );
      virtual void  Draw( CDC& device_context );
      virtual void  Empty( void );
      virtual DWORD GetLineThickness( void ) const;
      virtual void  Serialize( CArchive& archive );
      virtual void  SetGridLineColor( COLORREF grid_line_color );
      virtual void  SetGridLineType( DWORD type );
      virtual void  SetLineThickness( DWORD thickness );
      virtual void  SetNumberOfGridLines( WORD number_of_x_lines, WORD number_of_y_lines );
      virtual void  SetSquiggleData( CSquiggleData * source_p, BOOL auto_delete = FALSE );
};

#endif // SQUIGGLE_CLASS_HEADER
