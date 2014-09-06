// DeeBookDlg.h : header file
//

#if !defined(AFX_DEEBOOKDLG_H__270C45CD_A827_4381_AACD_0FFAE8267EC1__INCLUDED_)
#define AFX_DEEBOOKDLG_H__270C45CD_A827_4381_AACD_0FFAE8267EC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WindowFinder.h"
#include "IEDumper.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CDeeBookDlg dialog

class CDeeBookDlg : public CDialog
{
// Construction
public:
	CDeeBookDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDeeBookDlg)
	enum { IDD = IDD_DEEBOOK };
	CStatic	m_staJPEG;
	CButton	m_chkSnap;
	CEdit	m_edtJPEG;
	CSpinButtonCtrl	m_spnJPEG;
	CWindowFinder	m_wndIEFinder;
	CWindowFinder	m_wndTreeFinder;
	CButton	m_chkCompileCHM;
	CButton	m_chkCHM;
	CButton	m_btnCHM;
	CButton	m_btnGo;
	CStatic	m_staURL;
	CString	m_csIEHandle;
	CString	m_csTreeHandle;
	CString	m_csFilePath;
	CString	m_csHomePage;
	CString	m_csURLPath;
	CString	m_csOutputDir;
	UINT	m_nLoadTimeout;
	BOOL	m_bCreateCHM;
	BOOL	m_bCompileCHM;
	UINT	m_nLoadWait;
	UINT	m_nJPEGQuality;
	BOOL	m_bSnapImages;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeeBookDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDeeBookDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelIEWindow(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelTreeWindow(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnGo();
	afx_msg void OnBtnBrowseOutputdir();
	afx_msg void OnChkChm();
	afx_msg void OnBtnChm();
	afx_msg void OnClose();
	afx_msg void OnDecompileProgress(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnModeMenuItem(UINT nID);
	afx_msg void OnChkSnap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

private:
	CIEDumper m_dumperData;
	CIEDumper* m_pDumper;
	CIEDumper::CHM_OPTIONS m_chmOptions;
	CWinThread* m_pDumpThread;

	HICON	m_hFinderIcon;
	HICON	m_hFinderDragIcon;
	HCURSOR	m_hFinderCursor;
	
	BOOL	m_bWorking;
	BOOL	m_bEntireMode;
	void SetWindowStyle();
	static UINT DecompileThread(LPVOID lpParam);
	void OnDecompileFinish();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEEBOOKDLG_H__270C45CD_A827_4381_AACD_0FFAE8267EC1__INCLUDED_)
