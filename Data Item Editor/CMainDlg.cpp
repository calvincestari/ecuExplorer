/*/
	CMainDlg.cpp (2005.08.02)
/*/

#include "CMainDlg.h"
#include "LocalDefinition.h"

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
END_MESSAGE_MAP()

CMainDlg::CMainDlg() : CDialog(CMainDlg::IDD)
{
	listDataItem.RemoveAll();
	pos = NULL;
	usIndex = 0;
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_BYTE,sByte);
	DDX_Text(pDX,IDC_BIT,sBit);
	DDX_Text(pDX,IDC_NAME,sName);
	DDX_Text(pDX,IDC_CHECKSUM,sChecksum);
	DDX_Text(pDX,IDC_VERSION,sVersion);
	DDX_Text(pDX,IDC_UNIT,sUnit);
	DDX_Text(pDX,IDC_ADDRESS_HIGH,sAddress_high);
	DDX_Text(pDX,IDC_ADDRESS_LOW,sAddress_low);
	DDX_Text(pDX,IDC_TYPE,sType);
	DDX_Text(pDX,IDC_OPERAND_ADDITION,sOperand_addition);
	DDX_Text(pDX,IDC_OPERAND_SUBTRACT,sOperand_subtract);
	DDX_Text(pDX,IDC_OPERAND_MULTIPLIER,sOperand_multiplier);
	DDX_Text(pDX,IDC_OPERAND_DIVIDE,sOperand_divisor);
	DDX_Text(pDX,IDC_DECIMALS,sDecimals);
	DDX_Text(pDX,IDC_ITEM_DESCRIPTION,sDescription);
}

void CMainDlg::OnCancel()
{
	LPSTRUCT_DATAITEM lpDataItem = NULL;

	while(listDataItem.GetCount() > 0)
	{
		lpDataItem = (LPSTRUCT_DATAITEM)listDataItem.RemoveHead();
		free(lpDataItem->szName);
		free(lpDataItem->szUnit);
		free(lpDataItem->szDescription);
		delete lpDataItem;
	}

	CDialog::OnCancel();
}

void CMainDlg::OnBack()
{
	CString sText;
	LPSTRUCT_DATAITEM lpDataItem = NULL;

	UpdateData(TRUE);
	lpDataItem = (LPSTRUCT_DATAITEM)listDataItem.GetAt(pos);
	if(sName != lpDataItem->szName)
	{
		free(lpDataItem->szName);
		lpDataItem->cLength_name = sName.GetLength();
		lpDataItem->szName = (LPTSTR)malloc(lpDataItem->cLength_name + 1);
		memset(lpDataItem->szName,0,lpDataItem->cLength_name);
		strncpy(lpDataItem->szName,sName,lpDataItem->cLength_name);
		lpDataItem->szName[lpDataItem->cLength_name] = '\0';

		GenerateChecksum(lpDataItem);
	}
	if(sUnit != lpDataItem->szUnit)
	{
		free(lpDataItem->szUnit);
		lpDataItem->cLength_unit = sUnit.GetLength();
		lpDataItem->szUnit = (LPTSTR)malloc(lpDataItem->cLength_unit + 1);
		memset(lpDataItem->szUnit,0,lpDataItem->cLength_unit);
		strncpy(lpDataItem->szUnit,sUnit,lpDataItem->cLength_unit);
		lpDataItem->szUnit[lpDataItem->cLength_unit] = '\0';

		GenerateChecksum(lpDataItem);
	}
	if(sDescription != lpDataItem->szDescription)
	{
		free(lpDataItem->szDescription);
		lpDataItem->usLength_description = sDescription.GetLength();
		lpDataItem->szDescription = (LPTSTR)malloc(lpDataItem->usLength_description + 1);
		memset(lpDataItem->szDescription,0,lpDataItem->usLength_description);
		strncpy(lpDataItem->szDescription,sDescription,lpDataItem->usLength_description);
		lpDataItem->szDescription[lpDataItem->usLength_description] = '\0';

		GenerateChecksum(lpDataItem);
	}
	lpDataItem->cType = atoi(sType);
	lpDataItem->usOperand_addition = atoi(sOperand_addition);
	lpDataItem->usOperand_subtract = atoi(sOperand_subtract);
	lpDataItem->usOperand_multiplier = atoi(sOperand_multiplier);
	lpDataItem->usOperand_divisor = atoi(sOperand_divisor);
	lpDataItem->usDecimals = atoi(sDecimals);

	sName.Empty();
	sUnit.Empty();
	sDescription.Empty();

	listDataItem.GetPrev(pos);
	if(pos == NULL)
	{
		pos = listDataItem.GetHeadPosition();
		return;
	}

	lpDataItem = (LPSTRUCT_DATAITEM)listDataItem.GetAt(pos);
	sByte.Format("%i",lpDataItem->cByte);
	sBit.Format("%i",lpDataItem->cBit);
	sName = lpDataItem->szName;
	sAddress_high.Format("0x%X",lpDataItem->ulAddressHigh);
	sAddress_low.Format("0x%X",lpDataItem->ulAddressLow);
	sType.Format("%i",lpDataItem->cType);
	sOperand_addition.Format("%i",lpDataItem->usOperand_addition);
	sOperand_subtract.Format("%i",lpDataItem->usOperand_subtract);
	sOperand_multiplier.Format("%i",lpDataItem->usOperand_multiplier);
	sOperand_divisor.Format("%i",lpDataItem->usOperand_divisor);
	sDecimals.Format("%i",lpDataItem->usDecimals);
	sUnit = lpDataItem->szUnit;
	sDescription = lpDataItem->szDescription;
	sChecksum = lpDataItem->cChecksum;
	UpdateData(FALSE);

	usIndex--;
	sText.Format("ecuExplorer Data Item Editor - %i of %i",usIndex,listDataItem.GetCount());
	SetWindowText(sText);
}

