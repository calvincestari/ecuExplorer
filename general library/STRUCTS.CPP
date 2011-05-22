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

CAccessAllowedEntry::CAccessAllowedEntry()
{
   Empty();
}

CAccessAllowedEntry::CAccessAllowedEntry( const CAccessAllowedEntry& source )
{
   Empty();
   Copy( source );
}

CAccessAllowedEntry::CAccessAllowedEntry( const ACCESS_ALLOWED_ACE * source )
{
   Empty();
   Copy( source );
}

CAccessAllowedEntry::~CAccessAllowedEntry()
{
   Empty();
}

void CAccessAllowedEntry::Copy( const CAccessAllowedEntry& source )
{
   Copy( (const ACCESS_ALLOWED_ACE *) &source );
}

void CAccessAllowedEntry::Copy( const ACCESS_ALLOWED_ACE * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   Header.AceType  = source->Header.AceType;
   Header.AceFlags = source->Header.AceFlags;
   Header.AceSize  = source->Header.AceSize;
   Mask            = source->Mask;
   SidStart        = source->SidStart;
}

#if defined( _DEBUG )

void CAccessAllowedEntry::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CAccessAllowedEntry at " << (void *) this << "\n";

   dump_context << "Header.AceType = "  << Header.AceType  << "\n";
   dump_context << "Header.AceFlags = " << Header.AceFlags << "\n";
   dump_context << "Header.AceSize = "  << Header.AceSize  << "\n";
   dump_context << "Mask = "            << Mask            << "\n";
   dump_context << "SidStart = "        << SidStart        << "\n";
}

#endif // _DEBUG

void CAccessAllowedEntry::Empty( void )
{
   // ACE_HEADER
   Header.AceType  = 0;
   Header.AceFlags = 0;
   Header.AceSize  = 0;

   // ACE_MASK
   Mask            = 0;

   SidStart        = 0;
}

CAccessControlEntryHeader::CAccessControlEntryHeader()
{
   Empty();
}

CAccessControlEntryHeader::CAccessControlEntryHeader( const CAccessControlEntryHeader& source )
{
   Empty();
   Copy( source );
}

CAccessControlEntryHeader::CAccessControlEntryHeader( const ACE_HEADER * source )
{
   Empty();
   Copy( source );
}

CAccessControlEntryHeader::~CAccessControlEntryHeader()
{
   Empty();
}

void CAccessControlEntryHeader::Copy( const CAccessControlEntryHeader& source )
{
   Copy( (const ACE_HEADER *) &source );
}

void CAccessControlEntryHeader::Copy( const ACE_HEADER * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   AceType  = source->AceType;
   AceFlags = source->AceFlags;
   AceSize  = source->AceSize;
}

#if defined( _DEBUG )

void CAccessControlEntryHeader::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CAccessControlEntryHeader at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   AceType = "  << AceType  << "\n";
   dump_context << "   AceFlags = " << AceFlags << "\n";
   dump_context << "   AceSizee = " << AceSize     << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CAccessControlEntryHeader::Empty( void )
{
   AceType  = 0;
   AceFlags = 0;
   AceSize  = 0;
}

CAccessControlList::CAccessControlList()
{
   Empty();
}

CAccessControlList::CAccessControlList( const CAccessControlList& source )
{
   Empty();
   Copy( source );
}

CAccessControlList::CAccessControlList( const ACL * source )
{
   Empty();
   Copy( source );
}

CAccessControlList::~CAccessControlList()
{
   Empty();
}

void CAccessControlList::Copy( const CAccessControlList& source )
{
   Copy( (const ACL *) &source );
}

void CAccessControlList::Copy( const ACL * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   AclRevision = source->AclRevision;
   Sbz1        = source->Sbz1;
   AclSize     = source->AclSize;
   AceCount    = source->AceCount;
   Sbz2        = source->Sbz1;
}

#if defined( _DEBUG )

void CAccessControlList::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CAccessControlList at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   AclRevision = " << AclRevision << "\n";
   dump_context << "   Sbz1 = "        << Sbz1        << "\n";
   dump_context << "   AclSize = "     << AclSize     << "\n";
   dump_context << "   AceCount = "    << AceCount    << "\n";
   dump_context << "   Sbz2 = "        << Sbz2        << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CAccessControlList::Empty( void )
{
   AclRevision = ACL_REVISION;
   Sbz1        = 0;
   AclSize     = 0;
   AceCount    = 0;
   Sbz2        = 0;
}

CAccessDeniedEntry::CAccessDeniedEntry()
{
   Empty();
}

CAccessDeniedEntry::CAccessDeniedEntry( const CAccessDeniedEntry& source )
{
   Empty();
   Copy( source );
}

CAccessDeniedEntry::CAccessDeniedEntry( const ACCESS_DENIED_ACE * source )
{
   Empty();
   Copy( source );
}

CAccessDeniedEntry::~CAccessDeniedEntry()
{
   Empty();
}

void CAccessDeniedEntry::Copy( const CAccessDeniedEntry& source )
{
   Copy( (const ACCESS_DENIED_ACE *) &source );
}

void CAccessDeniedEntry::Copy( const ACCESS_DENIED_ACE * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   Header.AceType  = source->Header.AceType;
   Header.AceFlags = source->Header.AceFlags;
   Header.AceSize  = source->Header.AceSize;
   Mask            = source->Mask;
   SidStart        = source->SidStart;
}

#if defined( _DEBUG )

void CAccessDeniedEntry::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CAccessDeniedEntry at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   Header.AceType = "  << Header.AceType  << "\n";
   dump_context << "   Header.AceFlags = " << Header.AceFlags << "\n";
   dump_context << "   Header.AceSize = "  << Header.AceSize  << "\n";
   dump_context << "   Mask = "            << Mask            << "\n";
   dump_context << "   SidStart = "        << SidStart        << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CAccessDeniedEntry::Empty( void )
{
   // ACE_HEADER
   Header.AceType  = 0;
   Header.AceFlags = 0;
   Header.AceSize  = 0;

   // ACE_MASK
   Mask            = 0;

   SidStart        = 0;
}

CBitmapCoreHeader::CBitmapCoreHeader()
{
   Empty();
}

CBitmapCoreHeader::CBitmapCoreHeader( const CBitmapCoreHeader& source )
{
   Empty();
   Copy( source );
}

CBitmapCoreHeader::CBitmapCoreHeader( const tagBITMAPCOREHEADER * source )
{
   Empty();
   Copy( source );
}

CBitmapCoreHeader::~CBitmapCoreHeader()
{
   Empty();
}

void CBitmapCoreHeader::Copy( const CBitmapCoreHeader& source )
{
   Copy( (const CBitmapCoreHeader *) &source );
}

void CBitmapCoreHeader::Copy( const tagBITMAPCOREHEADER * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   bcSize     = source->bcSize;
   bcWidth    = source->bcWidth;
   bcHeight   = source->bcHeight;
   bcPlanes   = source->bcPlanes;
   bcBitCount = source->bcBitCount;
}

#if defined( _DEBUG )

void CBitmapCoreHeader::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CBitmapCoreHeader at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   bcSize = "     << bcSize     << "\n";
   dump_context << "   bcWidth = "    << bcWidth    << "\n";
   dump_context << "   bcHeight = "   << bcHeight   << "\n";
   dump_context << "   bcPlanes = "   << bcPlanes   << "\n";
   dump_context << "   bcBitCount = " << bcBitCount << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CBitmapCoreHeader::Empty( void )
{
   bcSize     = sizeof( tagBITMAPCOREHEADER );
   bcWidth    = 0;
   bcHeight   = 0;
   bcPlanes   = 0;
   bcBitCount = 0;
}

CBitmapFileHeader::CBitmapFileHeader()
{
   Empty();
}

CBitmapFileHeader::CBitmapFileHeader( const CBitmapFileHeader& source )
{
   Empty();
   Copy( source );
}

CBitmapFileHeader::CBitmapFileHeader( const tagBITMAPFILEHEADER * source )
{
   Empty();
   Copy( source );
}

CBitmapFileHeader::~CBitmapFileHeader()
{
   Empty();
}

void CBitmapFileHeader::Copy( const CBitmapFileHeader& source )
{
   Copy( (const tagBITMAPFILEHEADER *) &source );
}

void CBitmapFileHeader::Copy( const tagBITMAPFILEHEADER * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   bfType      = source->bfType;
   bfSize      = source->bfSize;
   bfReserved1 = source->bfReserved1;
   bfReserved2 = source->bfReserved2;
   bfOffBits   = source->bfOffBits;
}

#if defined( _DEBUG )

void CBitmapFileHeader::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CBitmapFileHeader at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   bfType = "      << bfType      << "\n";
   dump_context << "   bfSize = "      << bfSize      << "\n";
   dump_context << "   bfReserved1 = " << bfReserved1 << "\n";
   dump_context << "   bfReserved2 = " << bfReserved2 << "\n";
   dump_context << "   bfOffBits = "   << bfOffBits   << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CBitmapFileHeader::Empty( void )
{
   bfType      = 'BM';
   bfSize      = 0;
   bfReserved1 = 0;
   bfReserved2 = 0;
   bfOffBits   = 0;
}

/*
** CBitmapInfoHeader
*/

CBitmapInfoHeader::CBitmapInfoHeader()
{
   Empty();
}

CBitmapInfoHeader::CBitmapInfoHeader( const CBitmapInfoHeader& source )
{
   Empty();
   Copy( source );
}

CBitmapInfoHeader::CBitmapInfoHeader( const tagBITMAPINFOHEADER * source )
{
   Empty();
   Copy( source );
}

CBitmapInfoHeader::~CBitmapInfoHeader()
{
   Empty();
}

