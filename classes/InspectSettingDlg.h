#include "afxwin.h"
#if !defined(AFX_INSPECTSETTINGDLG_H__CBF282F0_183B_41F0_8FD5_F090A249ED1E__INCLUDED_)
#define AFX_INSPECTSETTINGDLG_H__CBF282F0_183B_41F0_8FD5_F090A249ED1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InspectSettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInspectSettingDlg dialog

class CInspectSettingDlg : public CDialog
{
// Construction
public:
	CInspectSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInspectSettingDlg)
	enum { IDD = IDD_INSPECT_SETTING };
	
	CEdit	c_slide_length;
	CEdit	c_pok_limit;
	CEdit	c_pok;
	CEdit	c_minchuck;
	CEdit	c_maxchuck;

	// jabara
	CEdit	c_jabara_distance_stdev;
	CEdit   c_jabara_gradient_stdev;
	CEdit	c_jabara_width_stdev;
	CEdit	c_jabara_num;
	CEdit	c_jabara_recognize;
	CEdit   c_jabara_neighbor;
	CEdit	c_jabara_margin_of_error;
	

	// etiquette
	CEdit c_etiquette_position;
	CEdit c_etiquette_recognize;
	CEdit c_etiquette_size;

	CEdit	c_insert_slide;
	CEdit	c_anglelimit_2;
	CEdit	c_anglelimit_1;
	CEdit	c_angle_diff;
	CEdit	c_maguri_length;
	CEdit	c_maguri_slide;

	// imul
	CEdit	c_imul_pix;
	CEdit	c_imul_mid_pix;
	CEdit	c_imul_jabara_pix;
	CEdit	c_imul_chuck_pix;
	CEdit	c_imul_insert_pix;
	CEdit	c_imul_top_threshold;
	CEdit	c_imul_mid_threshold;
	CEdit	c_imul_jabara_threshold;
	CEdit	c_imul_chuck_threshold;
	CEdit	c_imul_insert_threshold;
	CEdit	c_imul_top_mask;
	CEdit	c_imul_top_mask_cols;
	CEdit	c_imul_mid_mask;
	CEdit	c_imul_jabara_mask;
	CEdit	c_imul_chuck_mask;
	CEdit	c_imul_insert_mask;
	CEdit	c_imul_insert_pos;

	double	m_maguri_slide;
	BOOL	m_maguri_ok;
	BOOL	m_angle1_ok;
	BOOL	m_anglew_ok;
	double	m_angle1_limit;
	double	m_angle2_limit;
	BOOL	m_chuck_ok;
	BOOL	m_insert_ok;
	double	m_insert_slide;
	
	// jabara
	double	m_jabara_recognize;
	int 	m_jabara_num;
	double	m_jabara_width_stdev;
	double	m_jabara_distance_stdev;
	double  m_jabara_gradient_stdev;
	int		m_jabara_neighbor;
	int		m_jabara_margin_of_error;
	BOOL	m_jabara_ok;
	BOOL	m_jabara_edge_ok;

	// etiquette
	BOOL	m_etiquette_ok;
	double	m_etiquette_recognize;
	double	m_etiquette_position;
	int		m_etiquette_size;

	double	m_minchuck;
	double	m_maxchuck;
	double	m_pok;
	double	m_pok_limit;
	BOOL	m_pok_ok;
	double	m_resol;
	int		m_slide_length;
	
	// 이물
	BOOL	m_imul1_ok;
	BOOL	m_imul_mid_ok;
	BOOL	m_imul_jabara_ok;
	BOOL	m_imul_chuck_ok;
	BOOL	m_imul_insert_ok;
	
	double	m_imul_pix;
	int		m_imul_mid_pix;
	int		m_imul_jabara_pix;
	int		m_imul_chuck_pix;
	int		m_imul_insert_pix;
	
	int		m_imul_top_threshold;
	int		m_imul_mid_threshold;
	int		m_imul_jabara_threshold;
	int		m_imul_chuck_threshold;
	int		m_imul_insert_threshold;

	int		m_imul_top_mask;
	int		m_imul_top_mask_cols;
	int		m_imul_mid_mask;
	int		m_imul_jabara_mask;
	int		m_imul_chuck_mask;
	int		m_imul_insert_mask;
	int		m_imul_insert_pos;

	int		m_maguri_length;
	int     m_maguri_max_length;
	double	m_angle_diff;

	double	m_imul_top_persent;
	double	m_imul_mid_persent;
	int	m_imul_top_size;
	int	m_imul_mid_size;

	int	m_imul_mid_notinspect;

	int m_stabbed_insert_notinspect;
	int m_stabbed_insert_pix;

	

	//}}AFX_DATA

	// 추가된 사항
	int m_camWidth;
	int m_camHeight;
	int m_triggerPerPitch;
	int m_inspPitch;
	


	struct BACKUP_SETTING
	{
		int	m_maguri_length;
		double	m_maguri_slide;
		BOOL	m_maguri_ok;
		BOOL	m_angle1_ok;
		BOOL	m_anglew_ok;
		double	m_angle1_limit;
		double	m_angle2_limit;
		BOOL	m_chuck_ok;
		BOOL	m_insert_ok;
		double	m_insert_slide;
		double	m_jabara_recognize;
		int 	m_jabara_num;
		double	m_jabara_width_stdev;
		double	m_jabara_distance_stdev;
		double  m_jabara_gradient_stdev;
		int		m_jabara_neighbor;
		int		m_jabara_margin_of_error;
		BOOL	m_jabara_ok;
		BOOL	m_jabara_edge_ok;
		

		// etiquette
		BOOL	m_etiquette_ok;
		double  m_etiquette_recognize;
		double  m_etiquette_position;
		int		m_etiquette_size;

		double	m_minchuck;
		double	m_maxchuck;
		double	m_pok;
		double	m_pok_limit;
		BOOL	m_pok_ok;
		double	m_resol;
		int		m_slide_length;

		BOOL	m_imul1_ok;
		BOOL	m_imul_mid_ok;
		BOOL	m_imul_jabara_ok;
		BOOL	m_imul_chuck_ok;
		BOOL	m_imul_insert_ok;
		BOOL	m_imul_etq_left_ok;
		BOOL	m_imul_etq_right_ok;

		// imul
		double	m_imul_pix;
		int		m_imul_mid_pix;
		int		m_imul_jabara_pix;
		int		m_imul_chuck_pix;
		int		m_imul_insert_pix;
		int		m_imul_top_threshold;
		int		m_imul_mid_threshold;
		int		m_imul_jabara_threshold;
		int		m_imul_chuck_threshold;
		int		m_imul_insert_threshold;
		int		m_imul_top_mask;
		int		m_imul_top_mask_cols;
		int		m_imul_mid_mask;
		int		m_imul_jabara_mask;
		int		m_imul_chuck_mask;
		int		m_imul_insert_mask;
		int		m_imul_insert_pos;
		int		m_imul_etq_left_pix;
		int		m_imul_etq_left_threshold;
		int		m_imul_etq_left_mask;
		int		m_imul_etq_right_pix;
		int		m_imul_etq_right_threshold;
		int		m_imul_etq_right_mask;

		// camera
		int m_camWidth;
		int m_camHeight;
		int m_triggerPerPitch;
		int m_inspPitch;

		// 찍힘
		double m_maguri_diff_ratio;
		int m_maguri_angle_gap;
		int m_maguri_angle_continuous;
		int m_maguri_angle_diff;
		int m_mid_angle_gap;
		int m_mid_angle_continuous;
		int m_mid_angle_diff;
		int m_mid_angle_height_diff;
		int m_top_angle_gap;
		int m_top_angle_continuous;
		int m_top_angle_diff;
		int m_top_angle_height_diff;
		int m_chuck_angle_gap;
		int m_chuck_angle_continuous;
		int m_chuck_angle_diff;
		int m_chuck_angle_height_diff;
		int m_insert_angle_gap;
		int m_insert_angle_continuous;
		int m_insert_angle_diff;

		int m_maguri_max_length;
		int m_imul_chuck_mask_cols;

		double m_imul_top_persent;
		double m_imul_mid_persent;

		int m_imul_top_size;
		int m_imul_mid_size;

		int m_imul_mid_notinspect;

		int m_stabbed_insert_notinspect;
		int m_stabbed_insert_pix;
	};

	// 백업용
	BACKUP_SETTING m_backup;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectSettingDlg)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInspectSettingDlg)
	afx_msg void OnMaguriOk();
	afx_msg void OnClear();
	virtual void OnCancel();
	afx_msg void OnChuckOk();
	afx_msg void OnJabaraOk();
	afx_msg void OnJabaraEdgeOk();
	afx_msg void OnPokOk();
	afx_msg void OnInsertOk();
	afx_msg void OnAngle1();
	afx_msg void OnAngle2();
	afx_msg void OnEtiquetteOk();
	afx_msg void OnImul1Ok();
	afx_msg void OnImulMidOk();
	afx_msg void OnImulJabaraOk();
	afx_msg void OnImulChuckOk();
	afx_msg void OnImulInsertOk();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	// 찍힘 검사 버튼 관련
	BOOL m_maguri_side_ok;
	BOOL m_maguri_angle_ok;
	BOOL m_mid_angle_ok;
	BOOL m_top_angle_ok;
	BOOL m_chuck_angle_ok;
	BOOL m_insert_angle_ok;

	afx_msg void OnBnClickedMaguriSideOk();
	afx_msg void OnBnClickedMaguriAngleOk();
	afx_msg void OnBnClickedMidAngleOk();
	afx_msg void OnBnClickedTopAngleOk();
	afx_msg void OnBnClickedChuckAngleOk();
	afx_msg void OnBnClickedInsertAngleOk();
	afx_msg void OnImulEtqLeftOk();
	afx_msg void OnImulEtqRightOk();
	afx_msg void OnBnClickedReset();


	// 검사 변수들

	CEdit c_maguri_diff_ratio;
	CEdit c_maguri_angle_gap;
	CEdit c_maguri_angle_continuous;
	CEdit c_maguri_angle_diff;
	CEdit c_mid_angle_gap;
	CEdit c_mid_angle_continuous;
	CEdit c_mid_angle_diff;
	CEdit c_mid_angle_height_diff;
	CEdit c_top_angle_gap;
	CEdit c_top_angle_continuous;
	CEdit c_top_angle_diff;
	CEdit c_top_angle_height_diff;
	CEdit c_chuck_angle_gap;
	CEdit c_chuck_angle_continuous;
	CEdit c_chuck_angle_diff;
	CEdit c_chuck_angle_height_diff;
	CEdit c_insert_angle_gap;
	CEdit c_insert_angle_continuous;
	CEdit c_insert_angle_diff;

	double m_maguri_diff_ratio;
	int m_maguri_angle_gap;
	int m_maguri_angle_continuous;
	int m_maguri_angle_diff;
	int m_mid_angle_gap;
	int m_mid_angle_continuous;
	int m_mid_angle_diff;
	int m_mid_angle_height_diff;
	int m_top_angle_gap;
	int m_top_angle_continuous;
	int m_top_angle_diff;
	int m_top_angle_height_diff;
	int m_chuck_angle_gap;
	int m_chuck_angle_continuous;
	int m_chuck_angle_diff;
	int m_chuck_angle_height_diff;
	int m_insert_angle_gap;
	int m_insert_angle_continuous;
	int m_insert_angle_diff;

	
public:
	void InitCheckBox();
	void InitInspectionValues();
	CEdit c_maguri_max_length;
	CEdit c_imul_chuck_mask_cols;
	int m_imul_chuck_mask_cols;
	BOOL m_imul_etq_left_ok;
	BOOL m_imul_etq_right_ok;
	CEdit c_imul_etq_left_pix;
	CEdit c_imul_etq_left_threshold;
	CEdit c_imul_etq_left_mask;
	CEdit c_imul_etq_right_pix;
	CEdit c_imul_etq_right_threshold;
	CEdit c_imul_etq_right_mask;
	int m_imul_etq_left_pix;
	int m_imul_etq_left_threshold;
	int m_imul_etq_left_mask;
	int m_imul_etq_right_pix;
	int m_imul_etq_right_threshold;
	int m_imul_etq_right_mask;
	CEdit c_imul_top_persent;
	CEdit c_imul_mid_persent;
	CEdit c_imul_top_size;
	CEdit c_imul_mid_size;

	CEdit c_imul_mid_notinspect;


	
	CEdit c_stabbed_insert_notinspect;
	CEdit c_stabbed_insert_pix;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSPECTSETTINGDLG_H__CBF282F0_183B_41F0_8FD5_F090A249ED1E__INCLUDED_)