void CMainDlg::OnNext()
{
	CString sText;
	LPSTRUCT_DATAITEM lpDataItem = NULL;

	UpdateData(TRUE);
	lpDataItem = (LPSTRUCT_DATAITEM)listDataItem.GetAt(pos);
	if(sName != lpDataItem->szName)
	{
		free(lpDataItem->szName);
		lpDataItem->cLength_name = sName.GetLength();
		lpDataItem->szName = (LPTSTR)malloc(lpDataItem->cLength_name + 1);
		memset(lpDataItem->szName,0,lpDataItem->cLength_name);
		strncpy(lpDataItem->szName,sName,lpDataItem->cLength_name);
		lpDataItem->szName[lpDataItem->cLength_name] = '\0';

		GenerateChecksum(lpDataItem);
	}
	if(sUnit != lpDataItem->szUnit)
	{
		free(lpDataItem->szUnit);
		lpDataItem->cLength_unit = sUnit.GetLength();
		lpDataItem->szUnit = (LPTSTR)malloc(lpDataItem->cLength_unit + 1);
		memset(lpDataItem->szUnit,0,lpDataItem->cLength_unit);
		strncpy(lpDataItem->szUnit,sUnit,lpDataItem->cLength_unit);
		lpDataItem->szUnit[lpDataItem->cLength_unit] = '\0';

		GenerateChecksum(lpDataItem);
	}
	if(sDescription != lpDataItem->szDescription)
	{
		TRACE("[%s]\n",sDescription);

		free(lpDataItem->szDescription);
		lpDataItem->usLength_description = sDescription.GetLength();
		lpDataItem->szDescription = (LPTSTR)malloc(lpDataItem->usLength_description + 1);
		memset(lpDataItem->szDescription,0x31,lpDataItem->usLength_description);
		strncpy(lpDataItem->szDescription,sDescription,lpDataItem->usLength_description);
		lpDataItem->szDescription[lpDataItem->usLength_description] = '\0';

		TRACE("[%s]\n",lpDataItem->szDescription);

		GenerateChecksum(lpDataItem);
	}
	lpDataItem->cType = atoi(sType);
	lpDataItem->usOperand_addition = atoi(sOperand_addition);
	lpDataItem->usOperand_subtract = atoi(sOperand_subtract);
	lpDataItem->usOperand_multiplier = atoi(sOperand_multiplier);
	lpDataItem->usOperand_divisor = atoi(sOperand_divisor);
	lpDataItem->usDecimals = atoi(sDecimals);

	sName.Empty();
	sUnit.Empty();
	sDescription.Empty();

	listDataItem.GetNext(pos);
	if(pos == NULL)
	{
		pos = listDataItem.GetTailPosition();
		return;
	}

	lpDataItem = (LPSTRUCT_DATAITEM)listDataItem.GetAt(pos);
	sByte.Format("%i",lpDataItem->cByte);
	sBit.Format("%i",lpDataItem->cBit);
	sName = lpDataItem->szName;
	sAddress_high.Format("0x%X",lpDataItem->ulAddressHigh);
	sAddress_low.Format("0x%X",lpDataItem->ulAddressLow);
	sType.Format("%i",lpDataItem->cType);
	sOperand_addition.Format("%i",lpDataItem->usOperand_addition);
	sOperand_subtract.Format("%i",lpDataItem->usOperand_subtract);
	sOperand_multiplier.Format("%i",lpDataItem->usOperand_multiplier);
	sOperand_divisor.Format("%i",lpDataItem->usOperand_divisor);
	sDecimals.Format("%i",lpDataItem->usDecimals);
	sUnit = lpDataItem->szUnit;
	sDescription = lpDataItem->szDescription;
	sChecksum = lpDataItem->cChecksum;
	
	UpdateData(FALSE);

	usIndex++;
	sText.Format("ecuExplorer Data Item Editor - %i of %i",usIndex,listDataItem.GetCount());
	SetWindowText(sText);
}

