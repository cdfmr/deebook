// DeeBookDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeeBook.h"
#include "DeeBookDlg.h"
#include "CHMDlg.h"
#include "SysUtils.h"
#include "WndUtils.h"

using namespace SysUtils;
using namespace WndUtils;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeeBookDlg dialog

CDeeBookDlg::CDeeBookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeeBookDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeeBookDlg)
	m_csIEHandle = _T("");
	m_csTreeHandle = _T("");
	m_csFilePath = _T("");
	m_csHomePage = _T("");
	m_csURLPath = _T("");
	m_csOutputDir = _T("");
	m_nLoadTimeout = 10000;
	m_bCreateCHM = TRUE;
	m_bCompileCHM = FALSE;
	m_nLoadWait = 200;
	m_nJPEGQuality = 85;
	m_pDumper = NULL;
	m_bWorking = FALSE;
	m_pDumpThread = NULL;
	m_bEntireMode = TRUE;
	m_bSnapImages = TRUE;
	//}}AFX_DATA_INIT

	// load icons & cursors
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hFinderIcon = AfxGetApp()->LoadIcon(IDI_WINFINDER);
	m_hFinderDragIcon = AfxGetApp()->LoadIcon(IDI_WINFINDER_DRAG);
	m_hFinderCursor = AfxGetApp()->LoadCursor(IDC_WINFINDER_DRAG);

	// initialize chm options
	m_chmOptions.szWindowSize = CSize(640, 480);
	m_chmOptions.nPaneWidth = 200;
	m_chmOptions.bHideToolbarText = FALSE;
	m_chmOptions.bRememberWinPos = TRUE;
    m_chmOptions.bFullTextSearch = FALSE;
	m_chmOptions.nLanguage = 1;
}

void CDeeBookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeeBookDlg)
	DDX_Control(pDX, IDC_STA_JPEG, m_staJPEG);
	DDX_Control(pDX, IDC_CHK_SNAP, m_chkSnap);
	DDX_Control(pDX, IDC_EDT_JPEG, m_edtJPEG);
	DDX_Control(pDX, IDC_SPN_JPEG, m_spnJPEG);
	DDX_Control(pDX, IDC_WINFINDER_IE, m_wndIEFinder);
	DDX_Control(pDX, IDC_WINFINDER_TREE, m_wndTreeFinder);
	DDX_Control(pDX, IDC_CHK_COMPILECHM, m_chkCompileCHM);
	DDX_Control(pDX, IDC_CHK_CHM, m_chkCHM);
	DDX_Control(pDX, IDC_BTN_CHM, m_btnCHM);
	DDX_Control(pDX, IDC_BTN_GO, m_btnGo);
	DDX_Control(pDX, IDC_STA_URL, m_staURL);
	DDX_Text(pDX, IDC_STA_IEHANDLE, m_csIEHandle);
	DDX_Text(pDX, IDC_STA_TREEHANDLE, m_csTreeHandle);
	DDX_Text(pDX, IDC_EDT_FILEPATH, m_csFilePath);
	DDX_Text(pDX, IDC_EDT_HOMEPAGE, m_csHomePage);
	DDX_Text(pDX, IDC_EDT_URLPATH, m_csURLPath);
	DDX_Text(pDX, IDC_EDT_OUTPUTDIR, m_csOutputDir);
	DDX_Text(pDX, IDC_EDT_TIMEOUT, m_nLoadTimeout);
	DDV_MinMaxUInt(pDX, m_nLoadTimeout, 500, 60000);
	DDX_Check(pDX, IDC_CHK_CHM, m_bCreateCHM);
	DDX_Check(pDX, IDC_CHK_COMPILECHM, m_bCompileCHM);
	DDX_Text(pDX, IDC_EDT_WAIT, m_nLoadWait);
	DDV_MinMaxUInt(pDX, m_nLoadWait, 100, 2000);
	DDX_Text(pDX, IDC_EDT_JPEG, m_nJPEGQuality);
	DDX_Check(pDX, IDC_CHK_SNAP, m_bSnapImages);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDeeBookDlg, CDialog)
	//{{AFX_MSG_MAP(CDeeBookDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(WFN_SELWINDOW, IDC_WINFINDER_IE, OnSelIEWindow)
	ON_NOTIFY(WFN_SELWINDOW, IDC_WINFINDER_TREE, OnSelTreeWindow)
	ON_BN_CLICKED(IDC_BTN_GO, OnBtnGo)
	ON_BN_CLICKED(IDC_BTN_BROWSE_OUTPUTDIR, OnBtnBrowseOutputdir)
	ON_BN_CLICKED(IDC_CHK_CHM, OnChkChm)
	ON_BN_CLICKED(IDC_BTN_CHM, OnBtnChm)
	ON_WM_CLOSE()
	ON_NOTIFY(IDN_PROGRESS, 0, OnDecompileProgress)
	ON_COMMAND_RANGE(ID_MODE_ENTIRE, ID_MODE_CURRENT, OnModeMenuItem)
	ON_BN_CLICKED(IDC_CHK_SNAP, OnChkSnap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeeBookDlg message handlers

BOOL CDeeBookDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// enable new layered window style
	ModifyStyleEx(0, WS_EX_LAYERED);
	SetWindowStyle();
	
	// initialize window finder controls
	m_wndIEFinder.SetUIResource(m_hFinderIcon, m_hFinderDragIcon, m_hFinderCursor);
	m_wndTreeFinder.SetUIResource(m_hFinderIcon, m_hFinderDragIcon, m_hFinderCursor);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDeeBookDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDeeBookDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDeeBookDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDeeBookDlg::OnClose() 
{
	// don't allow to close while decompiling
	if (!m_bWorking)
	{
		CDialog::OnClose();
	}
}

void CDeeBookDlg::SetWindowStyle()
{
	if (m_bWorking)
	{
		m_btnGo.SetWindowText(_T("&Stop"));
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        ::SetLayeredWindowAttributes(GetSafeHwnd(), 0, m_bSnapImages ? 0 : 200, LWA_ALPHA);
	}
	else
	{
		m_btnGo.SetWindowText(_T("&Decompile"));
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		::SetLayeredWindowAttributes(GetSafeHwnd(), 0, 255, LWA_ALPHA);
		UpdateWindow();
	}
}

void CDeeBookDlg::OnSelIEWindow(NMHDR* pNMHDR, LRESULT* pResult)
{
	// store ie control handle
	HWND hIECtrl = m_wndIEFinder.GetSelWindow();
	if (!m_dumperData.SetIECtrlHandle(hIECtrl))
	{
		AfxMessageBox(_T("Can't get html interface, please select a valid Internet Explorer window."), MB_OK | MB_ICONERROR);
		return;
	}

	UpdateData();

	// update variables
	m_staURL.SetWindowText(_T(""));
	m_csIEHandle.Format(_T("0x%08X"), hIECtrl);
	m_csTreeHandle.Empty();
	m_csFilePath = GetProcessExecutable(GetWindowProcessId(hIECtrl));
	m_csHomePage = m_dumperData.GetPageURL();
	m_csURLPath = ExtractURLPath(m_csHomePage);
	m_csOutputDir.Empty();
	int p = m_csFilePath.ReverseFind(_T('.'));
	if (p >= 0)
	{
		m_csOutputDir = m_csFilePath.Left(p);
	}

	// update chm options
	HWND hEbookWnd = GetTopWindowFromControl(hIECtrl);
	CWnd* pEbookWnd = CWnd::FromHandle(hEbookWnd);
	pEbookWnd->GetWindowText(m_chmOptions.csWindowTitle);
	CRect rcEbook;
	pEbookWnd->GetWindowRect(&rcEbook);
	m_chmOptions.szWindowSize = CSize(rcEbook.Width(), rcEbook.Height());
    if (m_chmOptions.szWindowSize.cx < 100) m_chmOptions.szWindowSize.cx = 100;
    if (m_chmOptions.szWindowSize.cy < 100) m_chmOptions.szWindowSize.cy = 100;

	UpdateData(FALSE);
}

void CDeeBookDlg::OnSelTreeWindow(NMHDR* pNMHDR, LRESULT* pResult)
{
	// check whether ie control is selected
	if (!m_dumperData.GetIECtrlHandle())
	{
		AfxMessageBox(_T("Please select the Internet Explorer window first."), MB_OK | MB_ICONERROR);
		return;
	}

	// store tree control handle
	HWND hTreeCtrl = m_wndTreeFinder.GetSelWindow();
	if (!m_dumperData.SetTreeCtrlHandle(hTreeCtrl))
	{
		AfxMessageBox(_T("Please select the corresponding navigation tree control."), MB_OK | MB_ICONERROR);
		return;
	}

	// update variables
	UpdateData();
	m_csTreeHandle.Format(_T("0x%08X"), hTreeCtrl);
	m_staURL.SetWindowText(_T(""));
	UpdateData(FALSE);

	// update chm options
	CWnd* pTreeCtrl = CWnd::FromHandle(hTreeCtrl);
	CRect rcTree;
	pTreeCtrl->GetWindowRect(&rcTree);
	m_chmOptions.nPaneWidth = rcTree.Width();
}

void CDeeBookDlg::OnBtnBrowseOutputdir() 
{
	TCHAR		buf[1024];
	BROWSEINFO	bi;
	::ZeroMemory(&bi, sizeof(bi));
	bi.hwndOwner		= GetSafeHwnd();
	bi.pszDisplayName	= buf;
	bi.ulFlags			= BIF_RETURNONLYFSDIRS;
	bi.lParam			= BFFM_INITIALIZED;

	LPITEMIDLIST pItemIDList = SHBrowseForFolder(&bi);
	if (pItemIDList)
	{
		if (SHGetPathFromIDList(pItemIDList, buf))
		{
			UpdateData();
			m_csOutputDir = buf;
			UpdateData(FALSE);
		}
	}
}

void CDeeBookDlg::OnChkChm()
{
	m_chkCompileCHM.EnableWindow(m_chkCHM.GetCheck());
	m_btnCHM.EnableWindow(m_chkCHM.GetCheck());
}

void CDeeBookDlg::OnBtnChm() 
{
	CCHMDlg dlg;
	dlg.SetCHMOptions(m_chmOptions);
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCHMOptions(m_chmOptions);
	}
}

