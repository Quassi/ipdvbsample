// ipdvbsample.h : main header file for the IPDVBSAMPLE application
//

#if !defined(AFX_IPDVBSAMPLE_H__20181DA4_E78E_11D3_838D_CE68F49C300A__INCLUDED_)
#define AFX_IPDVBSAMPLE_H__20181DA4_E78E_11D3_838D_CE68F49C300A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIpdvbsampleApp:
// See ipdvbsample.cpp for the implementation of this class
//

class CIpdvbsampleApp : public CWinApp
{
public:
	CIpdvbsampleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIpdvbsampleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIpdvbsampleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPDVBSAMPLE_H__20181DA4_E78E_11D3_838D_CE68F49C300A__INCLUDED_)