void CMainDlg::OnLoad()
{
	CString sFile;
	int fhDataItem = 0;
	unsigned char cRead;
	unsigned char cStorage[1024];
	LPSTRUCT_DATAITEM lpDataItem = NULL;

	if(!BrowseForFile(&sFile))
		return;

	if((fhDataItem = _open(sFile,_O_BINARY | _O_RDONLY,_S_IREAD)) == -1)
		return;

	_lseek(fhDataItem,0,SEEK_SET);

	_read(fhDataItem,&cRead,1);
	_read(fhDataItem,&cStorage,cRead);
	strncpy(sVersion.GetBufferSetLength(cRead),(LPCTSTR)&cStorage[0],cRead);
	sVersion.ReleaseBuffer();
	_read(fhDataItem,&cRead,1);

	while(!_eof(fhDataItem))
	{
		if((lpDataItem = new STRUCT_DATAITEM) == NULL)
			return;

		_read(fhDataItem,&lpDataItem->cByte,1);
		_read(fhDataItem,&lpDataItem->cBit,1);

		_read(fhDataItem,&lpDataItem->cLength_name,1);
		lpDataItem->szName = (LPTSTR)malloc(lpDataItem->cLength_name + 1);
		memset(lpDataItem->szName,0,lpDataItem->cLength_name);
		_read(fhDataItem,&cStorage,lpDataItem->cLength_name);
		cStorage[lpDataItem->cLength_name] = '\0';
		strcpy(lpDataItem->szName,(LPCTSTR)&cStorage[0]);

		_read(fhDataItem,&lpDataItem->ulAddressHigh,4);
		_read(fhDataItem,&lpDataItem->ulAddressLow,4);
		_read(fhDataItem,&lpDataItem->cType,1);
		_read(fhDataItem,&lpDataItem->usOperand_addition,2);
		_read(fhDataItem,&lpDataItem->usOperand_subtract,2);
		_read(fhDataItem,&lpDataItem->usOperand_multiplier,2);
		_read(fhDataItem,&lpDataItem->usOperand_divisor,2);
		_read(fhDataItem,&lpDataItem->usDecimals,2);

		_read(fhDataItem,&lpDataItem->cLength_unit,1);
		lpDataItem->szUnit = (LPTSTR)malloc(lpDataItem->cLength_unit + 1);
		memset(lpDataItem->szUnit,0,lpDataItem->cLength_unit);
		_read(fhDataItem,&cStorage,lpDataItem->cLength_unit);
		cStorage[lpDataItem->cLength_unit] = '\0';
		strcpy(lpDataItem->szUnit,(LPCTSTR)&cStorage[0]);

		_read(fhDataItem,&lpDataItem->usLength_description,2);
		lpDataItem->szDescription = (LPTSTR)malloc(lpDataItem->usLength_description + 1);
		memset(lpDataItem->szDescription,0,lpDataItem->usLength_description);
		_read(fhDataItem,&cStorage,lpDataItem->usLength_description);
		cStorage[lpDataItem->usLength_description] = '\0';
		strcpy(lpDataItem->szDescription,(LPCTSTR)&cStorage[0]);

		_read(fhDataItem,&lpDataItem->cChecksum,1);

		listDataItem.AddTail(lpDataItem);
	}

	_close(fhDataItem);

	pos = listDataItem.GetHeadPosition();
	lpDataItem = (LPSTRUCT_DATAITEM)listDataItem.GetAt(pos);
	sByte.Format("%i",lpDataItem->cByte);
	sBit.Format("%i",lpDataItem->cBit);
	sName = lpDataItem->szName;
	sAddress_high.Format("0x%X",lpDataItem->ulAddressHigh);
	sAddress_low.Format("0x%X",lpDataItem->ulAddressLow);
	sType.Format("%i",lpDataItem->cType);
	sOperand_addition.Format("%i",lpDataItem->usOperand_addition);
	sOperand_subtract.Format("%i",lpDataItem->usOperand_subtract);
	sOperand_multiplier.Format("%i",lpDataItem->usOperand_multiplier);
	sOperand_divisor.Format("%i",lpDataItem->usOperand_divisor);
	sDecimals.Format("%i",lpDataItem->usDecimals);
	sUnit = lpDataItem->szUnit;
	sDescription = lpDataItem->szDescription;
	sChecksum = lpDataItem->cChecksum;
	UpdateData(FALSE);

	usIndex++;
	sFile.Format("ecuExplorer Data Item Editor - %i of %i",usIndex,listDataItem.GetCount());
	SetWindowText(sFile);
}

