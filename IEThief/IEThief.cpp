// IEThief.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "IEThief.h"
#include "../DeeBook/WndUtils.h"
#include <atlbase.h>
#include <afxinet.h>

using namespace WndUtils;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CIEThiefApp

BEGIN_MESSAGE_MAP(CIEThiefApp, CWinApp)
	//{{AFX_MSG_MAP(CIEThiefApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIEThiefApp construction

CIEThiefApp::CIEThiefApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIEThiefApp object

CIEThiefApp theApp;

BOOL CIEThiefApp::InitInstance()
{
	return CWinApp::InitInstance();
}

// select tree item automatically
extern "C" void __stdcall SelectTreeItem(HWND hTree, HTREEITEM hItem)
{
	// bring ebook window to front
	::SetForegroundWindow(GetTopWindowFromControl(hTree));
	::SetFocus(hTree);

	// get tree item position
	TreeView_EnsureVisible(hTree, hItem);
	CRect rcItem;
	TreeView_GetItemRect(hTree, hItem, &rcItem, TRUE);
	CPoint ptItem = rcItem.CenterPoint();
	::ClientToScreen(hTree, &ptItem);

	// emulate mouse click
	CPoint ptCursor;
	::GetCursorPos(&ptCursor);
	::ShowCursor(FALSE);
	::SetCursorPos(ptItem.x, ptItem.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	::SetCursorPos(ptCursor.x, ptCursor.y);
	::ShowCursor(TRUE);
}

// steal file from ebook
extern "C" void __stdcall StealFile(LPCSTR lpURL, LPCSTR lpFile)
{
	URLDownloadToFile(NULL, lpURL, lpFile, 0, NULL);
}
