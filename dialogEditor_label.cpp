#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "dialogEditor_label.h"
#include "definitionLocal.h"

BEGIN_MESSAGE_MAP(dialogEditor_label, CDialog)
	ON_COMMAND(IDBACK,OnBack)
	ON_COMMAND(IDNEXT,OnNext)
	ON_COMMAND(ID_MENU_EDITOR_DELETE, OnDelete)
	ON_COMMAND(ID_MENU_EDITOR_SAVE, OnSave)
END_MESSAGE_MAP()

dialogEditor_label::dialogEditor_label() : CDialog(dialogEditor_label::IDD)
{
	listLabel.RemoveAll();
	pFont = NULL;
}

BOOL dialogEditor_label::OnInitDialog()
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

	LoadDataFile();
	GetData();

	listLabel.GetCount() == 0 ? iIndex = 0 : iIndex = 1;
	sError.Format("Editor - Label Data File (%i of %i)",iIndex,listLabel.GetCount());
	SetWindowText(sError);

	return CDialog::OnInitDialog();
}

void dialogEditor_label::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_ECU_VERSION,sVersion);
	DDX_Text(pDX,IDC_EDIT_ECU_DESCRIPTION,sDescription);

	CString sError, sHex;
	sError = "ECU -";
	for(int index = 0; index < sCode.GetLength(); index++)
	{
		sHex.Format(" %02X",(unsigned char)sCode.GetAt(index));
		sError += sHex;
	}
	SetDlgItemText(IDC_FRAME_ECU_TYPE,sError);
}

void dialogEditor_label::OnOK()
{
	LPSTRUCT_LABELITEM lpLabelItem = NULL;

	while(listLabel.GetCount() > 0)
	{
		lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.RemoveHead();
		free(lpLabelItem->szID);
		free(lpLabelItem->szLabel);
		delete lpLabelItem;
	}

	if(pFont != NULL)
		delete pFont;

	CDialog::OnOK();
}

void dialogEditor_label::BrowseForFile(CString* pszReturn,LPCTSTR szTitle,DWORD dwFlags)
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

void dialogEditor_label::OnBack()
{
	CString sError;

	if(listLabel.GetCount() == 0) return;

	SaveChanges();
	iIndex--;
	if(iIndex <= 0)
	{
		iIndex = 1;
		pos = listLabel.GetHeadPosition();
	}
	else
		listLabel.GetPrev(pos);
	GetData();
	UpdateData(FALSE);

	sError.Format("Editor - Label Data File (%i of %i)",iIndex,listLabel.GetCount());
	SetWindowText(sError);
}

void dialogEditor_label::OnNext()
{
	CString sError;

	if(listLabel.GetCount() == 0) return;

	SaveChanges();
	iIndex++;
	if(iIndex >= listLabel.GetCount())
	{
		iIndex = listLabel.GetCount();
		pos = listLabel.GetTailPosition();
	}
	else
		listLabel.GetNext(pos);
	GetData();
	UpdateData(FALSE);

	sError.Format("Editor - Label Data File (%i of %i)",iIndex,listLabel.GetCount());
	SetWindowText(sError);
}

void dialogEditor_label::LoadDataFile()
{
	CString sError;
	int fh = 0;
	unsigned char cRead = 0;
	unsigned char cChecksum = 0;
	unsigned char cStorage[1024];
	LPSTRUCT_LABELITEM lpLabelItem = NULL;
	int iLoop = 0;

	// Label Data File Format
	// 1 bytes [Version Length]
	// x bytes [Version]
	// 1 bytes [Version Checksum]
	// [..]
	//		1 bytes [ECU ID Length]
	//		x bytes [ECU ID]
	//		1 bytes [Label Length]
	//		x bytes [Label]
	//		1 bytes [Label Checksum]
	// [..]

	BrowseForFile(&sError,"Select the label data file",OFN_FILEMUSTEXIST);
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
		if((lpLabelItem = new STRUCT_LABELITEM) == NULL)
			return;

		_read(fh,&lpLabelItem->cLength_id,1);
		lpLabelItem->szID = (LPTSTR)malloc(lpLabelItem->cLength_id + 1);
		memset(lpLabelItem->szID,0,lpLabelItem->cLength_id);
		_read(fh,&cStorage,lpLabelItem->cLength_id);
		cStorage[lpLabelItem->cLength_id] = '\0';
		strncpy(lpLabelItem->szID,(LPCTSTR)&cStorage[0],lpLabelItem->cLength_id + 1);

		_read(fh,&lpLabelItem->cLength_label,1);
		lpLabelItem->szLabel = (LPTSTR)malloc(lpLabelItem->cLength_label + 1);
		memset(lpLabelItem->szLabel,0,lpLabelItem->cLength_label);
		_read(fh,&cStorage,lpLabelItem->cLength_label);
		cStorage[lpLabelItem->cLength_label] = '\0';
		strncpy(lpLabelItem->szLabel,(LPCTSTR)&cStorage[0],lpLabelItem->cLength_label + 1);

		_read(fh,&cRead,1);		
		listLabel.AddTail(lpLabelItem);
	}

	_close(fh);
	pos = listLabel.GetHeadPosition();
}

