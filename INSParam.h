#if !defined(AFX_INSPARAM_H__A26F55EA_BA6B_44E9_A0ED_C68C914D118F__INCLUDED_)
#define AFX_INSPARAM_H__A26F55EA_BA6B_44E9_A0ED_C68C914D118F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// INSParam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CINSParam dialog
class CPreInspection;

class CINSParam : public CDialog
{
// Construction
public:
	CINSParam(CWnd* pParent = NULL);   // standard constructor
	CPreInspection * m_pPreDlg;

// Dialog Data
	//{{AFX_DATA(CINSParam)
	enum { IDD = IDD_DLG_PARAMETER };
	int		m_nMainThres;
	int		m_nSatuthres;
	int		m_nMaguriThres;
	int		m_nChuckThres;
	int		m_nAngleThres;
	int		m_midNotInspArea;
	float	m_chuckLeft;
	int		m_nEtqtoStopper;
	int		m_nEtqInspArea;
	int		m_maguriSideDepth;
	int		m_nEtqLength;
	int		m_chuckNotInspArea;
	int		m_nImulMidToStopper;
	int		m_nImulChuckToStopper;
	int		m_nImulInsertHeight;
	int		m_nImulInsertNoInsp;
	int		m_nImulInsertUpDown;
	int		m_nImulJabaraNoInsp;
	int		m_nImulMaguriNoInsp;
	//int	m_nImulUpDownNoInsp;
	int     m_transRatio;
	float	m_fCameraGain;
	float	m_fCameraShutter;
	float	m_fChuckPokAverage;
	float	m_fChuckDownScale;
	float	m_fChuckUpScale;
	int		m_nImageCenter;
	int		m_nTransInsertThres;
	int		m_nTransMaguriThres;
	BOOL	m_bUse14Trigger;
	int		m_nImulReduce;
	int		m_nColorThres01;
	int		m_nColorThres02;
	int		m_nColorThres03;
	int		m_nTriChangeMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CINSParam)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CINSParam)
	afx_msg void OnBtnApply();
	afx_msg void OnBtnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSPARAM_H__A26F55EA_BA6B_44E9_A0ED_C68C914D118F__INCLUDED_)
