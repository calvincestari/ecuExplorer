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

CDeviceControlBlock::CDeviceControlBlock()
{
   ::ZeroMemory( (DCB *) this, sizeof( DCB ) );
   DCBlength = sizeof( DCB );
   fBinary = TRUE; // Always TRUE for NT
}

CDeviceControlBlock::CDeviceControlBlock( const DCB * source )
{
   DCBlength = sizeof( DCB );

   Copy( source );
}

CDeviceControlBlock::CDeviceControlBlock( const CDeviceControlBlock& source )
{
   DCBlength = sizeof( DCB );

   Copy( source );
}

CDeviceControlBlock::~CDeviceControlBlock()
{
   DCBlength = 0;
}

void CDeviceControlBlock::Copy( const CDeviceControlBlock& source )
{
   Copy( (DCB *) &source );
}

void CDeviceControlBlock::Copy( const DCB* source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      ::ZeroMemory( (DCB *) this, sizeof( DCB ) );
      DCBlength = sizeof( DCB );
      fBinary = TRUE; // Always TRUE for NT
      return;
   }

   BaudRate          = source->BaudRate;
   fBinary           = source->fBinary;
   fParity           = source->fParity;
   fOutxCtsFlow      = source->fOutxCtsFlow;
   fOutxDsrFlow      = source->fOutxDsrFlow;
   fDtrControl       = source->fDtrControl;
   fDsrSensitivity   = source->fDsrSensitivity;
   fTXContinueOnXoff = source->fTXContinueOnXoff;
   fOutX             = source->fOutX;
   fInX              = source->fInX;
   fErrorChar        = source->fErrorChar;
   fNull             = source->fNull;
   fRtsControl       = source->fRtsControl;
   fDummy2           = source->fDummy2;
   wReserved         = source->wReserved;
   XonLim            = source->XonLim;
   XoffLim           = source->XoffLim;
   ByteSize          = source->ByteSize;
   Parity            = source->Parity;
   StopBits          = source->StopBits;
   XonChar           = source->XonChar;
   XoffChar          = source->XoffChar;
   ErrorChar         = source->ErrorChar;
   EofChar           = source->EofChar;
   EvtChar           = source->EvtChar;
}

#if defined( _DEBUG )

void CDeviceControlBlock::Dump( CDumpContext& dump_context ) const
{
   dump_context << "a CDeviceControlBlock at " << (void *) this << "\n";
   dump_context << "DCBlength = "         << DCBlength         << "\n";
   dump_context << "BaudRate ="           << BaudRate          << "\n";
   dump_context << "fBinary = "           << fBinary           << "\n";
   dump_context << "fParity = "           << fParity           << "\n";
   dump_context << "fOutxCtsFlow = "      << fOutxCtsFlow      << "\n";
   dump_context << "fOutxDsrFlow = "      << fOutxDsrFlow      << "\n";
   dump_context << "fDtrControl = "       << fDtrControl       << "\n";
   dump_context << "fDsrSensitivity = "   << fDsrSensitivity   << "\n";
   dump_context << "fTXContinueOnXoff = " << fTXContinueOnXoff << "\n";
   dump_context << "fOutX = "             << fOutX             << "\n";
   dump_context << "fInX = "              << fInX              << "\n";
   dump_context << "fErrorChar = "        << fErrorChar        << "\n";
   dump_context << "fNull = "             << fNull             << "\n";
   dump_context << "fRtsControl = "       << fRtsControl       << "\n";
   dump_context << "fDummy2 = "           << fDummy2           << "\n";;
   dump_context << "wReserved = "         << wReserved         << "\n";
   dump_context << "XonLim = "            << XonLim            << "\n";
   dump_context << "XoffLim = "           << XoffLim           << "\n";
   dump_context << "ByteSize = "          << ByteSize          << "\n";
   dump_context << "Parity = "            << Parity            << "\n";
   dump_context << "StopBits = "          << StopBits          << "\n";
   dump_context << "XonChar = "           << XonChar           << "\n";
   dump_context << "XoffChar = "          << XoffChar          << "\n";
   dump_context << "ErrorChar = "         << ErrorChar         << "\n";
   dump_context << "EofChar = "           << EofChar           << "\n";
   dump_context << "EvtChar = "           << EvtChar           << "\n";
}

#endif // _DEBUG
