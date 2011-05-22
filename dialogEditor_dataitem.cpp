#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "dialogEditor_dataitem.h"
#include "definitionLocal.h"

BEGIN_MESSAGE_MAP(dialogEditor_dataitem, CDialog)
	ON_COMMAND(IDBACK,OnBack)
	ON_COMMAND(IDNEXT,OnNext)
	ON_COMMAND(ID_MENU_EDITOR_DELETE, OnDelete)
	ON_COMMAND(ID_MENU_EDITOR_SAVE, OnSave)
	ON_EN_KILLFOCUS(IDC_EDITOR_DATAITEM_ADDRESSHIGH, OnKillFocus)
	ON_EN_KILLFOCUS(IDC_EDITOR_DATAITEM_ADDRESSLOW, OnKillFocus)
	ON_BN_CLICKED(IDC_EDITOR_DATAITEM_USERDEFINEDCHECK,UserDefinedItemChange)
	ON_BN_CLICKED(IDC_EDITOR_DATAITEM_CHECKQUADADDRESS,QuadSpanChange)
END_MESSAGE_MAP()

dialogEditor_dataitem::dialogEditor_dataitem() : CDialog(dialogEditor_dataitem::IDD)
{
	listDataItem.RemoveAll();
	pFont = NULL;
}

BOOL dialogEditor_dataitem::OnInitDialog()
{
	CString sError;
	DWORD dwExtendedStyle = 0;

	pFont = new CFont;
	LOGFONT lf;

	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 0xfffffff5;
	lf.lfWeight = FW_REGULAR;
	lf.lfOutPrecision = OUT_STROKE_PRECIS;
	lf.lfClipPrecision = CLIP_STROKE_PRECIS;
	lf.lfQuality = DRAFT_QUALITY;
	lf.lfPitchAndFamily = 0x22;
	lf.lfCharSet = 0;
	strcpy(lf.lfFaceName,"Lucida Console");

	pFont->CreateFontIndirect(&lf);

	if((lpComboBox_type = (CComboBox*)GetDlgItem(IDC_EDITOR_DATAITEM_TYPE)) == NULL)
		return FALSE;

	lpComboBox_type->SetItemData(lpComboBox_type->AddString("Temperature"),0x1);
	lpComboBox_type->SetItemData(lpComboBox_type->AddString("Pressure"),0x2);
	lpComboBox_type->SetItemData(lpComboBox_type->AddString("Air/Fuel"),0x3);
	lpComboBox_type->SetItemData(lpComboBox_type->AddString("Speed"),0x4);
	lpComboBox_type->SetItemData(lpComboBox_type->AddString("Injector"),0x5);
	lpComboBox_type->SetItemData(lpComboBox_type->AddString("General"),0x6);
	lpComboBox_type->SetItemData(lpComboBox_type->AddString("Switch"),0x7);
	lpComboBox_type->SetItemData(lpComboBox_type->AddString("Boost (Corrected)"),0x9);

	CSpinButtonCtrl* pSpin = NULL;
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_SPINBYTE)) == NULL)
		return FALSE;
	pSpin->SetRange(1,100);
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_SPINBIT)) == NULL)
		return FALSE;
	pSpin->SetRange(1,8);
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_ADDRESSHIGHSPIN)) == NULL)
		return FALSE;
	pSpin->SetRange32(0,0xFFFFFF);
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_ADDRESSLOWSPIN)) == NULL)
		return FALSE;
	pSpin->SetRange32(0,0xFFFFFF);
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_OPERANDADDITIONSPIN)) == NULL)
		return FALSE;
	pSpin->SetRange32(0,65536);
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_OPERANDSUBTRACTSPIN)) == NULL)
		return FALSE;
	pSpin->SetRange32(0,65536);
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_OPERANDMULTIPLIERSPIN)) == NULL)
		return FALSE;
	pSpin->SetRange32(0,65536);
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_OPERANDDIVISORSPIN)) == NULL)
		return FALSE;
	pSpin->SetRange32(0,65536);
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_DECIMALSSPIN)) == NULL)
		return FALSE;
	pSpin->SetRange(0,3);

	LoadDataFile();
	GetData();
	UpdateLabels(FALSE);

	listDataItem.GetCount() == 0 ? iIndex = 0 : iIndex = 1;
	sError.Format("Editor - Data Item Data File (%i of %i)",iIndex,listDataItem.GetCount());
	SetWindowText(sError);

	return CDialog::OnInitDialog();
}

