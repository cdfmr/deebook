/*

-> ForceLib.h

ForceLibrary.dll prototypes...

*/

#ifndef __ForceLibraryHeader__
#define __ForceLibraryHeader__

#ifdef __cplusplus
extern "C" {
#endif

DWORD __stdcall ForceLibrary(CHAR* szLibraryPath,PROCESS_INFORMATION* pProcInfo);
DWORD __stdcall ForceLibraryNow(DWORD dwPID, char* szDllPath);
BOOL  __stdcall RemoteExec(DWORD dwPID, void* pCode, DWORD dwCodeSize, DWORD *dwCodeResult, DWORD dwMilliseconds);
BOOL  __stdcall TrapEntry(DWORD dwEntryPoint,PROCESS_INFORMATION *pPI);
BOOL  __stdcall ForceLibraryDBG(CHAR* szTargetLib,DWORD dwEntryPoint,PROCESS_INFORMATION *pPI);
DWORD __stdcall PerformCleanup(DWORD dwEntryPoint,PROCESS_INFORMATION *pPI);

#ifdef __cplusplus
}
#endif

#endif