void CMainDlg::OnSave()
{
	CString sFile;
	int fhDataItem = 0;
	LPSTRUCT_DATAITEM lpDataItem = NULL;
	unsigned char cTemp;

	UpdateData(TRUE);
	lpDataItem = (LPSTRUCT_DATAITEM)listDataItem.GetAt(pos);
	if(sName != lpDataItem->szName)
	{
		free(lpDataItem->szName);
		lpDataItem->cLength_name = sName.GetLength();
		lpDataItem->szName = (LPTSTR)malloc(lpDataItem->cLength_name + 1);
		memset(lpDataItem->szName,0,lpDataItem->cLength_name);
		strncpy(lpDataItem->szName,sName,lpDataItem->cLength_name);
		lpDataItem->szName[lpDataItem->cLength_name] = '\0';

		GenerateChecksum(lpDataItem);
	}
	if(sUnit != lpDataItem->szUnit)
	{
		free(lpDataItem->szUnit);
		lpDataItem->cLength_unit = sUnit.GetLength();
		lpDataItem->szUnit = (LPTSTR)malloc(lpDataItem->cLength_unit + 1);
		memset(lpDataItem->szUnit,0,lpDataItem->cLength_unit);
		strncpy(lpDataItem->szUnit,sUnit,lpDataItem->cLength_unit);
		lpDataItem->szUnit[lpDataItem->cLength_unit] = '\0';

		GenerateChecksum(lpDataItem);
	}
	if(sDescription != lpDataItem->szDescription)
	{
		free(lpDataItem->szDescription);
		lpDataItem->usLength_description = sDescription.GetLength();
		lpDataItem->szDescription = (LPTSTR)malloc(lpDataItem->usLength_description + 1);
		memset(lpDataItem->szDescription,0,lpDataItem->usLength_description);
		strncpy(lpDataItem->szDescription,sDescription,lpDataItem->usLength_description);
		lpDataItem->szDescription[lpDataItem->usLength_description] = '\0';

		GenerateChecksum(lpDataItem);
	}
	lpDataItem->cType = atoi(sType);
	lpDataItem->usOperand_addition = atoi(sOperand_addition);
	lpDataItem->usOperand_subtract = atoi(sOperand_subtract);
	lpDataItem->usOperand_multiplier = atoi(sOperand_multiplier);
	lpDataItem->usOperand_divisor = atoi(sOperand_divisor);
	lpDataItem->usDecimals = atoi(sDecimals);

	if(!BrowseForFile(&sFile))
		return;

	if((fhDataItem = _open(sFile,_O_BINARY | _O_CREAT | _O_APPEND | _O_WRONLY,_S_IWRITE)) == -1)
		return;

	_chsize(fhDataItem,0);
	cTemp = sVersion.GetLength();
	_write(fhDataItem,&cTemp,1);
	_write(fhDataItem,sVersion,sVersion.GetLength());
	cTemp += GenerateChecksum(sVersion,sVersion.GetLength());
	_write(fhDataItem,&cTemp,1);

	pos = listDataItem.GetHeadPosition();
	while(pos != NULL)
	{
		lpDataItem = (LPSTRUCT_DATAITEM)listDataItem.GetNext(pos);
		_write(fhDataItem,&lpDataItem->cByte,1);
		_write(fhDataItem,&lpDataItem->cBit,1);
		_write(fhDataItem,&lpDataItem->cLength_name,1);
		_write(fhDataItem,lpDataItem->szName,lpDataItem->cLength_name);
		_write(fhDataItem,&lpDataItem->ulAddressHigh,4);
		_write(fhDataItem,&lpDataItem->ulAddressLow,4);
		_write(fhDataItem,&lpDataItem->cType,1);
		_write(fhDataItem,&lpDataItem->usOperand_addition,2);
		_write(fhDataItem,&lpDataItem->usOperand_subtract,2);
		_write(fhDataItem,&lpDataItem->usOperand_multiplier,2);
		_write(fhDataItem,&lpDataItem->usOperand_divisor,2);
		_write(fhDataItem,&lpDataItem->usDecimals,2);
		_write(fhDataItem,&lpDataItem->cLength_unit,1);
		_write(fhDataItem,lpDataItem->szUnit,lpDataItem->cLength_unit);
		_write(fhDataItem,&lpDataItem->usLength_description,2);
		_write(fhDataItem,lpDataItem->szDescription,lpDataItem->usLength_description);

		GenerateChecksum(lpDataItem);

		_write(fhDataItem,&lpDataItem->cChecksum,1);
	}

	_close(fhDataItem);
}