void CBitmapInfoHeader::Copy( const CBitmapInfoHeader& source )
{
   Copy( (const tagBITMAPINFOHEADER * ) &source );
}

void CBitmapInfoHeader::Copy( const tagBITMAPINFOHEADER * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   biSize          = source->biSize;
   biWidth         = source->biWidth;
   biHeight        = source->biHeight;
   biPlanes        = source->biPlanes;
   biBitCount      = source->biBitCount;
   biCompression   = source->biCompression;
   biSizeImage     = source->biSizeImage;
   biXPelsPerMeter = source->biXPelsPerMeter;
   biYPelsPerMeter = source->biYPelsPerMeter;
   biClrUsed       = source->biClrUsed;
   biClrImportant  = source->biClrImportant;
}

#if defined( _DEBUG )

void CBitmapInfoHeader::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CBitmapInfoHeader at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   biSize = "          << biSize          << "\n";
   dump_context << "   biWidth = "         << biWidth         << "\n";
   dump_context << "   biHeight = "        << biHeight        << "\n";
   dump_context << "   biPlanes = "        << biPlanes        << "\n";
   dump_context << "   biBitCount = "      << biBitCount      << "\n";
   dump_context << "   biCompression = "   << biCompression   << "\n";
   dump_context << "   biSizeImage = "     << biSizeImage     << "\n";
   dump_context << "   biXPelsPerMeter = " << biXPelsPerMeter << "\n";
   dump_context << "   biYPelsPerMeter = " << biYPelsPerMeter << "\n";
   dump_context << "   biClrUsed = "       << biClrUsed       << "\n";
   dump_context << "   biClrImportant = "  << biClrImportant  << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CBitmapInfoHeader::Empty( void )
{
   biSize          = sizeof( tagBITMAPINFOHEADER );
   biWidth         = 0;
   biHeight        = 0;
   biPlanes        = 0;
   biBitCount      = 0;
   biCompression   = 0;
   biSizeImage     = 0;
   biXPelsPerMeter = 0;
   biYPelsPerMeter = 0;
   biClrUsed       = 0;
   biClrImportant  = 0;
}

/*
** CColorAdjustment
*/

CColorAdjustment::CColorAdjustment()
{
   Empty();
}

CColorAdjustment::CColorAdjustment( const CColorAdjustment& source )
{
   Empty();
   Copy( source );
}

CColorAdjustment::CColorAdjustment( const tagCOLORADJUSTMENT * source )
{
   Empty();
   Copy( source );
}

CColorAdjustment::~CColorAdjustment()
{
   Empty();
}

void CColorAdjustment::Copy( const CColorAdjustment& source )
{
   Copy( (const tagCOLORADJUSTMENT *) &source );
}

void CColorAdjustment::Copy( const tagCOLORADJUSTMENT * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   caSize            = source->caSize;
   caFlags           = source->caFlags;
   caIlluminantIndex = source->caIlluminantIndex;
   caRedGamma        = source->caRedGamma;
   caGreenGamma      = source->caGreenGamma;
   caBlueGamma       = source->caBlueGamma;
   caReferenceBlack  = source->caReferenceBlack;
   caReferenceWhite  = source->caReferenceWhite;
   caContrast        = source->caContrast;
   caBrightness      = source->caBrightness;
   caColorfulness    = source->caColorfulness;
   caRedGreenTint    = source->caRedGreenTint;
}

#if defined( _DEBUG )

void CColorAdjustment::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CColorAdjustment at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   caSize = "             << caSize             << "\n";
   dump_context << "   caFlags = "            << caFlags            << "\n";
   dump_context << "   caIlluminantIndex = "  << caIlluminantIndex  << "\n";
   dump_context << "   caRedGamma = "         << caRedGamma         << "\n";
   dump_context << "   caGreenGamma = "       << caGreenGamma       << "\n";
   dump_context << "   caBlueGamma = "        << caBlueGamma        << "\n";
   dump_context << "   caReferenceBlack = "   << caReferenceBlack   << "\n";
   dump_context << "   caReferenceWhite = "   << caReferenceWhite   << "\n";
   dump_context << "   caContrast = "         << caContrast         << "\n";
   dump_context << "   caBrightness = "       << caBrightness       << "\n";
   dump_context << "   caColorfulness = "     << caColorfulness     << "\n";
   dump_context << "   caRedGreenTint = "     << caRedGreenTint     << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CColorAdjustment::Empty( void )
{
   caSize            = sizeof( tagCOLORADJUSTMENT );
   caFlags           = 0;
   caIlluminantIndex = 0;
   caRedGamma        = 0;
   caGreenGamma      = 0;
   caBlueGamma       = 0;
   caReferenceBlack  = 0;
   caReferenceWhite  = 0;
   caContrast        = 0;
   caBrightness      = 0;
   caColorfulness    = 0;
   caRedGreenTint    = 0;
}

/*
** CMemoryStatus
*/

CMemoryStatus::CMemoryStatus()
{
   Empty();
}

CMemoryStatus::CMemoryStatus( const CMemoryStatus& source )
{
   Empty();
   Copy( source );
}

CMemoryStatus::CMemoryStatus( const MEMORYSTATUS * source )
{
   Empty();
   Copy( source );
}

CMemoryStatus::~CMemoryStatus()
{
   Empty();
}

void CMemoryStatus::Copy( const CMemoryStatus& source )
{
   Copy( (const MEMORYSTATUS *) &source );
}

void CMemoryStatus::Copy( const MEMORYSTATUS * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   dwLength        = source->dwLength;
   dwMemoryLoad    = source->dwMemoryLoad;
   dwTotalPhys     = source->dwTotalPhys;
   dwAvailPhys     = source->dwAvailPhys;
   dwTotalPageFile = source->dwTotalPageFile;
   dwAvailPageFile = source->dwAvailPageFile;
   dwTotalVirtual  = source->dwTotalVirtual;
   dwAvailVirtual  = source->dwAvailVirtual;
}

#if defined( _DEBUG )

void CMemoryStatus::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CMemoryStatus at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   dwLength        = " << dwLength        << "\n";
   dump_context << "   dwMemoryLoad    = " << dwMemoryLoad    << "\n";
   dump_context << "   dwTotalPhys     = " << dwTotalPhys     << "\n";
   dump_context << "   dwAvailPhys     = " << dwAvailPhys     << "\n";
   dump_context << "   dwTotalPageFile = " << dwTotalPageFile << "\n";
   dump_context << "   dwAvailPageFile = " << dwAvailPageFile << "\n";
   dump_context << "   dwTotalVirtual  = " << dwTotalVirtual  << "\n";
   dump_context << "   dwAvailVirtual  = " << dwAvailVirtual  << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CMemoryStatus::Empty( void )
{
   dwLength        = sizeof( MEMORYSTATUS );
   dwMemoryLoad    = 0;
   dwTotalPhys     = 0;
   dwAvailPhys     = 0;
   dwTotalPageFile = 0;
   dwAvailPageFile = 0;
   dwTotalVirtual  = 0;
   dwAvailVirtual  = 0;
}

/*
** COFStruct
*/

COFStruct::COFStruct()
{
   Empty();
}

COFStruct::COFStruct( const COFStruct& source )
{
   Empty();
   Copy( source );
}

COFStruct::COFStruct( const _OFSTRUCT * source )
{
   Empty();
   Copy( source );
}

COFStruct::~COFStruct()
{
   Empty();
}

void COFStruct::Copy( const COFStruct& source )
{
   Copy( (const _OFSTRUCT *) &source );
}

void COFStruct::Copy( const _OFSTRUCT * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   cBytes     = source->cBytes;
   fFixedDisk = source->fFixedDisk;
   nErrCode   = source->nErrCode;
   Reserved1  = source->Reserved1;
   Reserved2  = source->Reserved2;
   ::strcpy( szPathName, source->szPathName );
}

#if defined( _DEBUG )

void COFStruct::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a COFStruct at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   cBytes     = "  << cBytes      << "\n";
   dump_context << "   fFixedDisk = "  << fFixedDisk  << "\n";
   dump_context << "   nErrCode   = "  << nErrCode    << "\n";
   dump_context << "   Reserved1  = "   << Reserved1  << "\n";
   dump_context << "   Reserved2  = "   << Reserved2  << "\n";
   dump_context << "   szPathName = \"" << szPathName << "\"\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void COFStruct::Empty( void )
{
   cBytes     = sizeof( _OFSTRUCT );
   fFixedDisk = 0;
   nErrCode   = 0;
   Reserved1  = 0;
   Reserved2  = 0;

   int index = 0;

   while( index < OFS_MAXPATHNAME )
   {
      szPathName[ index ] = 0;
      index++;
   }
}

/*
** COutlineTextMetricA
*/

COutlineTextMetricA::COutlineTextMetricA()
{
   Empty();
}

COutlineTextMetricA::COutlineTextMetricA( const COutlineTextMetricA& source )
{
   Empty();
   Copy( source );
}

COutlineTextMetricA::COutlineTextMetricA( const _OUTLINETEXTMETRICA * source )
{
   Empty();
   Copy( source );
}

COutlineTextMetricA::~COutlineTextMetricA()
{
   Empty();
}

void COutlineTextMetricA::Copy( const COutlineTextMetricA& source )
{
   Copy( (const _OUTLINETEXTMETRICA *) &source );
}

