// IEDumper.cpp: implementation of the CIEDumper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IEDumper.h"
#include "WndUtils.h"
#include "SysUtils.h"
#include <ximage.h>
#include <atlbase.h>
#include <oleacc.h>
#include <comdef.h>
#include <string>
#include <bitset>
#include <ForceLib.h>
#include "Quantize.h"

using namespace std;
using namespace WndUtils;
using namespace SysUtils;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIEDumper::CIEDumper()
	: m_hIECtrl(NULL)
	, m_hTreeCtrl(NULL)
	, m_pHtmlDoc2(NULL)
	, m_dwProcId(0)
	, m_hOwner(NULL)
	, m_dwLoadWait(200)
	, m_dwLoadTimeout(10000)
    , m_bSnap(TRUE)
    , m_nJPEGQuality(85)
{
	// fill injection code to call StealFile
	m_StealFileCode.__call0       = 0xE8;
	m_StealFileCode.__callAddr    = 0;
	m_StealFileCode.__popEAX      = 0x58;
	m_StealFileCode.__addEAX15    = 0x05;
	m_StealFileCode.__addVal1     = 0x15;
	m_StealFileCode.__pushEAX1    = 0x50;
	m_StealFileCode.__addEAX400_1 = 0x05;
	m_StealFileCode.__addVal2     = 0x400;
	m_StealFileCode.__pushEAX2    = 0x50;
	m_StealFileCode.__addEAX400_2 = 0x05;
	m_StealFileCode.__addVal3     = 0x400;
	m_StealFileCode.__callEAX     = 0x10FF;
	m_StealFileCode.__ret         = 0xC3;

	// fill injection code to call SelectTreeItem
	m_SelTreeItemCode.__call0     = 0xE8;
	m_SelTreeItemCode.__callAddr  = 0;
	m_SelTreeItemCode.__popEAX    = 0x58;
	m_SelTreeItemCode.__pushHITEM = 0x68;
	m_SelTreeItemCode.__pushHTREE = 0x68;
	m_SelTreeItemCode.__addEAX13  = 0x05;
	m_SelTreeItemCode.__addVal    = 0x13;
	m_SelTreeItemCode.__callEAX   = 0x10FF;
	m_SelTreeItemCode.__ret       = 0xC3;
}

CIEDumper::~CIEDumper()
{
	if (m_pHtmlDoc2)
	{
		m_pHtmlDoc2->Release();
	}
}

// get IHTMLDocument2 interface by WM_HTML_GETOBJECT message
IHTMLDocument2* CIEDumper::GetDocInterface(HWND hIECtrl)
{
	ASSERT(::IsWindow(hIECtrl));

	// load msaa library
	HINSTANCE hMSAALib = ::LoadLibrary(_T("OLEACC.DLL"));
	if (!hMSAALib)
	{
		return NULL;
	}

	// locate msaa function
	LPFNOBJECTFROMLRESULT pfObjectFromLresult = 
		(LPFNOBJECTFROMLRESULT)::GetProcAddress(hMSAALib, "ObjectFromLresult");
	if (!pfObjectFromLresult)
	{
		::FreeLibrary(hMSAALib);
		return NULL;
	}

	// send WM_HTML_GETOBJECT message
	IHTMLDocument2* pHtmlDoc2 = NULL;
	IHTMLDocumentPtr spDoc = NULL;
	LRESULT lResult;
	UINT nMsg = ::RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));
	::SendMessageTimeout(hIECtrl, nMsg, 0, 0, SMTO_ABORTIFHUNG, 1000, (DWORD*)&lResult);

	// get IHTMLDocument2 interface
	if (SUCCEEDED(pfObjectFromLresult(lResult, IID_IHTMLDocument, 0, (void**)&spDoc)))
	{
		IDispatchPtr spDisp;
		IHTMLWindow2Ptr spWin;
		spDoc->get_Script(&spDisp);
		spWin = spDisp;
		spWin->get_document(&pHtmlDoc2);
	}

	::FreeLibrary(hMSAALib);

	return pHtmlDoc2;
}

