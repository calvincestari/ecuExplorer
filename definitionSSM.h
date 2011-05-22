/*/
	definitionSSM.h (2005.06.29)
/*/
#pragma once

/*/
	INFORMATION ELEMENT DEFINITIONS
/*/
#define IE_SSM_HEADER									0x80
#define IE_SSM_SSM										0x10
#define IE_SSM_ECU										0xF0

/*/
	MESSAGE DEFINITIONS
/*/
#define MSG_SSM_ReadAddressBlock						0xA0
#define MSG_SSM_ReadAddressSingle						0xA8
#define MSG_SSM_WriteAddressBlock						0xB0
#define MSG_SSM_WriteAddressSingle						0xB8
#define MSG_SSM_QueryECUType							0xBF
#define MSG_SSM_ReadAddressBlock_Response				0xE0
#define MSG_SSM_ReadAddressSingle_Response				0xE8
#define MSG_SSM_WriteAddressSingle_Response				0xF8
#define MSG_SSM_QueryECUType_Response					0xFF

/*/
	ADDRESS DEFINITIONS
/*/
#define ADDRESS_RESET_ECU								0x00000060
#define ADDRESS_IGNITION_RETARD							0x0000006F
#define ADDRESS_IDLE_ADJUST_NORMAL						0x00000070
#define ADDRESS_IDLE_ADJUST_AIRCON						0x00000071

#define ADDRESS_DTC_CURRENT_99_00_START					0x0000008E
#define ADDRESS_DTC_CURRENT_99_00_END					0x00000097
#define ADDRESS_DTC_HISTORIC_99_00_START				0x000000A4
#define ADDRESS_DTC_HISTORIC_99_00_END					0x000000AD

#define ADDRESS_DTC_CURRENT_01_03_START_1				0x0000008E
#define ADDRESS_DTC_CURRENT_01_03_END_1					0x000000AD
#define ADDRESS_DTC_CURRENT_01_03_START_2				0x000000F0
#define ADDRESS_DTC_CURRENT_01_03_END_2					0x000000F3
#define ADDRESS_DTC_HISTORIC_01_03_START_1				0x000000AE
#define ADDRESS_DTC_HISTORIC_01_03_END_1				0x000000CD
#define ADDRESS_DTC_HISTORIC_01_03_START_2				0x000000F4
#define ADDRESS_DTC_HISTORIC_01_03_END_2				0x000000F7

#define ADDRESS_DTC_CURRENT_04_05_START_1				0x00000123
#define ADDRESS_DTC_CURRENT_04_05_END_1					0x0000012A
#define ADDRESS_DTC_CURRENT_04_05_START_2				0x00000150
#define ADDRESS_DTC_CURRENT_04_05_END_2					0x00000154
#define ADDRESS_DTC_CURRENT_04_05_START_3				0x00000160
#define ADDRESS_DTC_CURRENT_04_05_END_3					0x00000164
#define ADDRESS_DTC_HISTORIC_04_05_START_1				0x0000012B
#define ADDRESS_DTC_HISTORIC_04_05_END_1				0x00000132
#define ADDRESS_DTC_HISTORIC_04_05_START_2				0x00000155
#define ADDRESS_DTC_HISTORIC_04_05_END_2				0x00000159
#define ADDRESS_DTC_HISTORIC_04_05_START_3				0x00000165
#define ADDRESS_DTC_HISTORIC_04_05_END_3				0x00000169

/*/ DEVELOPERS NOTE /*/
//
// The following compiler directive is VERY IMPORTANT - DO NOT REMOVE !!!
// By default all packing alignment is 8 bits.  The following directive ensures
// that the structure elements follow straight after each other, ie 1 bit packing.
// If we do not tell the compiler to do the following we will end up with some very
// strange structure behaviour.
#pragma pack(1)

/*/
	INFORMATION ELEMENT STRUCTURES
/*/

typedef struct struct_IE_SSM_MessageHeader
{
	unsigned char										cHeaderIdentifier;
	unsigned char										cSource;
	unsigned char										cDestination;
	unsigned char										cDataLength;
} STRUCT_IE_SSM_MESSAGEHEADER, FAR* LPSTRUCT_IE_SSM_MESSAGEHEADER;