long CMainDlg::BrowseForFile(CString* szReturn)
{
	TCHAR szTitle[255];
	TCHAR szDefExt[255];
	TCHAR szFile[255] = "\0";
	OPENFILENAME ofnFileName;

	strcpy(szFile,"");
	strcpy(szTitle,"Select a data item file");
	strcpy(szDefExt,"dat");

	memset((void *)&ofnFileName,0,sizeof(OPENFILENAME));
	ofnFileName.lStructSize = sizeof(OPENFILENAME);
	ofnFileName.hwndOwner = m_hWnd;
	ofnFileName.hInstance = NULL;
	ofnFileName.lpstrFilter = "Data Files (*.dat)\0*.dat\0\0";
	ofnFileName.lpstrCustomFilter = NULL;
	ofnFileName.nMaxCustFilter = 0;
	ofnFileName.nFilterIndex = 1;
	ofnFileName.lpstrFile = szFile;
	ofnFileName.nMaxFile = sizeof(szFile);
	ofnFileName.lpstrFileTitle = NULL;
	ofnFileName.nMaxFileTitle = 0;
	ofnFileName.lpstrInitialDir = NULL;
	ofnFileName.lpstrTitle = (LPSTR)&szTitle;
	ofnFileName.nFileOffset = 0;
	ofnFileName.nFileExtension = 0;
	ofnFileName.lpstrDefExt = (LPSTR)&szDefExt;
	ofnFileName.lCustData = NULL;
	ofnFileName.lpfnHook = NULL;
	ofnFileName.lpTemplateName = NULL;
	ofnFileName.Flags = OFN_SHOWHELP | OFN_EXPLORER | OFN_HIDEREADONLY;

	if(GetOpenFileName(&ofnFileName) != 0)
		*szReturn = ofnFileName.lpstrFile;
	else
		return FALSE;

	return TRUE;
}

