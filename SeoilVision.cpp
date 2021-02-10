// SeoilVision.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SeoilVision.h"
#include "SeoilVisionDlg.h"

#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSeoilVisionApp

BEGIN_MESSAGE_MAP(CSeoilVisionApp, CWinApp)
	//{{AFX_MSG_MAP(CSeoilVisionApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeoilVisionApp construction

CSeoilVisionApp::CSeoilVisionApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSeoilVisionApp object

CSeoilVisionApp theApp;




/////////////////////////////////////////////////////////////////////////////
// CSeoilVisionApp initialization

BOOL CSeoilVisionApp::InitInstance()
{
	//USB Lock 여부
	//CVisionWorks2_Dll::CheckLock();       //by walca 코드 추가

	AfxEnableControlContainer();
	if (!AfxSocketInit())
	{
		AfxMessageBox(_T("소켓 초기화 실패"));
		return FALSE;
	}
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	CSeoilVisionDlg dlg;
	m_pMainWnd = &dlg;

	AfxInitRichEdit();

//	int nResponse = dlg.DoModal();

	if (dlg.DoModal() == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (dlg.DoModal() == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	return FALSE;
}