void COutlineTextMetricA::Copy( const _OUTLINETEXTMETRICA * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   otmSize = source->otmSize;

   // TEXTMETRICA
   otmTextMetrics.tmHeight           = source->otmTextMetrics.tmHeight;
   otmTextMetrics.tmAscent           = source->otmTextMetrics.tmAscent;
   otmTextMetrics.tmDescent          = source->otmTextMetrics.tmDescent;
   otmTextMetrics.tmInternalLeading  = source->otmTextMetrics.tmInternalLeading;
   otmTextMetrics.tmExternalLeading  = source->otmTextMetrics.tmExternalLeading;
   otmTextMetrics.tmAveCharWidth     = source->otmTextMetrics.tmAveCharWidth;
   otmTextMetrics.tmMaxCharWidth     = source->otmTextMetrics.tmMaxCharWidth;
   otmTextMetrics.tmWeight           = source->otmTextMetrics.tmWeight;
   otmTextMetrics.tmOverhang         = source->otmTextMetrics.tmOverhang;
   otmTextMetrics.tmDigitizedAspectX = source->otmTextMetrics.tmDigitizedAspectX;
   otmTextMetrics.tmDigitizedAspectY = source->otmTextMetrics.tmDigitizedAspectY;
   otmTextMetrics.tmFirstChar        = source->otmTextMetrics.tmFirstChar;
   otmTextMetrics.tmLastChar         = source->otmTextMetrics.tmLastChar;
   otmTextMetrics.tmDefaultChar      = source->otmTextMetrics.tmDefaultChar;
   otmTextMetrics.tmBreakChar        = source->otmTextMetrics.tmBreakChar;
   otmTextMetrics.tmItalic           = source->otmTextMetrics.tmItalic;
   otmTextMetrics.tmUnderlined       = source->otmTextMetrics.tmUnderlined;
   otmTextMetrics.tmStruckOut        = source->otmTextMetrics.tmStruckOut;
   otmTextMetrics.tmPitchAndFamily   = source->otmTextMetrics.tmPitchAndFamily;
   otmTextMetrics.tmCharSet          = source->otmTextMetrics.tmCharSet;
   
   otmFiller = source->otmFiller;
   
   // PANOSE
   otmPanoseNumber.bFamilyType      = source->otmPanoseNumber.bFamilyType;
   otmPanoseNumber.bSerifStyle      = source->otmPanoseNumber.bSerifStyle;
   otmPanoseNumber.bWeight          = source->otmPanoseNumber.bWeight;
   otmPanoseNumber.bProportion      = source->otmPanoseNumber.bProportion;
   otmPanoseNumber.bContrast        = source->otmPanoseNumber.bContrast;
   otmPanoseNumber.bStrokeVariation = source->otmPanoseNumber.bStrokeVariation;
   otmPanoseNumber.bArmStyle        = source->otmPanoseNumber.bArmStyle;
   otmPanoseNumber.bLetterform      = source->otmPanoseNumber.bLetterform;
   otmPanoseNumber.bMidline         = source->otmPanoseNumber.bMidline;
   otmPanoseNumber.bXHeight         = source->otmPanoseNumber.bXHeight;

   otmfsSelection    = source->otmfsSelection;
   otmfsType         = source->otmfsType;
   otmsCharSlopeRise = source->otmsCharSlopeRise;
   otmsCharSlopeRun  = source->otmsCharSlopeRun;
   otmItalicAngle    = source->otmItalicAngle;
   otmEMSquare       = source->otmEMSquare;
   otmAscent         = source->otmAscent;
   otmDescent        = source->otmDescent;
   otmLineGap        = source->otmLineGap;
   otmsCapEmHeight   = source->otmsCapEmHeight;
   otmsXHeight       = source->otmsXHeight;
   
   // RECT
   otmrcFontBox.left   = source->otmrcFontBox.left;
   otmrcFontBox.top    = source->otmrcFontBox.top;
   otmrcFontBox.right  = source->otmrcFontBox.right;
   otmrcFontBox.bottom = source->otmrcFontBox.bottom;

   otmMacAscent     = source->otmMacAscent;
   otmMacDescent    = source->otmMacDescent;
   otmMacLineGap    = source->otmMacLineGap;
   otmusMinimumPPEM = source->otmusMinimumPPEM;

   // POINT
   otmptSubscriptSize.x     = source->otmptSubscriptSize.x;
   otmptSubscriptSize.y     = source->otmptSubscriptSize.y;
   otmptSubscriptOffset.x   = source->otmptSubscriptOffset.x;
   otmptSubscriptOffset.y   = source->otmptSubscriptOffset.y;
   otmptSuperscriptSize.x   = source->otmptSuperscriptSize.x;
   otmptSuperscriptSize.y   = source->otmptSuperscriptSize.y;
   otmptSuperscriptOffset.x = source->otmptSuperscriptOffset.x;
   otmptSuperscriptOffset.y = source->otmptSuperscriptOffset.y;

   otmsStrikeoutSize      = source->otmsStrikeoutSize;
   otmsStrikeoutPosition  = source->otmsStrikeoutPosition;
   otmsUnderscoreSize     = source->otmsUnderscoreSize;
   otmsUnderscorePosition = source->otmsUnderscorePosition;
   otmpFamilyName         = source->otmpFamilyName;
   otmpFaceName           = source->otmpFaceName;
   otmpStyleName          = source->otmpStyleName;
   otmpFullName           = source->otmpFullName;
}

#if defined( _DEBUG )

