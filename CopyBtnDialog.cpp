// CopyBtnDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SeoilVision.h"
#include "CopyBtnDialog.h"
#include "classes/PreInspection.h"
#include "Extern.h"


// CCopyBtnDialog dialog

IMPLEMENT_DYNAMIC(CCopyBtnDialog, CDialog)

CCopyBtnDialog::CCopyBtnDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCopyBtnDialog::IDD, pParent),
	k_maxBtns(8)
	, k_maxChks(9)
	, m_checkROI1(TRUE)
	, m_checkROI2(TRUE)
	, m_checkROI3(TRUE)
	, m_checkROI4(TRUE)
	, m_checkROI5(TRUE)
	, m_checkROI6(TRUE)
	, m_checkROI7(TRUE)
	, m_checkROI8(TRUE)
	, m_checkROI9(TRUE)
	, m_curPos(0)
{
	m_pBtns[0] = &m_btnROI2;
	m_pBtns[1] = &m_btnROI3;
	m_pBtns[2] = &m_btnROI4;
	m_pBtns[3] = &m_btnROI5;
	m_pBtns[4] = &m_btnROI6;
	m_pBtns[5] = &m_btnROI7;
	m_pBtns[6] = &m_btnROI8;
	m_pBtns[7] = &m_btnROI9;

	m_pChks[0] = &m_btnChkROI1;
	m_pChks[1] = &m_btnChkROI2;
	m_pChks[2] = &m_btnChkROI3;
	m_pChks[3] = &m_btnChkROI4;
	m_pChks[4] = &m_btnChkROI5;
	m_pChks[5] = &m_btnChkROI6;
	m_pChks[6] = &m_btnChkROI7;
	m_pChks[7] = &m_btnChkROI8;
	m_pChks[8] = &m_btnChkROI9;
}

void CCopyBtnDialog::ShowBtns()
{
	int i;

	for ( i = 0; i < _MAXROI - 1; i++ )
	{
		m_pBtns[i]->ShowWindow( SW_SHOW );
		m_pChks[i]->ShowWindow( SW_SHOW );
	}
	// _MAXROI 개수만큼 show
	m_pChks[i]->ShowWindow( SW_SHOW );

	for ( ; i < k_maxBtns; i++ )
	{
		m_pBtns[i]->ShowWindow( SW_HIDE );
		m_pChks[i + 1]->ShowWindow( SW_HIDE );
	}
	// 하나 더 많음
}

CCopyBtnDialog::~CCopyBtnDialog()
{
}

void CCopyBtnDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ROIBTN_2, m_btnROI2);
	DDX_Control(pDX, IDC_ROIBTN_3, m_btnROI3);
	DDX_Control(pDX, IDC_ROIBTN_4, m_btnROI4);
	DDX_Control(pDX, IDC_ROIBTN_5, m_btnROI5);
	DDX_Control(pDX, IDC_ROIBTN_6, m_btnROI6);
	DDX_Control(pDX, IDC_ROIBTN_7, m_btnROI7);
	DDX_Control(pDX, IDC_ROIBTN_8, m_btnROI8);
	DDX_Control(pDX, IDC_ROIBTN_9, m_btnROI9);

	DDX_Check(pDX, IDC_CHECK_ROI1, m_checkROI1);
	DDX_Check(pDX, IDC_CHECK_ROI2, m_checkROI2);
	DDX_Check(pDX, IDC_CHECK_ROI3, m_checkROI3);
	DDX_Check(pDX, IDC_CHECK_ROI4, m_checkROI4);
	DDX_Check(pDX, IDC_CHECK_ROI5, m_checkROI5);
	DDX_Check(pDX, IDC_CHECK_ROI6, m_checkROI6);
	DDX_Check(pDX, IDC_CHECK_ROI7, m_checkROI7);
	DDX_Check(pDX, IDC_CHECK_ROI8, m_checkROI8);
	DDX_Check(pDX, IDC_CHECK_ROI9, m_checkROI9);

	DDX_Control(pDX, IDC_CHECK_ROI1, m_btnChkROI1);
	DDX_Control(pDX, IDC_CHECK_ROI2, m_btnChkROI2);
	DDX_Control(pDX, IDC_CHECK_ROI3, m_btnChkROI3);
	DDX_Control(pDX, IDC_CHECK_ROI4, m_btnChkROI4);
	DDX_Control(pDX, IDC_CHECK_ROI5, m_btnChkROI5);
	DDX_Control(pDX, IDC_CHECK_ROI6, m_btnChkROI6);
	DDX_Control(pDX, IDC_CHECK_ROI7, m_btnChkROI7);
	DDX_Control(pDX, IDC_CHECK_ROI8, m_btnChkROI8);
	DDX_Control(pDX, IDC_CHECK_ROI9, m_btnChkROI9);
}