// get IHTMLDocument2 interface by msaa
IHTMLDocument2* CIEDumper::GetDocInterfaceByMSAA(HWND hIECtrl)
{
	ASSERT(::IsWindow(hIECtrl));

	// load msaa library
	HINSTANCE hMSAALib = ::LoadLibrary(_T("OLEACC.DLL"));
	if (!hMSAALib)
	{
		return NULL;
	}

	// locate msaa function
	LPFNACCESSIBLEOBJECTFROMWINDOW pfAccessibleObjectFromWindow =
		(LPFNACCESSIBLEOBJECTFROMWINDOW)::GetProcAddress(hMSAALib, "AccessibleObjectFromWindow");
	if (!pfAccessibleObjectFromWindow)
	{
		::FreeLibrary(hMSAALib);
		return NULL;
	}

	// get IHTMLDocument2 interface
	IHTMLDocument2* pHtmlDoc2 = NULL;
	IAccessiblePtr spAccess;
	if (SUCCEEDED(pfAccessibleObjectFromWindow(hIECtrl, 0, IID_IAccessible, (void**)&spAccess)))
	{
		IServiceProviderPtr spServiceProv;
		if(SUCCEEDED(spAccess->QueryInterface(&spServiceProv)))
		{
			IHTMLWindow2Ptr spWin;
			if(SUCCEEDED(spServiceProv->QueryService(IID_IHTMLWindow2, &spWin)))
			{
				spWin->get_document(&pHtmlDoc2);
			}
		}
	}

	::FreeLibrary(hMSAALib);

	return pHtmlDoc2;
}

// set internet explorer window
BOOL CIEDumper::SetIECtrlHandle(HWND hIECtrl)
{
	ASSERT(::IsWindow(hIECtrl));

	// check window class
	if (GetWindowClassName(hIECtrl) != _T("Internet Explorer_Server"))
	{
		return FALSE;
	}

	// get IHTMLDocument2 interface
	IHTMLDocument2* pHtmlDoc2 = GetDocInterface(hIECtrl);
	if (!pHtmlDoc2)
	{
		pHtmlDoc2 = GetDocInterfaceByMSAA(hIECtrl);
		if (!pHtmlDoc2)
		{
			return FALSE;
		}
	}

	// update member variable
	m_hIECtrl = hIECtrl;
	m_hTreeCtrl = NULL;
	if (m_pHtmlDoc2)
	{
		m_pHtmlDoc2->Release();
	}
	m_pHtmlDoc2 = pHtmlDoc2;
	m_dwProcId = GetWindowProcessId(m_hIECtrl);

	return TRUE;
}

// set navigation tree window
BOOL CIEDumper::SetTreeCtrlHandle(HWND hTreeCtrl)
{
	ASSERT(::IsWindow(hTreeCtrl));

	if (!m_hIECtrl)
	{
		return FALSE;
	}

	// check window class
	CString csClassName = GetWindowClassName(hTreeCtrl);
	if (csClassName != _T("SysTreeView32") && csClassName != _T("TTreeView"))
	{
		return FALSE;
	}

	// verify same process
	if (GetWindowProcessId(hTreeCtrl) != m_dwProcId)
	{
		return FALSE;
	}

	m_hTreeCtrl = hTreeCtrl;

	return TRUE;
}

// get url of current page
CString CIEDumper::GetPageURL()
{
	if (!m_pHtmlDoc2)
	{
		return _T("");
	}

	CString csURL;
	CComBSTR bsURL;
	if (m_pHtmlDoc2->get_URL(&bsURL) == S_OK)
	{
		csURL = bsURL;
	}

	return csURL;
}

CString CIEDumper::NormalizeURL(LPCTSTR lpURL)
{
	CString csURL = lpURL;
	csURL.Replace(_T('\\'), _T('/'));
	csURL.MakeLower();
	return csURL;
}

// check whether a page should be processed
BOOL CIEDumper::IsPageNeedProcess(LPCTSTR lpURL)
{
	// check url base
	CString csURL = NormalizeURL(lpURL);
	if (csURL.IsEmpty() ||
		(!m_csURLBase.IsEmpty() && csURL.Find(m_csURLBase) != 0))
	{
		return FALSE;
	}

	// check remote urls
	if (// csURL.Find(_T("http:")) == 0 ||
		csURL.Find(_T("https:")) == 0 ||
		csURL.Find(_T("ftp:")) == 0 ||
		csURL.Find(_T("mailto:")) == 0)
	{
		return FALSE;
	}

	return TRUE;
}

// check whether a page is processed
BOOL CIEDumper::IsPageProcessed(LPCTSTR lpURL)
{
	CURLMap::iterator ite = m_mapPage.find(NormalizeURL(lpURL));
	return ite != m_mapPage.end() && ite->second.bSaved;
}

// add url to file map
void CIEDumper::AddURLToMap(LPCTSTR lpURL, CURLMap& mapURL, BOOL bSaved)
{
	// check whether the page should be processed
	if (!IsPageNeedProcess(lpURL))
	{
		return;
	}

	// add to map
	CString csURL = NormalizeURL(lpURL);
	CURLMap::iterator ite = mapURL.find(csURL);
	if (ite == mapURL.end())
	{
		ELEMENT_INFO eleInfo;
		eleInfo.csURL = csURL;
		eleInfo.bSaved = bSaved;
		mapURL.insert(CURLMap::value_type(csURL, eleInfo));
	}
	else if (bSaved)
	{
		ite->second.bSaved = TRUE;
	}
}

