#if !defined(AFX_CHMDLG_H__71167F2E_9E09_4DF4_A807_9358F6D9370D__INCLUDED_)
#define AFX_CHMDLG_H__71167F2E_9E09_4DF4_A807_9358F6D9370D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CHMDlg.h : header file
//

#include "IEDumper.h"

/////////////////////////////////////////////////////////////////////////////
// CCHMDlg dialog

class CCHMDlg : public CDialog
{
// Construction
public:
	CCHMDlg(CWnd* pParent = NULL);   // standard constructor

	void GetCHMOptions(CIEDumper::CHM_OPTIONS& chmOptions);
	void SetCHMOptions(CIEDumper::CHM_OPTIONS& chmOptions);

// Dialog Data
	//{{AFX_DATA(CCHMDlg)
	enum { IDD = IDD_CHM };
	CStatic	m_staMultiple;
	CEdit	m_edtHeight;
	CEdit	m_edtWidth;
	CString	m_csTitle;
	UINT	m_nWidth;
	UINT	m_nHeight;
	UINT	m_nPaneWidth;
	BOOL	m_bHideToolbarText;
	BOOL	m_bRememberWinPos;
    BOOL    m_bFullTextSearch;
	int		m_nSizeType;
	int		m_nLanguage;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCHMDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCHMDlg)
	afx_msg void OnRadioWindowSize();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSize m_szWinType[6];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHMDLG_H__71167F2E_9E09_4DF4_A807_9358F6D9370D__INCLUDED_)
