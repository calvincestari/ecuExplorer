/*/
	LocalDefinitions.h (2005.06.17)
/*/
#pragma once

#define INSTANCE_EVENTNAME				"TARI Instance Event"

#define SEMAPHORE_TIMEOUT				1250

#define APPVERSION_MAJOR				3
#define APPVERSION_MINOR				0
#define APPVERSION_BUILD				28

#define MESSAGETYPE_GENERAL				0
#define MESSAGETYPE_INFO				1
#define MESSAGETYPE_WARNING				2
#define MESSAGETYPE_ERROR				3

#define USERLEVEL_BEGINNER				1
#define USERLEVEL_INTERMEDIATE			2
#define USERLEVEL_ADVANCED				3

#define TYPE_TEMPERATURE				1
#define TYPE_PRESSURE					2
#define TYPE_AIRFUEL					3
#define TYPE_SPEED						4
#define TYPE_INJECTOR					5
#define TYPE_GENERAL					6
#define TYPE_SWITCH						7
#define TYPE_CORRECTED_BOOST			9

#define CONFIG_TRIGGERONDEFOGSWITCH		0x00000001
#define CONFIG_CONVERT_INJECTOR			0x00000002
#define CONFIG_CONVERT_TEMPERATURE		0x00000004
#define CONFIG_CONVERT_SPEED			0x00000008
#define CONFIG_CONVERT_PRESSURE			0x00000010
#define CONFIG_CONVERT_AIRFUEL			0x00000020
#define CONFIG_AUTOADDNEWLOGFILE		0x00000040
#define CONFIG_CAPTURESSMTRACEFILE		0x00000080
#define CONFIG_SHOWDEBUGCONSOLE			0x00000100
#define CONFIG_USERLEVEL_BEGINNER		0x00000200
#define CONFIG_USERLEVEL_INTERMEDIATE	0x00000400
#define CONFIG_USERLEVEL_ADVANCED		0x00000800
#define CONFIG_USEABSOLUTETIME			0x00001000

#define ECUFLASH_USDM_WRX_02_03			0x00000001
#define ECUFLASH_USDM_WRX_04_05			0x00000002
#define ECUFLASH_INTL_WRX_01_05			0x00000004
#define ECUFLASH_INTL_STI_01_05			0x00000008

#define NODE_DATAITEMS					0x01
#define NODE_TROUBLECODES				0x02
#define NODE_SAVEDLOGFILEPARENT			0x03
#define NODE_ECUPARENT					0x04
#define NODE_ECUREALTIME				0x05
#define NODE_ECUTROUBLECODEREADER		0x06
#define NODE_ROMTABLES					0x07

#define MESSAGE_UPDATENODETEXT			(WM_USER+1)
#define MESSAGE_PARSESUPPORTLIST		(WM_USER+2)
#define MESSAGE_UPDATELISTITEM			(WM_USER+3)
#define MESSAGE_UPDATECSVFILE			(WM_USER+4)
#define MESSAGE_UPDATECURRENTDTC		(WM_USER+5)
#define MESSAGE_UPDATEHISTORICDTC		(WM_USER+6)
#define MESSAGE_UPDATENODEDATA			(WM_USER+7)

#define TAB_FUEL						0
#define TAB_BOOST						1
#define TAB_TURBO						2
#define TAB_IGNITION					3
#define TAB_AVCS						4
#define TAB_MISCELLANEOUS				5

#define MAPSIZE_STANDARD				0x30000
#define MAPSIZE_COMPACT					0x28000
#define MAPSIZE_RAM						0x8000

#define MAPTYPE_AVCS					0x08
#define MAPTYPE_BOOST					0x09
#define MAPTYPE_TURBO					0x0A
#define MAPTYPE_FUEL					0x0C
#define MAPTYPE_IGNITION				0x0D
#define MAPTYPE_MISCELLANEOUS			0x0E

#define MAP_3D_y16x8d8					0x04
#define MAP_3D_y16x8d16					0x14
#define MAP_3D_y16x16d8					0x05
#define MAP_3D_y16x16d16				0x15