BEGIN_MESSAGE_MAP(CCopyBtnDialog, CDialog)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_ROIBTN_2, &CCopyBtnDialog::OnBnClickedRoibtn2)
	ON_BN_CLICKED(IDC_ROIBTN_3, &CCopyBtnDialog::OnBnClickedRoibtn3)
	ON_BN_CLICKED(IDC_ROIBTN_4, &CCopyBtnDialog::OnBnClickedRoibtn4)
	ON_BN_CLICKED(IDC_ROIBTN_5, &CCopyBtnDialog::OnBnClickedRoibtn5)
	ON_BN_CLICKED(IDC_ROIBTN_6, &CCopyBtnDialog::OnBnClickedRoibtn6)
	ON_BN_CLICKED(IDC_ROIBTN_7, &CCopyBtnDialog::OnBnClickedRoibtn7)
	ON_BN_CLICKED(IDC_ROIBTN_8, &CCopyBtnDialog::OnBnClickedRoibtn8)
	ON_BN_CLICKED(IDC_ROIBTN_9, &CCopyBtnDialog::OnBnClickedRoibtn9)

	ON_BN_CLICKED(IDC_CHECK_ROI1, &CCopyBtnDialog::OnBnClickedCheckRoi1)
	ON_BN_CLICKED(IDC_CHECK_ROI2, &CCopyBtnDialog::OnBnClickedCheckRoi2)
	ON_BN_CLICKED(IDC_CHECK_ROI3, &CCopyBtnDialog::OnBnClickedCheckRoi3)
	ON_BN_CLICKED(IDC_CHECK_ROI4, &CCopyBtnDialog::OnBnClickedCheckRoi4)
	ON_BN_CLICKED(IDC_CHECK_ROI5, &CCopyBtnDialog::OnBnClickedCheckRoi5)
	ON_BN_CLICKED(IDC_CHECK_ROI6, &CCopyBtnDialog::OnBnClickedCheckRoi6)
	ON_BN_CLICKED(IDC_CHECK_ROI7, &CCopyBtnDialog::OnBnClickedCheckRoi7)
	ON_BN_CLICKED(IDC_CHECK_ROI8, &CCopyBtnDialog::OnBnClickedCheckRoi8)
	ON_BN_CLICKED(IDC_CHECK_ROI9, &CCopyBtnDialog::OnBnClickedCheckRoi9)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CCopyBtnDialog message handlers

void CCopyBtnDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int minPos, maxPos;
	this->GetScrollRange(SB_VERT, &minPos, &maxPos);
	
	m_curPos = this->GetScrollPos(SB_VERT);
	
	switch(nSBCode)
	{
	case SB_LINEDOWN:
		if(maxPos > m_curPos)
		{
			this->SetScrollPos(SB_VERT, m_curPos + 10);
			this->ScrollWindow(0, -10);
		}
		break;

	case SB_LINEUP:
		if(minPos < m_curPos)
		{
			this->SetScrollPos(SB_VERT, m_curPos - 10);
			this->ScrollWindow(0, 10);
		}
		break;

	case SB_THUMBTRACK:

		break;

	case SB_PAGEDOWN:

		break;

	case SB_PAGEUP:
		break;
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CCopyBtnDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	
	int minPos, maxPos;
	GetScrollRange( SB_VERT, &minPos, &maxPos );
	
	m_curPos = GetScrollPos( SB_VERT );
	
	if( zDelta < 0 )
	{
		if ( maxPos > m_curPos )
		{
			this->SetScrollPos( SB_VERT, m_curPos + 10 );
			this->ScrollWindow( 0, -10 );
		}
	}
	else if ( zDelta > 0 )
	{
		if ( minPos < m_curPos )
		{
			this->SetScrollPos( SB_VERT, m_curPos - 10 );
			this->ScrollWindow( 0, 10 );
		}
	}

	//return CDialog::OnMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}

void CCopyBtnDialog::SetAllCheckBox()
{
	m_checkROI1 = TRUE;
	m_checkROI2 = TRUE;
	m_checkROI3 = TRUE;
	m_checkROI4 = TRUE;
	m_checkROI5 = TRUE;
	m_checkROI6 = TRUE;
	m_checkROI7 = TRUE;
	m_checkROI8 = TRUE;
	m_checkROI9 = TRUE;

	m_btnChkROI1.SetCheck(BST_CHECKED);
	m_btnChkROI2.SetCheck(BST_CHECKED);
	m_btnChkROI3.SetCheck(BST_CHECKED);
	m_btnChkROI4.SetCheck(BST_CHECKED);
	m_btnChkROI5.SetCheck(BST_CHECKED);
	m_btnChkROI6.SetCheck(BST_CHECKED);
	m_btnChkROI7.SetCheck(BST_CHECKED);
	m_btnChkROI8.SetCheck(BST_CHECKED);
	m_btnChkROI9.SetCheck(BST_CHECKED);
}

BOOL CCopyBtnDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	int maxPos = _MAXROI - 5;
	
	if ( maxPos < 0 )
	{
		maxPos = 0;
	}

	this->SetScrollRange(SB_VERT, 0, maxPos * 30);

	return TRUE;
}

