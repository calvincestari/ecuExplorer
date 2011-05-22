/*/
	DataItemEditor.cpp (2005.08.02)
/*/

#include "DataItemEditor.h"
#include "CMainDlg.h"

CDataItemEditorApp theApp;


BEGIN_MESSAGE_MAP(CDataItemEditorApp, CWinApp)
END_MESSAGE_MAP()

CDataItemEditorApp::CDataItemEditorApp()
{}

BOOL CDataItemEditorApp::InitInstance()
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