void dialogEditor_dataitem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDITOR_DATAITEM_VERSION,sVersion);
	DDX_Check(pDX,IDC_EDITOR_DATAITEM_USERDEFINEDCHECK,bUserDefinedItem);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_NAME,sName);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_UNIT,sUnit);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_BYTE,sByte);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_BIT,sBit);
	DDX_Check(pDX,IDC_EDITOR_DATAITEM_CHECKQUADADDRESS,bQuadSpan);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_ADDRESSHIGH,iAddress_high);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_ADDRESSLOW,iAddress_low);
	DDX_CBIndex(pDX,IDC_EDITOR_DATAITEM_TYPE,iIndex_type);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_OPERANDADDITION,sOperand_addition);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_OPERANDSUBTRACT,sOperand_subtract);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_OPERANDMULTIPLIER,sOperand_multiplier);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_OPERANDDIVISOR,sOperand_divisor);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_DECIMALS,sDecimals);
	DDX_Text(pDX,IDC_EDITOR_DATAITEM_DESCRIPTION,sDescription);
}

void dialogEditor_dataitem::OnOK()
{
	LPSTRUCT_LIVEBITITEM lpDataItem = NULL;

	while(listDataItem.GetCount() > 0)
	{
		lpDataItem = (LPSTRUCT_LIVEBITITEM)listDataItem.RemoveHead();
		free(lpDataItem->szName);
		free(lpDataItem->szUnit);
		free(lpDataItem->szDescription);
		delete lpDataItem;
	}

	if(pFont != NULL)
		delete pFont;

	CDialog::OnOK();
}

void dialogEditor_dataitem::BrowseForFile(CString* pszReturn,LPCTSTR szTitle,DWORD dwFlags)
{
	TCHAR szDefExt[255];
	TCHAR szFile[255] = "\0";
	OPENFILENAME ofnFileName;

	strcpy(szFile,"");
	strcpy(szDefExt,"");

	memset((void *)&ofnFileName,0,sizeof(OPENFILENAME));
	ofnFileName.lStructSize = sizeof(OPENFILENAME);
	ofnFileName.hwndOwner = m_hWnd;
	ofnFileName.hInstance = NULL;
	ofnFileName.lpstrFilter = "Data Files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0\0";
	ofnFileName.lpstrCustomFilter = NULL;
	ofnFileName.nMaxCustFilter = 0;
	ofnFileName.nFilterIndex = 1;
	ofnFileName.lpstrFile = szFile;
	ofnFileName.nMaxFile = sizeof(szFile);
	ofnFileName.lpstrFileTitle = NULL;
	ofnFileName.nMaxFileTitle = 0;
	ofnFileName.lpstrInitialDir = NULL;
	ofnFileName.lpstrTitle = (LPSTR)szTitle;
	ofnFileName.nFileOffset = 0;
	ofnFileName.nFileExtension = 0;
	ofnFileName.lpstrDefExt = (LPSTR)&szDefExt;
	ofnFileName.lCustData = NULL;
	ofnFileName.lpfnHook = NULL;
	ofnFileName.lpTemplateName = NULL;
	ofnFileName.Flags = OFN_SHOWHELP | OFN_EXPLORER | OFN_HIDEREADONLY | dwFlags;

	if(dwFlags & OFN_OVERWRITEPROMPT)
	{
		if(GetSaveFileName(&ofnFileName) != 0)
			*pszReturn = ofnFileName.lpstrFile;
	}
	else
	{
		if(GetOpenFileName(&ofnFileName) != 0)
			*pszReturn = ofnFileName.lpstrFile;
	}

	return;
}

