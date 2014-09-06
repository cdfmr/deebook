#ifndef __WND_UTILS_H__
#define __WND_UTILS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

namespace WndUtils
{

CString GetWindowClassName(HWND hWnd);
DWORD GetWindowProcessId(HWND hWnd);
HWND GetTopWindowFromControl(HWND hCtrl);
CString GetRemoteTreeItemText(DWORD dwProcId, HWND hTree, HTREEITEM hItem);

}

#endif // __WND_UTILS_H__