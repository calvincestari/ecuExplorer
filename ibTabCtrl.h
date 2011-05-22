#pragma once
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>
#endif // _AFX_NO_AFXCMN_SUPPORT

// CibTabCtrl

class CibTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CibTabCtrl)

public:
	CibTabCtrl();
	virtual ~CibTabCtrl();

protected:
	DECLARE_MESSAGE_MAP()

private:
	CArray<CString,CString> TabCaptions;
	CArray<CDialog *,CDialog *> TabPanes;
	int m_iCurSel;
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	void AddTabPane(CString strCaption,CDialog * pDlg);
	afx_msg void OnMove(int x, int y);
	void SetDefaultPane(int iPaneIndex);
};


