// CHMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "deebook.h"
#include "CHMDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCHMDlg dialog


CCHMDlg::CCHMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCHMDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCHMDlg)
	m_csTitle = _T("");
	m_nWidth = 0;
	m_nHeight = 0;
	m_nPaneWidth = 0;
	m_bHideToolbarText = FALSE;
	m_bRememberWinPos = FALSE;
    m_bFullTextSearch = FALSE;
	m_nSizeType = 1;
	m_nLanguage = 1;
	//}}AFX_DATA_INIT

	m_szWinType[0] = CSize(600, 450);
	m_szWinType[1] = CSize(640, 480);
	m_szWinType[2] = CSize(720, 540);
	m_szWinType[3] = CSize(760, 570);
	m_szWinType[4] = CSize(800, 600);
	m_szWinType[5] = CSize(960, 720);
}


void CCHMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCHMDlg)
	DDX_Control(pDX, IDC_STA_MULTIPLE, m_staMultiple);
	DDX_Control(pDX, IDC_EDT_HEIGHT, m_edtHeight);
	DDX_Control(pDX, IDC_EDT_WIDTH, m_edtWidth);
	DDX_Text(pDX, IDC_EDT_TITLE, m_csTitle);
	DDX_Text(pDX, IDC_EDT_WIDTH, m_nWidth);
	DDV_MinMaxUInt(pDX, m_nWidth, 100, 4000);
	DDX_Text(pDX, IDC_EDT_HEIGHT, m_nHeight);
	DDV_MinMaxUInt(pDX, m_nHeight, 100, 2000);
	DDX_Text(pDX, IDC_EDT_PANEWIDTH, m_nPaneWidth);
	DDV_MinMaxUInt(pDX, m_nPaneWidth, 0, 500);
	DDX_Check(pDX, IDC_CHK_HIDETOOLBARTEXT, m_bHideToolbarText);
	DDX_Check(pDX, IDC_CHK_REMWINPOS, m_bRememberWinPos);
    DDX_Check(pDX, IDC_CHK_FULLTEXTSEARCH, m_bFullTextSearch);
	DDX_Radio(pDX, IDC_RAD_600, m_nSizeType);
	DDX_Radio(pDX, IDC_RAD_ENGLISH, m_nLanguage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCHMDlg, CDialog)
	//{{AFX_MSG_MAP(CCHMDlg)
	ON_BN_CLICKED(IDC_RAD_600, OnRadioWindowSize)
	ON_BN_CLICKED(IDC_RAD_640, OnRadioWindowSize)
	ON_BN_CLICKED(IDC_RAD_720, OnRadioWindowSize)
	ON_BN_CLICKED(IDC_RAD_760, OnRadioWindowSize)
	ON_BN_CLICKED(IDC_RAD_800, OnRadioWindowSize)
	ON_BN_CLICKED(IDC_RAD_960, OnRadioWindowSize)
	ON_BN_CLICKED(IDC_RAD_CUSTOMIZE, OnRadioWindowSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCHMDlg message handlers

BOOL CCHMDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnRadioWindowSize();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCHMDlg::OnRadioWindowSize() 
{
	UpdateData();

	m_edtWidth.EnableWindow(m_nSizeType == 6);
	m_edtHeight.EnableWindow(m_nSizeType == 6);
	m_staMultiple.EnableWindow(m_nSizeType == 6);
}

void CCHMDlg::GetCHMOptions(CIEDumper::CHM_OPTIONS &chmOptions)
{
	chmOptions.csWindowTitle = m_csTitle;
	if (m_nSizeType >= 0 && m_nSizeType < 6)
	{
		chmOptions.szWindowSize = m_szWinType[m_nSizeType];
	}
	else
	{
		chmOptions.szWindowSize = CSize(m_nWidth, m_nHeight);
	}
	chmOptions.nPaneWidth = m_nPaneWidth;
	chmOptions.bHideToolbarText = m_bHideToolbarText;
	chmOptions.bRememberWinPos = m_bRememberWinPos;
    chmOptions.bFullTextSearch = m_bFullTextSearch;
	chmOptions.nLanguage = m_nLanguage;
}

void CCHMDlg::SetCHMOptions(CIEDumper::CHM_OPTIONS &chmOptions)
{
	m_csTitle = chmOptions.csWindowTitle;
	m_nSizeType = 6;
	for (int i = 0; i < 6; i++)
	{
		if (m_szWinType[i] == chmOptions.szWindowSize)
		{
			m_nSizeType = i;
			break;
		}
	}
	m_nWidth = chmOptions.szWindowSize.cx;
	m_nHeight = chmOptions.szWindowSize.cy;
	m_nPaneWidth = chmOptions.nPaneWidth;
	m_bHideToolbarText = chmOptions.bHideToolbarText;
	m_bRememberWinPos = chmOptions.bRememberWinPos;
    m_bFullTextSearch = chmOptions.bFullTextSearch;
	m_nLanguage = chmOptions.nLanguage;
}
