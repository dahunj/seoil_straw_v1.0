// SeoilVision.h : main header file for the SEOILVISION application
//

#if !defined(AFX_SEOILVISION_H__6856087A_BA5B_42AA_9C0D_D51E22766268__INCLUDED_)
#define AFX_SEOILVISION_H__6856087A_BA5B_42AA_9C0D_D51E22766268__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSeoilVisionApp:
// See SeoilVision.cpp for the implementation of this class
//

class CSeoilVisionApp : public CWinApp
{
public:
	CSeoilVisionApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeoilVisionApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSeoilVisionApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEOILVISION_H__6856087A_BA5B_42AA_9C0D_D51E22766268__INCLUDED_)