// get first file unprocessed in url map
CIEDumper::ELEMENT_INFO* CIEDumper::GetFileToProcess(CURLMap& mapURL)
{
	for (CURLMap::iterator ite = mapURL.begin(); ite != mapURL.end(); ite++)
	{
		if (!ite->second.bSaved)
		{
			return &(ite->second);
		}
	}

	return NULL;
}

// get output file name with original url
CString CIEDumper::GetFileName(LPCTSTR lpURL)
{
	// delete url base
	CString csURL = lpURL;
	if (!m_csURLBase.IsEmpty())
	{
		csURL.Delete(0, m_csURLBase.GetLength());
	}

	// delete protocol name
	int n = csURL.ReverseFind(_T(':'));
	if (n != -1)
	{
		csURL.Delete(0, n + 1);
	}

	// delete root directory separators
	while (!csURL.IsEmpty() &&
		(csURL[0] == _T('\\') || csURL[0] == _T('/')))
	{
		csURL.Delete(0);
	}

	// replace directory separators
	csURL.Replace(_T('/'), _T('\\'));

	// un-escape
	USES_CONVERSION;
	string strURL = T2A(csURL.GetBuffer(csURL.GetLength()));
	csURL.ReleaseBuffer();
	size_t pos = strURL.find_first_of('%');
	while (pos != string::npos && strURL.size() >= pos + 3)
	{
		char chr[2] = "\0";
		char hexvalue[3] = "";
		hexvalue[0] = strURL[pos + 1];
		hexvalue[1] = strURL[pos + 2];
		sscanf(hexvalue, "%x", &chr[0]);
		strURL = strURL.substr(0, pos) + chr + strURL.substr(pos + 3);

		pos = strURL.find_first_of('%', pos);
	}
	csURL = A2T(const_cast<LPSTR>(strURL.c_str()));

	// combine with output directory
	return m_csOutputDir + csURL;
}

// start decompiling
void CIEDumper::Decompile(BOOL bOnePageOnly /* = FALSE */)
{
	// check target
	if (!m_hIECtrl)
	{
		return;
	}

	// inject iethief.dll into ebook process
	if (!InjectIEThief())
	{
		return;
	}

	// initialize variables
	m_bBreakFlag = FALSE;
	ForceDirectories(m_csOutputDir);
	if (!DirectoryExists(m_csOutputDir))
	{
		return;
	}
	if (!m_csOutputDir.IsEmpty() &&
		m_csOutputDir[m_csOutputDir.GetLength() - 1] != _T('\\'))
	{
		m_csOutputDir += _T('\\');
	}
	m_csURLBase = NormalizeURL(m_csURLBase);

	// initialize url maps
	m_mapOther.clear();
	m_mapPage.clear();
	CString csURL = GetPageURL();
	AddURLToMap(csURL, m_mapPage);
	if (IsPageNeedProcess(csURL))
	{
		m_chmOptions.csHomePage = GetFileName(csURL).Mid(m_csOutputDir.GetLength());
	}
	else
	{
		m_chmOptions.csHomePage = csURL;
	}

	// process one page only
	if (bOnePageOnly)
	{
		ProcessPageMap(TRUE);
		return;
	}

	// process pages in ebook
	if (!m_bBreakFlag)
	{
		ProcessNavigationTree();
	}
	if (!m_bBreakFlag)
	{	
		ProcessPageMap();
	}
	
	// create html help
	if (!m_bBreakFlag && m_chmOptions.bCreateCHM)
	{
		CreateCHMProject();
		if (!m_bBreakFlag && m_chmOptions.bCompileCHM)
		{
			CompileCHMProject();
		}
	}
}

// inject iethief dll to ebook process
BOOL CIEDumper::InjectIEThief()
{
	USES_CONVERSION;
	
	// inject dll
	CString csIEThief = ExtractFilePath(GetAppFileName()) + _T("IEThief.dll");
	DWORD dwIEThief = ForceLibraryNow(m_dwProcId, T2A(csIEThief.GetBuffer(csIEThief.GetLength())));
	csIEThief.ReleaseBuffer();
	
	// get address of exported functions in remote process
	m_StealFileCode.pStealFile = NULL;
	m_SelTreeItemCode.pSelectTreeItem = NULL;
	if (dwIEThief)
	{
		DWORD dwLocalEntry = (DWORD)::LoadLibrary(csIEThief);
		if (dwLocalEntry)
		{
			DWORD dwSFLocalAddress = (DWORD)::GetProcAddress((HINSTANCE)dwLocalEntry, "StealFile");
			if (dwSFLocalAddress)
			{
				m_StealFileCode.pStealFile = (void*)(dwIEThief + dwSFLocalAddress - dwLocalEntry);
			}
			DWORD dwSTILocalAddress = (DWORD)::GetProcAddress((HINSTANCE)dwLocalEntry, "SelectTreeItem");
			if (dwSTILocalAddress)
			{
				m_SelTreeItemCode.pSelectTreeItem = (void*)(dwIEThief + dwSTILocalAddress - dwLocalEntry);
			}
			::FreeLibrary((HINSTANCE)dwLocalEntry);
		}
	}

	return m_StealFileCode.pStealFile != NULL && m_SelTreeItemCode.pSelectTreeItem != NULL;
}