void dialogEditor_dataitem::OnBack()
{
	CString sError;

	if(listDataItem.GetCount() == 0) return;

	SaveChanges();
	iIndex--;
	if(iIndex <= 0)
	{
		iIndex = 1;
		pos = listDataItem.GetHeadPosition();
	}
	else
		listDataItem.GetPrev(pos);
	GetData();
	UpdateLabels(FALSE);

	sError.Format("Editor - Data Item Data File (%i of %i)",iIndex,listDataItem.GetCount());
	SetWindowText(sError);
}

void dialogEditor_dataitem::OnNext()
{
	CString sError;

	if(listDataItem.GetCount() == 0) return;

	SaveChanges();
	iIndex++;
	if(iIndex >= listDataItem.GetCount())
	{
		iIndex = listDataItem.GetCount();
		pos = listDataItem.GetTailPosition();
	}
	else
		listDataItem.GetNext(pos);
	GetData();
	UpdateLabels(FALSE);

	sError.Format("Editor - Data Item Data File (%i of %i)",iIndex,listDataItem.GetCount());
	SetWindowText(sError);
}

void dialogEditor_dataitem::LoadDataFile()
{
	CString sError;
	int fh = 0;
	unsigned char cRead = 0;
	unsigned char cChecksum = 0;
	unsigned char cStorage[1024];
	LPSTRUCT_LIVEBITITEM lpDataItem = NULL;
	int iLoop = 0;

	// Live Data File Format
	// 1 bytes [Version Length]
	// x bytes [Version]
	// 1 bytes [Version Checksum]
	// [..]
	//		1 bytes [Byte]
	//		1 bytes [Bit]
	//		1 bytes [Name Length]
	//		x bytes [Name]
	//		4 bytes [Address High]
	//		4 bytes [Address Low]
	//		1 bytes [Type]
	//		2 bytes [Operand Addition]
	//		2 bytes [Operand Subtract]
	//		2 bytes [Operand Multiplier]
	//		2 bytes [Operand Divisor]
	//		2 bytes [Decimals]
	//		1 bytes [Unit Length]
	//		x bytes [Unit]
	//		2 bytes [Description Length]
	//		x bytes [Description]
	//		1 bytes [Checksum]
	// [..]

	BrowseForFile(&sError,"Select the Data Item Data File",OFN_FILEMUSTEXIST);
	if((fh = _open(sError,_O_BINARY | _O_RDONLY,_S_IREAD)) == -1)
		return;

	_lseek(fh,0,SEEK_SET);

	_read(fh,&cRead,1);
	_read(fh,&cStorage[0],cRead);
	cStorage[cRead] = '\0';
	sVersion = cStorage;

	_read(fh,&cRead,1);

	while(!_eof(fh))
	{
		if((lpDataItem = new STRUCT_LIVEBITITEM) == NULL)
			return;

		_read(fh,&lpDataItem->cByte,1);
		_read(fh,&lpDataItem->cBit,1);

		_read(fh,&lpDataItem->cLength_name,1);
		_read(fh,&cStorage,lpDataItem->cLength_name);
		cStorage[lpDataItem->cLength_name] = '\0';
		lpDataItem->szName = (LPTSTR)malloc(lpDataItem->cLength_name + 1);
		memset(lpDataItem->szName,0,lpDataItem->cLength_name);
		strncpy(lpDataItem->szName,(LPCTSTR)&cStorage[0],lpDataItem->cLength_name + 1);

		_read(fh,&cStorage,4);
		memcpy(&lpDataItem->ulAddress_high,&cStorage,4);
		_read(fh,&cStorage,4);
		memcpy(&lpDataItem->ulAddress_low,&cStorage,4);

		_read(fh,&lpDataItem->cType,1);

		_read(fh,&cStorage,2);
		memcpy(&lpDataItem->usOperand_addition,&cStorage,2);
		_read(fh,&cStorage,2);
		memcpy(&lpDataItem->usOperand_subtract,&cStorage,2);
		_read(fh,&cStorage,2);
		memcpy(&lpDataItem->usOperand_multiplier,&cStorage,2);
		_read(fh,&cStorage,2);
		memcpy(&lpDataItem->usOperand_divisor,&cStorage,2);
		_read(fh,&cStorage,2);
		memcpy(&lpDataItem->usDecimals,&cStorage,2);

		_read(fh,&lpDataItem->cLength_unit,1);
		_read(fh,&cStorage,lpDataItem->cLength_unit);
		cStorage[lpDataItem->cLength_unit] = '\0';
		lpDataItem->szUnit = (LPTSTR)malloc(lpDataItem->cLength_unit + 1);
		memset(lpDataItem->szUnit,0,lpDataItem->cLength_unit);
		strncpy(lpDataItem->szUnit,(LPCTSTR)&cStorage[0],lpDataItem->cLength_unit + 1);

		_read(fh,&lpDataItem->usLength_description,2);
		_read(fh,&cStorage,lpDataItem->usLength_description);
		cStorage[lpDataItem->usLength_description] = '\0';
		lpDataItem->szDescription = (LPTSTR)malloc(lpDataItem->usLength_description + 1);
		memset(lpDataItem->szDescription,0,lpDataItem->usLength_description);
		strncpy(lpDataItem->szDescription,(LPCTSTR)&cStorage[0],lpDataItem->usLength_description + 1);

		_read(fh,&cRead,1);		
		listDataItem.AddTail(lpDataItem);
	}

	_close(fh);
	pos = listDataItem.GetHeadPosition();
}