void CDeeBookDlg::OnBtnGo() 
{
	if (!m_bWorking)
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU_MODE);
		CRect rect;
		m_btnGo.GetWindowRect(&rect);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTBUTTON | TPM_RIGHTBUTTON, rect.left, rect.bottom, this);
	}
	else
	{
		m_pDumper->Terminate();
	}
}

void CDeeBookDlg::OnModeMenuItem(UINT nID)
{
	m_bWorking = TRUE;
	m_bEntireMode = nID == ID_MODE_ENTIRE;
	m_staURL.SetWindowText(_T(""));
	UpdateData();
	m_pDumpThread = AfxBeginThread(DecompileThread, this);
}

UINT CDeeBookDlg::DecompileThread(LPVOID lpParam)
{
	// initialize com library
	CoInitialize(NULL);

	// get objects
	CDeeBookDlg* pDlg = (CDeeBookDlg*)lpParam;
	CIEDumper* pDumperData = &pDlg->m_dumperData;
	pDlg->m_pDumper = new CIEDumper();
	CIEDumper* pDumper = pDlg->m_pDumper;

	// set working dumper parameters
	HWND hIE = pDumperData->GetIECtrlHandle();
	HWND hTree = pDumperData->GetTreeCtrlHandle();
	if (hIE) pDumper->SetIECtrlHandle(hIE);
	if (hTree) pDumper->SetTreeCtrlHandle(hTree);
	pDumper->SetOwnerWindow(pDlg->GetSafeHwnd());
	pDumper->SetLoadWait(pDlg->m_nLoadWait);
	pDumper->SetLoadTimeout(pDlg->m_nLoadTimeout);
	pDumper->SetURLBase(pDlg->m_csURLPath);
	pDumper->SetOutputDir(pDlg->m_csOutputDir);
    pDumper->SetSnapImages(pDlg->m_bSnapImages);
    pDumper->SetJPEGQuality(pDlg->m_nJPEGQuality);

	// set chm options
	if (!pDlg->m_csOutputDir.IsEmpty() &&
		pDlg->m_csOutputDir[pDlg->m_csOutputDir.GetLength() - 1] != _T('\\'))
	{
		pDlg->m_csOutputDir += _T('\\');
	}
	pDlg->m_chmOptions.bCreateCHM = pDlg->m_bCreateCHM;
	pDlg->m_chmOptions.bCompileCHM = pDlg->m_bCompileCHM;
	pDlg->m_chmOptions.csProjectFile = pDlg->m_csOutputDir + ChangeFileExt(ExtractFileName(pDlg->m_csFilePath), _T("hhp"));
	pDlg->m_chmOptions.csContentFile = pDlg->m_csOutputDir + ChangeFileExt(ExtractFileName(pDlg->m_csFilePath), _T("hhc"));
	pDumper->SetCHMOptions(pDlg->m_chmOptions);

	// update window style
	pDlg->SetWindowStyle();

	// decompile
	pDumper->Decompile(!pDlg->m_bEntireMode);

	// uninitialize com library
	CoUninitialize();

	// notify message
	pDlg->OnDecompileFinish();

	return 0;
}

void CDeeBookDlg::OnDecompileProgress(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_pDumper)
	{
		IDNMHDR* pidnmhdr = (IDNMHDR*)pNMHDR;
		CString csText = _T(" ");
		csText += (LPCTSTR)pidnmhdr->dwData;
		m_staURL.SetWindowText(csText);
	}
}

void CDeeBookDlg::OnDecompileFinish()
{	
	// activate main dialog
	SetForegroundWindow();

	// update window content
	m_bWorking = FALSE;
	m_staURL.SetWindowText(_T(" Finish!"));
	SetWindowStyle();

	// delete working dumper
	delete m_pDumper;
	m_pDumper = NULL;
	m_pDumpThread = NULL;
}

BOOL CDeeBookDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	if (::IsWindow(pNMHDR->hwndFrom))
	{
		return CDialog::OnNotify(wParam, lParam, pResult);
	}

	return 0;
}

void CDeeBookDlg::OnChkSnap() 
{
	m_staJPEG.EnableWindow(m_chkSnap.GetCheck());
	m_edtJPEG.EnableWindow(m_chkSnap.GetCheck());
	m_spnJPEG.EnableWindow(m_chkSnap.GetCheck());	
}
