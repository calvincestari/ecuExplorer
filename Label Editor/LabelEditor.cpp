/*/
	LabelEditor.cpp (2005.07.09)
/*/

#include "LabelEditor.h"
#include "CMainDlg.h"

CLabelEditorApp theApp;


BEGIN_MESSAGE_MAP(CLabelEditorApp, CWinApp)
END_MESSAGE_MAP()

CLabelEditorApp::CLabelEditorApp()
{}

BOOL CLabelEditorApp::InitInstance()
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