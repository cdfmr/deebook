// IEDumper.h: interface for the CIEDumper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IEDUMPER_H__ECB86F48_5092_47E8_82E9_FDD9E3992ABC__INCLUDED_)
#define AFX_IEDUMPER_H__ECB86F48_5092_47E8_82E9_FDD9E3992ABC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mshtml.h>
#include <map>
#include "../HTMLReader/LiteHTMLReader.h"

using namespace std;

#pragma pack(1)

#define IDN_PROGRESS	12850
typedef struct tagIDNMHDR
{
	NMHDR nmhdr;
	DWORD dwData;
} IDNMHDR;

class CIEDumper : public ILiteHTMLReaderEvents
{
public:

	CIEDumper();
	virtual ~CIEDumper();

	// chm options structure
	typedef struct tagCHMOptions
	{
		BOOL	bCreateCHM;
		BOOL	bCompileCHM;
		CString csHomePage;
		CString	csProjectFile;
		CString	csContentFile;
		CString	csWindowTitle;
		CSize	szWindowSize;
		UINT	nPaneWidth;
		BOOL	bHideToolbarText;
		BOOL	bRememberWinPos;
        BOOL    bFullTextSearch;
		UINT	nLanguage;
	} CHM_OPTIONS;

	// get IHTMLDocument2 interface
	static IHTMLDocument2* GetDocInterface(HWND hIECtrl);
	static IHTMLDocument2* GetDocInterfaceByMSAA(HWND hIECtrl);

	// get/set window handles
	inline HWND GetIECtrlHandle()	{ return m_hIECtrl; }
	inline HWND GetTreeCtrlHandle()	{ return m_hTreeCtrl; }
	BOOL SetIECtrlHandle(HWND hIECtrl);
	BOOL SetTreeCtrlHandle(HWND hTreeCtrl);

	// set decompiling parameters
	inline void SetOwnerWindow(HWND hOwner) { m_hOwner = hOwner; }
	inline void SetURLBase(LPCTSTR lpURLBase) { m_csURLBase = lpURLBase; }
	inline void SetOutputDir(LPCTSTR lpDir) { m_csOutputDir = lpDir; }
	inline void SetLoadTimeout(DWORD dwValue) { m_dwLoadTimeout = dwValue; }
	inline void SetLoadWait(DWORD dwValue) { m_dwLoadWait = dwValue; }
	inline void SetCHMOptions(CHM_OPTIONS chmOptions) { m_chmOptions = chmOptions; }
    inline void SetSnapImages(BOOL bSnap) { m_bSnap = bSnap; }
    inline void SetJPEGQuality(UINT nQuality) { m_nJPEGQuality = nQuality; }

	// get url of current page
	CString GetPageURL();

	// start/stop decompiling
	void Decompile(BOOL bOnePageOnly = FALSE);
	inline void Terminate() { m_bBreakFlag = TRUE; }

private:

	// injection code to call StealFile
	typedef struct tagStealFile
	{
		// get current address
		BYTE   __call0;            // E8 00 00 00 00
		DWORD  __callAddr;
		BYTE   __popEAX;           // 58

		// push file name
		BYTE   __addEAX15;         // 05 15 00 00 00
		DWORD  __addVal1;
		BYTE   __pushEAX1;         // 50

		// push url
		BYTE   __addEAX400_1;      // 05 00 04 00 00
		DWORD  __addVal2;
		BYTE   __pushEAX2;         // 50

		// get procedure address
		BYTE   __addEAX400_2;      // 05 00 04 00 00
		DWORD  __addVal3;

		// call
		WORD   __callEAX;          // FF 10
		BYTE   __ret;              // C3

		char   cFile[1024];
		char   cURL[1024];
		void*  pStealFile;
	} FLN_STEAL_FILE;

