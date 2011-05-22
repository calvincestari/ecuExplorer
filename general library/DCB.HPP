#if ! defined( DCB_LASS_HEADER )

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

#define DCB_CLASS_HEADER

class CDeviceControlBlock : public _DCB
{
   public:

      CDeviceControlBlock();
      CDeviceControlBlock( const DCB * source );
      CDeviceControlBlock( const CDeviceControlBlock& source );

      /*
      ** Destructor should be virtual according to MSJ article in Sept 1992
      ** "Do More with Less Code:..."
      */

      virtual ~CDeviceControlBlock();

      /*
      ** Methods
      */

      virtual void Copy( const CDeviceControlBlock& source );
      virtual void Copy( const DCB *source );

#if defined( _DEBUG )

      virtual void Dump( CDumpContext& dump_context ) const;

#endif // _DEBUG
};

#endif // DCB_CLASS_HEADER