typedef struct struct_IE_SSM_MemoryAddress
{
	unsigned char										cAddress[3];
} STRUCT_IE_SSM_MEMORYADDRESS, FAR* LPSTRUCT_IE_SSM_MEMORYADDRESS;

/*/
	MESSAGE STRUCTURES
/*/

typedef struct struct_MSG_SSM_QUERY
{
	STRUCT_IE_SSM_MESSAGEHEADER							Header;
	unsigned char										cMessageType;
	unsigned char										cChecksum;
} STRUCT_MSG_SSM_QUERY, FAR* LPSTRUCT_MSG_SSM_QUERY;

typedef struct struct_MSG_SSM_QUERYRESPONSE
{
	STRUCT_IE_SSM_MESSAGEHEADER							Header;
	unsigned char										cMessageType;
	unsigned char										cUnknown[3];
	unsigned char										cID[5];
	unsigned char										cSupported;
} STRUCT_MSG_SSM_QUERYRESPONSE, FAR* LPSTRUCT_MSG_SSM_QUERYRESPONSE;

typedef struct struct_MSG_SSM_ReadAddressSingle
{
	STRUCT_IE_SSM_MESSAGEHEADER							Header;
	unsigned char										cMessageType;
	unsigned char										cPad;
	unsigned char										cAddress[3];
	unsigned char										cChecksum;
} STRUCT_MSG_SSM_READADDRESSSINGLE, FAR* LPSTRUCT_MSG_SSM_READADDRESSSINGLE;

typedef struct struct_MSG_SSM_WriteAddressSingle
{
	STRUCT_IE_SSM_MESSAGEHEADER							Header;
	unsigned char										cMessageType;
	unsigned char										cAddress[3];
	unsigned char										cValue;
	unsigned char										cChecksum;
} STRUCT_MSG_SSM_WRITEADDRESSSINGLE, FAR* LPSTRUCT_MSG_SSM_WRITEADDRESSSINGLE;

typedef struct struct_MSG_SSM_LiveQuery
{
	STRUCT_IE_SSM_MESSAGEHEADER							Header;
	unsigned char										cMessageType;
	unsigned char										cPad;
} STRUCT_MSG_SSM_LIVEQUERY, FAR* LPSTRUCT_MSG_SSM_LIVEQUERY;

typedef struct struct_MSG_SSM_DTCQuery
{
	STRUCT_IE_SSM_MESSAGEHEADER							Header;
	unsigned char										cMessageType;
	unsigned char										cPad;
} STRUCT_MSG_SSM_DTCQUERY, FAR* LPSTRUCT_MSG_SSM_DTCQUERY;

typedef struct struct_MSG_SSM_ReadAddressSingleResponse
{
	STRUCT_IE_SSM_MESSAGEHEADER							Header;
	unsigned char										cMessageType;
} STRUCT_MSG_SSM_READADDRESSSINGLERESPONSE, FAR* LPSTRUCT_MSG_SSM_READADDRESSSINGLERESPONSE;

typedef struct struct_MSG_SSM_WriteAddressSingleResponse
{
	STRUCT_IE_SSM_MESSAGEHEADER							Header;
	unsigned char										cMessageType;
} STRUCT_MSG_SSM_WRITEADDRESSSINGLERESPONSE, FAR* LPSTRUCT_MSG_SSM_WRITEADDRESSSINGLERESPONSE;

typedef struct struct_MSG_SSM_LiveQueryResponse
{
	STRUCT_IE_SSM_MESSAGEHEADER							Header;
	unsigned char										cMessageType;
} STRUCT_MSG_SSM_LIVEQUERYRESPONSE, FAR* LPSTRUCT_MSG_SSM_LIVEQUERYRESPONSE;

typedef struct struct_MSG_SSM_DTCQueryResponse
{
	STRUCT_IE_SSM_MESSAGEHEADER							Header;
	unsigned char										cMessageType;
} STRUCT_MSG_SSM_DTCQUERYRESPONSE, FAR* LPSTRUCT_MSG_SSM_DTCQUERYRESPONSE;

/*/ DEVELOPERS NOTE /*/
//
// Restore the original packing method
#pragma pack()