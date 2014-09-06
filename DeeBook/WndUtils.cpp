#include "StdAfx.h"
#include "WndUtils.h"

namespace WndUtils
{

// get window class name
CString GetWindowClassName(HWND hWnd)
{
	TCHAR chClassName[256];
	if (::GetClassName(hWnd, chClassName, 255) > 0)
	{
		return chClassName;
	}

	return _T("");
}

// get process id of specified window
DWORD GetWindowProcessId(HWND hWnd)
{
	DWORD dwWndProcessID;
	::GetWindowThreadProcessId(hWnd, &dwWndProcessID);
	return dwWndProcessID;
}

// get top-level window of specified control
HWND GetTopWindowFromControl(HWND hCtrl)
{
	HWND hMainWnd = ::GetParent(hCtrl);
	HWND hParent = ::GetParent(hMainWnd);
	while (hParent != 0)
	{
		hMainWnd = hParent;
		hParent = ::GetParent(hMainWnd);
	}

	return hMainWnd;
}

// get tree item text from remote process
CString GetRemoteTreeItemText(DWORD dwProcId, HWND hTree, HTREEITEM hItem)
{
	CString csText;

	// open process
	HANDLE hRemoteProc = ::OpenProcess(
		PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwProcId);
	if (hRemoteProc)
	{
		// allocate TVITEM structure in remote process
		TVITEM* pRemoteItem = (TVITEM*)VirtualAllocEx(hRemoteProc, NULL, sizeof(TVITEM),
			MEM_COMMIT, PAGE_READWRITE);
		if (pRemoteItem)
		{
			// allocate text buffer in remote process
			TCHAR* pszRemoteText = (TCHAR*)VirtualAllocEx(hRemoteProc, NULL, sizeof(TCHAR) * 256,
				MEM_COMMIT, PAGE_READWRITE);
			if (pszRemoteText)
			{
				// fill remote TVITEM structure
				TVITEM item;
				item.mask = TVIF_HANDLE | TVIF_TEXT;
				item.hItem = hItem;
				item.cchTextMax = 256;
				item.pszText = pszRemoteText;
				DWORD dwWrite;
				::WriteProcessMemory(hRemoteProc, pRemoteItem, &item, sizeof(TVITEM), &dwWrite);

				// send message
				::SendMessage(hTree, TVM_GETITEM, 0, (LPARAM)pRemoteItem);

				// copy data from remote process
				TCHAR pszBuffer[256];
				DWORD dwRead;
				if (::ReadProcessMemory(hRemoteProc, pszRemoteText, pszBuffer, sizeof(TCHAR) * 256, &dwRead))
				{
					csText = pszBuffer;
				}

				::VirtualFreeEx(hRemoteProc, pszRemoteText, sizeof(TCHAR) * 256, MEM_DECOMMIT);
			}
			::VirtualFreeEx(hRemoteProc, pRemoteItem, sizeof(TVITEM), MEM_DECOMMIT);
		}
		::CloseHandle(hRemoteProc);
	}

	return csText;
}

}