void COutlineTextMetricA::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a COutlineTextMetricA at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   otmSize                           = " << otmSize                           << "\n";
   dump_context << "   otmTextMetrics.tmHeight           = " << otmTextMetrics.tmHeight           << "\n";
   dump_context << "   otmTextMetrics.tmAscent           = " << otmTextMetrics.tmAscent           << "\n";
   dump_context << "   otmTextMetrics.tmDescent          = " << otmTextMetrics.tmDescent          << "\n";
   dump_context << "   otmTextMetrics.tmInternalLeading  = " << otmTextMetrics.tmInternalLeading  << "\n";
   dump_context << "   otmTextMetrics.tmExternalLeading  = " << otmTextMetrics.tmExternalLeading  << "\n";
   dump_context << "   otmTextMetrics.tmAveCharWidth     = " << otmTextMetrics.tmAveCharWidth     << "\n";
   dump_context << "   otmTextMetrics.tmMaxCharWidth     = " << otmTextMetrics.tmMaxCharWidth     << "\n";
   dump_context << "   otmTextMetrics.tmWeight           = " << otmTextMetrics.tmWeight           << "\n";
   dump_context << "   otmTextMetrics.tmOverhang         = " << otmTextMetrics.tmOverhang         << "\n";
   dump_context << "   otmTextMetrics.tmDigitizedAspectX = " << otmTextMetrics.tmDigitizedAspectX << "\n";
   dump_context << "   otmTextMetrics.tmDigitizedAspectY = " << otmTextMetrics.tmDigitizedAspectY << "\n";
   dump_context << "   otmTextMetrics.tmFirstChar        = " << otmTextMetrics.tmFirstChar        << "\n";
   dump_context << "   otmTextMetrics.tmLastChar         = " << otmTextMetrics.tmLastChar         << "\n";
   dump_context << "   otmTextMetrics.tmDefaultChar      = " << otmTextMetrics.tmDefaultChar      << "\n";
   dump_context << "   otmTextMetrics.tmBreakChar        = " << otmTextMetrics.tmBreakChar        << "\n";
   dump_context << "   otmTextMetrics.tmItalic           = " << otmTextMetrics.tmItalic           << "\n";
   dump_context << "   otmTextMetrics.tmUnderlined       = " << otmTextMetrics.tmUnderlined       << "\n";
   dump_context << "   otmTextMetrics.tmStruckOut        = " << otmTextMetrics.tmStruckOut        << "\n";
   dump_context << "   otmTextMetrics.tmPitchAndFamily   = " << otmTextMetrics.tmPitchAndFamily   << "\n";
   dump_context << "   otmTextMetrics.tmCharSet          = " << otmTextMetrics.tmCharSet          << "\n";
   dump_context << "   otmFiller                         = " << otmFiller                         << "\n";
   dump_context << "   otmPanoseNumber.bFamilyType       = " << otmPanoseNumber.bFamilyType       << "\n";
   dump_context << "   otmPanoseNumber.bSerifStyle       = " << otmPanoseNumber.bSerifStyle       << "\n";
   dump_context << "   otmPanoseNumber.bWeight           = " << otmPanoseNumber.bWeight           << "\n";
   dump_context << "   otmPanoseNumber.bProportion       = " << otmPanoseNumber.bProportion       << "\n";
   dump_context << "   otmPanoseNumber.bContrast         = " << otmPanoseNumber.bContrast         << "\n";
   dump_context << "   otmPanoseNumber.bStrokeVariation  = " << otmPanoseNumber.bStrokeVariation  << "\n";
   dump_context << "   otmPanoseNumber.bArmStyle         = " << otmPanoseNumber.bArmStyle         << "\n";
   dump_context << "   otmPanoseNumber.bLetterform       = " << otmPanoseNumber.bLetterform       << "\n";
   dump_context << "   otmPanoseNumber.bMidline          = " << otmPanoseNumber.bMidline          << "\n";
   dump_context << "   otmPanoseNumber.bXHeight          = " << otmPanoseNumber.bXHeight          << "\n";
   dump_context << "   otmfsSelection                    = " << otmfsSelection                    << "\n";
   dump_context << "   otmfsType                         = " << otmfsType                         << "\n";
   dump_context << "   otmsCharSlopeRise                 = " << otmsCharSlopeRise                 << "\n";
   dump_context << "   otmsCharSlopeRun                  = " << otmsCharSlopeRun                  << "\n";
   dump_context << "   otmItalicAngle                    = " << otmItalicAngle                    << "\n";
   dump_context << "   otmEMSquare                       = " << otmEMSquare                       << "\n";
   dump_context << "   otmAscent                         = " << otmAscent                         << "\n";
   dump_context << "   otmDescent                        = " << otmDescent                        << "\n";
   dump_context << "   otmLineGap                        = " << otmLineGap                        << "\n";
   dump_context << "   otmsCapEmHeight                   = " << otmsCapEmHeight                   << "\n";
   dump_context << "   otmsXHeight                       = " << otmsXHeight                       << "\n";
   dump_context << "   otmrcFontBox.left                 = " << otmrcFontBox.left                 << "\n";
   dump_context << "   otmrcFontBox.top                  = " << otmrcFontBox.top                  << "\n";
   dump_context << "   otmrcFontBox.right                = " << otmrcFontBox.right                << "\n";
   dump_context << "   otmrcFontBox.bottom               = " << otmrcFontBox.bottom               << "\n";
   dump_context << "   otmMacAscent                      = " << otmMacAscent                      << "\n";
   dump_context << "   otmMacDescent                     = " << otmMacDescent                     << "\n";
   dump_context << "   otmMacLineGap                     = " << otmMacLineGap                     << "\n";
   dump_context << "   otmusMinimumPPEM                  = " << otmusMinimumPPEM                  << "\n";
   dump_context << "   otmptSubscriptSize.x              = " << otmptSubscriptSize.x              << "\n";
   dump_context << "   otmptSubscriptSize.y              = " << otmptSubscriptSize.y              << "\n";
   dump_context << "   otmptSubscriptOffset.x            = " << otmptSubscriptOffset.x            << "\n";
   dump_context << "   otmptSubscriptOffset.y            = " << otmptSubscriptOffset.y            << "\n";
   dump_context << "   otmptSuperscriptSize.x            = " << otmptSuperscriptSize.x            << "\n";
   dump_context << "   otmptSuperscriptSize.y            = " << otmptSuperscriptSize.y            << "\n";
   dump_context << "   otmptSuperscriptOffset.x          = " << otmptSuperscriptOffset.x          << "\n";
   dump_context << "   otmptSuperscriptOffset.y          = " << otmptSuperscriptOffset.y          << "\n";
   dump_context << "   otmsStrikeoutSize                 = " << otmsStrikeoutSize                 << "\n";
   dump_context << "   otmsStrikeoutPosition             = " << otmsStrikeoutPosition             << "\n";
   dump_context << "   otmsUnderscoreSize                = " << otmsUnderscoreSize                << "\n";
   dump_context << "   otmsUnderscorePosition            = " << otmsUnderscorePosition            << "\n";
   dump_context << "   otmpFamilyName                    = " << otmpFamilyName                    << "\n";
   dump_context << "   otmpFaceName                      = " << otmpFaceName                      << "\n";
   dump_context << "   otmpStyleName                     = " << otmpStyleName                     << "\n";
   dump_context << "   otmpFullName                      = " << otmpFullName                      << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void COutlineTextMetricA::Empty( void )
{
   otmSize = sizeof( _OUTLINETEXTMETRICA );

   // TEXTMETRICA
   otmTextMetrics.tmHeight           = 0;
   otmTextMetrics.tmAscent           = 0;
   otmTextMetrics.tmDescent          = 0;
   otmTextMetrics.tmInternalLeading  = 0;
   otmTextMetrics.tmExternalLeading  = 0;
   otmTextMetrics.tmAveCharWidth     = 0;
   otmTextMetrics.tmMaxCharWidth     = 0;
   otmTextMetrics.tmWeight           = 0;
   otmTextMetrics.tmOverhang         = 0;
   otmTextMetrics.tmDigitizedAspectX = 0;
   otmTextMetrics.tmDigitizedAspectY = 0;
   otmTextMetrics.tmFirstChar        = 0;
   otmTextMetrics.tmLastChar         = 0;
   otmTextMetrics.tmDefaultChar      = 0;
   otmTextMetrics.tmBreakChar        = 0;
   otmTextMetrics.tmItalic           = 0;
   otmTextMetrics.tmUnderlined       = 0;
   otmTextMetrics.tmStruckOut        = 0;
   otmTextMetrics.tmPitchAndFamily   = 0;
   otmTextMetrics.tmCharSet          = 0;
   
   otmFiller = 0;
   
   // PANOSE
   otmPanoseNumber.bFamilyType      = 0;
   otmPanoseNumber.bSerifStyle      = 0;
   otmPanoseNumber.bWeight          = 0;
   otmPanoseNumber.bProportion      = 0;
   otmPanoseNumber.bContrast        = 0;
   otmPanoseNumber.bStrokeVariation = 0;
   otmPanoseNumber.bArmStyle        = 0;
   otmPanoseNumber.bLetterform      = 0;
   otmPanoseNumber.bMidline         = 0;
   otmPanoseNumber.bXHeight         = 0;

   otmfsSelection    = 0;
   otmfsType         = 0;
   otmsCharSlopeRise = 0;
   otmsCharSlopeRun  = 0;
   otmItalicAngle    = 0;
   otmEMSquare       = 0;
   otmAscent         = 0;
   otmDescent        = 0;
   otmLineGap        = 0;
   otmsCapEmHeight   = 0;
   otmsXHeight       = 0;
   
   // RECT
   otmrcFontBox.left   = 0;
   otmrcFontBox.top    = 0;
   otmrcFontBox.right  = 0;
   otmrcFontBox.bottom = 0;

   otmMacAscent     = 0;
   otmMacDescent    = 0;
   otmMacLineGap    = 0;
   otmusMinimumPPEM = 0;

   // POINT
   otmptSubscriptSize.x     = 0;
   otmptSubscriptSize.y     = 0;
   otmptSubscriptOffset.x   = 0;
   otmptSubscriptOffset.y   = 0;
   otmptSuperscriptSize.x   = 0;
   otmptSuperscriptSize.y   = 0;
   otmptSuperscriptOffset.x = 0;
   otmptSuperscriptOffset.y = 0;

   otmsStrikeoutSize      = 0;
   otmsStrikeoutPosition  = 0;
   otmsUnderscoreSize     = 0;
   otmsUnderscorePosition = 0;
   otmpFamilyName         = NULL;
   otmpFaceName           = NULL;
   otmpStyleName          = NULL;
   otmpFullName           = NULL;
}

COutlineTextMetricW::COutlineTextMetricW()
{
   Empty();
}

COutlineTextMetricW::COutlineTextMetricW( const COutlineTextMetricW& source )
{
   Empty();
   Copy( source );
}

COutlineTextMetricW::COutlineTextMetricW( const _OUTLINETEXTMETRICW * source )
{
   Empty();
   Copy( source );
}

COutlineTextMetricW::~COutlineTextMetricW()
{
   Empty();
}

void COutlineTextMetricW::Copy( const COutlineTextMetricW& source )
{
   Copy( (const _OUTLINETEXTMETRICW *) &source );
}

void COutlineTextMetricW::Copy( const _OUTLINETEXTMETRICW * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   otmSize = source->otmSize;

   // TEXTMETRICA
   otmTextMetrics.tmHeight           = source->otmTextMetrics.tmHeight;
   otmTextMetrics.tmAscent           = source->otmTextMetrics.tmAscent;
   otmTextMetrics.tmDescent          = source->otmTextMetrics.tmDescent;
   otmTextMetrics.tmInternalLeading  = source->otmTextMetrics.tmInternalLeading;
   otmTextMetrics.tmExternalLeading  = source->otmTextMetrics.tmExternalLeading;
   otmTextMetrics.tmAveCharWidth     = source->otmTextMetrics.tmAveCharWidth;
   otmTextMetrics.tmMaxCharWidth     = source->otmTextMetrics.tmMaxCharWidth;
   otmTextMetrics.tmWeight           = source->otmTextMetrics.tmWeight;
   otmTextMetrics.tmOverhang         = source->otmTextMetrics.tmOverhang;
   otmTextMetrics.tmDigitizedAspectX = source->otmTextMetrics.tmDigitizedAspectX;
   otmTextMetrics.tmDigitizedAspectY = source->otmTextMetrics.tmDigitizedAspectY;
   otmTextMetrics.tmFirstChar        = source->otmTextMetrics.tmFirstChar;
   otmTextMetrics.tmLastChar         = source->otmTextMetrics.tmLastChar;
   otmTextMetrics.tmDefaultChar      = source->otmTextMetrics.tmDefaultChar;
   otmTextMetrics.tmBreakChar        = source->otmTextMetrics.tmBreakChar;
   otmTextMetrics.tmItalic           = source->otmTextMetrics.tmItalic;
   otmTextMetrics.tmUnderlined       = source->otmTextMetrics.tmUnderlined;
   otmTextMetrics.tmStruckOut        = source->otmTextMetrics.tmStruckOut;
   otmTextMetrics.tmPitchAndFamily   = source->otmTextMetrics.tmPitchAndFamily;
   otmTextMetrics.tmCharSet          = source->otmTextMetrics.tmCharSet;
   
   otmFiller = source->otmFiller;
   
   // PANOSE
   otmPanoseNumber.bFamilyType      = source->otmPanoseNumber.bFamilyType;
   otmPanoseNumber.bSerifStyle      = source->otmPanoseNumber.bSerifStyle;
   otmPanoseNumber.bWeight          = source->otmPanoseNumber.bWeight;
   otmPanoseNumber.bProportion      = source->otmPanoseNumber.bProportion;
   otmPanoseNumber.bContrast        = source->otmPanoseNumber.bContrast;
   otmPanoseNumber.bStrokeVariation = source->otmPanoseNumber.bStrokeVariation;
   otmPanoseNumber.bArmStyle        = source->otmPanoseNumber.bArmStyle;
   otmPanoseNumber.bLetterform      = source->otmPanoseNumber.bLetterform;
   otmPanoseNumber.bMidline         = source->otmPanoseNumber.bMidline;
   otmPanoseNumber.bXHeight         = source->otmPanoseNumber.bXHeight;

   otmfsSelection    = source->otmfsSelection;
   otmfsType         = source->otmfsType;
   otmsCharSlopeRise = source->otmsCharSlopeRise;
   otmsCharSlopeRun  = source->otmsCharSlopeRun;
   otmItalicAngle    = source->otmItalicAngle;
   otmEMSquare       = source->otmEMSquare;
   otmAscent         = source->otmAscent;
   otmDescent        = source->otmDescent;
   otmLineGap        = source->otmLineGap;
   otmsCapEmHeight   = source->otmsCapEmHeight;
   otmsXHeight       = source->otmsXHeight;
   
   // RECT
   otmrcFontBox.left   = source->otmrcFontBox.left;
   otmrcFontBox.top    = source->otmrcFontBox.top;
   otmrcFontBox.right  = source->otmrcFontBox.right;
   otmrcFontBox.bottom = source->otmrcFontBox.bottom;

   otmMacAscent     = source->otmMacAscent;
   otmMacDescent    = source->otmMacDescent;
   otmMacLineGap    = source->otmMacLineGap;
   otmusMinimumPPEM = source->otmusMinimumPPEM;

   // POINT
   otmptSubscriptSize.x     = source->otmptSubscriptSize.x;
   otmptSubscriptSize.y     = source->otmptSubscriptSize.y;
   otmptSubscriptOffset.x   = source->otmptSubscriptOffset.x;
   otmptSubscriptOffset.y   = source->otmptSubscriptOffset.y;
   otmptSuperscriptSize.x   = source->otmptSuperscriptSize.x;
   otmptSuperscriptSize.y   = source->otmptSuperscriptSize.y;
   otmptSuperscriptOffset.x = source->otmptSuperscriptOffset.x;
   otmptSuperscriptOffset.y = source->otmptSuperscriptOffset.y;

   otmsStrikeoutSize      = source->otmsStrikeoutSize;
   otmsStrikeoutPosition  = source->otmsStrikeoutPosition;
   otmsUnderscoreSize     = source->otmsUnderscoreSize;
   otmsUnderscorePosition = source->otmsUnderscorePosition;
   otmpFamilyName         = source->otmpFamilyName;
   otmpFaceName           = source->otmpFaceName;
   otmpStyleName          = source->otmpStyleName;
   otmpFullName           = source->otmpFullName;
}

