/*/
	CMainDlg.cpp (2005.07.09)
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
END_MESSAGE_MAP()

CMainDlg::CMainDlg() : CDialog(CMainDlg::IDD)
{
	listLabel.RemoveAll();
	pos = NULL;
	usIndex = 0;
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_ECUID,sID);
	DDX_Text(pDX,IDC_LABEL,sLabel);
	DDX_Text(pDX,IDC_CHECKSUM,sChecksum);
	DDX_Text(pDX,IDC_VERSION,sVersion);
}

void CMainDlg::OnCancel()
{
	LPSTRUCT_LABELITEM lpLabelItem = NULL;

	while(listLabel.GetCount() > 0)
	{
		lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.RemoveHead();
		free(lpLabelItem->szID);
		free(lpLabelItem->szLabel);
//		free(lpLabelItem->szType);
		delete lpLabelItem;
	}

	CDialog::OnCancel();
}

void CMainDlg::OnBack()
{
	CString sText;
	LPSTRUCT_LABELITEM lpLabelItem = NULL;
	int iLoop;

	UpdateData(TRUE);
	lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetAt(pos);
	if(sLabel != lpLabelItem->szLabel)
	{
		free(lpLabelItem->szLabel);
		lpLabelItem->cLength_label = sLabel.GetLength();
		lpLabelItem->szLabel = (LPTSTR)malloc(lpLabelItem->cLength_label + 1);
		memset(lpLabelItem->szLabel,0,lpLabelItem->cLength_label);
		strncpy(lpLabelItem->szLabel,sLabel,lpLabelItem->cLength_label);
		lpLabelItem->szLabel[lpLabelItem->cLength_label] = '\0';

		GenerateChecksum(lpLabelItem);
	}
	sID.Empty();

	listLabel.GetPrev(pos);
	if(pos == NULL)
	{
		pos = listLabel.GetHeadPosition();
		return;
	}

	lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetAt(pos);
	for(iLoop = 0;iLoop < lpLabelItem->cLength_id;iLoop++)
	{
		sText.Format("%02X ",(unsigned char)lpLabelItem->szID[iLoop]);
		sID += sText;
	}
	sLabel = lpLabelItem->szLabel;
//	sType = lpLabelItem->szType;
	sChecksum = lpLabelItem->cChecksum;
	UpdateData(FALSE);

	usIndex--;
	sText.Format("ecuExplorer Label Editor - %i of %i",usIndex,listLabel.GetCount());
	SetWindowText(sText);
}

void CMainDlg::OnNext()
{
	CString sText;
	LPSTRUCT_LABELITEM lpLabelItem = NULL;
	int iLoop;

	UpdateData(TRUE);
	lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetAt(pos);
	if(sLabel != lpLabelItem->szLabel)
	{
		free(lpLabelItem->szLabel);
		lpLabelItem->cLength_label = sLabel.GetLength();
		lpLabelItem->szLabel = (LPTSTR)malloc(lpLabelItem->cLength_label + 1);
		memset(lpLabelItem->szLabel,0,lpLabelItem->cLength_label);
		strncpy(lpLabelItem->szLabel,sLabel,lpLabelItem->cLength_label);
		lpLabelItem->szLabel[lpLabelItem->cLength_label] = '\0';

		GenerateChecksum(lpLabelItem);
	}
	sID.Empty();

	listLabel.GetNext(pos);
	if(pos == NULL)
	{
		pos = listLabel.GetTailPosition();
		return;
	}

	lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetAt(pos);
	for(iLoop = 0;iLoop < lpLabelItem->cLength_id;iLoop++)
	{
		sText.Format("%02X ",(unsigned char)lpLabelItem->szID[iLoop]);
		sID += sText;
	}
	sLabel = lpLabelItem->szLabel;
//	sType = lpLabelItem->szType;
	sChecksum = lpLabelItem->cChecksum;
	UpdateData(FALSE);

	usIndex++;
	sText.Format("ecuExplorer Label Editor - %i of %i",usIndex,listLabel.GetCount());
	SetWindowText(sText);
}

void CMainDlg::OnLoad()
{
	CString sFile;
	int fhLabel = 0;
	unsigned char cRead;
	unsigned char cStorage[255];
	LPSTRUCT_LABELITEM lpLabelItem = NULL;
	int iLoop;

	if(!BrowseForFile(&sFile))
		return;

	if((fhLabel = _open(sFile,_O_BINARY | _O_RDONLY,_S_IREAD)) == -1)
		return;

	_lseek(fhLabel,0,SEEK_SET);

	_read(fhLabel,&cRead,1);
	_read(fhLabel,&cStorage,cRead);
	strncpy(sVersion.GetBufferSetLength(cRead),(LPCTSTR)&cStorage[0],cRead);
	sVersion.ReleaseBuffer();
	_read(fhLabel,&cRead,1);

	while(!_eof(fhLabel))
	{
		if((lpLabelItem = new STRUCT_LABELITEM) == NULL)
			return;

		_read(fhLabel,&lpLabelItem->cLength_id,1);
		lpLabelItem->szID = (LPTSTR)malloc(lpLabelItem->cLength_id + 1);
		memset(lpLabelItem->szID,0,lpLabelItem->cLength_id);
		_read(fhLabel,&cStorage,lpLabelItem->cLength_id);
		cStorage[lpLabelItem->cLength_id] = '\0';
		strncpy(lpLabelItem->szID,(LPCTSTR)&cStorage[0],lpLabelItem->cLength_id + 1);

		_read(fhLabel,&lpLabelItem->cLength_label,1);
		lpLabelItem->szLabel = (LPTSTR)malloc(lpLabelItem->cLength_label + 1);
		memset(lpLabelItem->szLabel,0,lpLabelItem->cLength_label);
		_read(fhLabel,&cStorage,lpLabelItem->cLength_label);
		cStorage[lpLabelItem->cLength_label] = '\0';
		strncpy(lpLabelItem->szLabel,(LPCTSTR)&cStorage[0],lpLabelItem->cLength_label + 1);

//		_read(fhLabel,&lpLabelItem->cLength_type,1);
//		lpLabelItem->szType = (LPTSTR)malloc(lpLabelItem->cLength_type + 1);
//		memset(lpLabelItem->szType,0,lpLabelItem->cLength_type);
//		_read(fhLabel,&cStorage,lpLabelItem->cLength_type);
//		cStorage[lpLabelItem->cLength_type] = '\0';
//		strncpy(lpLabelItem->szType,(LPCTSTR)&cStorage[0],lpLabelItem->cLength_type + 1);

		_read(fhLabel,&lpLabelItem->cChecksum,1);

		listLabel.AddTail(lpLabelItem);
	}

	_close(fhLabel);

	pos = listLabel.GetHeadPosition();
	lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetAt(pos);
	for(iLoop = 0;iLoop < lpLabelItem->cLength_id;iLoop++)
	{
		sFile.Format("%02X ",(unsigned char)lpLabelItem->szID[iLoop]);
		sID += sFile;
	}
	sLabel = lpLabelItem->szLabel;
//	sType = lpLabelItem->szType;
	sChecksum = lpLabelItem->cChecksum;
	UpdateData(FALSE);

	usIndex++;
	sFile.Format("ecuExplorer Label Editor - %i of %i",usIndex,listLabel.GetCount());
	SetWindowText(sFile);
}

void CMainDlg::OnSave()
{
	CString sFile;
	int fhLabel = 0;
	LPSTRUCT_LABELITEM lpLabelItem = NULL;
	unsigned char cTemp;

	UpdateData(TRUE);
	lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetAt(pos);
	if(sLabel != lpLabelItem->szLabel)
	{
		free(lpLabelItem->szLabel);
		lpLabelItem->cLength_label = sLabel.GetLength();
		lpLabelItem->szLabel = (LPTSTR)malloc(lpLabelItem->cLength_label + 1);
		memset(lpLabelItem->szLabel,0,lpLabelItem->cLength_label);
		strncpy(lpLabelItem->szLabel,sLabel,lpLabelItem->cLength_label);
		lpLabelItem->szLabel[lpLabelItem->cLength_label] = '\0';

		GenerateChecksum(lpLabelItem);
	}

	if(!BrowseForFile(&sFile))
		return;

	if((fhLabel = _open(sFile,_O_BINARY | _O_CREAT | _O_APPEND | _O_WRONLY,_S_IWRITE)) == -1)
		return;

	_chsize(fhLabel,0);
	cTemp = sVersion.GetLength();
	_write(fhLabel,&cTemp,1);
	_write(fhLabel,sVersion,sVersion.GetLength());
	cTemp += GenerateChecksum(sVersion,sVersion.GetLength());
	_write(fhLabel,&cTemp,1);

	pos = listLabel.GetHeadPosition();
	while(pos != NULL)
	{
		lpLabelItem = (LPSTRUCT_LABELITEM)listLabel.GetNext(pos);
		_write(fhLabel,&lpLabelItem->cLength_id,1);
		_write(fhLabel,lpLabelItem->szID,lpLabelItem->cLength_id);
		_write(fhLabel,&lpLabelItem->cLength_label,1);
		_write(fhLabel,lpLabelItem->szLabel,lpLabelItem->cLength_label);
		GenerateChecksum(lpLabelItem);

		_write(fhLabel,&lpLabelItem->cChecksum,1);
	}

	_close(fhLabel);
}

long CMainDlg::BrowseForFile(CString* szReturn)
{
	TCHAR szTitle[255];
	TCHAR szDefExt[255];
	TCHAR szFile[255] = "\0";
	OPENFILENAME ofnFileName;

	strcpy(szFile,"");
	strcpy(szTitle,"Select a label data file");
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

void CMainDlg::GenerateChecksum(LPSTRUCT_LABELITEM lpLabelItem)
{
	int iLoop;

	lpLabelItem->cChecksum = lpLabelItem->cLength_id;
	for(iLoop = 0;iLoop < lpLabelItem->cLength_id;iLoop++)
		lpLabelItem->cChecksum += lpLabelItem->szID[iLoop];
	lpLabelItem->cChecksum += lpLabelItem->cLength_label;
	for(iLoop = 0;iLoop < lpLabelItem->cLength_label;iLoop++)
		lpLabelItem->cChecksum += lpLabelItem->szLabel[iLoop];
//	lpLabelItem->cChecksum += lpLabelItem->cLength_type;
//	for(iLoop = 0;iLoop < lpLabelItem->cLength_type;iLoop++)
//		lpLabelItem->cChecksum += lpLabelItem->szType[iLoop];
}

unsigned char CMainDlg::GenerateChecksum(LPCTSTR szBuffer,unsigned char ucBufferLength)
{
	int iLoop;
	unsigned char cReturn = 0;

	for(iLoop = 0;iLoop < ucBufferLength;iLoop++)
		cReturn += szBuffer[iLoop];

	return cReturn;
}