#define MAP_2D_y8d8						0x00
#define MAP_2D_y16d8					0x01
#define MAP_2D_y8d16					0x10
#define MAP_2D_y16d16					0x11

#define MAP_DATA_d8						0x90
#define MAP_DATA_d16					0x91

/*/ DEVELOPERS NOTE /*/
//
// The following compiler directive is VERY IMPORTANT - DO NOT REMOVE !!!
// By default all packing alignment is 8 bits.  The following directive ensures
// that the structure elements follow straight after each other, ie 1 bit packing.
// If we do not tell the compiler to do the following we will end up with some very
// strange structure behaviour.
#pragma pack(1)

typedef struct structLiveByteItem
{
	CPtrList listBitItems;
} STRUCT_LIVEBYTEITEM, FAR* LPSTRUCT_LIVEBYTEITEM;

typedef struct structLiveBitItem
{
	unsigned char cByte;
	unsigned char cBit;
	unsigned char cLength_name;
	LPTSTR szName;
	unsigned long ulAddress_high;
	unsigned long ulAddress_low;
	char cType;
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
	int iListIndex;
	BOOL bDisplay;
	float fValue;
} STRUCT_LIVEBITITEM, FAR* LPSTRUCT_LIVEBITITEM;

typedef struct structLabelItem
{
	unsigned char cLength_id;
	LPTSTR szID;
	unsigned char cLength_label;
	LPTSTR szLabel;
} STRUCT_LABELITEM, FAR* LPSTRUCT_LABELITEM;

typedef struct structDTCItem
{
	unsigned char cLength_id;
	LPTSTR szID;
	unsigned char cLength_label;
	LPTSTR szLabel;
	unsigned long ulAddress_current;
	unsigned long ulAddress_historic;
	unsigned char cCheckBit;
} STRUCT_DTCITEM, FAR* LPSTRUCT_DTCITEM;

typedef struct structLiveQueryItem
{
	unsigned long ulAddress_high;
	unsigned long ulAddress_low;
	unsigned int iCount;
	float fValue_raw;
	void *lpLiveBitItem;
} STRUCT_LIVEQUERYITEM, FAR* LPSTRUCT_LIVEQUERYITEM;

typedef struct structMapAxis
{
	unsigned char cLength_label;
	LPTSTR szLabel;
	unsigned char cLength_unit;
	LPTSTR szUnit;
	unsigned char cType_unit;
	double dblOperand_multiplier;
	double dblOperand_addition;
	unsigned char cDecimals;
} STRUCT_MAPAXIS, FAR* LPSTRUCT_MAPAXIS;

typedef struct structMapItem
{
	unsigned int iIndex_lookup;
	unsigned char cType;
	unsigned char cLength_label;
	LPTSTR szLabel;
	unsigned short usDataMap_NumberOfItems;
	unsigned short usDataMap_BlockLength;
	STRUCT_MAPAXIS axisX;
	STRUCT_MAPAXIS axisY;
	STRUCT_MAPAXIS axisZ;
	unsigned char cLength_help;
	LPTSTR szHelp;
} STRUCT_MAPITEM, FAR* LPSTRUCT_MAPITEM;

typedef struct structROMItem
{
	unsigned char cLength_revision;
	LPTSTR szRevision;
	unsigned char cLength_description;
	LPTSTR szDescription;
	CMapWordToPtr mapOffset;
} STRUCT_ROMITEM, FAR* LPSTRUCT_ROMITEM;

typedef struct structEcuFlashThread
{
	void* lpParent;
	void* lpDialog;
	int func;
	bool delay;
} STRUCT_ECUFLASHTHREAD, FAR* LPSTRUCT_ECUFLASHTHREAD;

typedef struct structNodeData
{
	void* lpNode;
	int iView;
	int iType;
} STRUCT_NODEDATA, FAR* LPSTRUCT_NODEDATA;

typedef struct structExternalInput
{
	CString sName;
	HANDLE hDll;
	CString sDll;
} STRUCT_EXTERNALINPUT, FAR* LPSTRUCT_EXTERNALINPUT;

/*/ DEVELOPERS NOTE /*/
//
// Restore the original packing method
#pragma pack()