#if defined( _DEBUG )

void COutlineTextMetricW::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a COutlineTextMetricW at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   otmSize                           = " << otmSize                           << "\n";
   dump_context << "   otmTextMetrics.tmHeight           = " << otmTextMetrics.tmHeight           << "\n";
   dump_context << "   otmTextMetrics.tmAscent           = " << otmTextMetrics.tmAscent           << "\n";
   dump_context << "   otmTextMetrics.tmDescent          = " << otmTextMetrics.tmDescent          << "\n";
   dump_context << "   otmTextMetrics.tmInternalLeading  = " << otmTextMetrics.tmInternalLeading  << "\n";
   dump_context << "   otmTextMetrics.tmExternalLeading  = " << otmTextMetrics.tmExternalLeading  << "\n";
   dump_context << "   otmTextMetrics.tmAveCharWidth     = " << otmTextMetrics.tmAveCharWidth     << "\n";
   dump_context << "   otmTextMetrics.tmMaxCharWidth     = " << otmTextMetrics.tmMaxCharWidth     << "\n";
   dump_context << "   otmTextMetrics.tmWeight           = " << otmTextMetrics.tmWeight           << "\n";
   dump_context << "   otmTextMetrics.tmOverhang         = " << otmTextMetrics.tmOverhang         << "\n";
   dump_context << "   otmTextMetrics.tmDigitizedAspectX = " << otmTextMetrics.tmDigitizedAspectX << "\n";
   dump_context << "   otmTextMetrics.tmDigitizedAspectY = " << otmTextMetrics.tmDigitizedAspectY << "\n";
   dump_context << "   otmTextMetrics.tmFirstChar        = " << otmTextMetrics.tmFirstChar        << "\n";
   dump_context << "   otmTextMetrics.tmLastChar         = " << otmTextMetrics.tmLastChar         << "\n";
   dump_context << "   otmTextMetrics.tmDefaultChar      = " << otmTextMetrics.tmDefaultChar      << "\n";
   dump_context << "   otmTextMetrics.tmBreakChar        = " << otmTextMetrics.tmBreakChar        << "\n";
   dump_context << "   otmTextMetrics.tmItalic           = " << otmTextMetrics.tmItalic           << "\n";
   dump_context << "   otmTextMetrics.tmUnderlined       = " << otmTextMetrics.tmUnderlined       << "\n";
   dump_context << "   otmTextMetrics.tmStruckOut        = " << otmTextMetrics.tmStruckOut        << "\n";
   dump_context << "   otmTextMetrics.tmPitchAndFamily   = " << otmTextMetrics.tmPitchAndFamily   << "\n";
   dump_context << "   otmTextMetrics.tmCharSet          = " << otmTextMetrics.tmCharSet          << "\n";
   dump_context << "   otmFiller                         = " << otmFiller                         << "\n";
   dump_context << "   otmPanoseNumber.bFamilyType       = " << otmPanoseNumber.bFamilyType       << "\n";
   dump_context << "   otmPanoseNumber.bSerifStyle       = " << otmPanoseNumber.bSerifStyle       << "\n";
   dump_context << "   otmPanoseNumber.bWeight           = " << otmPanoseNumber.bWeight           << "\n";
   dump_context << "   otmPanoseNumber.bProportion       = " << otmPanoseNumber.bProportion       << "\n";
   dump_context << "   otmPanoseNumber.bContrast         = " << otmPanoseNumber.bContrast         << "\n";
   dump_context << "   otmPanoseNumber.bStrokeVariation  = " << otmPanoseNumber.bStrokeVariation  << "\n";
   dump_context << "   otmPanoseNumber.bArmStyle         = " << otmPanoseNumber.bArmStyle         << "\n";
   dump_context << "   otmPanoseNumber.bLetterform       = " << otmPanoseNumber.bLetterform       << "\n";
   dump_context << "   otmPanoseNumber.bMidline          = " << otmPanoseNumber.bMidline          << "\n";
   dump_context << "   otmPanoseNumber.bXHeight          = " << otmPanoseNumber.bXHeight          << "\n";
   dump_context << "   otmfsSelection                    = " << otmfsSelection                    << "\n";
   dump_context << "   otmfsType                         = " << otmfsType                         << "\n";
   dump_context << "   otmsCharSlopeRise                 = " << otmsCharSlopeRise                 << "\n";
   dump_context << "   otmsCharSlopeRun                  = " << otmsCharSlopeRun                  << "\n";
   dump_context << "   otmItalicAngle                    = " << otmItalicAngle                    << "\n";
   dump_context << "   otmEMSquare                       = " << otmEMSquare                       << "\n";
   dump_context << "   otmAscent                         = " << otmAscent                         << "\n";
   dump_context << "   otmDescent                        = " << otmDescent                        << "\n";
   dump_context << "   otmLineGap                        = " << otmLineGap                        << "\n";
   dump_context << "   otmsCapEmHeight                   = " << otmsCapEmHeight                   << "\n";
   dump_context << "   otmsXHeight                       = " << otmsXHeight                       << "\n";
   dump_context << "   otmrcFontBox.left                 = " << otmrcFontBox.left                 << "\n";
   dump_context << "   otmrcFontBox.top                  = " << otmrcFontBox.top                  << "\n";
   dump_context << "   otmrcFontBox.right                = " << otmrcFontBox.right                << "\n";
   dump_context << "   otmrcFontBox.bottom               = " << otmrcFontBox.bottom               << "\n";
   dump_context << "   otmMacAscent                      = " << otmMacAscent                      << "\n";
   dump_context << "   otmMacDescent                     = " << otmMacDescent                     << "\n";
   dump_context << "   otmMacLineGap                     = " << otmMacLineGap                     << "\n";
   dump_context << "   otmusMinimumPPEM                  = " << otmusMinimumPPEM                  << "\n";
   dump_context << "   otmptSubscriptSize.x              = " << otmptSubscriptSize.x              << "\n";
   dump_context << "   otmptSubscriptSize.y              = " << otmptSubscriptSize.y              << "\n";
   dump_context << "   otmptSubscriptOffset.x            = " << otmptSubscriptOffset.x            << "\n";
   dump_context << "   otmptSubscriptOffset.y            = " << otmptSubscriptOffset.y            << "\n";
   dump_context << "   otmptSuperscriptSize.x            = " << otmptSuperscriptSize.x            << "\n";
   dump_context << "   otmptSuperscriptSize.y            = " << otmptSuperscriptSize.y            << "\n";
   dump_context << "   otmptSuperscriptOffset.x          = " << otmptSuperscriptOffset.x          << "\n";
   dump_context << "   otmptSuperscriptOffset.y          = " << otmptSuperscriptOffset.y          << "\n";
   dump_context << "   otmsStrikeoutSize                 = " << otmsStrikeoutSize                 << "\n";
   dump_context << "   otmsStrikeoutPosition             = " << otmsStrikeoutPosition             << "\n";
   dump_context << "   otmsUnderscoreSize                = " << otmsUnderscoreSize                << "\n";
   dump_context << "   otmsUnderscorePosition            = " << otmsUnderscorePosition            << "\n";
   dump_context << "   otmpFamilyName                    = " << otmpFamilyName                    << "\n";
   dump_context << "   otmpFaceName                      = " << otmpFaceName                      << "\n";
   dump_context << "   otmpStyleName                     = " << otmpStyleName                     << "\n";
   dump_context << "   otmpFullName                      = " << otmpFullName                      << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void COutlineTextMetricW::Empty( void )
{
   otmSize = sizeof( _OUTLINETEXTMETRICW );

   // TEXTMETRICW
   otmTextMetrics.tmHeight           = 0;
   otmTextMetrics.tmAscent           = 0;
   otmTextMetrics.tmDescent          = 0;
   otmTextMetrics.tmInternalLeading  = 0;
   otmTextMetrics.tmExternalLeading  = 0;
   otmTextMetrics.tmAveCharWidth     = 0;
   otmTextMetrics.tmMaxCharWidth     = 0;
   otmTextMetrics.tmWeight           = 0;
   otmTextMetrics.tmOverhang         = 0;
   otmTextMetrics.tmDigitizedAspectX = 0;
   otmTextMetrics.tmDigitizedAspectY = 0;
   otmTextMetrics.tmFirstChar        = 0;
   otmTextMetrics.tmLastChar         = 0;
   otmTextMetrics.tmDefaultChar      = 0;
   otmTextMetrics.tmBreakChar        = 0;
   otmTextMetrics.tmItalic           = 0;
   otmTextMetrics.tmUnderlined       = 0;
   otmTextMetrics.tmStruckOut        = 0;
   otmTextMetrics.tmPitchAndFamily   = 0;
   otmTextMetrics.tmCharSet          = 0;
   
   otmFiller = 0;
   
   // PANOSE
   otmPanoseNumber.bFamilyType      = 0;
   otmPanoseNumber.bSerifStyle      = 0;
   otmPanoseNumber.bWeight          = 0;
   otmPanoseNumber.bProportion      = 0;
   otmPanoseNumber.bContrast        = 0;
   otmPanoseNumber.bStrokeVariation = 0;
   otmPanoseNumber.bArmStyle        = 0;
   otmPanoseNumber.bLetterform      = 0;
   otmPanoseNumber.bMidline         = 0;
   otmPanoseNumber.bXHeight         = 0;

   otmfsSelection    = 0;
   otmfsType         = 0;
   otmsCharSlopeRise = 0;
   otmsCharSlopeRun  = 0;
   otmItalicAngle    = 0;
   otmEMSquare       = 0;
   otmAscent         = 0;
   otmDescent        = 0;
   otmLineGap        = 0;
   otmsCapEmHeight   = 0;
   otmsXHeight       = 0;
   
   // RECT
   otmrcFontBox.left   = 0;
   otmrcFontBox.top    = 0;
   otmrcFontBox.right  = 0;
   otmrcFontBox.bottom = 0;

   otmMacAscent     = 0;
   otmMacDescent    = 0;
   otmMacLineGap    = 0;
   otmusMinimumPPEM = 0;

   // POINT
   otmptSubscriptSize.x     = 0;
   otmptSubscriptSize.y     = 0;
   otmptSubscriptOffset.x   = 0;
   otmptSubscriptOffset.y   = 0;
   otmptSuperscriptSize.x   = 0;
   otmptSuperscriptSize.y   = 0;
   otmptSuperscriptOffset.x = 0;
   otmptSuperscriptOffset.y = 0;

   otmsStrikeoutSize      = 0;
   otmsStrikeoutPosition  = 0;
   otmsUnderscoreSize     = 0;
   otmsUnderscorePosition = 0;
   otmpFamilyName         = NULL;
   otmpFaceName           = NULL;
   otmpStyleName          = NULL;
   otmpFullName           = NULL;
}

