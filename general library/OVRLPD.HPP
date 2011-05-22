#if ! defined( OVERLAPPED_CLASS_HEADER )

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

#define OVERLAPPED_CLASS_HEADER

class COverlapped
{
   protected:

      OVERLAPPED m_Overlapped;

   public:

      COverlapped();
      COverlapped( const OVERLAPPED * source );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~COverlapped();

      /*
      ** Methods
      */

      inline HANDLE GetEvent( void )                { return( m_Overlapped.hEvent ); }
      inline DWORD  GetOffset( void )               { return( m_Overlapped.Offset ); }
      inline void   IncrementOffset( DWORD amount ) { m_Overlapped.Offset += amount; }
      inline void   SetEvent( HANDLE event_handle );
      inline void   SetOffset( DWORD new_offset )   { m_Overlapped.Offset = new_offset; }

      /*
      ** Operators
      */

      inline operator OVERLAPPED *() { return( &m_Overlapped ); }
};

#endif // OVERLAPPED_CLASS_HEADER
