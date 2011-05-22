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
	listDTC.RemoveAll();
	pos = NULL;
	usIndex = 0;
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_CODE,sCode);
	DDX_Text(pDX,IDC_DESCRIPTION,sDescription);
	DDX_Text(pDX,IDC_CHECKSUM,sChecksum);
	DDX_Text(pDX,IDC_VERSION,sVersion);
	DDX_Text(pDX,IDC_CURRENT,sCurrent);
	DDX_Text(pDX,IDC_HISTORIC,sHistoric);
	DDX_Text(pDX,IDC_BIT,sBit);
}

void CMainDlg::OnCancel()
{
	LPSTRUCT_DTCITEM lpDTCItem = NULL;

	while(listDTC.GetCount() > 0)
	{
		lpDTCItem = (LPSTRUCT_DTCITEM)listDTC.RemoveHead();
		free(lpDTCItem->szCode);
		free(lpDTCItem->szDescription);
		delete lpDTCItem;
	}

	CDialog::OnCancel();
}

void CMainDlg::OnBack()
{
	CString sText;
	LPSTRUCT_DTCITEM lpDTCItem = NULL;

	UpdateData(TRUE);
	lpDTCItem = (LPSTRUCT_DTCITEM)listDTC.GetAt(pos);
	if(sCode != lpDTCItem->szCode)
	{
		free(lpDTCItem->szCode);
		lpDTCItem->cLength_code = sCode.GetLength();
		lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
		memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
		strncpy(lpDTCItem->szCode,sCode,lpDTCItem->cLength_code);
		lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';

		GenerateChecksum(lpDTCItem);
	}
	if(sDescription != lpDTCItem->szDescription)
	{
		free(lpDTCItem->szDescription);
		lpDTCItem->cLength_description = sDescription.GetLength();
		lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
		memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
		strncpy(lpDTCItem->szDescription,sDescription,lpDTCItem->cLength_description);
		lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';

		GenerateChecksum(lpDTCItem);
	}
	sCode.Empty();
	sDescription.Empty();

	listDTC.GetPrev(pos);
	if(pos == NULL)
	{
		pos = listDTC.GetHeadPosition();
		return;
	}

	lpDTCItem = (LPSTRUCT_DTCITEM)listDTC.GetAt(pos);
	sCode = lpDTCItem->szCode;
	sDescription = lpDTCItem->szDescription;
	sChecksum = lpDTCItem->cChecksum;
	sCurrent.Format("0x%X",lpDTCItem->ulCurrent);
	sHistoric.Format("0x%X",lpDTCItem->ulHistoric);
	sBit = lpDTCItem->cBit;
	UpdateData(FALSE);

	usIndex--;
	sText.Format("ecuExplorer DTC Editor - %i of %i",usIndex,listDTC.GetCount());
	SetWindowText(sText);
}

void CMainDlg::OnNext()
{
	CString sText;
	LPSTRUCT_DTCITEM lpDTCItem = NULL;

	UpdateData(TRUE);
	lpDTCItem = (LPSTRUCT_DTCITEM)listDTC.GetAt(pos);
	if(sCode != lpDTCItem->szCode)
	{
		free(lpDTCItem->szCode);
		lpDTCItem->cLength_code = sCode.GetLength();
		lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
		memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
		strncpy(lpDTCItem->szCode,sCode,lpDTCItem->cLength_code);
		lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';

		GenerateChecksum(lpDTCItem);
	}
	if(sDescription != lpDTCItem->szDescription)
	{
		free(lpDTCItem->szDescription);
		lpDTCItem->cLength_description = sDescription.GetLength();
		lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
		memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
		strncpy(lpDTCItem->szDescription,sDescription,lpDTCItem->cLength_description);
		lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';

		GenerateChecksum(lpDTCItem);
	}
	sCode.Empty();
	sDescription.Empty();

	listDTC.GetNext(pos);
	if(pos == NULL)
	{
		pos = listDTC.GetTailPosition();
		return;
	}

	lpDTCItem = (LPSTRUCT_DTCITEM)listDTC.GetAt(pos);
	sCode = lpDTCItem->szCode;
	sDescription = lpDTCItem->szDescription;
	sChecksum = lpDTCItem->cChecksum;
	sCurrent.Format("0x%X",lpDTCItem->ulCurrent);
	sHistoric.Format("0x%X",lpDTCItem->ulHistoric);
	sBit = lpDTCItem->cBit;
	UpdateData(FALSE);

	usIndex++;
	sText.Format("ecuExplorer DTC Editor - %i of %i",usIndex,listDTC.GetCount());
	SetWindowText(sText);
}