CPerfCounterDefinition::CPerfCounterDefinition()
{
   Empty();
}

CPerfCounterDefinition::~CPerfCounterDefinition()
{
   Empty();
}

void CPerfCounterDefinition::Empty( void )
{
   ByteLength            = sizeof( PERF_COUNTER_DEFINITION );
   CounterNameTitleIndex = 0;
   CounterNameTitle      = NULL;
   CounterHelpTitleIndex = 0;
   CounterHelpTitle      = NULL;
   DefaultScale          = 0;
   DetailLevel           = 0;
   CounterType           = 0;
   CounterSize           = 0;
   CounterOffset         = 0;
}

CPerfInstanceDefinition::CPerfInstanceDefinition()
{
   Empty();
}

CPerfInstanceDefinition::~CPerfInstanceDefinition()
{
   Empty();
}

void CPerfInstanceDefinition::Empty( void )
{
   ByteLength             = sizeof( PERF_INSTANCE_DEFINITION );
   ParentObjectTitleIndex = 0;
   ParentObjectInstance   = 0;
   UniqueID               = 0;
   NameOffset             = 0;
   NameLength             = 0;
}

CPixelFormatDescriptor::CPixelFormatDescriptor()
{
   Empty();
}

CPixelFormatDescriptor::CPixelFormatDescriptor( const CPixelFormatDescriptor& source )
{
   Empty();
   Copy( source );
}

CPixelFormatDescriptor::CPixelFormatDescriptor( const tagPIXELFORMATDESCRIPTOR * source )
{
   Empty();
   Copy( source );
}

CPixelFormatDescriptor::~CPixelFormatDescriptor()
{
   Empty();
}

void CPixelFormatDescriptor::Copy( const CPixelFormatDescriptor& source )
{
   Copy( (const tagPIXELFORMATDESCRIPTOR *) &source );
}

void CPixelFormatDescriptor::Copy( const tagPIXELFORMATDESCRIPTOR * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   nSize           = source->nSize;
   nVersion        = source->nVersion;
   dwFlags         = source->dwFlags;
   iPixelType      = source->iPixelType;
   cColorBits      = source->cColorBits;
   cRedBits        = source->cRedBits;
   cRedShift       = source->cRedShift;
   cGreenBits      = source->cGreenBits;
   cGreenShift     = source->cGreenShift;
   cBlueBits       = source->cBlueBits;
   cBlueShift      = source->cBlueShift;
   cAlphaBits      = source->cAlphaBits;
   cAlphaShift     = source->cAlphaShift;
   cAccumBits      = source->cAccumBits;
   cAccumRedBits   = source->cAccumRedBits;
   cAccumGreenBits = source->cAccumGreenBits;
   cAccumBlueBits  = source->cAccumBlueBits;
   cAccumAlphaBits = source->cAccumAlphaBits;
   cDepthBits      = source->cDepthBits;
   cStencilBits    = source->cStencilBits;
   cAuxBuffers     = source->cAuxBuffers;
   iLayerType      = source->iLayerType;
   bReserved       = source->bReserved;
   dwLayerMask     = source->dwLayerMask;
   dwVisibleMask   = source->dwVisibleMask;
   dwDamageMask    = source->dwDamageMask;
}

#if defined( _DEBUG )

void CPixelFormatDescriptor::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CPixelFormatDescriptor at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   nSize           = " << nSize           << "\n";
   dump_context << "   nVersion        = " << nVersion        << "\n";
   dump_context << "   dwFlags         = " << dwFlags         << "\n";
   dump_context << "   iPixelType      = " << iPixelType      << "\n";
   dump_context << "   cColorBits      = " << cColorBits      << "\n";
   dump_context << "   cRedBits        = " << cRedBits        << "\n";
   dump_context << "   cRedShift       = " << cRedShift       << "\n";
   dump_context << "   cGreenBits      = " << cGreenBits      << "\n";
   dump_context << "   cGreenShift     = " << cGreenShift     << "\n";
   dump_context << "   cBlueBits       = " << cBlueBits       << "\n";
   dump_context << "   cBlueShift      = " << cBlueShift      << "\n";
   dump_context << "   cAlphaBits      = " << cAlphaBits      << "\n";
   dump_context << "   cAlphaShift     = " << cAlphaShift     << "\n";
   dump_context << "   cAccumBits      = " << cAccumBits      << "\n";
   dump_context << "   cAccumRedBits   = " << cAccumRedBits   << "\n";
   dump_context << "   cAccumGreenBits = " << cAccumGreenBits << "\n";
   dump_context << "   cAccumBlueBits  = " << cAccumBlueBits  << "\n";
   dump_context << "   cAccumAlphaBits = " << cAccumAlphaBits << "\n";
   dump_context << "   cDepthBits      = " << cDepthBits      << "\n";
   dump_context << "   cStencilBits    = " << cStencilBits    << "\n";
   dump_context << "   cAuxBuffers     = " << cAuxBuffers     << "\n";
   dump_context << "   iLayerType      = " << iLayerType      << "\n";
   dump_context << "   bReserved       = " << bReserved       << "\n";
   dump_context << "   dwLayerMask     = " << dwLayerMask     << "\n";
   dump_context << "   dwVisibleMask   = " << dwVisibleMask   << "\n";
   dump_context << "   dwDamageMask    = " << dwDamageMask    << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CPixelFormatDescriptor::Empty( void )
{
   nSize           = sizeof( tagPIXELFORMATDESCRIPTOR );
   nVersion        = 0;
   dwFlags         = 0;
   iPixelType      = 0;
   cColorBits      = 0;
   cRedBits        = 0;
   cRedShift       = 0;
   cGreenBits      = 0;
   cGreenShift     = 0;
   cBlueBits       = 0;
   cBlueShift      = 0;
   cAlphaBits      = 0;
   cAlphaShift     = 0;
   cAccumBits      = 0;
   cAccumRedBits   = 0;
   cAccumGreenBits = 0;
   cAccumBlueBits  = 0;
   cAccumAlphaBits = 0;
   cDepthBits      = 0;
   cStencilBits    = 0;
   cAuxBuffers     = 0;
   iLayerType      = 0;
   bReserved       = 0;
   dwLayerMask     = 0;
   dwVisibleMask   = 0;
   dwDamageMask    = 0;
}

