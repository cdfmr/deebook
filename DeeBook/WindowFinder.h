/////////////////////////////////////////////////////////////////////////////
// CWindowFinder
// A Spy++ like Window Finder control

#if !defined(AFX_WINDOWFINDER_H__2CF7769C_F34D_4679_8923_F09BE6874199__INCLUDED_)
#define AFX_WINDOWFINDER_H__2CF7769C_F34D_4679_8923_F09BE6874199__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindowFinder.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWindowFinder control

#define WFN_SELWINDOW	12800	// notify owner window

class CWindowFinder : public CStatic
{
public:

	CWindowFinder();
	virtual ~CWindowFinder();

	// set gui resource (icon & cursor)
	void SetUIResource(HICON hIcon, HICON hDragIcon, HCURSOR hDragCursor);

	// get selected window (effective with WFN_SELWINDOW notify message)
	inline HWND GetSelWindow()
	{
		return m_hwndFound;
	}

private:

	HICON	m_hIcon;		// normal icon
	HICON	m_hDragIcon;	// icon when dragging
	HCURSOR	m_hDragCursor;	// dragging cursor

	BOOL	m_bCapture;		// is in capture mode
	HCURSOR	m_hOldCursor;	// screen cursor before dragging
	HWND	m_hwndFound;	// window found

	HWND CaptureWindow();				// capture window
	void RefreshWindow(HWND hWnd);		// redraw window
	void HighlightWindow(HWND hWnd);	// highlight window

protected:

	//{{AFX_VIRTUAL(CWindowFinder)
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CWindowFinder)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_WINDOWFINDER_H__2CF7769C_F34D_4679_8923_F09BE6874199__INCLUDED_)
