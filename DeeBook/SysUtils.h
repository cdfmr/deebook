#ifndef __SYS_UTILS_H__
#define __SYS_UTILS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace SysUtils
{

CString GetAppFileName();
BOOL FileExists(LPCTSTR lpFile);
BOOL DirectoryExists(LPCTSTR lpDir);
CString ExtractFilePath(LPCTSTR lpFile);
CString ExtractFileName(LPCTSTR lpFile);
CString GetFileExt(LPCTSTR lpFile);
CString ChangeFileExt(LPCTSTR lpFile, LPCTSTR lpExt);
void ForceDirectories(LPCTSTR lpDir);
CString ExtractURLPath(LPCTSTR lpURL);
CString GetAbsoluteURL(LPCTSTR lpBase, LPCTSTR lpRelative);
CString GetProcessExecutable(DWORD dwProcId);

}

#endif // __SYS_UTILS_H__