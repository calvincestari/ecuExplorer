#ifndef INC_STABCTRL
#define INC_STABCTRL

#include <afxtempl.h>

// STabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSTabCtrl window
//
// Implements a tab control that automatically handles the showing and hiding
// of objects attached to a tab's various pages eliminating the need to do so
// via the ON_NOTIFY(TCN_SELCHANGE... ) message.
//
// 1.	Simply replace any instance of a CTabCtrl with CSTabCtrl,
//		initialize it as you would an MFC CTabCtrl.
// 2.	Use the AttachControlToTab member to attach you objects 
//		to the various avaliable pages.
// 
// 3.	(optional) Use the SetCurSel member to programaticly show
//		a tabs particular page.
//
// Once done the tab control will show and hide the attached objects depending
// on the users tab selection.

// example of CSTabCtrl's use.
//
//

/*
	// file : SomeDialogClass.h

	class CSomeDialogClass : public CDialog
	{
		CSTabCtrl m_TabCtrl;
		CTreeCtrl m_TreeCtrl;
		CListCtrl m_ListCtrl;
		CComboBox m_ComboCtrl;

		virtual BOOL OnInitDialog();
	};


	// file : SomeDialogClass.cpp	

	BOOL CSomeDialogClass::OnInitDialog()
	{
		CDialog::OnInitDialog();

		////////////////////////////////////////////////////////
		// set up tabs.
		
		PSTR pszTabItems[] =
		{
			"Tab Sheet 1 : Tree control",
			"Tab Sheet 2 : List control",
			"Tab Sheet 3 : Combobox control",
			NULL
		};

		TC_ITEM tcItem;

		for(INT i = 0;
			pszTabItems[i] != NULL;
			i++)
		{
			tcItem.mask = TCIF_TEXT;
			tcItem.pszText = pszTabItems[i];
			tcItem.cchTextMax = strlen(pszTabItems[i]);
			m_TabCtrl.InsertItem(i,&tcItem);
		}

		// attach controls to tabs pages.

		m_TabCtrl.AttachControlToTab(&m_TreeCtrl,0);	// attach tree control to first page
		m_TabCtrl.AttachControlToTab(&m_ListCtrl,1);	// attach list control to second page
		m_TabCtrl.AttachControlToTab(&m_ComboCtrl,2);	// attach combo box control to third page

		// initialize tab to first page.
		m_TabCtrl.SetCurSel(0);
		////////////////////////////////////////////////////////
	}


 */

class CSTabCtrl : public CTabCtrl
{
// Construction
public:
	CSTabCtrl();

// Attributes
public:

// Operations
public:
	void ResizeControl(CRect* rect);
	BOOL CreateView(INT iTab,CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext);
	CWnd* GetTab(INT iTab);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTabCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSTabCtrl();
	virtual BOOL AttachControlToTab(CWnd * _pControl,INT _nTabNum);
	virtual SetCurSel( int nItem );

	// Generated message map functions
protected:
	//{{AFX_MSG(CSTabCtrl)
	afx_msg BOOL OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnable( BOOL bEnable );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	class CSTabPage
	{
	private:
		CList <CWnd *, CWnd *> m_ControlList;

	public:
		CSTabPage();
		~CSTabPage();

		BOOL AttachControl (CWnd * _pControl);
		BOOL ShowWindows ( INT _nCmdShow );
		BOOL EnableWindows ( BOOL _bEnable );
		BOOL ResizeControl(CRect* rect);
		CWnd* GetTab();
	};

	class CPageToControlsMap : public CMap <INT, INT&,CSTabPage *, CSTabPage *>
	{
	public:
		CPageToControlsMap( );
		~CPageToControlsMap( );


		BOOL AttachControl(INT _nTabNum,CWnd * _pControl);
		BOOL ShowWindows ( INT _nCurrPage, INT _nCmdShow );
		BOOL EnableWindows ( INT _nCurrPage, BOOL _bEnable );
		BOOL ResizeControl(INT _nCurrPage,CRect* rect);
		CWnd* GetTab(INT iTab);
		void RemoveAll( );
	};

	INT m_nPrevSel;
	CPageToControlsMap m_TabPagesMap;
};

#endif // INC_STABCTRL
/////////////////////////////////////////////////////////////////////////////
