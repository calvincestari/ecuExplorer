/*/
	LocalDefinition.h (2005.08.11)
/*/
#pragma once

typedef struct structDataItem
{
	unsigned char cByte;
	unsigned char cBit;
	unsigned char cLength_name;
	LPTSTR szName;
	unsigned long ulAddressHigh;
	unsigned long ulAddressLow;
	unsigned char cType;
	unsigned short usOperand_addition;
	unsigned short usOperand_subtract;
	unsigned short usOperand_multiplier;
	unsigned short usOperand_divisor;
	unsigned short usDecimals;
	unsigned char cLength_unit;
	LPTSTR szUnit;
	unsigned short usLength_description;
	LPTSTR szDescription;
	unsigned char cChecksum;
} STRUCT_DATAITEM, FAR* LPSTRUCT_DATAITEM;