CRasterizerStatus::CRasterizerStatus()
{
   Empty();
}

CRasterizerStatus::~CRasterizerStatus()
{
   Empty();
}

void CRasterizerStatus::Empty( void )
{
   nSize       = sizeof( _RASTERIZER_STATUS );
   wFlags      = 0;
   nLanguageID = 0;
}

CSecurityAttributes::CSecurityAttributes()
{
   Empty();
}

CSecurityAttributes::~CSecurityAttributes()
{
   Empty();
}

void CSecurityAttributes::Empty( void )
{
   nLength              = sizeof( SECURITY_ATTRIBUTES );
   lpSecurityDescriptor = NULL;
   bInheritHandle       = FALSE;
}

CSecurityQualityOfService::CSecurityQualityOfService()
{
   Empty();
}

CSecurityQualityOfService::~CSecurityQualityOfService()
{
   Empty();
}

void CSecurityQualityOfService::Empty( void )
{
   Length = sizeof( SECURITY_QUALITY_OF_SERVICE );

   // SECURITY_IMPERSONATION_LEVEL
   ImpersonationLevel = SecurityAnonymous;
   
   // SECURITY_CONTEXT_TRACKING_MODE
   ContextTrackingMode = FALSE;

   //BOOLEAN
   EffectiveOnly = FALSE;
}

/*
** CSystemAuditEntry
*/

CSystemAuditEntry::CSystemAuditEntry()
{
   Empty();
}

CSystemAuditEntry::CSystemAuditEntry( const CSystemAuditEntry& source )
{
   Empty();
   Copy( source );
}

CSystemAuditEntry::CSystemAuditEntry( const _SYSTEM_AUDIT_ACE * source )
{
   Empty();
   Copy( source );
}

CSystemAuditEntry::~CSystemAuditEntry()
{
   Empty();
}

void CSystemAuditEntry::Copy( const CSystemAuditEntry& source )
{
   Copy( (const _SYSTEM_AUDIT_ACE *) &source );
}

void CSystemAuditEntry::Copy( const _SYSTEM_AUDIT_ACE * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   // ACE_HEADER
   Header.AceType  = source->Header.AceType;
   Header.AceFlags = source->Header.AceFlags;
   Header.AceSize  = source->Header.AceSize;

   // ACCESS_MASK
   Mask            = source->Mask;
   SidStart        = source->SidStart;
}

#if defined( _DEBUG )

void CSystemAuditEntry::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CSystemAuditEntry at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   Header.AceType  = " << Header.AceType  << "\n";
   dump_context << "   Header.AceFlags = " << Header.AceFlags << "\n";
   dump_context << "   Header.AceSize  = " << Header.AceSize  << "\n";
   dump_context << "   Mask            = " << Mask            << "\n";
   dump_context << "   SidStart        = " << SidStart        << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CSystemAuditEntry::Empty( void )
{
   // ACE_HEADER
   Header.AceType  = 0;
   Header.AceFlags = 0;
   Header.AceSize  = 0;

   // ACCESS_MASK
   Mask            = 0;

   SidStart        = 0;
}

/*
** CTextMetricA
*/

CTextMetricA::CTextMetricA()
{
   Empty();
}

CTextMetricA::CTextMetricA( const CTextMetricA& source )
{
   Empty();
   Copy( source );
}

CTextMetricA::CTextMetricA( const tagTEXTMETRICA * source )
{
   Empty();
   Copy( source );
}

CTextMetricA::~CTextMetricA()
{
   Empty();
}

void CTextMetricA::Copy( const CTextMetricA& source )
{
   Copy( (const tagTEXTMETRICA *) &source );
}

void CTextMetricA::Copy( const tagTEXTMETRICA * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   tmHeight           = source->tmHeight;
   tmAscent           = source->tmAscent;
   tmDescent          = source->tmDescent;
   tmInternalLeading  = source->tmInternalLeading;
   tmExternalLeading  = source->tmExternalLeading;
   tmAveCharWidth     = source->tmAveCharWidth;
   tmMaxCharWidth     = source->tmMaxCharWidth;
   tmWeight           = source->tmWeight;
   tmOverhang         = source->tmOverhang;
   tmDigitizedAspectX = source->tmDigitizedAspectX;
   tmDigitizedAspectY = source->tmDigitizedAspectY;
   tmFirstChar        = source->tmFirstChar;
   tmLastChar         = source->tmLastChar;
   tmDefaultChar      = source->tmDefaultChar;
   tmBreakChar        = source->tmBreakChar;
   tmItalic           = source->tmItalic;
   tmUnderlined       = source->tmUnderlined;
   tmStruckOut        = source->tmStruckOut;
   tmPitchAndFamily   = source->tmPitchAndFamily;
   tmCharSet          = source->tmCharSet;
}

#if defined( _DEBUG )

void CTextMetricA::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CTextMetricA at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   tmHeight           = " << tmHeight           << "\n";
   dump_context << "   tmAscent           = " << tmAscent           << "\n";
   dump_context << "   tmDescent          = " << tmDescent          << "\n";
   dump_context << "   tmInternalLeading  = " << tmInternalLeading  << "\n";
   dump_context << "   tmExternalLeading  = " << tmExternalLeading  << "\n";
   dump_context << "   tmAveCharWidth     = " << tmAveCharWidth     << "\n";
   dump_context << "   tmMaxCharWidth     = " << tmMaxCharWidth     << "\n";
   dump_context << "   tmWeight           = " << tmWeight           << "\n";
   dump_context << "   tmOverhang         = " << tmOverhang         << "\n";
   dump_context << "   tmDigitizedAspectX = " << tmDigitizedAspectX << "\n";
   dump_context << "   tmDigitizedAspectY = " << tmDigitizedAspectY << "\n";
   dump_context << "   tmFirstChar        = " << tmFirstChar        << "\n";
   dump_context << "   tmLastChar         = " << tmLastChar         << "\n";
   dump_context << "   tmDefaultChar      = " << tmDefaultChar      << "\n";
   dump_context << "   tmBreakChar        = " << tmBreakChar        << "\n";
   dump_context << "   tmItalic           = " << tmItalic           << "\n";
   dump_context << "   tmUnderlined       = " << tmUnderlined       << "\n";
   dump_context << "   tmStruckOut        = " << tmStruckOut        << "\n";
   dump_context << "   tmPitchAndFamily   = " << tmPitchAndFamily   << "\n";
   dump_context << "   tmCharSet          = " << tmCharSet          << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CTextMetricA::Empty( void )
{
   tmHeight           = 0;
   tmAscent           = 0;
   tmDescent          = 0;
   tmInternalLeading  = 0;
   tmExternalLeading  = 0;
   tmAveCharWidth     = 0;
   tmMaxCharWidth     = 0;
   tmWeight           = 0;
   tmOverhang         = 0;
   tmDigitizedAspectX = 0;
   tmDigitizedAspectY = 0;
   tmFirstChar        = 0;
   tmLastChar         = 0;
   tmDefaultChar      = 0;
   tmBreakChar        = 0;
   tmItalic           = 0;
   tmUnderlined       = 0;
   tmStruckOut        = 0;
   tmPitchAndFamily   = 0;
   tmCharSet          = 0;
}

/*
** CTextMetricW
*/

CTextMetricW::CTextMetricW()
{
   Empty();
}

CTextMetricW::CTextMetricW( const CTextMetricW& source )
{
   Empty();
   Copy( source );
}

CTextMetricW::CTextMetricW( const tagTEXTMETRICW * source )
{
   Empty();
   Copy( source );
}

CTextMetricW::~CTextMetricW()
{
   Empty();
}

void CTextMetricW::Copy( const CTextMetricW& source )
{
   Copy( (const tagTEXTMETRICW *) &source );
}

void CTextMetricW::Copy( const tagTEXTMETRICW * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   tmHeight           = source->tmHeight;
   tmAscent           = source->tmAscent;
   tmDescent          = source->tmDescent;
   tmInternalLeading  = source->tmInternalLeading;
   tmExternalLeading  = source->tmExternalLeading;
   tmAveCharWidth     = source->tmAveCharWidth;
   tmMaxCharWidth     = source->tmMaxCharWidth;
   tmWeight           = source->tmWeight;
   tmOverhang         = source->tmOverhang;
   tmDigitizedAspectX = source->tmDigitizedAspectX;
   tmDigitizedAspectY = source->tmDigitizedAspectY;
   tmFirstChar        = source->tmFirstChar;
   tmLastChar         = source->tmLastChar;
   tmDefaultChar      = source->tmDefaultChar;
   tmBreakChar        = source->tmBreakChar;
   tmItalic           = source->tmItalic;
   tmUnderlined       = source->tmUnderlined;
   tmStruckOut        = source->tmStruckOut;
   tmPitchAndFamily   = source->tmPitchAndFamily;
   tmCharSet          = source->tmCharSet;
}

#if defined( _DEBUG )

