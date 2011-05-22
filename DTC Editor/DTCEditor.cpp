/*/
	DTCEditor.cpp (2005.08.02)
/*/

#include "DTCEditor.h"
#include "CMainDlg.h"

CDTCEditorApp theApp;


BEGIN_MESSAGE_MAP(CDTCEditorApp, CWinApp)
END_MESSAGE_MAP()

CDTCEditorApp::CDTCEditorApp()
{}

BOOL CDTCEditorApp::InitInstance()
{
	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	CMainDlg dlgMain;
	dlgMain.DoModal();

	return FALSE;
}