void CMainDlg::GenerateChecksum(LPSTRUCT_DATAITEM lpDataItem)
{
	lpDataItem->cChecksum = lpDataItem->cByte;
	lpDataItem->cChecksum += lpDataItem->cBit;
	lpDataItem->cChecksum += lpDataItem->cLength_name;
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->szName,lpDataItem->cLength_name);
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->ulAddressHigh);
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->ulAddressLow);
	lpDataItem->cChecksum += lpDataItem->cType;
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->usOperand_addition);
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->usOperand_subtract);
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->usOperand_multiplier);
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->usOperand_divisor);
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->usDecimals);
	lpDataItem->cChecksum += lpDataItem->cLength_unit;
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->szUnit,lpDataItem->cLength_unit);
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->usLength_description);
	lpDataItem->cChecksum += GenerateChecksum(lpDataItem->szDescription,lpDataItem->usLength_description);
}

unsigned char CMainDlg::GenerateChecksum(LPCTSTR szBuffer,unsigned short usBufferLength)
{
	int iLoop;
	unsigned char cReturn = 0;

	for(iLoop = 0;iLoop < usBufferLength;iLoop++)
		cReturn += szBuffer[iLoop];

	return cReturn;
}

unsigned char CMainDlg::GenerateChecksum(unsigned long ulValue)
{
	unsigned char* szValue = NULL;
	int iLoop;
	unsigned char cReturn = 0;

	if((szValue = (unsigned char*)malloc(4)) == NULL)
		return 0;

	memset(szValue,0,4);
	memcpy(szValue,&ulValue,4);

	for(iLoop = 0;iLoop < 4;iLoop++)
		cReturn += szValue[iLoop];

	if(szValue != NULL) free(szValue);

	return cReturn;
}

unsigned char CMainDlg::GenerateChecksum(unsigned short usValue)
{
	unsigned char* szValue = NULL;
	int iLoop;
	unsigned char cReturn = 0;

	if((szValue = (unsigned char*)malloc(2)) == NULL)
		return 0;

	memset(szValue,0,2);
	memcpy(szValue,&usValue,2);

	for(iLoop = 0;iLoop < 2;iLoop++)
		cReturn += szValue[iLoop];

	if(szValue != NULL) free(szValue);

	return cReturn;
}

void CMainDlg::OnDelete()
{}