void dialogEditor_dataitem::OnSave()
{
	int fhOutput = -1;
	CString sError;
	int iLoop = 0;
	int iSub = 0;
	unsigned char cTemp;
	LPSTRUCT_LIVEBITITEM lpDataItem = NULL;
	unsigned long* lpOffset = NULL;
	POSITION pos = NULL;

	SaveChanges();
	BrowseForFile(&sError,"Select a filename for the Data Item Data File",OFN_OVERWRITEPROMPT);
	if((fhOutput = _open(sError,_O_BINARY | _O_CREAT | _O_WRONLY,_S_IWRITE)) == -1)
		return;
	if((_chsize(fhOutput,0)) == -1)
		return;

	// Live Data File Format
	// 1 bytes [Version Length]
	// x bytes [Version]
	// 1 bytes [Version Checksum]
	// [..]
	//		1 bytes [Byte]
	//		1 bytes [Bit]
	//		1 bytes [Name Length]
	//		x bytes [Name]
	//		4 bytes [Address High]
	//		4 bytes [Address Low]
	//		1 bytes [Type]
	//		2 bytes [Operand Addition]
	//		2 bytes [Operand Subtract]
	//		2 bytes [Operand Multiplier]
	//		2 bytes [Operand Divisor]
	//		2 bytes [Decimals]
	//		1 bytes [Unit Length]
	//		x bytes [Unit]
	//		2 bytes [Description Length]
	//		x bytes [Description]
	//		1 bytes [Checksum]
	// [..]

	cTemp = (char)sVersion.GetLength();
	_write(fhOutput,&cTemp,1);
	_write(fhOutput,(LPCTSTR)sVersion,cTemp);
	cTemp += GenerateChecksum(sVersion,sVersion.GetLength());
	_write(fhOutput,&cTemp,1);

	pos = listDataItem.GetHeadPosition();
	while(pos != NULL)
	{
		lpDataItem = (LPSTRUCT_LIVEBITITEM)listDataItem.GetNext(pos);
		_write(fhOutput,&lpDataItem->cByte,1);
		_write(fhOutput,&lpDataItem->cBit,1);
		_write(fhOutput,&lpDataItem->cLength_name,1);
		_write(fhOutput,lpDataItem->szName,lpDataItem->cLength_name);
		_write(fhOutput,&lpDataItem->ulAddress_high,4);
		_write(fhOutput,&lpDataItem->ulAddress_low,4);
		_write(fhOutput,&lpDataItem->cType,1);
		_write(fhOutput,&lpDataItem->usOperand_addition,2);
		_write(fhOutput,&lpDataItem->usOperand_subtract,2);
		_write(fhOutput,&lpDataItem->usOperand_multiplier,2);
		_write(fhOutput,&lpDataItem->usOperand_divisor,2);
		_write(fhOutput,&lpDataItem->usDecimals,2);
		_write(fhOutput,&lpDataItem->cLength_unit,1);
		_write(fhOutput,lpDataItem->szUnit,lpDataItem->cLength_unit);
		_write(fhOutput,&lpDataItem->usLength_description,2);
		_write(fhOutput,lpDataItem->szDescription,lpDataItem->usLength_description);

		cTemp = GenerateChecksum(lpDataItem);
		_write(fhOutput,&cTemp,1);
	}
}