	// inject code to call SeleteTreeItem
	typedef struct tagSelectTreeItem
	{
		// get current address
		BYTE   __call0;            // E8 00 00 00 00
		DWORD  __callAddr;
		BYTE   __popEAX;           // 58

		// push htreeitem
		BYTE   __pushHITEM;        // 68 00 00 00 00
		DWORD  dwItem;

		// push htree
		BYTE   __pushHTREE;        // 68 00 00 00 00
		DWORD  dwTree;

		// get procedure address
		BYTE   __addEAX13;         // 05 13 00 00 00
		DWORD  __addVal;

		// call
		WORD   __callEAX;          // FF 10
		BYTE   __ret;              // C3

		void*  pSelectTreeItem;
	} FLN_SELECT_TREE_ITEM;

	// page element information
	typedef struct tagElementInfo
	{
		CString csURL;
		BOOL bSaved;
	} ELEMENT_INFO;

	// url map
	typedef map<CString, ELEMENT_INFO> CURLMap;

	// class to write text file with line break
	class CLineFile : public CStdioFile
	{
	public:
		void WriteString(LPCTSTR lpsz)
		{
			CString csLine = lpsz;
			csLine += _T("\n");
			CStdioFile::WriteString(csLine);
		}
		BOOL IsOpen()
		{
			return m_pStream != NULL;
		}
	};

	BOOL m_bBreakFlag;				// break decompiling

	DWORD m_dwProcId;				// process id of ebook
	HWND m_hIECtrl;					// internet explorer window
	HWND m_hTreeCtrl;				// navigation tree window
	IHTMLDocument2* m_pHtmlDoc2;	// IHTMLDocument2 interface

	HWND	m_hOwner;				// owner window
	DWORD	m_dwLoadWait;			// wait time for loading page
	DWORD	m_dwLoadTimeout;		// time out for loading page
	CString m_csOutputDir;			// output directory
	CString m_csURLBase;			// base url
	CHM_OPTIONS m_chmOptions;		// chm options
    BOOL    m_bSnap;                // snap images
    UINT    m_nJPEGQuality;         // jpeg quality

	CURLMap m_mapPage;				// web pages in ebook
	CURLMap m_mapOther;				// other files in ebook
	CString m_csCurrentURL;			// current page url

	CLineFile fileHHP;				// html help project file
	CLineFile fileHHC;				// html help content file

	FLN_STEAL_FILE m_StealFileCode;			// injection code to call StealFile
	FLN_SELECT_TREE_ITEM m_SelTreeItemCode;	// injection code to call SelectTreeItem

	CString NormalizeURL(LPCTSTR lpURL);
	BOOL IsPageNeedProcess(LPCTSTR lpURL);
	BOOL IsPageProcessed(LPCTSTR lpURL);
	void AddURLToMap(LPCTSTR lpURL, CURLMap& mapURL, BOOL bSaved = FALSE);
	ELEMENT_INFO* GetFileToProcess(CURLMap& mapURL);
	CString GetFileName(LPCTSTR lpURL);

	BOOL InjectIEThief();
	
	void ProcessNavigationTree();
	void ProcessTreeItem(HTREEITEM hItem);
	void ProcessPageMap(BOOL bOnePageOnly = FALSE);
	void ProcessFileMap();

	void WaitForLoadingDocument();
	BOOL SaveHTMLSource(LPCTSTR lpFileName);
	void ParsePage(LPCTSTR lpFileName);
	void StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort);
	CString SafeGetAttrValue(CLiteHTMLTag* pTag, LPCTSTR lpAttrName);
	void DownloadFile(LPCTSTR lpURL, LPCTSTR lpFileName);
    void SnapImage(LPCTSTR lpURL, LPCTSTR lpFileName, DWORD dwFormat);

	void CreateCHMProject();
	void SearchCHMFileList();
	void CompileCHMProject();
};

#endif // !defined(AFX_IEDUMPER_H__ECB86F48_5092_47E8_82E9_FDD9E3992ABC__INCLUDED_)