void CMainDlg::OnLoad()
{
	CString sFile;
	int fhDTC = 0;
	unsigned char cRead;
	unsigned char cStorage[255];
	LPSTRUCT_DTCITEM lpDTCItem = NULL;

	if(!BrowseForFile(&sFile))
		return;

	if((fhDTC = _open(sFile,_O_BINARY | _O_RDONLY,_S_IREAD)) == -1)
		return;

	_lseek(fhDTC,0,SEEK_SET);

	_read(fhDTC,&cRead,1);
	_read(fhDTC,&cStorage,cRead);
	strncpy(sVersion.GetBufferSetLength(cRead),(LPCTSTR)&cStorage[0],cRead);
	sVersion.ReleaseBuffer();
	_read(fhDTC,&cRead,1);

//	LoadList();

	while(!_eof(fhDTC))
	{
		if((lpDTCItem = new STRUCT_DTCITEM) == NULL)
			return;

		_read(fhDTC,&lpDTCItem->cLength_code,1);
		lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
		memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
		_read(fhDTC,&cStorage,lpDTCItem->cLength_code);
		cStorage[lpDTCItem->cLength_code] = '\0';
		strncpy(lpDTCItem->szCode,(LPCTSTR)&cStorage[0],lpDTCItem->cLength_code + 1);

		_read(fhDTC,&lpDTCItem->cLength_description,1);
		lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
		memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
		_read(fhDTC,&cStorage,lpDTCItem->cLength_description);
		cStorage[lpDTCItem->cLength_description] = '\0';
		strncpy(lpDTCItem->szDescription,(LPCTSTR)&cStorage[0],lpDTCItem->cLength_description + 1);

		_read(fhDTC,&lpDTCItem->ulCurrent,4);
		_read(fhDTC,&lpDTCItem->ulHistoric,4);
		_read(fhDTC,&lpDTCItem->cBit,1);

		_read(fhDTC,&lpDTCItem->cChecksum,1);

		listDTC.AddTail(lpDTCItem);
	}

	_close(fhDTC);

	pos = listDTC.GetHeadPosition();
	lpDTCItem = (LPSTRUCT_DTCITEM)listDTC.GetAt(pos);
	sCode = lpDTCItem->szCode;
	sDescription = lpDTCItem->szDescription;
	sChecksum = lpDTCItem->cChecksum;
	sCurrent.Format("0x%X",lpDTCItem->ulCurrent);
	sHistoric.Format("0x%X",lpDTCItem->ulHistoric);
	sBit = lpDTCItem->cBit;
	UpdateData(FALSE);

	usIndex++;
	sFile.Format("ecuExplorer DTC Editor - %i of %i",usIndex,listDTC.GetCount());
	SetWindowText(sFile);
}