unsigned char dialogEditor_dataitem::GenerateChecksum(LPSTRUCT_LIVEBITITEM lpDataItem)
{
	unsigned char cReturn = 0;

	cReturn = lpDataItem->cByte;
	cReturn += lpDataItem->cBit;
	cReturn += lpDataItem->cLength_name;
	cReturn += GenerateChecksum(lpDataItem->szName,lpDataItem->cLength_name);
	cReturn += GenerateChecksum(lpDataItem->ulAddress_high);
	cReturn += GenerateChecksum(lpDataItem->ulAddress_low);
	cReturn += lpDataItem->cType;
	cReturn += GenerateChecksum(lpDataItem->usOperand_addition);
	cReturn += GenerateChecksum(lpDataItem->usOperand_subtract);
	cReturn += GenerateChecksum(lpDataItem->usOperand_multiplier);
	cReturn += GenerateChecksum(lpDataItem->usOperand_divisor);
	cReturn += GenerateChecksum(lpDataItem->usDecimals);
	cReturn += lpDataItem->cLength_unit;
	cReturn += GenerateChecksum(lpDataItem->szUnit,lpDataItem->cLength_unit);
	cReturn += GenerateChecksum(lpDataItem->usLength_description);
	cReturn += GenerateChecksum(lpDataItem->szDescription,lpDataItem->usLength_description);

	return cReturn;
}

unsigned char dialogEditor_dataitem::GenerateChecksum(LPCTSTR szBuffer,unsigned short usBufferLength)
{
	int iLoop;
	unsigned char cReturn = 0;

	for(iLoop = 0;iLoop < usBufferLength;iLoop++)
		cReturn += szBuffer[iLoop];

	return cReturn;
}

unsigned char dialogEditor_dataitem::GenerateChecksum(unsigned long ulValue)
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

unsigned char dialogEditor_dataitem::GenerateChecksum(unsigned short usValue)
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

void dialogEditor_dataitem::GetData()
{
	LPSTRUCT_LIVEBITITEM lpDataItem = NULL;

	if(listDataItem.GetCount() == 0) return;

	if((lpDataItem = (LPSTRUCT_LIVEBITITEM)listDataItem.GetAt(pos)) == NULL)
		return;

	if(lpDataItem->cByte == 0 && lpDataItem->cBit == 0)
	{
		sByte.Empty();
		sBit.Empty();
		bUserDefinedItem = TRUE;
	}
	else
	{
		sByte.Format("%i",lpDataItem->cByte);
		if(lpDataItem->cBit & 0x1)
			sBit = "1";
		else if(lpDataItem->cBit & 0x2)
			sBit = "2";
		else if(lpDataItem->cBit & 0x4)
			sBit = "3";
		else if(lpDataItem->cBit & 0x8)
			sBit = "4";
		else if(lpDataItem->cBit & 0x10)
			sBit = "5";
		else if(lpDataItem->cBit & 0x20)
			sBit = "6";
		else if(lpDataItem->cBit & 0x40)
			sBit = "7";
		else if(lpDataItem->cBit & 0x80)
			sBit = "8";
		bUserDefinedItem = FALSE;
	}
	sName = lpDataItem->szName;
	sUnit = lpDataItem->szUnit;
	if(lpDataItem->ulAddress_high == 0xFFFFFFFF)
		bQuadSpan = TRUE;
	else
		bQuadSpan = FALSE;
	iAddress_high = lpDataItem->ulAddress_high;
	iAddress_low = lpDataItem->ulAddress_low;
	iIndex_type = lpDataItem->cType-1;
	sOperand_addition.Format("%i",lpDataItem->usOperand_addition);
	sOperand_subtract.Format("%i",lpDataItem->usOperand_subtract);
	sOperand_multiplier.Format("%i",lpDataItem->usOperand_multiplier);
	sOperand_divisor.Format("%i",lpDataItem->usOperand_divisor);
	sDecimals.Format("%i",lpDataItem->usDecimals);
	sDescription = lpDataItem->szDescription;

	SetByteBitState(!bUserDefinedItem);
	SetAddressState(!bQuadSpan);
}