void dialogEditor_label::OnSave()
{
	int fhOutput = -1;
	CString sError;
	int iLoop = 0;
	int iSub = 0;
	unsigned char cTemp;
	LPSTRUCT_LABELITEM lpLabelItem = NULL;
	unsigned long* lpOffset = NULL;
	POSITION pos = NULL;

	SaveChanges();
	BrowseForFile(&sError,"Select a filename for the label data file",OFN_OVERWRITEPROMPT);
	if((fhOutput = _open(sError,_O_BINARY | _O_CREAT | _O_WRONLY,_S_IWRITE)) == -1)
		return;
	if((_chsize(fhOutput,0)) == -1)
		return;

	// Label Data File Format
	// 1 bytes [Version Length]
	// x bytes [Version]
	// 1 bytes [Version Checksum]
	// [..]
	//		1 bytes [ECU ID Length]
	//		x bytes [ECU ID]
	//		1 bytes [Label Length]
	//		x bytes [Label]
	//		1 bytes [Label Checksum]
	// [..]

	cTemp = (char)sVersion.GetLength();
	_write(fhOutput,&cTemp,1);
	_write(fhOutput,(LPCTSTR)sVersion,cTemp);
	cTemp += GenerateChecksum(sVersion,sVersion.GetLength());
	_write(fhOutput,&cTemp,1);

	pos = listLabel.GetHeadPosition();
	while(pos != NULL)
	{
		lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetNext(pos);

		_write(fhOutput,&lpLabelItem->cLength_id,1);
		_write(fhOutput,lpLabelItem->szID,lpLabelItem->cLength_id);
		_write(fhOutput,&lpLabelItem->cLength_label,1);
		_write(fhOutput,lpLabelItem->szLabel,lpLabelItem->cLength_label);
		cTemp = GenerateChecksum(lpLabelItem);
		_write(fhOutput,&cTemp,1);
	}
}

unsigned char dialogEditor_label::GenerateChecksum(LPSTRUCT_LABELITEM lpLabelItem)
{
	int iLoop;
	unsigned char cReturn = 0;

	cReturn = lpLabelItem->cLength_id;
	for(iLoop = 0;iLoop < lpLabelItem->cLength_id;iLoop++)
		cReturn += lpLabelItem->szID[iLoop];
	cReturn += lpLabelItem->cLength_label;
	for(iLoop = 0;iLoop < lpLabelItem->cLength_label;iLoop++)
		cReturn += lpLabelItem->szLabel[iLoop];

	return cReturn;
}

unsigned char dialogEditor_label::GenerateChecksum(LPCTSTR szBuffer,unsigned char ucBufferLength)
{
	int iLoop;
	unsigned char cReturn = 0;

	for(iLoop = 0;iLoop < ucBufferLength;iLoop++)
		cReturn += szBuffer[iLoop];

	return cReturn;
}

void dialogEditor_label::GetData()
{
	LPSTRUCT_LABELITEM lpLabelItem = NULL;

	if(listLabel.GetCount() == 0) return;

	if((lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetAt(pos)) == NULL)
		return;

	sCode = lpLabelItem->szID;
	sDescription = lpLabelItem->szLabel;
}

void dialogEditor_label::SaveChanges()
{
	LPSTRUCT_LABELITEM lpLabelItem = NULL;

	if(listLabel.GetCount() == 0) return;

	UpdateData(TRUE);
	if((lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetAt(pos)) == NULL)
		return;

	if(lpLabelItem->szLabel != sDescription)
	{
		free(lpLabelItem->szLabel);
		lpLabelItem->cLength_label = sDescription.GetLength();
		lpLabelItem->szLabel = (LPTSTR)malloc(lpLabelItem->cLength_label + 1);
		memset(lpLabelItem->szLabel,0,lpLabelItem->cLength_label);
		strncpy(lpLabelItem->szLabel,sDescription,lpLabelItem->cLength_label);
		lpLabelItem->szLabel[lpLabelItem->cLength_label] = '\0';
	}
}

void dialogEditor_label::OnAddNew()
{
	SaveChanges();

	LPSTRUCT_LABELITEM lpLabelItem = NULL;
	if((lpLabelItem = new STRUCT_LABELITEM) == NULL)
		return;
}

void dialogEditor_label::OnDelete()
{
	CString sError;
	LPSTRUCT_LABELITEM lpLabelItem = NULL;
	POSITION posDelete = NULL;

	if(listLabel.GetCount() == 0) return;

	if((lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetAt(pos)) == NULL)
		return;

	posDelete = pos;
	listLabel.GetNext(pos);
	listLabel.RemoveAt(posDelete);
	if(pos == NULL) pos = listLabel.GetTailPosition();

	free(lpLabelItem->szID);
	free(lpLabelItem->szLabel);
	delete lpLabelItem;

	if(listLabel.GetCount() == 0)
	{
		sCode.Empty();
		sDescription.Empty();
	}
	else
		GetData();
	UpdateData(FALSE);

	if(iIndex > listLabel.GetCount()) iIndex = listLabel.GetCount();
	sError.Format("Editor - Label Data File (%i of %i)",iIndex,listLabel.GetCount());
	SetWindowText(sError);
}