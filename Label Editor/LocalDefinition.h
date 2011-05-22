/*/
	LocalDefinition.h (2005.07.09)
/*/
#pragma once

typedef struct structLabelItem
{
	unsigned char cLength_id;
	LPTSTR szID;
	unsigned char cLength_label;
	LPTSTR szLabel;
//	unsigned char cLength_type;
//	LPTSTR szType;
	unsigned char cChecksum;
} STRUCT_LABELITEM, FAR* LPSTRUCT_LABELITEM;