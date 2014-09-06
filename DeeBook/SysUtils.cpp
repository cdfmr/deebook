#include "StdAfx.h"
#include "SysUtils.h"
#include <psapi.h>

namespace SysUtils
{

// get executable filename of current application
CString GetAppFileName()
{
	TCHAR buf[1024];
	if (::GetModuleFileName(0, buf, sizeof(buf)) > 0)
	{
		return buf;
	}
	else
	{
		return _T("");
	}
}

// detect whether a file exists
BOOL FileExists(LPCTSTR lpFile)
{
	int nCode = ::GetFileAttributes(lpFile);
	return (nCode != -1) && ((nCode & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

// detect whether a directory exists
BOOL DirectoryExists(LPCTSTR lpDir)
{
	int nCode = ::GetFileAttributes(lpDir);
	return (nCode != -1) && ((nCode & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

// extract path from filename
CString ExtractFilePath(LPCTSTR lpFile)
{
	CString csFile = lpFile;
	int pos = csFile.ReverseFind(_T('\\'));
	if (pos != -1)
	{
		return csFile.Left(pos + 1);
	}
	else
	{
		return _T("");
	}
}

// extract filename without path from filename
CString ExtractFileName(LPCTSTR lpFile)
{
	CString csFile = lpFile;
	int pos = csFile.ReverseFind(_T('\\'));
	if (pos != -1)
	{
		return csFile.Mid(pos + 1);
	}
	else
	{
		return csFile;
	}
}

// get extension of a filename
CString GetFileExt(LPCTSTR lpFile)
{
	CString csFile = lpFile;
	int pos = csFile.ReverseFind(_T('.'));
	if (pos != -1)
	{
		csFile.Delete(0, pos + 1);
        return csFile;
	}

    return _T("");
}

// change extension of a filename
CString ChangeFileExt(LPCTSTR lpFile, LPCTSTR lpExt)
{
	CString csFile = lpFile;
	int pos = csFile.ReverseFind(_T('.'));
	if (pos != -1)
	{
		return csFile.Left(pos + 1) + lpExt;
	}
	else
	{
		return csFile + _T('.') + lpExt;
	}
}

// create directory and sub directories recursively
void ForceDirectories(LPCTSTR lpDir)
{
	CString csDir = lpDir;
	if (csDir.IsEmpty())
	{
		return;
	}

	if (csDir[csDir.GetLength() - 1] == _T('\\'))
	{
		csDir.Delete(csDir.GetLength() - 1);
	}

	if ((csDir.GetLength() == 2 && csDir[1] == _T(':')) ||
		DirectoryExists(csDir) ||
		ExtractFilePath(csDir) == csDir)

	{
		return;
	}

	ForceDirectories(ExtractFilePath(csDir));
	::CreateDirectory(csDir, NULL);
}

// extract path from a url string
CString ExtractURLPath(LPCTSTR lpURL)
{
	const CString csSeparator = _T("/\\:");
	CString csURL = lpURL;

	while (!csURL.IsEmpty() &&
		csSeparator.Find(csURL[csURL.GetLength() - 1]) >= 0)
	{
		csURL.Delete(csURL.GetLength() - 1);
	}

	int pos = -1;
	for (int i = 0; i < csSeparator.GetLength(); i++)
	{
		int p = csURL.ReverseFind(csSeparator[i]);
		if (p > pos)
		{
			pos = p;
		}
	}
	if (pos >= 0)
	{
		csURL = csURL.Left(pos + 1);
	}

	return csURL;
}

// get absolute url string from relative url
CString GetAbsoluteURL(LPCTSTR lpBase, LPCTSTR lpRelative)
{
	CString csRelative = lpRelative;
	csRelative.Replace(_T('\\'), _T('/'));

	if (csRelative.IsEmpty())
	{
		return _T("");
	}

	if (csRelative.Find(_T(':')) >= 0)
	{
		return csRelative;
	}

	CString csPath = ExtractURLPath(lpBase);
	while (csRelative.Find(_T("./")) == 0)
	{
		csRelative.Delete(0, 2);
	}
	while (csRelative.Find(_T("../")) == 0)
	{
		csPath = ExtractURLPath(csPath);
		csRelative.Delete(0, 3);
	}

	return csPath + csRelative;
}

// get executable filename from process id
CString GetProcessExecutable(DWORD dwProcId)
{
	if (dwProcId == 0)
	{
		return _T("");
	}

	CString csExecutable;
	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
	if (hProcess)
	{
		HMODULE hModule;
		DWORD dwCount;
		if (::EnumProcessModules(hProcess, &hModule, sizeof(HMODULE), &dwCount))
		{
			TCHAR chFileName[1024];
			if (::GetModuleFileNameEx(hProcess, hModule, chFileName, sizeof(chFileName)) > 0)
			{
				csExecutable = chFileName;
			}
		}
		::CloseHandle(hProcess);
	}

	return csExecutable;
}

}