void CCopyBtnDialog::OnBnClickedRoibtn2()
{
	CPreInspection *pParent = (CPreInspection*)this->GetParent();

	pParent->OnRoi2();
}

void CCopyBtnDialog::OnBnClickedRoibtn3()
{
	CPreInspection *pParent = (CPreInspection*)this->GetParent();

	pParent->OnRoi3();
}

void CCopyBtnDialog::OnBnClickedRoibtn4()
{
	CPreInspection *pParent = (CPreInspection*)this->GetParent();

	pParent->OnRoi4();
}

void CCopyBtnDialog::OnBnClickedRoibtn5()
{
	CPreInspection *pParent = (CPreInspection*)this->GetParent();

	pParent->OnRoi5();
}

void CCopyBtnDialog::OnBnClickedRoibtn6()
{
	CPreInspection *pParent = (CPreInspection*)this->GetParent();

	pParent->OnRoi6();
}

void CCopyBtnDialog::OnBnClickedRoibtn7()
{
	CPreInspection *pParent = (CPreInspection*)this->GetParent();

	pParent->OnRoi7();
}

void CCopyBtnDialog::OnBnClickedRoibtn8()
{
	CPreInspection *pParent = (CPreInspection*)this->GetParent();

	pParent->OnRoi8();
}

void CCopyBtnDialog::OnBnClickedRoibtn9()
{
	CPreInspection *pParent = (CPreInspection*)this->GetParent();

	pParent->OnRoi9();
}

BOOL CCopyBtnDialog::PreTranslateMessage(MSG* pMsg) 
{
	switch( pMsg->message)
	{
		case WM_KEYDOWN:
		{
			int s;
			
			s = (int)pMsg->wParam;

			if ( s == VK_RETURN )
			{
				return true;
			}
			else if (s==VK_TAB)
			{
				return true;
			}
			else if (s==VK_SPACE)
			{
				CPreInspection *parent = ( CPreInspection* )GetParent();
				parent->OnTest();
				return true;
			}
		}
    }

	CPreInspection *parent = ( CPreInspection* ) GetParent();
	parent->PreTranslateMessage(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}
void CCopyBtnDialog::OnBnClickedCheckRoi1()
{
	m_checkROI1 = !m_checkROI1;

	CPreInspection *parent = ( CPreInspection* ) GetParent();
	parent->ShowROIS( 0, m_checkROI1 );
}

void CCopyBtnDialog::OnBnClickedCheckRoi2()
{
	m_checkROI2 = !m_checkROI2;

	CPreInspection *parent = ( CPreInspection* ) GetParent();
	parent->ShowROIS( 1, m_checkROI2 );
}

void CCopyBtnDialog::OnBnClickedCheckRoi3()
{
	m_checkROI3 = !m_checkROI3;

	CPreInspection *parent = ( CPreInspection* ) GetParent();
	parent->ShowROIS( 2, m_checkROI3 );
}

void CCopyBtnDialog::OnBnClickedCheckRoi4()
{
	m_checkROI4 = !m_checkROI4;

	CPreInspection *parent = ( CPreInspection* ) GetParent();
	parent->ShowROIS( 3, m_checkROI4 );
}

void CCopyBtnDialog::OnBnClickedCheckRoi5()
{
	m_checkROI5 = !m_checkROI5;

	CPreInspection *parent = ( CPreInspection* ) GetParent();
	parent->ShowROIS( 4, m_checkROI5 );
}

void CCopyBtnDialog::OnBnClickedCheckRoi6()
{
	m_checkROI6 = !m_checkROI6;

	CPreInspection *parent = ( CPreInspection* ) GetParent();
	parent->ShowROIS( 5, m_checkROI6 );
}

void CCopyBtnDialog::OnBnClickedCheckRoi7()
{
	m_checkROI7 = !m_checkROI7;

	CPreInspection *parent = ( CPreInspection* ) GetParent();
	parent->ShowROIS( 6, m_checkROI7 );
}

void CCopyBtnDialog::OnBnClickedCheckRoi8()
{
	m_checkROI8 = !m_checkROI8;

	CPreInspection *parent = ( CPreInspection* ) GetParent();
	parent->ShowROIS( 7, m_checkROI8 );
}

void CCopyBtnDialog::OnBnClickedCheckRoi9()
{
	m_checkROI9 = !m_checkROI9;

	CPreInspection *parent = ( CPreInspection* ) GetParent();
	parent->ShowROIS( 8, m_checkROI9 );
}

