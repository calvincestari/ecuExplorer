/*/
	DTCDefinition.h (2005.08.02)
/*/
#pragma once

typedef struct structDTCItem
{
	unsigned char cLength_code;
	LPTSTR szCode;
	unsigned char cLength_description;
	LPTSTR szDescription;
	unsigned long ulCurrent;
	unsigned long ulHistoric;
	unsigned char cBit;
	unsigned char cChecksum;
} STRUCT_DTCITEM, FAR* LPSTRUCT_DTCITEM;