void CMainDlg::OnSave()
{
	CString sFile;
	int fhDTC = 0;
	LPSTRUCT_DTCITEM lpDTCItem = NULL;
	unsigned char cTemp;

	UpdateData(TRUE);
	lpDTCItem = (LPSTRUCT_DTCITEM)listDTC.GetAt(pos);
	if(sCode != lpDTCItem->szCode)
	{
		free(lpDTCItem->szCode);
		lpDTCItem->cLength_code = sCode.GetLength();
		lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
		memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
		strncpy(lpDTCItem->szCode,sCode,lpDTCItem->cLength_code);
		lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';

		GenerateChecksum(lpDTCItem);
	}
	if(sDescription != lpDTCItem->szDescription)
	{
		free(lpDTCItem->szDescription);
		lpDTCItem->cLength_description = sDescription.GetLength();
		lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
		memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
		strncpy(lpDTCItem->szDescription,sDescription,lpDTCItem->cLength_description);
		lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';

		GenerateChecksum(lpDTCItem);
	}

	if(!BrowseForFile(&sFile))
		return;

	if((fhDTC = _open(sFile,_O_BINARY | _O_CREAT | _O_APPEND | _O_WRONLY,_S_IWRITE)) == -1)
		return;

	_chsize(fhDTC,0);
	cTemp = sVersion.GetLength();
	_write(fhDTC,&cTemp,1);
	_write(fhDTC,sVersion,sVersion.GetLength());
	cTemp += GenerateChecksum(sVersion,sVersion.GetLength());
	_write(fhDTC,&cTemp,1);

	pos = listDTC.GetHeadPosition();
	while(pos != NULL)
	{
		lpDTCItem = (LPSTRUCT_DTCITEM)listDTC.GetNext(pos);
		_write(fhDTC,&lpDTCItem->cLength_code,1);
		_write(fhDTC,lpDTCItem->szCode,lpDTCItem->cLength_code);
		_write(fhDTC,&lpDTCItem->cLength_description,1);
		_write(fhDTC,lpDTCItem->szDescription,lpDTCItem->cLength_description);
		_write(fhDTC,&lpDTCItem->ulCurrent,4);
		_write(fhDTC,&lpDTCItem->ulHistoric,4);
		_write(fhDTC,&lpDTCItem->cBit,1);

		GenerateChecksum(lpDTCItem);

		_write(fhDTC,&lpDTCItem->cChecksum,1);
	}

	_close(fhDTC);
}