void dialogEditor_dataitem::SaveChanges()
{
	LPSTRUCT_LIVEBITITEM lpDataItem = NULL;

	if(listDataItem.GetCount() == 0) return;

	UpdateData(TRUE);
	if((lpDataItem = (LPSTRUCT_LIVEBITITEM)listDataItem.GetAt(pos)) == NULL)
		return;

	if(lpDataItem->szName != sName)
	{
		free(lpDataItem->szName);
		lpDataItem->cLength_name = sName.GetLength();
		lpDataItem->szName = (LPTSTR)malloc(lpDataItem->cLength_name + 1);
		memset(lpDataItem->szName,0,lpDataItem->cLength_name);
		strncpy(lpDataItem->szName,sName,lpDataItem->cLength_name);
		lpDataItem->szName[lpDataItem->cLength_name] = '\0';
	}
	if(lpDataItem->szUnit != sUnit)
	{
		free(lpDataItem->szUnit);
		lpDataItem->cLength_unit = sUnit.GetLength();
		lpDataItem->szUnit = (LPTSTR)malloc(lpDataItem->cLength_unit + 1);
		memset(lpDataItem->szUnit,0,lpDataItem->cLength_unit);
		strncpy(lpDataItem->szUnit,sUnit,lpDataItem->cLength_unit);
		lpDataItem->szUnit[lpDataItem->cLength_unit] = '\0';
	}
	if(lpDataItem->szDescription != sDescription)
	{
		free(lpDataItem->szDescription);
		lpDataItem->usLength_description = sDescription.GetLength();
		lpDataItem->szDescription = (LPTSTR)malloc(lpDataItem->usLength_description + 1);
		memset(lpDataItem->szDescription,0,lpDataItem->usLength_description);
		strncpy(lpDataItem->szDescription,sDescription,lpDataItem->usLength_description);
		lpDataItem->szDescription[lpDataItem->usLength_description] = '\0';
	}
	if(bUserDefinedItem)
		lpDataItem->cByte = lpDataItem->cBit = 0;
	else
	{
		lpDataItem->cByte = atoi(sByte);
		switch(atoi(sBit))
		{
		case 1:
			lpDataItem->cBit = 0x1;
			break;
		case 2:
			lpDataItem->cBit = 0x2;
			break;
		case 3:
			lpDataItem->cBit = 0x4;
			break;
		case 4:
			lpDataItem->cBit = 0x8;
			break;
		case 5:
			lpDataItem->cBit = 0x10;
			break;
		case 6:
			lpDataItem->cBit = 0x20;
			break;
		case 7:
			lpDataItem->cBit = 0x40;
			break;
		case 8:
			lpDataItem->cBit = 0x80;
			break;
		}
	}
	if(bQuadSpan) lpDataItem->ulAddress_high = 0xFFFFFFFF;
	else lpDataItem->ulAddress_high = iAddress_high;
	lpDataItem->ulAddress_low = iAddress_low;
	lpDataItem->cType = (char)lpComboBox_type->GetItemData(lpComboBox_type->GetCurSel());
	lpDataItem->usOperand_addition = atoi(sOperand_addition);
	lpDataItem->usOperand_subtract = atoi(sOperand_subtract);
	lpDataItem->usOperand_multiplier = atoi(sOperand_multiplier);
	lpDataItem->usOperand_divisor = atoi(sOperand_divisor);
	lpDataItem->usDecimals = atoi(sDecimals);
}

