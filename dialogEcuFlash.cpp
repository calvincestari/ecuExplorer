/*/
	dialogEcuFlash.cpp (2005.11.23)
/*/

#include "dialogEcuFlash.h"
#include "definitionError.h"
#include "handlerError.h"
#include "ecuExplorer.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

BEGIN_MESSAGE_MAP(dialogEcuFlash, CDialog)
END_MESSAGE_MAP()

dialogEcuFlash::dialogEcuFlash() : CDialog(dialogEcuFlash::IDD)
{
	lpParent = NULL;
	iLength = 0;
	lpFont = NULL;
	lpButton = NULL;
}

void dialogEcuFlash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void dialogEcuFlash::OnOK()
{
	if(lpFont != NULL)
		delete lpFont;

	CDialog::OnOK();
}

BOOL dialogEcuFlash::OnInitDialog()
{
	lpFont = new CFont;
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

	lpFont->CreateFontIndirect(&lf);

	CEdit* lpEdit = NULL;
	if((lpEdit = (CEdit*)GetDlgItem(IDC_ECUFLASH_PROGRESS)) != NULL)
		lpEdit->SetFont(lpFont);

	if((lpButton = (CButton*)GetDlgItem(IDOK)) == NULL)
		return FALSE;

	return CDialog::OnInitDialog();
}

void dialogEcuFlash::UpdateProgress(LPCTSTR cBuffer)
{
	CString sError;
	CEdit* lpEdit = NULL;

	CString sNew = cBuffer;

	GetDlgItemText(IDC_ECUFLASH_PROGRESS,sError);
	sNew.Replace("\n","\r\n");
	sError += sNew;
	SetDlgItemText(IDC_ECUFLASH_PROGRESS,sError);

	if((lpEdit = (CEdit*)GetDlgItem(IDC_ECUFLASH_PROGRESS)) != NULL)
		lpEdit->LineScroll(lpEdit->GetLineCount());
}