void CMainDlg::OnNew()
{
	LPSTRUCT_DATAITEM lpDataItem = NULL;
	CString sText;

	UpdateData(TRUE);
	lpDataItem = (LPSTRUCT_DATAITEM)listDataItem.GetAt(pos);
	if(sName != lpDataItem->szName)
	{
		free(lpDataItem->szName);
		lpDataItem->cLength_name = sName.GetLength();
		lpDataItem->szName = (LPTSTR)malloc(lpDataItem->cLength_name + 1);
		memset(lpDataItem->szName,0,lpDataItem->cLength_name);
		strncpy(lpDataItem->szName,sName,lpDataItem->cLength_name);
		lpDataItem->szName[lpDataItem->cLength_name] = '\0';

		GenerateChecksum(lpDataItem);
	}
	if(sUnit != lpDataItem->szUnit)
	{
		free(lpDataItem->szUnit);
		lpDataItem->cLength_unit = sUnit.GetLength();
		lpDataItem->szUnit = (LPTSTR)malloc(lpDataItem->cLength_unit + 1);
		memset(lpDataItem->szUnit,0,lpDataItem->cLength_unit);
		strncpy(lpDataItem->szUnit,sUnit,lpDataItem->cLength_unit);
		lpDataItem->szUnit[lpDataItem->cLength_unit] = '\0';

		GenerateChecksum(lpDataItem);
	}
	if(sDescription != lpDataItem->szDescription)
	{
		free(lpDataItem->szDescription);
		lpDataItem->usLength_description = sDescription.GetLength();
		lpDataItem->szDescription = (LPTSTR)malloc(lpDataItem->usLength_description + 1);
		memset(lpDataItem->szDescription,0,lpDataItem->usLength_description);
		strncpy(lpDataItem->szDescription,sDescription,lpDataItem->usLength_description);
		lpDataItem->szDescription[lpDataItem->usLength_description] = '\0';

		GenerateChecksum(lpDataItem);
	}
	lpDataItem->cType = atoi(sType);
	lpDataItem->usOperand_addition = atoi(sOperand_addition);
	lpDataItem->usOperand_subtract = atoi(sOperand_subtract);
	lpDataItem->usOperand_multiplier = atoi(sOperand_multiplier);
	lpDataItem->usOperand_divisor = atoi(sOperand_divisor);
	lpDataItem->usDecimals = atoi(sDecimals);

	lpDataItem = NULL;
	if((lpDataItem = new STRUCT_DATAITEM) == NULL)
		return;

	lpDataItem->cByte = 0;
	lpDataItem->cBit = 0;

	lpDataItem->cLength_name = 0;
	lpDataItem->szName = (LPTSTR)malloc(lpDataItem->cLength_name + 1);
	memset(lpDataItem->szName,0,lpDataItem->cLength_name);
	lpDataItem->szName[lpDataItem->cLength_name] = '\0';

	lpDataItem->ulAddressHigh = 0;
	lpDataItem->ulAddressLow = 0;
	lpDataItem->cType = 0;
	lpDataItem->usOperand_addition = 0;
	lpDataItem->usOperand_subtract = 0;
	lpDataItem->usOperand_multiplier = 0;
	lpDataItem->usOperand_divisor = 0;
	lpDataItem->usDecimals = 0;

	lpDataItem->cLength_unit = 0;
	lpDataItem->szUnit = (LPTSTR)malloc(lpDataItem->cLength_unit + 1);
	memset(lpDataItem->szUnit,0,lpDataItem->cLength_unit);
	lpDataItem->szUnit[lpDataItem->cLength_unit] = '\0';

	lpDataItem->usLength_description = 0;
	lpDataItem->szDescription = (LPTSTR)malloc(lpDataItem->usLength_description + 1);
	memset(lpDataItem->szDescription,0,lpDataItem->usLength_description);
	lpDataItem->szDescription[lpDataItem->usLength_description] = '\0';

	lpDataItem->cChecksum = 0;

	listDataItem.AddTail(lpDataItem);
	pos = listDataItem.GetTailPosition();

	lpDataItem = (LPSTRUCT_DATAITEM)listDataItem.GetAt(pos);
	sByte.Format("%i",lpDataItem->cByte);
	sBit.Format("%i",lpDataItem->cBit);
	sName = lpDataItem->szName;
	sAddress_high.Format("0x%X",lpDataItem->ulAddressHigh);
	sAddress_low.Format("0x%X",lpDataItem->ulAddressLow);
	sType.Format("%i",lpDataItem->cType);
	sOperand_addition.Format("%i",lpDataItem->usOperand_addition);
	sOperand_subtract.Format("%i",lpDataItem->usOperand_subtract);
	sOperand_multiplier.Format("%i",lpDataItem->usOperand_multiplier);
	sOperand_divisor.Format("%i",lpDataItem->usOperand_divisor);
	sDecimals.Format("%i",lpDataItem->usDecimals);
	sUnit = lpDataItem->szUnit;
	sDescription = lpDataItem->szDescription;
	sChecksum = lpDataItem->cChecksum;
	
	UpdateData(FALSE);

	usIndex = listDataItem.GetCount();
	sText.Format("ecuExplorer Data Item Editor - %i of %i",usIndex,listDataItem.GetCount());
	SetWindowText(sText);
}