void dialogEditor_dataitem::OnAddNew()
{
	SaveChanges();

	LPSTRUCT_LIVEBITITEM lpDataItem = NULL;
	if((lpDataItem = new STRUCT_LIVEBITITEM) == NULL)
		return;
}

void dialogEditor_dataitem::OnDelete()
{
	CString sError;
	LPSTRUCT_LIVEBITITEM lpDataItem = NULL;
	POSITION posDelete = NULL;

	if(listDataItem.GetCount() == 0) return;

	if((lpDataItem = (LPSTRUCT_LIVEBITITEM)listDataItem.GetAt(pos)) == NULL)
		return;

	posDelete = pos;
	listDataItem.GetNext(pos);
	listDataItem.RemoveAt(posDelete);
	if(pos == NULL) pos = listDataItem.GetTailPosition();

	free(lpDataItem->szName);
	free(lpDataItem->szUnit);
	free(lpDataItem->szDescription);
	delete lpDataItem;

	if(listDataItem.GetCount() == 0)
	{}
	else
		GetData();
	UpdateLabels(FALSE);

	if(iIndex > listDataItem.GetCount()) iIndex = listDataItem.GetCount();
	sError.Format("Editor - Data Item Data File (%i of %i)",iIndex,listDataItem.GetCount());
	SetWindowText(sError);
}

void dialogEditor_dataitem::OnKillFocus()
{
	CDialog::OnKillFocus(this);

	UpdateLabels(TRUE);
}

void dialogEditor_dataitem::UpdateLabels(bool bMode)
{
	UpdateData(bMode);

	CString sError;
	sError.Format("Address (High Byte)\t- 0x%06X",iAddress_high);
	SetDlgItemText(IDC_EDITOR_DATAITEM_ADDRESSHIGHSTATIC,sError);
	sError.Format("Address (Low Byte)\t- 0x%06X",iAddress_low);
	SetDlgItemText(IDC_EDITOR_DATAITEM_ADDRESSLOWSTATIC,sError);
}

void dialogEditor_dataitem::UserDefinedItemChange()
{
	CButton* pButton = NULL;

	if((pButton = (CButton*)GetDlgItem(IDC_EDITOR_DATAITEM_USERDEFINEDCHECK)) == NULL)
		return;

	switch(pButton->GetCheck())
	{
	case BST_CHECKED:
		SetByteBitState(FALSE);
		break;
	case BST_UNCHECKED:
		SetByteBitState(TRUE);
		break;
	}
}

void dialogEditor_dataitem::SetByteBitState(BOOL bState)
{
	CSpinButtonCtrl* pSpin = NULL;
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_SPINBYTE)) == NULL)
		return;
	pSpin->EnableWindow(bState);
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_SPINBIT)) == NULL)
		return;
	pSpin->EnableWindow(bState);
	CEdit* pEdit = NULL;
	if((pEdit = (CEdit*)GetDlgItem(IDC_EDITOR_DATAITEM_BYTE)) == NULL)
		return;
	pEdit->EnableWindow(bState);
	if((pEdit = (CEdit*)GetDlgItem(IDC_EDITOR_DATAITEM_BIT)) == NULL)
		return;
	pEdit->EnableWindow(bState);
}

void dialogEditor_dataitem::QuadSpanChange()
{
	CButton* pButton = NULL;

	if((pButton = (CButton*)GetDlgItem(IDC_EDITOR_DATAITEM_CHECKQUADADDRESS)) == NULL)
		return;

	switch(pButton->GetCheck())
	{
	case BST_CHECKED:
		SetAddressState(FALSE);
		break;
	case BST_UNCHECKED:
		SetAddressState(TRUE);
		break;
	}
}

void dialogEditor_dataitem::SetAddressState(BOOL bState)
{
	CEdit* pEdit = NULL;
	if((pEdit = (CEdit*)GetDlgItem(IDC_EDITOR_DATAITEM_ADDRESSHIGH)) == NULL)
		return;
	pEdit->EnableWindow(bState);
	CSpinButtonCtrl* pSpin = NULL;
	if((pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_EDITOR_DATAITEM_ADDRESSHIGHSPIN)) == NULL)
		return;
	pSpin->EnableWindow(bState);
}