long CMainDlg::BrowseForFile(CString* szReturn)
{
	TCHAR szTitle[255];
	TCHAR szDefExt[255];
	TCHAR szFile[255] = "\0";
	OPENFILENAME ofnFileName;

	strcpy(szFile,"");
	strcpy(szTitle,"Select a dtc data file");
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

void CMainDlg::GenerateChecksum(LPSTRUCT_DTCITEM lpDTCItem)
{
	int iLoop;

	lpDTCItem->cChecksum = lpDTCItem->cLength_code;
	for(iLoop = 0;iLoop < lpDTCItem->cLength_code;iLoop++)
		lpDTCItem->cChecksum += lpDTCItem->szCode[iLoop];
	lpDTCItem->cChecksum += lpDTCItem->cLength_description;
	for(iLoop = 0;iLoop < lpDTCItem->cLength_description;iLoop++)
		lpDTCItem->cChecksum += lpDTCItem->szDescription[iLoop];
	lpDTCItem->cChecksum += GenerateChecksum(lpDTCItem->ulCurrent);
	lpDTCItem->cChecksum += GenerateChecksum(lpDTCItem->ulHistoric);
	lpDTCItem->cChecksum += lpDTCItem->cBit;
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

unsigned char CMainDlg::GenerateChecksum(LPCTSTR szBuffer,unsigned char ucBufferLength)
{
	int iLoop;
	unsigned char cReturn = 0;

	for(iLoop = 0;iLoop < ucBufferLength;iLoop++)
		cReturn += szBuffer[iLoop];

	return cReturn;
}

void CMainDlg::OnDelete()
{}

void CMainDlg::OnNew()
{}

void CMainDlg::LoadList()
{
	LPSTRUCT_DTCITEM lpDTCItem = NULL;
	CString sError;

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "24";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Idle Control System Malfunction";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8E;
	lpDTCItem->ulHistoric = 0xA4;
	lpDTCItem->cBit = 0x1;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "23";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Mass Air Flow Sensor Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8E;
	lpDTCItem->ulHistoric = 0xA4;
	lpDTCItem->cBit = 0x2;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "22";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Knock Sensor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8E;
	lpDTCItem->ulHistoric = 0xA4;
	lpDTCItem->cBit = 0x4;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "21";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Engine Coolant Temperature Sensor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8E;
	lpDTCItem->ulHistoric = 0xA4;
	lpDTCItem->cBit = 0x8;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "13";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Camshaft Position Sensor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8E;
	lpDTCItem->ulHistoric = 0xA4;
	lpDTCItem->cBit = 0x20;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "12";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Starter Signal";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8E;
	lpDTCItem->ulHistoric = 0xA4;
	lpDTCItem->cBit = 0x40;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "11";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Crankshaft Position Sensor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8E;
	lpDTCItem->ulHistoric = 0xA4;
	lpDTCItem->cBit = 0x80;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "37";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Rear O2 Sensor Circuit / A/F Sensor Circuit Malfunction";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8F;
	lpDTCItem->ulHistoric = 0xA5;
	lpDTCItem->cBit = 0x1;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "35";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "CPC System";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8F;
	lpDTCItem->ulHistoric = 0xA5;
	lpDTCItem->cBit = 0x2;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "33";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Vehicle Speed Sensor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8F;
	lpDTCItem->ulHistoric = 0xA5;
	lpDTCItem->cBit = 0x4;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "32";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Oxygen Sensor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8F;
	lpDTCItem->ulHistoric = 0xA5;
	lpDTCItem->cBit = 0x8;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "31";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Throttle Position Sensor Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8F;
	lpDTCItem->ulHistoric = 0xA5;
	lpDTCItem->cBit = 0x10;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "29";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Crankshaft Position Sensor 2";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8F;
	lpDTCItem->ulHistoric = 0xA5;
	lpDTCItem->cBit = 0x20;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "28";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Knock Sensor #2";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8F;
	lpDTCItem->ulHistoric = 0xA5;
	lpDTCItem->cBit = 0x40;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "26";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Intake Air Temperature Sensor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x8F;
	lpDTCItem->ulHistoric = 0xA5;
	lpDTCItem->cBit = 0x80;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "45";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Pressure Sensor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x90;
	lpDTCItem->ulHistoric = 0xA6;
	lpDTCItem->cBit = 0x1;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "44";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Turbocharging Pressure Control Signal";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x90;
	lpDTCItem->ulHistoric = 0xA6;
	lpDTCItem->cBit = 0x2;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "43";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Accelerator Pedal Switch";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x90;
	lpDTCItem->ulHistoric = 0xA6;
	lpDTCItem->cBit = 0x4;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "42";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Idle Switch";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x90;
	lpDTCItem->ulHistoric = 0xA6;
	lpDTCItem->cBit = 0x8;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "39";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Traction Control System";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x90;
	lpDTCItem->ulHistoric = 0xA6;
	lpDTCItem->cBit = 0x10;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "38";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Torque Permission Signal";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x90;
	lpDTCItem->ulHistoric = 0xA6;
	lpDTCItem->cBit = 0x20;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "38";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Torque Control Signal #2";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x90;
	lpDTCItem->ulHistoric = 0xA6;
	lpDTCItem->cBit = 0x40;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "38";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Torque Control Signal #1";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x90;
	lpDTCItem->ulHistoric = 0xA6;
	lpDTCItem->cBit = 0x80;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "62";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Exhaust Manifold Valve Negative Pressure Control Solenoid";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x91;
	lpDTCItem->ulHistoric = 0xA7;
	lpDTCItem->cBit = 0x1;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "61";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Air Suction Control Solenoid Valve";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x91;
	lpDTCItem->ulHistoric = 0xA7;
	lpDTCItem->cBit = 0x2;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "56";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "EGR System";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x91;
	lpDTCItem->ulHistoric = 0xA7;
	lpDTCItem->cBit = 0x4;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "55";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "EGR Valve Lift Sensor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x91;
	lpDTCItem->ulHistoric = 0xA7;
	lpDTCItem->cBit = 0x8;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "54";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Air Intake System";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x91;
	lpDTCItem->ulHistoric = 0xA7;
	lpDTCItem->cBit = 0x10;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "51";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Neutral Position Switch";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x91;
	lpDTCItem->ulHistoric = 0xA7;
	lpDTCItem->cBit = 0x20;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "49";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "CO Resistor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x91;
	lpDTCItem->ulHistoric = 0xA7;
	lpDTCItem->cBit = 0x40;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "48";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Lean Burn System";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x91;
	lpDTCItem->ulHistoric = 0xA7;
	lpDTCItem->cBit = 0x80;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "53";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Anti-Quick Operation Mode";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x92;
	lpDTCItem->ulHistoric = 0xA8;
	lpDTCItem->cBit = 0x1;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "66";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Twin Turbocharger System(T)";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x92;
	lpDTCItem->ulHistoric = 0xA8;
	lpDTCItem->cBit = 0x2;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "66";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Twin Turbocharger System(S)";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x92;
	lpDTCItem->ulHistoric = 0xA8;
	lpDTCItem->cBit = 0x4;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "66";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Twin Turbocharger System(H)";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x92;
	lpDTCItem->ulHistoric = 0xA8;
	lpDTCItem->cBit = 0x8;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "66";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Two Stage Twin Turbocharger System";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x92;
	lpDTCItem->ulHistoric = 0xA8;
	lpDTCItem->cBit = 0x10;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "65";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Differential Pressure Sensor";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x92;
	lpDTCItem->ulHistoric = 0xA8;
	lpDTCItem->cBit = 0x20;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "64";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Relief Valve Control Solenoid Valve 1";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x92;
	lpDTCItem->ulHistoric = 0xA8;
	lpDTCItem->cBit = 0x40;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "64";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Relief Valve Control Solenoid Valve 2";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x92;
	lpDTCItem->ulHistoric = 0xA8;
	lpDTCItem->cBit = 0x80;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "53";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "IMM Control Module EEPROM";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x93;
	lpDTCItem->ulHistoric = 0xA9;
	lpDTCItem->cBit = 0x1;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "53";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "EGI Control Module EEPROM";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x93;
	lpDTCItem->ulHistoric = 0xA9;
	lpDTCItem->cBit = 0x2;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "53";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Use of Unregistered Key";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x93;
	lpDTCItem->ulHistoric = 0xA9;
	lpDTCItem->cBit = 0x4;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "53";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Key Communication Failure";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x93;
	lpDTCItem->ulHistoric = 0xA9;
	lpDTCItem->cBit = 0x8;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "53";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Communication Error (Time Over)";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x93;
	lpDTCItem->ulHistoric = 0xA9;
	lpDTCItem->cBit = 0x10;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "53";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "IMM Circuit Failure (Except Antenna Circuit)";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x93;
	lpDTCItem->ulHistoric = 0xA9;
	lpDTCItem->cBit = 0x20;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "53";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Reference Code Incompatibility";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x93;
	lpDTCItem->ulHistoric = 0xA9;
	lpDTCItem->cBit = 0x40;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "53";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Antenna";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x93;
	lpDTCItem->ulHistoric = 0xA9;
	lpDTCItem->cBit = 0x80;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "89";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "VVT Systems (R)";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x94;
	lpDTCItem->ulHistoric = 0xAA;
	lpDTCItem->cBit = 0x1;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "88";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Fuel Pump Circuit Malfunction";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x94;
	lpDTCItem->ulHistoric = 0xAA;
	lpDTCItem->cBit = 0x2;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "87";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Variable Induction Solenoid Valve Circuit Malfunction";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x94;
	lpDTCItem->ulHistoric = 0xAA;
	lpDTCItem->cBit = 0x4;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "86";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "CAN (Communication System)";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x94;
	lpDTCItem->ulHistoric = 0xAA;
	lpDTCItem->cBit = 0x8;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "85";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Charger System Circuit Malfunction";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x94;
	lpDTCItem->ulHistoric = 0xAA;
	lpDTCItem->cBit = 0x10;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "81";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Torque Up Control Valve";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x94;
	lpDTCItem->ulHistoric = 0xAA;
	lpDTCItem->cBit = 0x20;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "68";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Turbocharging Pressure Control Output Signal #2";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x94;
	lpDTCItem->ulHistoric = 0xAA;
	lpDTCItem->cBit = 0x40;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "67";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Exhaust Manifold Valve Positive Pressure Control Solenoid";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x94;
	lpDTCItem->ulHistoric = 0xAA;
	lpDTCItem->cBit = 0x80;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "89";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "VVT Systems (L)";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x95;
	lpDTCItem->ulHistoric = 0xAB;
	lpDTCItem->cBit = 0x1;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "24";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "ISC Valve (Stick)";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x95;
	lpDTCItem->ulHistoric = 0xAB;
	lpDTCItem->cBit = 0x2;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "24";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Open/Short in ISC Valve Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x95;
	lpDTCItem->ulHistoric = 0xAB;
	lpDTCItem->cBit = 0x4;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "32";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "A/F Sensor #2 System";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x95;
	lpDTCItem->ulHistoric = 0xAB;
	lpDTCItem->cBit = 0x10;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "32";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "A/F Sensor #1 System";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x95;
	lpDTCItem->ulHistoric = 0xAB;
	lpDTCItem->cBit = 0x20;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "92";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Electrical Generation Control Signal Circuit Malfunction";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x95;
	lpDTCItem->ulHistoric = 0xAB;
	lpDTCItem->cBit = 0x40;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "91";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "TCS Relief Valve";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x95;
	lpDTCItem->ulHistoric = 0xAB;
	lpDTCItem->cBit = 0x80;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "71";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Shift Solenoid Valve";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x96;
	lpDTCItem->ulHistoric = 0xAC;
	lpDTCItem->cBit = 0x1;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "72";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Shift Solenoid #2 Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x96;
	lpDTCItem->ulHistoric = 0xAC;
	lpDTCItem->cBit = 0x2;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "73";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Low Clutch Timing Solenoid Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x96;
	lpDTCItem->ulHistoric = 0xAC;
	lpDTCItem->cBit = 0x4;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "74";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Brake Clutch Timing Solenoid Circuit Malfunction";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x96;
	lpDTCItem->ulHistoric = 0xAC;
	lpDTCItem->cBit = 0x8;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "77";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Lock-Up Duty Solenoid Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x96;
	lpDTCItem->ulHistoric = 0xAC;
	lpDTCItem->cBit = 0x10;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "75";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Line Pressure Duty Solenoid Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x96;
	lpDTCItem->ulHistoric = 0xAC;
	lpDTCItem->cBit = 0x20;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "76";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Brake Clutch Pressure Duty Solenoid Circuit Malfunction";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x96;
	lpDTCItem->ulHistoric = 0xAC;
	lpDTCItem->cBit = 0x40;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "79";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Transfer Clutch Duty Solenoid Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x96;
	lpDTCItem->ulHistoric = 0xAC;
	lpDTCItem->cBit = 0x80;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "93";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Vehicle Speed Sensor #1 Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x97;
	lpDTCItem->ulHistoric = 0xAD;
	lpDTCItem->cBit = 0x1;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "27";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "ATF Temperature Sensor Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x97;
	lpDTCItem->ulHistoric = 0xAD;
	lpDTCItem->cBit = 0x2;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "94";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Lateral G Sensor Signal Circuit Malfunction";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x97;
	lpDTCItem->ulHistoric = 0xAD;
	lpDTCItem->cBit = 0x4;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "1";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Engine Speed Signal Circuit";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x97;
	lpDTCItem->ulHistoric = 0xAD;
	lpDTCItem->cBit = 0x8;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "38";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Torque Control Signal System";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x97;
	lpDTCItem->ulHistoric = 0xAD;
	lpDTCItem->cBit = 0x20;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "36";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Turbine Speed Signal";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x97;
	lpDTCItem->ulHistoric = 0xAD;
	lpDTCItem->cBit = 0x40;
	listDTC.AddTail(lpDTCItem);

	if((lpDTCItem = new STRUCT_DTCITEM) == NULL) return;
	sError = "78";
	lpDTCItem->cLength_code = sError.GetLength();
	lpDTCItem->szCode = (LPTSTR)malloc(lpDTCItem->cLength_code + 1);
	memset(lpDTCItem->szCode,0,lpDTCItem->cLength_code);
	strncpy(lpDTCItem->szCode,sError,lpDTCItem->cLength_code);
	lpDTCItem->szCode[lpDTCItem->cLength_code] = '\0';
	sError = "Tiptronic Solenoid System";
	lpDTCItem->cLength_description = sError.GetLength();
	lpDTCItem->szDescription = (LPTSTR)malloc(lpDTCItem->cLength_description + 1);
	memset(lpDTCItem->szDescription,0,lpDTCItem->cLength_description);
	strncpy(lpDTCItem->szDescription,sError,lpDTCItem->cLength_description);
	lpDTCItem->szDescription[lpDTCItem->cLength_description] = '\0';
	lpDTCItem->ulCurrent = 0x97;
	lpDTCItem->ulHistoric = 0xAD;
	lpDTCItem->cBit = 0x80;
	listDTC.AddTail(lpDTCItem);
}