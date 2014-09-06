// WindowFinder.cpp : implementation file
//

#include "stdafx.h"
#include "WindowFinder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindowFinder

CWindowFinder::CWindowFinder()
	: m_bCapture(FALSE)
	, m_hOldCursor(NULL)
	, m_hwndFound(NULL)
{
}

CWindowFinder::~CWindowFinder()
{
}

// set gui resource (icon & cursor)
void CWindowFinder::SetUIResource(HICON hIcon, HICON hDragIcon, HCURSOR hDragCursor)
{
	m_hIcon = hIcon;
	m_hDragIcon = hDragIcon;
	m_hDragCursor = hDragCursor;

	if (m_bCapture)
	{
		SetIcon(m_hDragIcon);
		::SetCursor(m_hDragCursor);
	}
	else
	{
		SetIcon(m_hIcon);
	}
}

BEGIN_MESSAGE_MAP(CWindowFinder, CStatic)
	//{{AFX_MSG_MAP(CWindowFinder)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindowFinder message handlers

void CWindowFinder::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();

	// verify control style
	ModifyStyle(0, SS_NOTIFY);
	ASSERT((GetStyle() & 0x0000000FL) == SS_ICON);

	SetIcon(m_hIcon);
}

void CWindowFinder::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// start capture
	m_bCapture = TRUE;
	SetIcon(m_hDragIcon);
	m_hOldCursor = ::SetCursor(m_hDragCursor);
	SetCapture();
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CWindowFinder::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bCapture)
	{
		HWND hWnd = CaptureWindow();

		// new window
		if (hWnd != m_hwndFound)
		{
			RefreshWindow(m_hwndFound);
			m_hwndFound = hWnd;
			HighlightWindow(m_hwndFound);
		}
	}
	
	CStatic::OnMouseMove(nFlags, point);
}

void CWindowFinder::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bCapture)
	{
		// stop capture
		m_bCapture = FALSE;
		ReleaseCapture();
		RefreshWindow(m_hwndFound);
		SetIcon(m_hIcon);
		::SetCursor(m_hOldCursor);

		// notify message
		if (m_hwndFound)
		{
			NMHDR nmhdr;
			nmhdr.hwndFrom = GetSafeHwnd();
			nmhdr.idFrom = GetDlgCtrlID();
			nmhdr.code = WFN_SELWINDOW;
			CWnd* pOwner = GetOwner();
			if (pOwner && ::IsWindow(pOwner->m_hWnd))
			{
				pOwner->SendMessage(WM_NOTIFY, nmhdr.idFrom, (LPARAM)&nmhdr);
			}
		}

		// reset
		m_hwndFound = NULL;
	}
	
	CStatic::OnLButtonUp(nFlags, point);
}

HWND CWindowFinder::CaptureWindow()
{
	// get window
	CPoint pt;
	::GetCursorPos(&pt);
	HWND hWnd = ::WindowFromPoint(pt);

	// check process
	if (hWnd)
	{
		DWORD dwWndProcessID;
		::GetWindowThreadProcessId(hWnd, &dwWndProcessID);
		if (dwWndProcessID == ::GetCurrentProcessId())
		{
			hWnd = NULL;
		}
	}

	return hWnd;
}

void CWindowFinder::RefreshWindow(HWND hWnd)
{
	if (!::IsWindow(hWnd))
	{
		return;
	}

	CWnd* pWnd = CWnd::FromHandle(hWnd);
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	pWnd->RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

void CWindowFinder::HighlightWindow(HWND hWnd)
{
	if (!::IsWindow(hWnd))
	{
		return;
	}

	CWnd* pWnd = CWnd::FromHandle(hWnd);
	CRect rcWnd;
	pWnd->GetWindowRect(&rcWnd);
	rcWnd.OffsetRect(-rcWnd.left, -rcWnd.top);
	CDC* pDC = pWnd->GetWindowDC();

	if (pDC)
	{
		CPen pen(PS_SOLID, 3, RGB(0, 0, 0));
		CPen* pOldPen = pDC->SelectObject(&pen);
		HBRUSH hOldBrush = (HBRUSH)pDC->SelectObject(::GetStockObject(HOLLOW_BRUSH));
		pDC->Rectangle(&rcWnd);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(hOldBrush);
		pWnd->ReleaseDC(pDC);
	}
}