void CTextMetricW::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CTextMetricW at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   tmHeight           = " << tmHeight           << "\n";
   dump_context << "   tmAscent           = " << tmAscent           << "\n";
   dump_context << "   tmDescent          = " << tmDescent          << "\n";
   dump_context << "   tmInternalLeading  = " << tmInternalLeading  << "\n";
   dump_context << "   tmExternalLeading  = " << tmExternalLeading  << "\n";
   dump_context << "   tmAveCharWidth     = " << tmAveCharWidth     << "\n";
   dump_context << "   tmMaxCharWidth     = " << tmMaxCharWidth     << "\n";
   dump_context << "   tmWeight           = " << tmWeight           << "\n";
   dump_context << "   tmOverhang         = " << tmOverhang         << "\n";
   dump_context << "   tmDigitizedAspectX = " << tmDigitizedAspectX << "\n";
   dump_context << "   tmDigitizedAspectY = " << tmDigitizedAspectY << "\n";
   dump_context << "   tmFirstChar        = " << tmFirstChar        << "\n";
   dump_context << "   tmLastChar         = " << tmLastChar         << "\n";
   dump_context << "   tmDefaultChar      = " << tmDefaultChar      << "\n";
   dump_context << "   tmBreakChar        = " << tmBreakChar        << "\n";
   dump_context << "   tmItalic           = " << tmItalic           << "\n";
   dump_context << "   tmUnderlined       = " << tmUnderlined       << "\n";
   dump_context << "   tmStruckOut        = " << tmStruckOut        << "\n";
   dump_context << "   tmPitchAndFamily   = " << tmPitchAndFamily   << "\n";
   dump_context << "   tmCharSet          = " << tmCharSet          << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CTextMetricW::Empty( void )
{
   tmHeight           = 0;
   tmAscent           = 0;
   tmDescent          = 0;
   tmInternalLeading  = 0;
   tmExternalLeading  = 0;
   tmAveCharWidth     = 0;
   tmMaxCharWidth     = 0;
   tmWeight           = 0;
   tmOverhang         = 0;
   tmDigitizedAspectX = 0;
   tmDigitizedAspectY = 0;
   tmFirstChar        = 0;
   tmLastChar         = 0;
   tmDefaultChar      = 0;
   tmBreakChar        = 0;
   tmItalic           = 0;
   tmUnderlined       = 0;
   tmStruckOut        = 0;
   tmPitchAndFamily   = 0;
   tmCharSet          = 0;
}

CWindowPlacement::CWindowPlacement()
{
   Empty();
}

CWindowPlacement::CWindowPlacement( const CWindowPlacement& source )
{
   Empty();
   Copy( source );
}

CWindowPlacement::CWindowPlacement( const tagWINDOWPLACEMENT * source )
{
   Empty();
   Copy( source );
}

CWindowPlacement::~CWindowPlacement()
{
   Empty();
}

void CWindowPlacement::Copy( const CWindowPlacement& source )
{
   Copy( (const tagWINDOWPLACEMENT *) &source );
}

void CWindowPlacement::Copy( const tagWINDOWPLACEMENT * source )
{
   ASSERT( source != NULL );

   if ( source == NULL )
   {
      return;
   }

   length  = source->length;
   flags   = source->flags;
   showCmd = source->showCmd;

   // POINT
   ptMinPosition.x = source->ptMinPosition.x;
   ptMinPosition.y = source->ptMinPosition.y;
   ptMaxPosition.x = source->ptMaxPosition.x;
   ptMaxPosition.y = source->ptMaxPosition.y;

   // RECT
   rcNormalPosition.left   = source->rcNormalPosition.left;
   rcNormalPosition.top    = source->rcNormalPosition.top;
   rcNormalPosition.right  = source->rcNormalPosition.right;
   rcNormalPosition.bottom = source->rcNormalPosition.bottom;
}

#if defined( _DEBUG )

void CWindowPlacement::Dump( CDumpContext& dump_context ) const
{
   dump_context << " a CWindowPlacement at " << (void *) this << "\n";
   dump_context << "{\n";
   dump_context << "   length                  = " << length                  << "\n";
   dump_context << "   flags                   = " << flags                   << "\n";
   dump_context << "   showCmd                 = " << showCmd                 << "\n";
   dump_context << "   ptMinPosition.x         = " << ptMinPosition.x         << "\n";
   dump_context << "   ptMinPosition.y         = " << ptMinPosition.y         << "\n";
   dump_context << "   ptMaxPosition.x         = " << ptMaxPosition.x         << "\n";
   dump_context << "   ptMaxPosition.y         = " << ptMaxPosition.y         << "\n";
   dump_context << "   rcNormalPosition.left   = " << rcNormalPosition.left   << "\n";
   dump_context << "   rcNormalPosition.top    = " << rcNormalPosition.top    << "\n";
   dump_context << "   rcNormalPosition.right  = " << rcNormalPosition.right  << "\n";
   dump_context << "   rcNormalPosition.bottom = " << rcNormalPosition.bottom << "\n";
   dump_context << "}\n";
}

#endif // _DEBUG

void CWindowPlacement::Empty( void )
{
   length  = sizeof( tagWINDOWPLACEMENT );
   flags   = 0;
   showCmd = 0;

   // POINT
   ptMinPosition.x = 0;
   ptMinPosition.y = 0;
   ptMaxPosition.x = 0;
   ptMaxPosition.y = 0;

   // RECT
   rcNormalPosition.left   = 0;
   rcNormalPosition.top    = 0;
   rcNormalPosition.right  = 0;
   rcNormalPosition.bottom = 0;
}

#if 0

#if defined( _WINUSER_ )

class CFilterKeys : public tagFILTERKEYS
{
   public:

      CFilterKeys()
      {
         ::ZeroMemory( (tagFILTERKEYS *) this, sizeof( tagFILTERKEYS ) );
         cbSize = sizeof( tagFILTERKEYS );
      }

     ~CFilterKeys()
      {
         ::ZeroMemory( (tagFILTERKEYS *) this, sizeof( tagFILTERKEYS ) );
      }
};

class CStickyKeys : public tagSTICKYKEYS
{
   public:

      CStickyKeys()
      {
         ::ZeroMemory( (tagSTICKYKEYS *) this, sizeof( tagSTICKYKEYS ) );
         cbSize = sizeof( tagSTICKYKEYS );
      }

     ~CStickyKeys()
      {
         ::ZeroMemory( (tagSTICKYKEYS *) this, sizeof( tagSTICKYKEYS ) );
      }
};

class CMouseKeys : public tagMOUSEKEYS
{
   public:

      CMouseKeys()
      {
         ::ZeroMemory( (tagMOUSEKEYS *) this, sizeof( tagMOUSEKEYS ) );
         cbSize = sizeof( tagMOUSEKEYS );
      }

     ~CMouseKeys()
      {
         ::ZeroMemory( (tagMOUSEKEYS *) this, sizeof( tagMOUSEKEYS ) );
      }
};

class CToggleKeys : public tagTOGGLEKEYS
{
   public:

      CToggleKeys()
      {
         ::ZeroMemory( (tagTOGGLEKEYS *) this, sizeof( tagTOGGLEKEYS ) );
         cbSize = sizeof( tagTOGGLEKEYS );
      }

     ~CToggleKeys()
      {
         ::ZeroMemory( (tagTOGGLEKEYS *) this, sizeof( tagTOGGLEKEYS ) );
      }
};

class CAccessTimeout : public tagACCESSTIMEOUT
{
   public:

      CAccessTimeout()
      {
         ::ZeroMemory( (tagACCESSTIMEOUT *) this, sizeof( tagACCESSTIMEOUT ) );
         cbSize = sizeof( tagACCESSTIMEOUT );
      }

     ~CAccessTimeout()
      {
         ::ZeroMemory( (tagACCESSTIMEOUT *) this, sizeof( tagACCESSTIMEOUT ) );
      }
};

class CSoundSentryA : public tagSOUNDSENTRYA
{
   public:

      CSoundSentryA()
      {
         ::ZeroMemory( (tagSOUNDSENTRYA *) this, sizeof( tagSOUNDSENTRYA ) );
         cbSize = sizeof( tagSOUNDSENTRYA );
      }

     ~CSoundSentryA()
      {
         ::ZeroMemory( (tagSOUNDSENTRYA *) this, sizeof( tagSOUNDSENTRYA ) );
      }
};

class CSoundSentryW : public tagSOUNDSENTRYW
{
   public:

      CSoundSentryW()
      {
         ::ZeroMemory( (tagSOUNDSENTRYW *) this, sizeof( tagSOUNDSENTRYW ) );
         cbSize = sizeof( tagSOUNDSENTRYW );
      }

     ~CSoundSentryW()
      {
         ::ZeroMemory( (tagSOUNDSENTRYW *) this, sizeof( tagSOUNDSENTRYW ) );
      }
};

#if defined( UNICODE )
#define CSoundSentry CSoundSentryW
#else
#define CSoundSentry CSoundSentryA
#endif // UNICODE

#endif // _WINUSER_

#if defined( _INC_VFW )

class CCompVars : public COMPVARS
{
   public:

      CCompVars()
      {
         ::ZeroMemory( (COMPVARS *) this, sizeof( COMPVARS ) );
         cbSize = sizeof( COMPVARS );
      }

     ~CCompVars()
      {
         ::ZeroMemory( (COMPVARS *) this, sizeof( COMPVARS ) );
      }
};

#endif // _INC_VFW

#if defined( _WINGDI_ )

class CDocInfoA : public _DOCINFOA
{
   public:

      CDocInfoA()
      {
         ::ZeroMemory( (LPDOCINFOA) this, sizeof( DOCINFOA ) );
         cbSize = sizeof( DOCINFOA );
      }

     ~CDocInfoA()
      {
         ::ZeroMemory( (LPDOCINFOA) this, sizeof( DOCINFOA ) );
      }
};

class CDocInfoW : public _DOCINFOW
{
   public:

      CDocInfoW()
      {
         ::ZeroMemory( (LPDOCINFOW) this, sizeof( DOCINFOW ) );
         cbSize = sizeof( DOCINFOW );
      }

     ~CDocInfoW()
      {
         ::ZeroMemory( (LPDOCINFOW) this, sizeof( DOCINFOW ) );
      }
};

#if defined( UNICODE )
#define CDocInfo CDocInfoW
#else
#define CDocInfo CDocInfoA
#endif // UNICODE

#endif // _WINGDI_

#endif // 0