// process navigation tree
void CIEDumper::ProcessNavigationTree()
{
	// write html help content
	if (m_chmOptions.bCreateCHM)
	{
		if (fileHHC.Open(m_chmOptions.csContentFile, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		{
			fileHHC.WriteString(_T("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">"));
			fileHHC.WriteString(_T("<HTML>"));
			fileHHC.WriteString(_T("<HEAD>"));
			fileHHC.WriteString(_T("<meta name=\"GENERATOR\" content=\"Microsoft&reg; HTML Help Workshop 4.1\">"));
			fileHHC.WriteString(_T("<!-- Sitemap 1.0 -->"));
			fileHHC.WriteString(_T("</HEAD>"));
			fileHHC.WriteString(_T("<BODY>"));
			fileHHC.WriteString(_T("<OBJECT type=\"text/site properties\">"));
            fileHHC.WriteString(_T("    <param name=\"ImageType\" value=\"Folder\">"));
			fileHHC.WriteString(_T("</OBJECT>"));
			fileHHC.WriteString(_T("<UL>"));
		}
	}
	
	// navigate with navigation tree
	if (::IsWindow(m_hTreeCtrl))
	{
		ProcessTreeItem(NULL);
	}
	
	// write html help content
	if (m_chmOptions.bCreateCHM && fileHHC.IsOpen())
	{
        if (!::IsWindow(m_hTreeCtrl))
        {
            fileHHC.WriteString(_T("<LI> <OBJECT type=\"text/sitemap\">"));
            fileHHC.WriteString(_T("<param name=\"Name\" value=\"Home\">"));
            fileHHC.WriteString(_T("<param name=\"Local\" value=\"index.htm\">"));
            fileHHC.WriteString(_T("</OBJECT>"));
        }
		fileHHC.WriteString(_T("</UL>"));
		fileHHC.WriteString(_T("</BODY>"));
		fileHHC.WriteString(_T("</HTML>"));
		fileHHC.Close();
	}
}

// process specified navigation tree item
void CIEDumper::ProcessTreeItem(HTREEITEM hItem)
{
	if (hItem)
	{
		// navigate to target page
		DWORD dwRet;
		m_SelTreeItemCode.dwTree = (DWORD)m_hTreeCtrl;
		m_SelTreeItemCode.dwItem = (DWORD)hItem;
		RemoteExec(m_dwProcId, &m_SelTreeItemCode, sizeof(m_SelTreeItemCode), &dwRet, 300);
		WaitForLoadingDocument();

		// save and parse page
		CString csURL = GetPageURL();
		CString csFileName;
		if (IsPageNeedProcess(csURL) && !IsPageProcessed(csURL))
		{
			m_csCurrentURL = csURL;
			csFileName = GetFileName(csURL);
			SaveHTMLSource(csFileName);
			AddURLToMap(csURL, m_mapPage, TRUE);
			ParsePage(csFileName);
			ProcessFileMap();			
		}
		
		// create html help content item
		if (m_chmOptions.bCreateCHM && fileHHC.IsOpen())
		{
			if (TreeView_GetChild(m_hTreeCtrl, hItem))
			{
				CString csLine;
				fileHHC.WriteString(_T("<LI><OBJECT type=\"text/sitemap\">"));
				csLine.Format(_T("<param name=\"Name\" value=\"%s\">"),
					GetRemoteTreeItemText(m_dwProcId, m_hTreeCtrl, hItem));
                fileHHC.WriteString(csLine);
				fileHHC.WriteString(_T("<param name=\"ImageNumber\" value=\"1\">"));
				fileHHC.WriteString(_T("</OBJECT>"));
				fileHHC.WriteString(_T("<UL>"));
			}
			else
			{
				CString csLine;
				fileHHC.WriteString(_T("<LI><OBJECT type=\"text/sitemap\">"));
				csLine.Format(_T("<param name=\"Name\" value=\"%s\">"),
					GetRemoteTreeItemText(m_dwProcId, m_hTreeCtrl, hItem));
				fileHHC.WriteString(csLine);
				csLine.Format(_T("<param name=\"Local\" value=\"%s\">"),
					csFileName.IsEmpty() ? _T("") : csFileName.Mid(m_csOutputDir.GetLength()));
				fileHHC.WriteString(csLine);
				fileHHC.WriteString(_T("<param name=\"ImageNumber\" value=\"11\">"));
				fileHHC.WriteString(_T("</OBJECT>"));
			}
		}
	}

	// get child item
	HTREEITEM hChild;
	if (hItem)
	{
		hChild = TreeView_GetChild(m_hTreeCtrl, hItem);
	}
	else
	{
		hChild = TreeView_GetRoot(m_hTreeCtrl);
	}

	// process child items recursively
	while (!m_bBreakFlag && hChild)
	{
		ProcessTreeItem(hChild);
		hChild = TreeView_GetNextSibling(m_hTreeCtrl, hChild);
	}

	// write html help content item
	if (m_chmOptions.bCreateCHM && hItem && TreeView_GetChild(m_hTreeCtrl, hItem) && fileHHC.IsOpen())
	{
		fileHHC.WriteString(_T("</UL>"));
	}
}

// process page url map
void CIEDumper::ProcessPageMap(BOOL bOnePageOnly /* = FALSE */)
{
	ELEMENT_INFO* pPageInfo = GetFileToProcess(m_mapPage);
	while (!m_bBreakFlag && pPageInfo)
	{
		// navigate to target page
		if (NormalizeURL(GetPageURL()) != pPageInfo->csURL)
		{
			m_pHtmlDoc2->put_URL(CComBSTR(pPageInfo->csURL));
			WaitForLoadingDocument();
		}
		
		// save and parse page
		m_csCurrentURL = pPageInfo->csURL;
		CString csFileName = GetFileName(pPageInfo->csURL);
		SaveHTMLSource(csFileName);
		pPageInfo->bSaved = TRUE;
		ParsePage(csFileName);		
		ProcessFileMap();

		// one page only
		if (bOnePageOnly)
		{
			break;
		}

		// get next page
		pPageInfo = GetFileToProcess(m_mapPage);
	}
}

// process other file url map
void CIEDumper::ProcessFileMap()
{
	ELEMENT_INFO* pFileInfo = GetFileToProcess(m_mapOther);
	while (!m_bBreakFlag && pFileInfo)
	{
		DownloadFile(pFileInfo->csURL, GetFileName(pFileInfo->csURL));
		pFileInfo->bSaved = TRUE;
		pFileInfo = GetFileToProcess(m_mapOther);
	}
}

// wait for navigation complete
void CIEDumper::WaitForLoadingDocument()
{
	CComBSTR bstr;
	CString csState;
	DWORD dwBeginTime = ::GetTickCount();
	if (SUCCEEDED(m_pHtmlDoc2->get_readyState(&bstr)))
	{
		csState = bstr;
	}
	DWORD dwTickCount = ::GetTickCount();
	while ((dwTickCount - dwBeginTime < m_dwLoadWait) ||
		(csState != _T("complete") && (dwTickCount - dwBeginTime < m_dwLoadTimeout)))
	{
		Sleep(100);
		dwTickCount = ::GetTickCount();
		if (SUCCEEDED(m_pHtmlDoc2->get_readyState(&bstr)))
		{
			csState = bstr;
		}
	}
}

// save html source of current page
BOOL CIEDumper::SaveHTMLSource(LPCTSTR lpFileName)
{
	// get current url
	TCHAR cURL[1024];
	lstrcpy(cURL, m_csCurrentURL.GetBuffer(m_csCurrentURL.GetLength()));
	m_csCurrentURL.ReleaseBuffer();

	// notify message
	if (::IsWindow(m_hOwner))
	{
		IDNMHDR idnmhdr;
		idnmhdr.nmhdr.hwndFrom = m_hOwner;
		idnmhdr.nmhdr.idFrom = 0;
		idnmhdr.nmhdr.code = IDN_PROGRESS;
		idnmhdr.dwData = (DWORD)cURL;
		::PostMessage(m_hOwner, WM_NOTIFY, idnmhdr.nmhdr.idFrom, (LPARAM)&idnmhdr);
	}

	// save file
	ForceDirectories(ExtractFilePath(lpFileName));
	CComPtr<IPersistFile> spFile;
	USES_CONVERSION;
	if (SUCCEEDED(m_pHtmlDoc2->QueryInterface(&spFile)) &&
		SUCCEEDED(spFile->Save(T2OLE(const_cast<LPTSTR>(lpFileName)), FALSE)))
	{
		return TRUE;
	}

	return FALSE;
}

// parse page from saved file
void CIEDumper::ParsePage(LPCTSTR lpFileName)
{
	CFile file;
	if (file.Open(lpFileName, CFile::modeRead))
	{
		CLiteHTMLReader theReader;
		theReader.setEventHandler(this);
		theReader.ReadFile((HANDLE)file.m_hFile);
		file.Close();
	}
}

// page parser event handler
void CIEDumper::StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort)
{
	// get tag name
	ASSERT(pTag);
	CString csTagName = pTag->getTagName();
	csTagName.MakeLower();

	// process tags by kind
	if (csTagName == _T("a") ||
        csTagName == _T("area"))
	{
		AddURLToMap(GetAbsoluteURL(m_csCurrentURL, SafeGetAttrValue(pTag, _T("href"))), m_mapPage);
	}
	else if (csTagName == _T("frame") ||
			 csTagName == _T("iframe"))
	{
		AddURLToMap(GetAbsoluteURL(m_csCurrentURL, SafeGetAttrValue(pTag, _T("src"))), m_mapPage);
	}
	else if (csTagName == _T("link"))
	{
		AddURLToMap(GetAbsoluteURL(m_csCurrentURL, SafeGetAttrValue(pTag, _T("href"))), m_mapOther);
	}
	else if (csTagName == _T("img")		||
			 csTagName == _T("embed")	||
			 csTagName == _T("applet")	||
			 csTagName == _T("bgsound")	||
			 csTagName == _T("script"))
	{
		AddURLToMap(GetAbsoluteURL(m_csCurrentURL, SafeGetAttrValue(pTag, _T("src"))), m_mapOther);
	}
	else if (csTagName == _T("body") ||
			 csTagName == _T("table") ||
			 csTagName == _T("td"))
	{
		AddURLToMap(GetAbsoluteURL(m_csCurrentURL, SafeGetAttrValue(pTag, _T("background"))), m_mapOther);
	}
}

// get tag attribute
CString CIEDumper::SafeGetAttrValue(CLiteHTMLTag* pTag, LPCTSTR lpAttrName)
{
	ASSERT(pTag);
	CLiteHTMLAttributes* pAttrs = const_cast<CLiteHTMLAttributes*>(pTag->getAttributes());
	if (pAttrs)
	{
		int n = pAttrs->getIndexFromName(lpAttrName);
		if (n != -1)
		{
			return pAttrs->getAttribute(n).getValue();
		}
	}

	return _T("");
}

// download file from ebook process
void CIEDumper::DownloadFile(LPCTSTR lpURL, LPCTSTR lpFileName)
{
	ForceDirectories(ExtractFilePath(lpFileName));

    CString csExt = GetFileExt(lpFileName);
    csExt.MakeLower();
    if (m_bSnap)
    {
        if (csExt == _T("bmp") || csExt == _T("jpg") || csExt == _T("png") || csExt == _T("gif"))
        {
            DWORD dwFormat = 0;
            if (csExt == _T("bmp")) dwFormat = CXIMAGE_FORMAT_BMP;
            else if (csExt == _T("jpg")) dwFormat = CXIMAGE_FORMAT_JPG;
            else if (csExt == _T("gif")) dwFormat = CXIMAGE_FORMAT_GIF;
            else if (csExt == _T("png")) dwFormat = CXIMAGE_FORMAT_PNG;
            SnapImage(lpURL, lpFileName, dwFormat);
        }
    }
    else
    {
	    USES_CONVERSION;
	    DWORD dwRet;
	    lstrcpyA(m_StealFileCode.cURL, T2A(const_cast<LPTSTR>(lpURL)));
	    lstrcpyA(m_StealFileCode.cFile, T2A(const_cast<LPTSTR>(lpFileName)));
	    RemoteExec(m_dwProcId, &m_StealFileCode, sizeof(m_StealFileCode), &dwRet, 300);
    }
}

void CIEDumper::SnapImage(LPCTSTR lpURL, LPCTSTR lpFileName, DWORD dwFormat)
{
    // navigate to target page
	if (NormalizeURL(GetPageURL()) != lpURL)
	{
		m_pHtmlDoc2->put_URL(CComBSTR(lpURL));
		WaitForLoadingDocument();
	}

    IViewObjectPtr spViewObj;
    IHTMLElementCollectionPtr spColl;
    IDispatchPtr spDispatch;
    IHTMLElementPtr spElement;
    VARIANT index;
    index.vt = VT_I4;
    index.lVal = 0;
    if (SUCCEEDED(m_pHtmlDoc2->get_images(&spColl)) &&
        SUCCEEDED(spColl->item(index, index, &spDispatch)) &&
        SUCCEEDED(spDispatch->QueryInterface(&spElement)))
    {
        long left, top, width, height;
        spElement->get_offsetLeft(&left);
        spElement->get_offsetTop(&top);
        spElement->get_offsetWidth(&width);
        spElement->get_offsetHeight(&height);

	    CDC dcScreen;
        CDC dcMem;
        CBitmap bmpMem;
	    dcScreen.CreateDC("DISPLAY", NULL, NULL, NULL);
        dcMem.CreateCompatibleDC(&dcScreen);
        bmpMem.CreateCompatibleBitmap(&dcScreen, width, height);
        CBitmap* pOldBmp = dcMem.SelectObject(&bmpMem);
        CRect rcCtrl;
        ::GetWindowRect(m_hIECtrl, &rcCtrl);
        left += (rcCtrl.left + 2);
        top += (rcCtrl.top + 2);
        dcMem.BitBlt(0, 0, width, height, &dcScreen, left, top, SRCCOPY);
        dcMem.SelectObject(pOldBmp);

        CxImage image;
        image.CreateFromHBITMAP(bmpMem);
        if (dwFormat == CXIMAGE_FORMAT_JPG)
            image.SetJpegQuality(m_nJPEGQuality);
        else if (dwFormat == CXIMAGE_FORMAT_GIF)
        {
            dwFormat = CXIMAGE_FORMAT_GIF;
            CQuantizer q(256, 7);            
			RGBQUAD* ppal = NULL;
			q.ProcessImage(image.GetDIB());
			ppal = (RGBQUAD*)calloc(256 * sizeof(RGBQUAD), 1);
			q.SetColorTable(ppal);
            image.DecreaseBpp(8, false, ppal, 256);
			if (ppal) free(ppal);
        }
        image.Save(lpFileName, dwFormat);
    }
}

// create html help project file
void CIEDumper::CreateCHMProject()
{
	// notify message
	if (::IsWindow(m_hOwner))
	{
		TCHAR cMsg[64];
		lstrcpy(cMsg, _T("Creating html help project ..."));
		IDNMHDR idnmhdr;
		idnmhdr.nmhdr.hwndFrom = m_hOwner;
		idnmhdr.nmhdr.idFrom = 0;
		idnmhdr.nmhdr.code = IDN_PROGRESS;
		idnmhdr.dwData = (DWORD)cMsg;
		::PostMessage(m_hOwner, WM_NOTIFY, idnmhdr.nmhdr.idFrom, (LPARAM)&idnmhdr);
	}

	// delete output file and error log file
	::DeleteFile(ChangeFileExt(m_chmOptions.csProjectFile, _T("chm")));
	::DeleteFile(ExtractURLPath(m_chmOptions.csProjectFile) + _T("_errorlog.txt"));

	// get filenames
	CString csHHCFile = ExtractFileName(m_chmOptions.csContentFile);
	CString csCHMFile = ChangeFileExt(ExtractFileName(m_chmOptions.csProjectFile), _T("chm"));
	CString csLine;

	// open file
	if (!fileHHP.Open(m_chmOptions.csProjectFile, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		return;
	}

	// write OPTIONS section
	fileHHP.WriteString(_T("[OPTIONS]"));
	fileHHP.WriteString(_T("Binary TOC=No"));
	fileHHP.WriteString(_T("Binary Index=Yes"));
	csLine.Format(_T("Compiled File=%s"), csCHMFile);
	fileHHP.WriteString(csLine);
	csLine.Format(_T("Contents File=%s"), csHHCFile);
	fileHHP.WriteString(csLine);
	fileHHP.WriteString(_T("Index File="));
	fileHHP.WriteString(_T("Default Window=main"));
	csLine.Format(_T("Default Topic=%s"), m_chmOptions.csHomePage);
	fileHHP.WriteString(csLine);
	csLine.Format(_T("Default Font=%s"), m_chmOptions.nLanguage == 1 ? _T("ËÎÌו, 9,134") : _T("Tahoma, 8,1"));
	fileHHP.WriteString(csLine);
    if (m_chmOptions.bFullTextSearch)
	    fileHHP.WriteString(_T("Full-text search=Yes"));
	fileHHP.WriteString(_T("Auto Index=Yes"));
	csLine.Format(_T("Language=%s"), m_chmOptions.nLanguage == 1 ? _T("0x804 Chinese (Simplified/PRC)") : _T("0x409 English (United States)"));
	fileHHP.WriteString(csLine);
	csLine.Format(_T("Title=%s"), m_chmOptions.csWindowTitle);
	fileHHP.WriteString(csLine);
	fileHHP.WriteString(_T("Create CHI file=No"));
	fileHHP.WriteString(_T("Compatibility=1.1 or later"));
	fileHHP.WriteString(_T("Error log file=_errorlog.txt"));
	fileHHP.WriteString(_T("Full text search stop list file="));
	fileHHP.WriteString(_T("Display compile progress=Yes"));
	fileHHP.WriteString(_T("Display compile notes=Yes"));

	// write WINDOWS section
	fileHHP.WriteString(_T("[WINDOWS]"));
	bitset<32> dwWinStyle;
	dwWinStyle[5] = 1;
	if (m_chmOptions.bHideToolbarText) dwWinStyle[6] = 1;
	dwWinStyle[13] = 1;
	if (m_chmOptions.bRememberWinPos) dwWinStyle[18] = 1;
	DWORD dwToolbar = 0x10307C;
	if (m_hTreeCtrl != NULL)
	{
		dwToolbar |= (1 << 1);
	}
	csLine.Format(_T("main=\"%s\",\"%s\",,\"%s\",\"%s\",,,,,0x%x,%d,0x%x,[0,0,%d,%d],0xB0000,,,%s,,,0"),
		m_chmOptions.csWindowTitle, csHHCFile, m_chmOptions.csHomePage, m_chmOptions.csHomePage,
		dwWinStyle.to_ulong(), m_chmOptions.nPaneWidth, dwToolbar,
		m_chmOptions.szWindowSize.cx, m_chmOptions.szWindowSize.cy,
		m_hTreeCtrl ? _T("") : _T("1"));
	fileHHP.WriteString(csLine);

	// write FILES section
	fileHHP.WriteString(_T("[FILES]"));
	::SetCurrentDirectory(m_csOutputDir);
	SearchCHMFileList();

	// close file
	fileHHP.Close();
}

// write file list into html help project file
void CIEDumper::SearchCHMFileList()
{
	// get current directory
	TCHAR buf[1024];
	::GetCurrentDirectory(1024, buf);
	CString csDir = buf;
	if (csDir[csDir.GetLength() - 1] != _T('\\'))
	{
		csDir += _T('\\');
	}

	// find files recursively
	WIN32_FIND_DATA wfd;
	HANDLE hSearch = ::FindFirstFile(_T("*.*"), &wfd);
	BOOL bContinue = hSearch != NULL;
	while (!m_bBreakFlag && bContinue)
	{
		CString csFile = wfd.cFileName;
		if (csFile[0] != _T('.'))
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				::SetCurrentDirectory(csFile);
				SearchCHMFileList();
				::SetCurrentDirectory(_T(".."));
			}
			else
			{
				csFile = csDir + csFile;
				fileHHP.WriteString(csFile.Mid(m_csOutputDir.GetLength()));
			}
		}
		bContinue = ::FindNextFile(hSearch, &wfd);
	}
	::FindClose(hSearch);
}

// compile html help project
void CIEDumper::CompileCHMProject()
{
	// notify message
	if (::IsWindow(m_hOwner))
	{
		TCHAR cMsg[64];
		lstrcpy(cMsg, _T("Compiling html help project ..."));
		IDNMHDR idnmhdr;
		idnmhdr.nmhdr.hwndFrom = m_hOwner;
		idnmhdr.nmhdr.idFrom = 0;
		idnmhdr.nmhdr.code = IDN_PROGRESS;
		idnmhdr.dwData = (DWORD)cMsg;
		::PostMessage(m_hOwner, WM_NOTIFY, idnmhdr.nmhdr.idFrom, (LPARAM)&idnmhdr);
	}

	// get compiler (hhc.exe) filename
	CString csCompiler;
	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\HTML Help Workshop"), KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR buf[256];
		DWORD dwCount = sizeof(buf);
		if (key.QueryValue(buf, _T("InstallDir"), &dwCount) == ERROR_SUCCESS)
		{
			csCompiler = buf;
			csCompiler += _T("\\hhc.exe");
		}
		key.Close();
	}

	// check compiler
	if (!FileExists(csCompiler))
	{
		return;
	}

	// create compiling process
	CString csCommandLine;
	csCommandLine.Format(_T("\"%s\" \"%s\""), csCompiler, m_chmOptions.csProjectFile);
	SECURITY_ATTRIBUTES sa;
	::ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	STARTUPINFO si;
	::ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	if (CreateProcess(NULL, csCommandLine.GetBuffer(csCommandLine.GetLength()), NULL, NULL, TRUE,
		CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS, NULL, m_csOutputDir, &si, &pi))
	{
		DWORD dwRet;
		::WaitForSingleObject(pi.hProcess, INFINITE);
		::GetExitCodeProcess(pi.hProcess, &dwRet);
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);

		// open output .chm file
		if (dwRet == 1)
		{
			ShellExecute(0, _T("open"), ChangeFileExt(m_chmOptions.csProjectFile, _T("chm")), NULL, NULL, SW_SHOWNORMAL);
		}
	}
	csCommandLine.ReleaseBuffer();
}
