// DeeBook.h : main header file for the DEEBOOK application
//

#if !defined(AFX_DEEBOOK_H__9936931E_FF2C_45D5_AB67_7D7F4E8D599C__INCLUDED_)
#define AFX_DEEBOOK_H__9936931E_FF2C_45D5_AB67_7D7F4E8D599C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDeeBookApp:
// See DeeBook.cpp for the implementation of this class
//

class CDeeBookApp : public CWinApp
{
public:
	CDeeBookApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeeBookApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDeeBookApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEEBOOK_H__9936931E_FF2C_45D5_AB67_7D7F4E8D599C__INCLUDED_)
