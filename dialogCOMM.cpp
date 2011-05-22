/*/
	dialogCOMM.cpp (2005.11.08)
/*/

#include "dialogCOMM.h"
#include "definitionError.h"
#include "handlerError.h"
#include "ecuExplorer.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

BEGIN_MESSAGE_MAP(dialogCOMM, CDialog)
END_MESSAGE_MAP()

dialogCOMM::dialogCOMM() : CDialog(dialogCOMM::IDD)
{
	lpParent = NULL;
	iXonLim = iXoffLim = 0;
	iReadIntervalTimeout = iReadTotalTimeoutConstant = iWriteTotalTimeoutConstant = 0;
}

void dialogCOMM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_COMM_XONLIM,iXonLim);
	DDX_Text(pDX,IDC_COMM_XOFFLIM,iXoffLim);

	DDX_Text(pDX,IDC_COMM_READINTERVALTIMEOUT,iReadIntervalTimeout);
	DDX_Text(pDX,IDC_COMM_READTOTALTIMEOUTCONSTANT,iReadTotalTimeoutConstant);
	DDX_Text(pDX,IDC_COMM_WRITETOTALTIMEOUTCONSTANT,iWriteTotalTimeoutConstant);
}

void dialogCOMM::OnCancel()
{
	CDialog::OnCancel();
}

void dialogCOMM::OnOK()
{
	UpdateData();

	((ecuExplorer*)lpParent)->WriteProfileInt("Settings","XonLim",iXonLim);
	((ecuExplorer*)lpParent)->WriteProfileInt("Settings","XoffLim",iXoffLim);
	((ecuExplorer*)lpParent)->WriteProfileInt("Settings","ReadIntervalTimeout",iReadIntervalTimeout);
	((ecuExplorer*)lpParent)->WriteProfileInt("Settings","ReadTotalTimeoutConstant",iReadTotalTimeoutConstant);
	((ecuExplorer*)lpParent)->WriteProfileInt("Settings","WriteTotalTimeoutConstant",iWriteTotalTimeoutConstant);

	MessageBox("You must restart ecuExplorer for the changes to take effect","Restart",MB_ICONINFORMATION);

	CDialog::OnOK();
}

BOOL dialogCOMM::OnInitDialog()
{
	iXonLim = ((ecuExplorer*)lpParent)->GetProfileInt("Settings","XonLim",iXonLim);
	iXoffLim = ((ecuExplorer*)lpParent)->GetProfileInt("Settings","XoffLim",iXoffLim);

	iReadIntervalTimeout = ((ecuExplorer*)lpParent)->GetProfileInt("Settings","ReadIntervalTimeout",iReadIntervalTimeout);
	iReadTotalTimeoutConstant = ((ecuExplorer*)lpParent)->GetProfileInt("Settings","ReadTotalTimeoutConstant",iReadTotalTimeoutConstant);
	iWriteTotalTimeoutConstant = ((ecuExplorer*)lpParent)->GetProfileInt("Settings","WriteTotalTimeoutConstant",iWriteTotalTimeoutConstant);

	return CDialog::OnInitDialog();
}