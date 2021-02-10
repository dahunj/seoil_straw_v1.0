// PreInspection.cpp : implementation file
//

#include "stdafx.h"
//#include "CViewerTest.h"
#include "../SeoilVision.h"
#include "PreInspection.h"
#include "../SeoilVisionDlg.h"
#include "../InspectResultDlg.h"


#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreInspection dialog

#include "../Extern.h"

CPreInspection::CPreInspection(CWnd* pParent /*=NULL*/)
	: CDialog(CPreInspection::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreInspection)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// ¼ÂÆÃÃ¢ ¸¸µé±â

	TRACE(_T("\n\nCPreInspection\n\n"));

	m_settingdlg = new CInspectSettingDlg;

	// »óÅÂ ÃÊ±âÈ­
	m_prepared = false;
	m_DBLoaded = false;
	m_bAutoOpen = false;
	m_bViewROI01 = TRUE;
	m_bViewROI02 = TRUE;
	m_bViewROI03 = TRUE;
	m_bViewROI04 = TRUE;
	m_bViewROI05 = TRUE;

	m_bViewROIS = NULL;
}

CPreInspection::~CPreInspection()
{
	//delete m_settingdlg; // GetPicture¿¡¼­ »èÁ¦ÇÔ

	if(m_ptParam != NULL)
		delete m_ptParam;

	if(m_bViewROIS != NULL)
		delete [] m_bViewROIS;
}

BOOL CPreInspection::Create(CWnd *pParentWnd)
{
	return CDialog::Create(IDD,pParentWnd);
}

void CPreInspection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreInspection)
	DDX_Control(pDX, IDC_ANGLE_RIGHT, m_angle_right);
	DDX_Control(pDX, IDC_ANGLE_LEFT, m_angle_left);
	DDX_Control(pDX, IDC_CHUCK, m_chuck);
	DDX_Control(pDX, IDC_POK, m_pok);
	DDX_Control(pDX, IDC_MAGURI, m_maguri);
	DDX_Control(pDX, IDC_JABARA, m_jabara);
	DDX_Control(pDX, IDC_INSERT, m_insert);
	DDX_Control(pDX, IDC_ETIQUETTE, m_etiquette);

	DDX_Check(pDX, IDC_CHK_ROI01, m_bViewROI01);
	DDX_Check(pDX, IDC_CHK_ROI02, m_bViewROI02);
	DDX_Check(pDX, IDC_CHK_ROI03, m_bViewROI03);
	DDX_Check(pDX, IDC_CHK_ROI04, m_bViewROI04);
	DDX_Check(pDX, IDC_CHK_ROI05, m_bViewROI05);
	DDX_Control(pDX, IDC_CHK_ROI01, m_checkROI1);
	DDX_Control(pDX, IDC_SAVEDB, m_btnSaveDB);
	DDX_Control(pDX, IDC_DBLOAD, m_btnLoadDB);
	DDX_Control(pDX, IDC_ETQ_LEFT, m_btn_imul_etq_left);
	DDX_Control(pDX, IDC_ETQ_RIGHT, m_btn_imul_etq_right);
}


BEGIN_MESSAGE_MAP(CPreInspection, CDialog)
	//{{AFX_MSG_MAP(CPreInspection)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_LOADIMAGE, OnLoadimage)
	ON_BN_CLICKED(IDC_SET_VALUE, OnSetValue)
	ON_BN_CLICKED(IDC_SET_VALUE_PARAM, OnSetParam)
	ON_BN_CLICKED(IDC_ROI1, OnRoi1)
	ON_BN_CLICKED(IDC_ROI2, OnRoi2)
	ON_BN_CLICKED(IDC_ROI3, OnRoi3)
	ON_BN_CLICKED(IDC_ROI4, OnRoi4)
	ON_BN_CLICKED(IDC_ROI5, OnRoi5)
	ON_BN_CLICKED(IDC_ROI1_1,OnROI1_1)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_SAVEDB, OnSavedb)
	ON_BN_CLICKED(IDC_DBLOAD, OnDbload)
	ON_BN_CLICKED(IDC_MAGURI, OnMaguri)
	ON_BN_CLICKED(IDC_CHUCK, OnChuck)
	ON_BN_CLICKED(IDC_JABARA, OnJabara)
	ON_BN_CLICKED(IDC_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_POK, OnPok)
	ON_BN_CLICKED(IDC_ANGLE_LEFT, OnAngleLeft)
	ON_BN_CLICKED(IDC_ANGLE_RIGHT, OnAngleRight)

	ON_BN_CLICKED(IDC_BTN_TTEST, OnBtnTtest)
	ON_BN_CLICKED(IDC_CHK_ROI01, OnChkRoi01)
	ON_BN_CLICKED(IDC_CHK_ROI02, OnChkRoi02)
	ON_BN_CLICKED(IDC_CHK_ROI03, OnChkRoi03)
	ON_BN_CLICKED(IDC_CHK_ROI04, OnChkRoi04)
	ON_BN_CLICKED(IDC_CHK_ROI05, OnChkRoi05)
	
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ETIQUETTE, &CPreInspection::OnEtiquette)
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_BUTTON_INSP_RESULT, &CPreInspection::OnBnClickedButtonInspResult)
	ON_BN_CLICKED(IDC_ETQ_LEFT, &CPreInspection::OnBnClickedEtqLeft)
	ON_BN_CLICKED(IDC_ETQ_RIGHT, &CPreInspection::OnBnClickedEtqRight)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreInspection message handlers

BOOL CPreInspection::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// ±×¸² ¸¸µé±â
	CRect size;
	size.left = 10;
	size.top = 10;
	size.right = _ShowWidth+10;
	size.bottom =_ShowHeight+10;

	m_imageView.Create(NULL, WS_VISIBLE, size, this, this->m_nFlags);	
	
	m_sLoadImage="";

	m_ptParam = NULL;
	m_ptParam = new CINSParam;
	m_ptParam->Create(this);
	m_ptParam->ShowWindow(SW_HIDE);
	
	// ÇÇÄ¡¿¡ µû¸¥ ÅÇ »ý¼º
	m_dlgCopyBtn.Create(IDD_DLG_COPYROI, this);
	m_dlgCopyBtn.SetWindowPos(NULL, 963, 150, 210, 150, SWP_SHOWWINDOW);
	m_dlgCopyBtn.SetParent(this);
	m_dlgCopyBtn.ShowBtns();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreInspection::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	m_imageView.OnLButtonDown(nFlags,CPoint(point.x-9,point.y-9));
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CPreInspection::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_imageView.OnLButtonUp(nFlags,CPoint(point.x-9,point.y-9));
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CPreInspection::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_imageView.OnMouseMove(nFlags,CPoint(point.x-9,point.y-9));
	CString pstr;
	char line[64]; 
	int r=0, g=0, b=0;

	r= m_imageView.m_process->GVData(point.x-9, point.y-9, 2);
	g= m_imageView.m_process->GVData(point.x-9, point.y-9, 1);
	b= m_imageView.m_process->GVData(point.x-9, point.y-9, 0);

	sprintf(line,"°Ë»ç ¼³Á¤(àâïÒËþö´óÑâ¦) [%d, %d] RGB: %d, %d, %d", point.x-9, point.y-9, r, g, b);
	pstr = line;
	SetWindowText(pstr + m_sLoadImage);
	UpdateData(false);
	CDialog::OnMouseMove(nFlags, point);
}

void CPreInspection::OnLoadimage() 
{
	// TODO: Add your control notification handler code here
	LPCSTR 	lpszFilter = "Áö¿ø ¿µ»ó ÆÄÀÏ(*.bmp, *.jpg, *.png, *.tif)|*.bmp;*.jpg;*.png;*.tif|bmp files (*.bmp)|*.bmp|jpg files (*.jpg)|*.jpg|png files (*.png)|*.png|tif files (*.tif)|*.tif|";
	CFileDialog *diag = new CFileDialog(TRUE, NULL, NULL, OFN_EXPLORER | OFN_HIDEREADONLY, lpszFilter);
	
	//diag->m_ofn.lpstrInitialDir = "C:\\IMAGE"; // cwlee del

	bool result;

	if (diag->DoModal() != IDOK)
	{
		AfxMessageBox("ÆÄÀÏ ·Îµå°¡ ÀÌ·ç¾îÁöÁö ¾Ê¾Ò½À´Ï´Ù!\n(Ú±öèËÒÙþËì)");
		result = false;
		m_imageView.m_Loaded = false;
	}
	else
	{
		// ·ÎµåÇÏ±â
		CString imgFileName = diag->GetPathName();

		result = m_imageView.LoadImage(imgFileName);

		if (!result)
		{
			AfxMessageBox("ÀÌ¹ÌÁö°¡ Á¤»óÀûÀ¸·Î ·ÎµåµÇÁö ¾Ê¾Ò½À´Ï´Ù!");
			m_imageView.m_Loaded = false;
		}
		else
		{
			m_sLoadImage = " [ÇöÀç ÀÌ¹ÌÁö] - " + diag->GetFileName();
			
			for( int i = 0; i < _MAXROI; i++ )
			{
				m_imageView.m_bExistObj[i] = TRUE;
				m_dlgCopyBtn.SetAllCheckBox();
			}

			m_imageView.m_Loaded = true;
		}
	}

	m_prepared = result;

	delete diag;	// cwlee add
}

void CPreInspection::OnSetValue() 
{	
	if (m_settingdlg->DoModal() == IDOK)
	{
		// OK ÇÑ °æ¿ì¸¸
		// CancelÀÌ¸é Ã¢¿¡µµ ÀÌÀüÄ¡ µ¹·Á¹ö¸°´Ù.

		// ±¸Á¶Ã¼¿¡ º¹»ç
		m_imageView.m_inspect_ox.angle[0] = m_settingdlg->m_angle1_ok;
		m_imageView.m_inspect_ox.angle[1] = (m_settingdlg->m_anglew_ok);
		m_imageView.m_inspect_ox.chuck = m_settingdlg->m_chuck_ok;
				
		
		m_imageView.m_inspect_ox.imul = m_settingdlg->m_imul1_ok;
		m_imageView.m_inspect_ox.imul_mid = m_settingdlg->m_imul_mid_ok;
		m_imageView.m_inspect_ox.imul_jabara = m_settingdlg->m_imul_jabara_ok;
		m_imageView.m_inspect_ox.imul_chuck = m_settingdlg->m_imul_chuck_ok;
		m_imageView.m_inspect_ox.imul_insert = m_settingdlg->m_imul_insert_ok;
		m_imageView.m_inspect_ox.imul_etq_left = m_settingdlg->m_imul_etq_left_ok;
		m_imageView.m_inspect_ox.imul_etq_right = m_settingdlg->m_imul_etq_right_ok;


		m_imageView.m_inspect_ox.insert		 = m_settingdlg->m_insert_ok;
		m_imageView.m_inspect_ox.jabara		 = m_settingdlg->m_jabara_ok;
		m_imageView.m_inspect_ox.jabara_edge = m_settingdlg->m_jabara_edge_ok;
		m_imageView.m_inspect_ox.etiquette	 = m_settingdlg->m_etiquette_ok;
		m_imageView.m_inspect_ox.maguri      = m_settingdlg->m_maguri_ok;
		m_imageView.m_inspect_ox.pok	     = m_settingdlg->m_pok_ok;
		

		m_imageView.m_inspect_ox.maguri_angle = m_settingdlg->m_maguri_angle_ok;
		m_imageView.m_inspect_ox.mid_angle    = m_settingdlg->m_mid_angle_ok;
		m_imageView.m_inspect_ox.top_angle    = m_settingdlg->m_top_angle_ok;
		m_imageView.m_inspect_ox.chuck_angle  = m_settingdlg->m_chuck_angle_ok;
		m_imageView.m_inspect_ox.insert_angle = m_settingdlg->m_insert_angle_ok;
		m_imageView.m_inspect_ox.maguri_side  = m_settingdlg->m_maguri_side_ok;


		m_imageView.m_inspect_setting.pixsize = ( int )m_settingdlg->m_imul_pix;
		m_imageView.m_inspect_setting.mid_pixsize = m_settingdlg->m_imul_mid_pix;
		m_imageView.m_inspect_setting.jabara_pixsize = m_settingdlg->m_imul_jabara_pix;
		m_imageView.m_inspect_setting.chuck_pixsize = m_settingdlg->m_imul_chuck_pix;
		m_imageView.m_inspect_setting.insert_pixsize = m_settingdlg->m_imul_insert_pix;
		m_imageView.m_inspect_setting.imul_etq_left_pix = m_settingdlg->m_imul_etq_left_pix;
		m_imageView.m_inspect_setting.imul_etq_right_pix = m_settingdlg->m_imul_etq_right_pix;

		m_imageView.m_inspect_setting.imul_top_threshold = m_settingdlg->m_imul_top_threshold;
		m_imageView.m_inspect_setting.imul_mid_threshold = m_settingdlg->m_imul_mid_threshold;
		m_imageView.m_inspect_setting.imul_jabara_threshold = m_settingdlg->m_imul_jabara_threshold;
		m_imageView.m_inspect_setting.imul_chuck_threshold = m_settingdlg->m_imul_chuck_threshold;
		m_imageView.m_inspect_setting.imul_insert_threshold = m_settingdlg->m_imul_insert_threshold;
		m_imageView.m_inspect_setting.imul_etq_left_threshold = m_settingdlg->m_imul_etq_left_threshold;
		m_imageView.m_inspect_setting.imul_etq_right_threshold = m_settingdlg->m_imul_etq_right_threshold;

		m_imageView.m_inspect_setting.imul_top_mask = m_settingdlg->m_imul_top_mask;
		m_imageView.m_inspect_setting.imul_top_mask_cols = m_settingdlg->m_imul_top_mask_cols;
		m_imageView.m_inspect_setting.imul_mid_mask = m_settingdlg->m_imul_mid_mask;
		m_imageView.m_inspect_setting.imul_jabara_mask = m_settingdlg->m_imul_jabara_mask;
		m_imageView.m_inspect_setting.imul_chuck_mask = m_settingdlg->m_imul_chuck_mask;
		m_imageView.m_inspect_setting.imul_chuck_mask_cols = m_settingdlg->m_imul_chuck_mask_cols;
		m_imageView.m_inspect_setting.imul_insert_mask = m_settingdlg->m_imul_insert_mask;
		m_imageView.m_inspect_setting.imul_insert_pos = m_settingdlg->m_imul_insert_pos;
		m_imageView.m_inspect_setting.imul_etq_left_mask = m_settingdlg->m_imul_etq_left_mask;
		m_imageView.m_inspect_setting.imul_etq_right_mask = m_settingdlg->m_imul_etq_right_mask;


		// °Ë»ç ¹ë·ù º¹»ç
		m_imageView.m_inspect_setting.jabara_recognize = m_settingdlg->m_jabara_recognize;
		m_imageView.m_inspect_setting.jabara_num = m_settingdlg->m_jabara_num;
		m_imageView.m_inspect_setting.jabara_margin_of_error = m_settingdlg->m_jabara_margin_of_error;
		m_imageView.m_inspect_setting.jabara_width_stdev = m_settingdlg->m_jabara_width_stdev;
		m_imageView.m_inspect_setting.jabara_distance_stdev = m_settingdlg->m_jabara_distance_stdev;
		m_imageView.m_inspect_setting.jabara_gradient_stdev = m_settingdlg->m_jabara_gradient_stdev;
		m_imageView.m_inspect_setting.jabara_neighbor = m_settingdlg->m_jabara_neighbor;

		m_imageView.m_inspect_setting.etiquette_recognize = m_settingdlg->m_etiquette_recognize;
		m_imageView.m_inspect_setting.etiquette_position = m_settingdlg->m_etiquette_position;
		m_imageView.m_inspect_setting.etiquette_size = m_settingdlg->m_etiquette_size;

		m_imageView.m_inspect_setting.camWidth = m_settingdlg->m_camWidth;
		m_imageView.m_inspect_setting.camHeight = m_settingdlg->m_camHeight;
		m_imageView.m_inspect_setting.triggerPerPitch = m_settingdlg->m_triggerPerPitch;
		m_imageView.m_inspect_setting.inspPitch = m_settingdlg->m_inspPitch;

		m_imageView.m_inspect_setting.m_Magunus_Slide = m_settingdlg->m_maguri_slide;
		m_imageView.m_inspect_setting.m_Magunus_Slide2 = m_settingdlg->m_maguri_slide;
		m_imageView.m_inspect_setting.m_Magunus_SlideLength = m_settingdlg->m_maguri_length;
		m_imageView.m_inspect_setting.m_maguri_max_length = m_settingdlg->m_maguri_max_length;

		m_imageView.m_inspect_setting.m_minChuck = m_settingdlg->m_minchuck;
		m_imageView.m_inspect_setting.m_maxChuck = m_settingdlg->m_maxchuck;
		m_imageView.m_inspect_setting.m_pok_limit = m_settingdlg->m_pok_limit;
		m_imageView.m_inspect_setting.m_Slide =m_settingdlg->m_insert_slide;
		m_imageView.m_inspect_setting.m_SlideLength = m_settingdlg->m_slide_length;
		
		// Ãß°¡µÈ °Íµé
		m_imageView.m_inspect_setting.angle_diff = m_settingdlg->m_angle_diff;
		m_imageView.m_inspect_setting.m_pok = m_settingdlg->m_pok;
		m_imageView.m_inspect_setting.resol = m_settingdlg->m_resol;

		// °¢
		m_imageView.m_inspect_setting.angle_limit[0] = m_settingdlg->m_angle1_limit;
		m_imageView.m_inspect_setting.angle_limit[1] = m_settingdlg->m_angle2_limit;

		



		m_imageView.m_inspect_setting.m_maguri_diff_ratio = m_settingdlg->m_maguri_diff_ratio;
		m_imageView.m_inspect_setting.m_maguri_angle_gap = m_settingdlg->m_maguri_angle_gap;
		m_imageView.m_inspect_setting.m_maguri_angle_continuous = m_settingdlg->m_maguri_angle_continuous;
		m_imageView.m_inspect_setting.m_maguri_angle_diff = m_settingdlg->m_maguri_angle_diff;
		m_imageView.m_inspect_setting.m_mid_angle_gap = m_settingdlg->m_mid_angle_gap;
		m_imageView.m_inspect_setting.m_mid_angle_continuous = m_settingdlg->m_mid_angle_continuous;
		m_imageView.m_inspect_setting.m_mid_angle_diff = m_settingdlg->m_mid_angle_diff;
		m_imageView.m_inspect_setting.m_mid_angle_height_diff = m_settingdlg->m_mid_angle_height_diff;
		m_imageView.m_inspect_setting.m_top_angle_gap = m_settingdlg->m_top_angle_gap;
		m_imageView.m_inspect_setting.m_top_angle_continuous = m_settingdlg->m_top_angle_continuous;
		m_imageView.m_inspect_setting.m_top_angle_diff = m_settingdlg->m_top_angle_diff;
		m_imageView.m_inspect_setting.m_top_angle_height_diff = m_settingdlg->m_top_angle_height_diff;
		m_imageView.m_inspect_setting.m_chuck_angle_gap = m_settingdlg->m_chuck_angle_gap;
		m_imageView.m_inspect_setting.m_chuck_angle_continuous = m_settingdlg->m_chuck_angle_continuous;
		m_imageView.m_inspect_setting.m_chuck_angle_diff = m_settingdlg->m_chuck_angle_diff;
		m_imageView.m_inspect_setting.m_chuck_angle_height_diff = m_settingdlg->m_chuck_angle_height_diff;
		m_imageView.m_inspect_setting.m_insert_angle_gap = m_settingdlg->m_insert_angle_gap;
		m_imageView.m_inspect_setting.m_insert_angle_continuous = m_settingdlg->m_insert_angle_continuous;
		m_imageView.m_inspect_setting.m_insert_angle_diff = m_settingdlg->m_insert_angle_diff;

		m_imageView.m_inspect_setting.imul_top_persent = m_settingdlg->m_imul_top_persent;
		m_imageView.m_inspect_setting.imul_mid_persent = m_settingdlg->m_imul_mid_persent;
		m_imageView.m_inspect_setting.imul_top_size = m_settingdlg->m_imul_top_size;
		m_imageView.m_inspect_setting.imul_mid_size = m_settingdlg->m_imul_mid_size;

		m_imageView.m_inspect_setting.imul_mid_notinspect = m_settingdlg->m_imul_mid_notinspect;

		m_imageView.m_inspect_setting.stabbed_insert_notinspect = m_settingdlg->m_stabbed_insert_notinspect;
		m_imageView.m_inspect_setting.stabbed_insert_pix = m_settingdlg->m_stabbed_insert_pix;

		// ¹öÆ° Enable ÇØ ÁÖ±â
		m_maguri.EnableWindow(m_settingdlg->m_maguri_ok);
		m_chuck.EnableWindow(m_settingdlg->m_chuck_ok);
		m_insert.EnableWindow(m_settingdlg->m_insert_ok);
		m_pok.EnableWindow(m_settingdlg->m_pok_ok);
		m_angle_left.EnableWindow(m_settingdlg->m_angle1_ok);
		m_angle_right.EnableWindow(m_settingdlg->m_anglew_ok);
		m_jabara.EnableWindow(m_settingdlg->m_jabara_ok);
		m_etiquette.EnableWindow(m_settingdlg->m_etiquette_ok);
		m_btn_imul_etq_left.EnableWindow(m_settingdlg->m_imul_etq_left_ok);
		m_btn_imul_etq_right.EnableWindow(m_settingdlg->m_imul_etq_right_ok);

		m_imageView.Invalidate(FALSE);

	}
}

void CPreInspection::OnRoi1() 
{
	// TODO: Add your control notification handler code here
	// 1¹ø ROi¸¦ ¼³Á¤ÇÔ

	AfxMessageBox("1¹ø »¡´ë ¿µ¿ªÀ» µå·¡±× & µå·ÓÀ¸·Î ±×·ÁÁÖ¼¼¿ä!\n(÷êð¯ìéËþö´ÖÅæ´öéÛ¯ÓðÜÖð¤êÈöÇ)");
	m_imageView.SetROI(0);
	m_imageView.SetROIS(1);

}

void CPreInspection::OnRoi2() 
{
	// TODO: Add your control notification handler code here
	
	// 2¹ø ROi¸¦ ¼³Á¤ÇÔ
	AfxMessageBox("2¹ø »¡´ë ¿µ¿ªÀ» µå·¡±× & µå·ÓÀ¸·Î º¹»çÇÒ °÷À» Á¤ÇØ ÁÖ¼¼¿ä!\n(ð¯ì£ýåÎ·ÖÅæ´öéÛ¯Óðê«ëòì¹ÔÑîÜÖÅæ´)");
	m_imageView.CopyROI(1);
}

void CPreInspection::OnRoi3() 
{
	// TODO: Add your control notification handler code here
	// 3¹ø º¹»ç
	if (m_imageView.m_ROIS <2)
	{
		AfxMessageBox("¸ÕÀú 2¹ø ¿µ¿ª±îÁö ¼³Á¤ÇØÁÖ¼¼¿ä!(ôëà»àâïÒð¯ì£ÖÅæ´)");
		return;
	}

	AfxMessageBox("3¹ø »¡´ë ¿µ¿ªÀ» µå·¡±× & µå·ÓÀ¸·Î º¹»çÇÒ °÷À» Á¤ÇØ ÁÖ¼¼¿ä!(ôë÷êð¯ß²ýåÎ·ÖÅæ´öéÛ¯ÓðÜÖð¤êÈöÇ)");
	m_imageView.CopyROI(2);
}

void CPreInspection::OnRoi4() 
{
	// TODO: Add your control notification handler code here
	// 3¹ø º¹»ç
	if (m_imageView.m_ROIS <3)
	{
		AfxMessageBox("¸ÕÀú 3¹ø ¿µ¿ª±îÁö ¼³Á¤ÇØÁÖ¼¼¿ä!(ôëà»àâïÒð¯ß²ÖÅæ´)");
		return;
	}

	AfxMessageBox("4¹ø »¡´ë ¿µ¿ªÀ» µå·¡±× & µå·ÓÀ¸·Î º¹»çÇÒ °÷À» Á¤ÇØ ÁÖ¼¼¿ä!(ôë÷êð¯ÞÌýåÎ·ÖÅæ´öéÛ¯ÓðÜÖð¤êÈöÇ)");
	m_imageView.CopyROI(3);
}

void CPreInspection::OnRoi5() 
{
	// TODO: Add your control notification handler code here
	// 4¹ø º¹»ç
	if (m_imageView.m_ROIS <4)
	{
		AfxMessageBox("¸ÕÀú 4¹ø ¿µ¿ª±îÁö ¼³Á¤ÇØÁÖ¼¼¿ä!(ôëà»àâïÒð¯ÞÌÖÅæ´)");
		return;
	}

	AfxMessageBox("5¹ø »¡´ë ¿µ¿ªÀ» µå·¡±× & µå·ÓÀ¸·Î º¹»çÇÒ °÷À» Á¤ÇØ ÁÖ¼¼¿ä!(ôë÷êð¯çéýåÎ·ÖÅæ´öéÛ¯ÓðÜÖð¤êÈöÇ)");
	m_imageView.CopyROI(4);
}

void CPreInspection::OnRoi6() 
{
	// TODO: Add your control notification handler code here
	// 5¹ø º¹»ç
	if (m_imageView.m_ROIS < 5)
	{
		AfxMessageBox("¸ÕÀú 5¹ø ¿µ¿ª±îÁö ¼³Á¤ÇØÁÖ¼¼¿ä!(ôëà»àâïÒð¯çéÖÅæ´)");
		return;
	}

	AfxMessageBox("6¹ø »¡´ë ¿µ¿ªÀ» µå·¡±× & µå·ÓÀ¸·Î º¹»çÇÒ °÷À» Á¤ÇØ ÁÖ¼¼¿ä!(ôë÷êð¯ë»ýåÎ·ÖÅæ´öéÛ¯ÓðÜÖð¤êÈöÇ)");
	m_imageView.CopyROI(5);
}


void CPreInspection::OnRoi7() 
{
	// TODO: Add your control notification handler code here
	// 6¹ø º¹»ç
	if (m_imageView.m_ROIS < 6)
	{
		AfxMessageBox("¸ÕÀú 6¹ø ¿µ¿ª±îÁö ¼³Á¤ÇØÁÖ¼¼¿ä!(ôëà»àâïÒð¯ë»ÖÅæ´)");
		return;
	}

	AfxMessageBox("7¹ø »¡´ë ¿µ¿ªÀ» µå·¡±× & µå·ÓÀ¸·Î º¹»çÇÒ °÷À» Á¤ÇØ ÁÖ¼¼¿ä!(ôë÷êð¯öÒýåÎ·ÖÅæ´öéÛ¯ÓðÜÖð¤êÈöÇ)");
	m_imageView.CopyROI(6);
}

void CPreInspection::OnRoi8() 
{
	// TODO: Add your control notification handler code here
	// 7¹ø º¹»ç
	if (m_imageView.m_ROIS < 7)
	{
		AfxMessageBox("¸ÕÀú 7¹ø ¿µ¿ª±îÁö ¼³Á¤ÇØÁÖ¼¼¿ä!(ôëà»àâïÒð¯öÒÖÅæ´)");
		return;
	}

	AfxMessageBox("8¹ø »¡´ë ¿µ¿ªÀ» µå·¡±× & µå·ÓÀ¸·Î º¹»çÇÒ °÷À» Á¤ÇØ ÁÖ¼¼¿ä!(ôë÷êð¯ø¢ýåÎ·ÖÅæ´öéÛ¯ÓðÜÖð¤êÈöÇ)");
	m_imageView.CopyROI(7);
}


void CPreInspection::OnRoi9() 
{
	// TODO: Add your control notification handler code here
	// 8¹ø º¹»ç
	if (m_imageView.m_ROIS < 8)
	{
		AfxMessageBox("¸ÕÀú 8¹ø ¿µ¿ª±îÁö ¼³Á¤ÇØÁÖ¼¼¿ä!(ôëà»àâïÒð¯ø¢ÖÅæ´)");
		return;
	}

	AfxMessageBox("9¹ø »¡´ë ¿µ¿ªÀ» µå·¡±× & µå·ÓÀ¸·Î º¹»çÇÒ °÷À» Á¤ÇØ ÁÖ¼¼¿ä!(ôë÷êð¯ÎúýåÎ·ÖÅæ´öéÛ¯ÓðÜÖð¤êÈöÇ)");
	m_imageView.CopyROI(8);
}


void CPreInspection::OnROI1_1()
{
	if (m_imageView.m_ROIS <0)
	{
		AfxMessageBox("¸ÕÀú 1¹ø ¿µ¿ªÀ» ¼³Á¤ÇØÁÖ¼¼¿ä!(ôëà»àâïÒð¯ìéÖÅæ´)");
		return;
	}

	AfxMessageBox("1¹ø »¡´ë ¿µ¿ªÀ» µå·¡±× & µå·ÓÀ¸·Î º¹»çÇÒ °÷À» Á¤ÇØ ÁÖ¼¼¿ä!(ôë÷êð¯ìéýåÎ·ÖÅæ´öéÛ¯ÓðÜÖð¤êÈöÇ)");
	m_imageView.CopyROI2(10);
}

void CPreInspection::OnTest() 
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	// TODO: Add your control notification handler code here
	// doinspect
	// ÀÌ¹ÌÁö ¼ÂÆÃ
	m_imageView.m_process->SetImageSize(_CamWidth,_CamHeight);

	// °Ë»ç ÇÏ±â

	// thread ÇÏ³ª´ç 1 ÀÌ¹ÌÁö Ã³¸®
	// thread ÇÏ³ª´ç 2 ÀÌ¹ÌÁö Ã³¸®

	LARGE_INTEGER startingTime, endingTime, elapsedMilliseconds;
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startingTime);

	m_imageView.DoInspection();

	QueryPerformanceCounter(&endingTime);
	elapsedMilliseconds.QuadPart = endingTime.QuadPart - startingTime.QuadPart;

	elapsedMilliseconds.QuadPart *= 1000;
	elapsedMilliseconds.QuadPart /= frequency.QuadPart;

	TRACE(_T("\n\n//----------------\nElapsed Time: %d ms\n//----------------\n\n"), elapsedMilliseconds.QuadPart);
	
}

void CPreInspection::OnSavedb() 
{
	if(_MAXROI <= 0)
		return;

	LPCSTR 	lpszFilter = "°Ë»ç µ¥ÀÌÅÍ ÆÄÀÏ(*.csv)|*.csv";

	CFileDialog *diag = new CFileDialog(FALSE, "csv", NULL, OFN_EXPLORER | OFN_HIDEREADONLY| OFN_OVERWRITEPROMPT, lpszFilter);
	FILE *file;

	if (diag->DoModal() == IDOK)
	{
		// ÆÄÀÏ ¿­°Ô µÊ
		CString ksrFileName = diag->GetPathName();

		//CString ksrFileName = diag->GetFolderPath(); // cwlee add
		//ksrFileName += (_T("\\") + diag->GetFileName()); // cwlee add
		
		file = fopen(ksrFileName,"w");
		if (file == NULL)
		{
			// ¿¡·¯ ¹ß»ý
			AfxMessageBox("ÆÄÀÏÀ» ¿©´Â Áß ¹®Á¦°¡ ¹ß»ýÇß½À´Ï´Ù!");
			return;
		}

		// ÀúÀå ¼ø¼­
		// 1. °Ë»çÄ¡
		// 2. °Ë»ç ¿©ºÎ
		// 3. 1¹ø ROI ³»ºÎ °Ë»ç ÁÂÇ¥
		// 4. ROI °³¼ö
		// 5. »¡´ë ¿Ü°û ROI
		// 6. »¡´ë ³»ºÎ ROI
		
		m_imageView.m_inspect_ox.angle[0] = FALSE;
		m_imageView.m_inspect_ox.angle[1] = FALSE;
		m_imageView.m_inspect_ox.pok = FALSE;
		m_imageView.m_inspect_ox.etiquette = FALSE;

		m_imageView.m_ROIS = _MAXROI;

		//INSPECT_SETTING
		//fwrite(&m_imageView.m_inspect_setting, sizeof(m_imageView.m_inspect_setting), 1, file);
		
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.pixsize);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.mid_pixsize);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.jabara_pixsize);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.chuck_pixsize);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.insert_pixsize);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_etq_left_pix);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_etq_right_pix);

		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_top_threshold);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_mid_threshold);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_jabara_threshold);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_chuck_threshold);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_insert_threshold);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_etq_left_threshold);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_etq_right_threshold);

		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_top_mask);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_top_mask_cols);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_mid_mask);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_jabara_mask);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_chuck_mask);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_chuck_mask_cols);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_insert_mask);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_insert_pos);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_etq_left_mask);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_etq_right_mask);

		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.jabara_recognize);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.jabara_num);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.jabara_margin_of_error);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.jabara_width_stdev);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.jabara_distance_stdev);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.jabara_gradient_stdev);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.jabara_neighbor);

		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.etiquette_recognize);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.etiquette_position);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.etiquette_size);

		fprintf(file, "%d\n", m_imageView.m_inspect_setting.camWidth);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.camHeight);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.triggerPerPitch);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.inspPitch);

		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.m_Magunus_Slide);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.m_Magunus_Slide2);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_Magunus_SlideLength);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_maguri_max_length);

		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.m_minChuck);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.m_maxChuck);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.m_pok_limit);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.m_Slide);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_SlideLength);

		fprintf(file, "%d\n", m_imageView.m_inspect_setting.angle_diff);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_pok);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.resol);

		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.angle_limit[0]);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.angle_limit[1]);

		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.m_maguri_diff_ratio);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_maguri_angle_gap);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_maguri_angle_continuous);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_maguri_angle_diff);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_mid_angle_gap);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_mid_angle_continuous);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_mid_angle_diff);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_mid_angle_height_diff);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_top_angle_gap);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_top_angle_continuous);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_top_angle_diff);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_top_angle_height_diff);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_chuck_angle_gap);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_chuck_angle_continuous);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_chuck_angle_diff);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_chuck_angle_height_diff);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_insert_angle_gap);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_insert_angle_continuous);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.m_insert_angle_diff);
		
		//RESULT_INSPECT, m_inspect_ox
		//fwrite(&m_imageView.m_inspect_ox, sizeof(m_imageView.m_inspect_ox), 1, file);
		
		fprintf(file, "%lf\n",  m_imageView.m_inspect_ox.angle[0]);
		fprintf(file, "%lf\n",  m_imageView.m_inspect_ox.angle[1]);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.chuck);

		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.imul);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.imul_mid);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.imul_jabara);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.imul_chuck);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.imul_insert);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.imul_etq_left);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.imul_etq_right);

		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.insert);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.jabara);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.jabara_edge);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.etiquette);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.maguri);
		fprintf(file, "%lf\n",  m_imageView.m_inspect_ox.pok);

		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.maguri_angle);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.mid_angle);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.top_angle);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.chuck_angle);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.insert_angle);
		fprintf(file, "%d\n",  m_imageView.m_inspect_ox.maguri_side);
		
		//fprintf(file, "%d\n",  );

 		
		//RESULT_INSPECT, m_inspect_offset
		//fwrite(&m_imageView.m_inspect_offset, sizeof(m_imageView.m_inspect_offset), 1, file);
		
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.pok[0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.pok[1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.pok[0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.pok[1].y);

		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.maguri[0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.maguri[1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.maguri[0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.maguri[1].y);

		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.etiquette[0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.etiquette[1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.etiquette[0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.etiquette[1].y);
		
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.jabara[0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.jabara[1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.jabara[0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.jabara[1].y);

		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.chuck[0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.chuck[1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.chuck[0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.chuck[1].y);
		
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.insert[0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.insert[1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.insert[0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.insert[1].y);

		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.angle[0][0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.angle[0][1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.angle[0][0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.angle[0][1].y);
		
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.angle[1][0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.angle[1][1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.angle[1][0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.angle[1][1].y);

		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul_etq_left[0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul_etq_left[1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul_etq_left[0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul_etq_left[1].y);

		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul_etq_right[0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul_etq_right[1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul_etq_right[0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul_etq_right[1].y);

		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul[0].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul[1].x);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul[0].y);
		fprintf(file, "%d\n",  m_imageView.m_inspect_offset.imul[1].y);
		
		//fwrite(&m_imageView.m_ROIS, sizeof(int), 1, file);
		fprintf(file, "%d\n",  m_imageView.m_ROIS);
		
		for(int i = 0; i < _MAXROI; i++)
		{
			//fwrite(&m_imageView.m_ROI[i][0], sizeof(CPoint), 1, file);
			fprintf(file, "%d\n",  m_imageView.m_ROI[i][0].x);
			fprintf(file, "%d\n",  m_imageView.m_ROI[i][0].y);
			//fwrite(&m_imageView.m_ROI[i][1], sizeof(CPoint), 1, file);
			fprintf(file, "%d\n",  m_imageView.m_ROI[i][1].x);
			fprintf(file, "%d\n",  m_imageView.m_ROI[i][1].y);

		}
		
		for(int i = 0; i < _MAXROI; i++)
		{
			//fwrite(&m_imageView.m_inspect_ROI[i],sizeof(m_imageView.m_inspect_ROI[i]),1,file);
			
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].pok[0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].pok[1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].pok[0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].pok[1].y);

			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].maguri[0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].maguri[1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].maguri[0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].maguri[1].y);

			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].etiquette[0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].etiquette[1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].etiquette[0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].etiquette[1].y);

			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].jabara[0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].jabara[1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].jabara[0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].jabara[1].y);

			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].chuck[0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].chuck[1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].chuck[0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].chuck[1].y);

			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].insert[0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].insert[1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].insert[0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].insert[1].y);

			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].angle[0][0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].angle[0][1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].angle[0][0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].angle[0][1].y);

			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].angle[1][0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].angle[1][1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].angle[1][0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].angle[1][1].y);

			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul_etq_left[0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul_etq_left[1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul_etq_left[0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul_etq_left[1].y);

			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul_etq_right[0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul_etq_right[1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul_etq_right[0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul_etq_right[1].y);

			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul[0].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul[1].x);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul[0].y);
			fprintf(file, "%d\n",  m_imageView.m_inspect_ROI[i].imul[1].y);
		}

		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.imul_top_persent);
		fprintf(file, "%lf\n", m_imageView.m_inspect_setting.imul_mid_persent);

		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_top_size);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_mid_size);

		fprintf(file, "%d\n", m_imageView.m_inspect_setting.imul_mid_notinspect);

		fprintf(file, "%d\n", m_imageView.m_inspect_setting.stabbed_insert_notinspect);
		fprintf(file, "%d\n", m_imageView.m_inspect_setting.stabbed_insert_pix);

		fclose(file);

		m_DBLoaded = true;
	}
	delete diag;
}

void CPreInspection::OnDbload() 
{
	// TODO: Add your control notification handler code her
	LPCSTR 	lpszFilter = "°Ë»ç µ¥ÀÌÅÍ ÆÄÀÏ(*.csv)|*.csv";
	CFileDialog diag(TRUE,NULL,NULL,OFN_EXPLORER|OFN_HIDEREADONLY,lpszFilter);

	if(m_bAutoOpen == false)
	{
		m_MainPt->OnFileOpen();
		return;
		//diag = new CFileDialog(TRUE, "ksr", NULL, OFN_EXPLORER | OFN_HIDEREADONLY, lpszFilter);
	}

	FILE *file;

	if ( m_bAutoOpen == TRUE || diag.DoModal() == IDOK )
	{
		if ( m_bAutoOpen == TRUE )
		{
			file = fopen( m_sAutoOpenPath, "r" );
		}
		else
		{
			CString ksrFileName = diag.GetFolderPath();
			ksrFileName += ( _T("\\") + diag.GetFileName() );

			file = fopen( ksrFileName, _T("r") ); // cwlee add
		}
		if ( file == NULL )
		{
			// ¿¡·¯ ¹ß»ý
			AfxMessageBox( _T("ÆÄÀÏÀ» ¿©´Â Áß ¹®Á¦°¡ ¹ß»ýÇß½À´Ï´Ù!") );

			return;
		}
	
		// ·Îµå ¼ø¼­
		// 1. °Ë»çÄ¡
		// 2. °Ë»ç ¿©ºÎ
		// 3. 1¹ø ROI ³»ºÎ °Ë»ç ÁÂÇ¥
		// 4. ROI °³¼ö
		// 5. »¡´ë ¿Ü°û ROI
		// 6. »¡´ë ³»ºÎ ROI

		// ·ÎµåÇÒ ¶§´Â ÆÄÀÏ Å©±â¸¦ °í·ÁÇÏÀÚ.
		// Å©±â°¡ ÀÛÀ» °æ¿ì Ãß°¡·Î ÀÐÀ¸·Á ÇÒ ¶§ ¿¡·¯°¡ ³¯ ¼ö ÀÖÀ½
		// ¾ÆÁ÷Àº ¾øÀ¸´Ï ÆÐ½º....

		//fread(&m_imageView.m_inspect_setting, sizeof(m_imageView.m_inspect_setting),1,file);
		
		fscanf(file, "%d", &m_imageView.m_inspect_setting.pixsize);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.mid_pixsize);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.jabara_pixsize);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.chuck_pixsize);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.insert_pixsize);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_etq_left_pix);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_etq_right_pix);

		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_top_threshold);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_mid_threshold);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_jabara_threshold);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_chuck_threshold);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_insert_threshold);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_etq_left_threshold);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_etq_right_threshold);

		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_top_mask);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_top_mask_cols);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_mid_mask);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_jabara_mask);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_chuck_mask);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_chuck_mask_cols);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_insert_mask);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_insert_pos);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_etq_left_mask);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_etq_right_mask);

		fscanf(file, "%lf", &m_imageView.m_inspect_setting.jabara_recognize);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.jabara_num);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.jabara_margin_of_error);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.jabara_width_stdev);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.jabara_distance_stdev);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.jabara_gradient_stdev);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.jabara_neighbor);

		fscanf(file, "%lf", &m_imageView.m_inspect_setting.etiquette_recognize);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.etiquette_position);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.etiquette_size);

		fscanf(file, "%d", &m_imageView.m_inspect_setting.camWidth);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.camHeight);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.triggerPerPitch);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.inspPitch);

		fscanf(file, "%lf", &m_imageView.m_inspect_setting.m_Magunus_Slide);///
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.m_Magunus_Slide2);////
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_Magunus_SlideLength);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_maguri_max_length);

		fscanf(file, "%lf", &m_imageView.m_inspect_setting.m_minChuck);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.m_maxChuck);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.m_pok_limit);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.m_Slide);//
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_SlideLength);

		fscanf(file, "%lf", &m_imageView.m_inspect_setting.angle_diff);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.m_pok);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.resol);///

		fscanf(file, "%lf", &m_imageView.m_inspect_setting.angle_limit[0]);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.angle_limit[1]);

		fscanf(file, "%lf", &m_imageView.m_inspect_setting.m_maguri_diff_ratio);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_maguri_angle_gap);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_maguri_angle_continuous);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_maguri_angle_diff);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_mid_angle_gap);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_mid_angle_continuous);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_mid_angle_diff);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_mid_angle_height_diff);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_top_angle_gap);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_top_angle_continuous);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_top_angle_diff);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_top_angle_height_diff);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_chuck_angle_gap);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_chuck_angle_continuous);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_chuck_angle_diff);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_chuck_angle_height_diff);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_insert_angle_gap);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_insert_angle_continuous);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.m_insert_angle_diff);
		
		
		_MAXROI = m_imageView.m_inspect_setting.triggerPerPitch * m_imageView.m_inspect_setting.inspPitch;
		
		m_MainPt->SetGlobalVariable(&m_imageView.m_inspect_setting);

		//fread(&m_imageView.m_inspect_ox, sizeof(m_imageView.m_inspect_ox), 1, file);
		
		fscanf(file, "%lf",  &m_imageView.m_inspect_ox.angle[0]);
		fscanf(file, "%lf",  &m_imageView.m_inspect_ox.angle[1]);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.chuck);

		fscanf(file, "%d",  &m_imageView.m_inspect_ox.imul);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.imul_mid);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.imul_jabara);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.imul_chuck);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.imul_insert);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.imul_etq_left);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.imul_etq_right);

		fscanf(file, "%d",  &m_imageView.m_inspect_ox.insert);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.jabara);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.jabara_edge);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.etiquette);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.maguri);
		fscanf(file, "%lf",  &m_imageView.m_inspect_ox.pok);

		fscanf(file, "%d",  &m_imageView.m_inspect_ox.maguri_angle);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.mid_angle);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.top_angle);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.chuck_angle);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.insert_angle);
		fscanf(file, "%d",  &m_imageView.m_inspect_ox.maguri_side);
		

		//fread(&m_imageView.m_inspect_offset, sizeof(m_imageView.m_inspect_offset), 1, file);
		
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.pok[0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.pok[1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.pok[0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.pok[1].y);

		fscanf(file, "%d",  &m_imageView.m_inspect_offset.maguri[0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.maguri[1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.maguri[0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.maguri[1].y);

		fscanf(file, "%d",  &m_imageView.m_inspect_offset.etiquette[0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.etiquette[1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.etiquette[0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.etiquette[1].y);
		
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.jabara[0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.jabara[1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.jabara[0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.jabara[1].y);

		fscanf(file, "%d",  &m_imageView.m_inspect_offset.chuck[0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.chuck[1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.chuck[0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.chuck[1].y);
		
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.insert[0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.insert[1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.insert[0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.insert[1].y);

		fscanf(file, "%d",  &m_imageView.m_inspect_offset.angle[0][0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.angle[0][1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.angle[0][0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.angle[0][1].y);
		
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.angle[1][0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.angle[1][1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.angle[1][0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.angle[1][1].y);

		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul_etq_left[0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul_etq_left[1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul_etq_left[0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul_etq_left[1].y);

		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul_etq_right[0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul_etq_right[1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul_etq_right[0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul_etq_right[1].y);

		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul[0].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul[1].x);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul[0].y);
		fscanf(file, "%d",  &m_imageView.m_inspect_offset.imul[1].y);
		

		//fread(&m_imageView.m_ROIS, sizeof(int), 1, file);
		fscanf(file, "%d", &m_imageView.m_ROIS);
		
		m_imageView.m_inspect_ox.angle[0] = FALSE;
		m_imageView.m_inspect_ox.angle[1] = FALSE;
		m_imageView.m_inspect_ox.pok = FALSE;

		for ( int i = 0; i < _MAXROI; i++ )
		{
			if ( feof( file ) != 0 )
			{
				break;
			}

			//fread(&m_imageView.m_ROI[i][0], sizeof(CPoint), 1, file);
			fscanf(file, "%d",  &m_imageView.m_ROI[i][0].x);
			fscanf(file, "%d",  &m_imageView.m_ROI[i][0].y);
			//fread(&m_imageView.m_ROI[i][1], sizeof(CPoint), 1, file);
			fscanf(file, "%d",  &m_imageView.m_ROI[i][1].x);
			fscanf(file, "%d",  &m_imageView.m_ROI[i][1].y);
		}

		for ( int i = 0; i < _MAXROI; i++ )
		{
			if ( feof( file ) != 0 )
			{
				break;
			}

			//fread( &m_imageView.m_inspect_ROI[i], sizeof( m_imageView.m_inspect_ROI[i] ), 1, file );
			
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].pok[0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].pok[1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].pok[0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].pok[1].y);

			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].maguri[0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].maguri[1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].maguri[0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].maguri[1].y);

			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].etiquette[0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].etiquette[1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].etiquette[0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].etiquette[1].y);

			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].jabara[0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].jabara[1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].jabara[0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].jabara[1].y);

			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].chuck[0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].chuck[1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].chuck[0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].chuck[1].y);

			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].insert[0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].insert[1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].insert[0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].insert[1].y);

			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].angle[0][0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].angle[0][1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].angle[0][0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].angle[0][1].y);

			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].angle[1][0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].angle[1][1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].angle[1][0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].angle[1][1].y);

			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul_etq_left[0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul_etq_left[1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul_etq_left[0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul_etq_left[1].y);

			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul_etq_right[0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul_etq_right[1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul_etq_right[0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul_etq_right[1].y);

			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul[0].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul[1].x);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul[0].y);
			fscanf(file, "%d",  &m_imageView.m_inspect_ROI[i].imul[1].y);
			
		}

		fscanf(file, "%lf", &m_imageView.m_inspect_setting.imul_top_persent);
		fscanf(file, "%lf", &m_imageView.m_inspect_setting.imul_mid_persent);

		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_top_size);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_mid_size);

		fscanf(file, "%d", &m_imageView.m_inspect_setting.imul_mid_notinspect);

		fscanf(file, "%d", &m_imageView.m_inspect_setting.stabbed_insert_notinspect);
		fscanf(file, "%d", &m_imageView.m_inspect_setting.stabbed_insert_pix);

		// 1¹øÀÇ ±â¹Ý Á¤º¸·Î º¹±¸
		for ( int num = 0; num < _MAXROI; num++ )
		{

		// Æø	
			m_imageView.m_inspect_ROI[num].pok[0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.pok[0].x;
			m_imageView.m_inspect_ROI[num].pok[1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.pok[1].x;
			m_imageView.m_inspect_ROI[num].pok[0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.pok[0].y;
			m_imageView.m_inspect_ROI[num].pok[1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.pok[1].y;

		

		// ¸¶±¸¸®
			m_imageView.m_inspect_ROI[num].maguri[0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.maguri[0].x;
			m_imageView.m_inspect_ROI[num].maguri[1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.maguri[1].x;
			m_imageView.m_inspect_ROI[num].maguri[0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.maguri[0].y;
			m_imageView.m_inspect_ROI[num].maguri[1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.maguri[1].y;


		// ¿¡Æ¼ÄÏ
			m_imageView.m_inspect_ROI[num].etiquette[0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.etiquette[0].x;
			m_imageView.m_inspect_ROI[num].etiquette[1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.etiquette[1].x;
			m_imageView.m_inspect_ROI[num].etiquette[0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.etiquette[0].y;
			m_imageView.m_inspect_ROI[num].etiquette[1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.etiquette[1].y;

		

		// ÀÚ¹Ù¶ó
			m_imageView.m_inspect_ROI[num].jabara[0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.jabara[0].x;
			m_imageView.m_inspect_ROI[num].jabara[1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.jabara[1].x;
			m_imageView.m_inspect_ROI[num].jabara[0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.jabara[0].y;
			m_imageView.m_inspect_ROI[num].jabara[1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.jabara[1].y;

		

		// Ãà°ü
			m_imageView.m_inspect_ROI[num].chuck[0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.chuck[0].x;
			m_imageView.m_inspect_ROI[num].chuck[1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.chuck[1].x;
			m_imageView.m_inspect_ROI[num].chuck[0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.chuck[0].y;
			m_imageView.m_inspect_ROI[num].chuck[1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.chuck[1].y;


		
		
		// ¿ª»ð
			m_imageView.m_inspect_ROI[num].insert[0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.insert[0].x;
			m_imageView.m_inspect_ROI[num].insert[1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.insert[1].x;
			m_imageView.m_inspect_ROI[num].insert[0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.insert[0].y;
			m_imageView.m_inspect_ROI[num].insert[1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.insert[1].y;


		

		// ÁÂ°¢
			m_imageView.m_inspect_ROI[num].angle[0][0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.angle[0][0].x;
			m_imageView.m_inspect_ROI[num].angle[0][1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.angle[0][1].x;
			m_imageView.m_inspect_ROI[num].angle[0][0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.angle[0][0].y;
			m_imageView.m_inspect_ROI[num].angle[0][1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.angle[0][1].y;


		

		// ¿ì°¢
			m_imageView.m_inspect_ROI[num].angle[1][0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.angle[1][0].x;
			m_imageView.m_inspect_ROI[num].angle[1][1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.angle[1][1].x;
			m_imageView.m_inspect_ROI[num].angle[1][0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.angle[1][0].y;
			m_imageView.m_inspect_ROI[num].angle[1][1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.angle[1][1].y;



		// ÁÂÃø ¿¡Æ¼ÄÏ ÀÌ¹°
			m_imageView.m_inspect_ROI[num].imul_etq_left[0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.imul_etq_left[0].x;
			m_imageView.m_inspect_ROI[num].imul_etq_left[1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.imul_etq_left[1].x;
			m_imageView.m_inspect_ROI[num].imul_etq_left[0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.imul_etq_left[0].y;
			m_imageView.m_inspect_ROI[num].imul_etq_left[1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.imul_etq_left[1].y;


		// ¿ìÃø ¿¡Æ¼ÄÏ ÀÌ¹°
			m_imageView.m_inspect_ROI[num].imul_etq_right[0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.imul_etq_right[0].x;
			m_imageView.m_inspect_ROI[num].imul_etq_right[1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.imul_etq_right[1].x;
			m_imageView.m_inspect_ROI[num].imul_etq_right[0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.imul_etq_right[0].y;
			m_imageView.m_inspect_ROI[num].imul_etq_right[1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.imul_etq_right[1].y;


		// ÀÌ¹°

			if (m_imageView.m_inspect_ox.imul)
			{
				m_imageView.m_inspect_ROI[num].imul[0].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.imul[0].x;
				m_imageView.m_inspect_ROI[num].imul[1].x = m_imageView.m_ROI[num][0].x+m_imageView.m_inspect_offset.imul[1].x;
				m_imageView.m_inspect_ROI[num].imul[0].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.imul[0].y;
				m_imageView.m_inspect_ROI[num].imul[1].y = m_imageView.m_ROI[num][0].y+m_imageView.m_inspect_offset.imul[1].y;
			}		
		}

		// °¢Á¾ Ã¢¿¡ ¾÷µ« ½ÃÅ°±â
		if (m_imageView.m_Loaded)
			m_imageView.Invalidate(FALSE);

		// ¼³Á¤Ã¢ ¾÷µ«
		// ±¸Á¶Ã¼¿¡ º¹»ç
		m_settingdlg->m_camWidth		= m_imageView.m_inspect_setting.camWidth;
		m_settingdlg->m_camHeight		= m_imageView.m_inspect_setting.camHeight;
		m_settingdlg->m_inspPitch		= m_imageView.m_inspect_setting.inspPitch;
		m_settingdlg->m_triggerPerPitch = m_imageView.m_inspect_setting.triggerPerPitch;

 		m_settingdlg->m_angle1_ok = m_imageView.m_inspect_ox.angle[0];
 		m_settingdlg->m_anglew_ok = m_imageView.m_inspect_ox.angle[1];
 		m_settingdlg->m_chuck_ok  = m_imageView.m_inspect_ox.chuck;

 		m_settingdlg->m_imul1_ok		= m_imageView.m_inspect_ox.imul;
		m_settingdlg->m_imul_mid_ok		= m_imageView.m_inspect_ox.imul_mid;
		m_settingdlg->m_imul_jabara_ok	= m_imageView.m_inspect_ox.imul_jabara;
		m_settingdlg->m_imul_chuck_ok	= m_imageView.m_inspect_ox.imul_chuck;
		m_settingdlg->m_imul_insert_ok	= m_imageView.m_inspect_ox.imul_insert;
		m_settingdlg->m_imul_etq_left_ok = m_imageView.m_inspect_ox.imul_etq_left;
		m_settingdlg->m_imul_etq_right_ok = m_imageView.m_inspect_ox.imul_etq_right;
		
		m_settingdlg->m_insert_ok	   = m_imageView.m_inspect_ox.insert;
 		m_settingdlg->m_jabara_ok	   = m_imageView.m_inspect_ox.jabara;
		m_settingdlg->m_jabara_edge_ok = m_imageView.m_inspect_ox.jabara_edge;
		m_settingdlg->m_etiquette_ok   = m_imageView.m_inspect_ox.etiquette;
 		m_settingdlg->m_maguri_ok	   = m_imageView.m_inspect_ox.maguri;
 		m_settingdlg->m_pok_ok		   = m_imageView.m_inspect_ox.pok;
 		
		m_settingdlg->m_maguri_angle_ok = m_imageView.m_inspect_ox.maguri_angle;
		m_settingdlg->m_mid_angle_ok = m_imageView.m_inspect_ox.mid_angle;
		m_settingdlg->m_top_angle_ok = m_imageView.m_inspect_ox.top_angle;
		m_settingdlg->m_chuck_angle_ok = m_imageView.m_inspect_ox.chuck_angle;
		m_settingdlg->m_insert_angle_ok = m_imageView.m_inspect_ox.insert_angle;
		m_settingdlg->m_maguri_side_ok = m_imageView.m_inspect_ox.maguri_side;

 		m_settingdlg->m_imul_pix		= m_imageView.m_inspect_setting.pixsize;
		m_settingdlg->m_imul_mid_pix	= m_imageView.m_inspect_setting.mid_pixsize;
		m_settingdlg->m_imul_jabara_pix = m_imageView.m_inspect_setting.jabara_pixsize;
		m_settingdlg->m_imul_chuck_pix	= m_imageView.m_inspect_setting.chuck_pixsize;
		m_settingdlg->m_imul_insert_pix = m_imageView.m_inspect_setting.insert_pixsize;
		m_settingdlg->m_imul_etq_left_pix = m_imageView.m_inspect_setting.imul_etq_left_pix;
		m_settingdlg->m_imul_etq_right_pix = m_imageView.m_inspect_setting.imul_etq_right_pix;

		m_settingdlg->m_imul_top_threshold	  = m_imageView.m_inspect_setting.imul_top_threshold;
		m_settingdlg->m_imul_mid_threshold	  = m_imageView.m_inspect_setting.imul_mid_threshold;
		m_settingdlg->m_imul_jabara_threshold = m_imageView.m_inspect_setting.imul_jabara_threshold;
		m_settingdlg->m_imul_chuck_threshold  = m_imageView.m_inspect_setting.imul_chuck_threshold;
		m_settingdlg->m_imul_insert_threshold = m_imageView.m_inspect_setting.imul_insert_threshold;
		m_settingdlg->m_imul_etq_left_threshold = m_imageView.m_inspect_setting.imul_etq_left_threshold;
		m_settingdlg->m_imul_etq_right_threshold = m_imageView.m_inspect_setting.imul_etq_right_threshold;

		m_settingdlg->m_imul_top_mask	  = m_imageView.m_inspect_setting.imul_top_mask;
		m_settingdlg->m_imul_top_mask_cols= m_imageView.m_inspect_setting.imul_top_mask_cols;
		m_settingdlg->m_imul_mid_mask	  = m_imageView.m_inspect_setting.imul_mid_mask;
		m_settingdlg->m_imul_jabara_mask  = m_imageView.m_inspect_setting.imul_jabara_mask;
		m_settingdlg->m_imul_chuck_mask	  = m_imageView.m_inspect_setting.imul_chuck_mask;
		m_settingdlg->m_imul_chuck_mask_cols = m_imageView.m_inspect_setting.imul_chuck_mask_cols;
		m_settingdlg->m_imul_insert_mask  = m_imageView.m_inspect_setting.imul_insert_mask;
		m_settingdlg->m_imul_insert_pos	  = m_imageView.m_inspect_setting.imul_insert_pos;
		m_settingdlg->m_imul_etq_left_mask = m_imageView.m_inspect_setting.imul_etq_left_mask;
		m_settingdlg->m_imul_etq_right_mask = m_imageView.m_inspect_setting.imul_etq_right_mask;

 		m_settingdlg->m_jabara_recognize		= m_imageView.m_inspect_setting.jabara_recognize ;
		m_settingdlg->m_jabara_num				= m_imageView.m_inspect_setting.jabara_num ;
		m_settingdlg->m_jabara_margin_of_error  = m_imageView.m_inspect_setting.jabara_margin_of_error;
		m_settingdlg->m_jabara_width_stdev		= m_imageView.m_inspect_setting.jabara_width_stdev ;
 		m_settingdlg->m_jabara_distance_stdev	= m_imageView.m_inspect_setting.jabara_distance_stdev ;
		m_settingdlg->m_jabara_gradient_stdev	= m_imageView.m_inspect_setting.jabara_gradient_stdev;
		m_settingdlg->m_jabara_neighbor			= m_imageView.m_inspect_setting.jabara_neighbor;

		m_settingdlg->m_etiquette_position  = m_imageView.m_inspect_setting.etiquette_position;
		m_settingdlg->m_etiquette_recognize = m_imageView.m_inspect_setting.etiquette_recognize;
		m_settingdlg->m_etiquette_size		= m_imageView.m_inspect_setting.etiquette_size;

		m_settingdlg->m_camWidth		= m_imageView.m_inspect_setting.camWidth;
		m_settingdlg->m_camHeight		= m_imageView.m_inspect_setting.camHeight;
		m_settingdlg->m_triggerPerPitch = m_imageView.m_inspect_setting.triggerPerPitch;
		m_settingdlg->m_inspPitch		= m_imageView.m_inspect_setting.inspPitch;

 		m_settingdlg->m_maguri_slide  = m_imageView.m_inspect_setting.m_Magunus_Slide  ;
 		m_settingdlg->m_maguri_slide  = m_imageView.m_inspect_setting.m_Magunus_Slide2  ;
 		m_settingdlg->m_maguri_length = m_imageView.m_inspect_setting.m_Magunus_SlideLength ;
		m_settingdlg->m_maguri_max_length = m_imageView.m_inspect_setting.m_maguri_max_length;
 
 		m_settingdlg->m_minchuck	 = m_imageView.m_inspect_setting.m_minChuck ;
		m_settingdlg->m_maxchuck	 = m_imageView.m_inspect_setting.m_maxChuck ;
 		m_settingdlg->m_pok_limit	 = m_imageView.m_inspect_setting.m_pok_limit  ;
 		m_settingdlg->m_slide_length = m_imageView.m_inspect_setting.m_SlideLength  ;
 		
 		// Ãß°¡µÈ °Íµé
 		m_settingdlg->m_angle_diff	 = m_imageView.m_inspect_setting.angle_diff;
 		m_settingdlg->m_pok			 = m_imageView.m_inspect_setting.m_pok;
		m_settingdlg->m_angle1_limit = m_imageView.m_inspect_setting.angle_limit[0];
		m_settingdlg->m_angle2_limit = m_imageView.m_inspect_setting.angle_limit[1];
		m_settingdlg->m_resol		 = m_imageView.m_inspect_setting.resol;
		m_settingdlg->m_insert_slide = m_imageView.m_inspect_setting.m_Slide;

		m_settingdlg->m_maguri_diff_ratio = m_imageView.m_inspect_setting.m_maguri_diff_ratio;
		m_settingdlg->m_maguri_angle_gap = m_imageView.m_inspect_setting.m_maguri_angle_gap;
		m_settingdlg->m_maguri_angle_continuous = m_imageView.m_inspect_setting.m_maguri_angle_continuous;
		m_settingdlg->m_maguri_angle_diff = m_imageView.m_inspect_setting.m_maguri_angle_diff;
		m_settingdlg->m_mid_angle_gap = m_imageView.m_inspect_setting.m_mid_angle_gap;
		m_settingdlg->m_mid_angle_continuous = m_imageView.m_inspect_setting.m_mid_angle_continuous;
		m_settingdlg->m_mid_angle_diff = m_imageView.m_inspect_setting.m_mid_angle_diff;
		m_settingdlg->m_mid_angle_height_diff = m_imageView.m_inspect_setting.m_mid_angle_height_diff;
		m_settingdlg->m_top_angle_gap = m_imageView.m_inspect_setting.m_top_angle_gap;
		m_settingdlg->m_top_angle_continuous = m_imageView.m_inspect_setting.m_top_angle_continuous;
		m_settingdlg->m_top_angle_diff = m_imageView.m_inspect_setting.m_top_angle_diff;
		m_settingdlg->m_top_angle_height_diff = m_imageView.m_inspect_setting.m_top_angle_height_diff;
		m_settingdlg->m_chuck_angle_gap = m_imageView.m_inspect_setting.m_chuck_angle_gap;
		m_settingdlg->m_chuck_angle_continuous = m_imageView.m_inspect_setting.m_chuck_angle_continuous;
		m_settingdlg->m_chuck_angle_diff = m_imageView.m_inspect_setting.m_chuck_angle_diff;
		m_settingdlg->m_chuck_angle_height_diff = m_imageView.m_inspect_setting.m_chuck_angle_height_diff;
		m_settingdlg->m_insert_angle_gap = m_imageView.m_inspect_setting.m_insert_angle_gap;
		m_settingdlg->m_insert_angle_continuous = m_imageView.m_inspect_setting.m_insert_angle_continuous;
		m_settingdlg->m_insert_angle_diff = m_imageView.m_inspect_setting.m_insert_angle_diff;

		m_settingdlg->m_imul_top_persent	  = m_imageView.m_inspect_setting.imul_top_persent;
		m_settingdlg->m_imul_mid_persent	  = m_imageView.m_inspect_setting.imul_mid_persent;
		m_settingdlg->m_imul_top_size	  = m_imageView.m_inspect_setting.imul_top_size;
		m_settingdlg->m_imul_mid_size	  = m_imageView.m_inspect_setting.imul_mid_size;

		m_settingdlg->m_imul_mid_notinspect	  = m_imageView.m_inspect_setting.imul_mid_notinspect;

		m_settingdlg->m_stabbed_insert_notinspect	  = m_imageView.m_inspect_setting.stabbed_insert_notinspect;
		m_settingdlg->m_stabbed_insert_pix	  = m_imageView.m_inspect_setting.stabbed_insert_pix;

		// ¹öÆ° Enable ÇØ ÁÖ±â
 		m_maguri.EnableWindow(m_settingdlg->m_maguri_ok);
 		m_chuck.EnableWindow(m_settingdlg->m_chuck_ok);
 		m_insert.EnableWindow(m_settingdlg->m_insert_ok);
 		m_pok.EnableWindow(m_settingdlg->m_pok_ok);
 		m_angle_left.EnableWindow(m_settingdlg->m_angle1_ok);
 		m_angle_right.EnableWindow(m_settingdlg->m_anglew_ok);
 		m_jabara.EnableWindow(m_settingdlg->m_jabara_ok);
		m_etiquette.EnableWindow(m_settingdlg->m_etiquette_ok);
		m_btn_imul_etq_left.EnableWindow(m_settingdlg->m_imul_etq_left_ok);
		m_btn_imul_etq_right.EnableWindow(m_settingdlg->m_imul_etq_right_ok);
		

		fclose(file);

		m_DBLoaded = true;

		for( int i = 0; i < _MAXROI; i++ )
		{
			m_imageView.m_bExistObj[i] = TRUE;
		}
	}

	if(m_bAutoOpen==TRUE)
	{
		m_bAutoOpen = FALSE;

	}

}
void CPreInspection::OnMaguri() 
{
	// TODO: Add your control notification handler code here

	// ¸¶±¸¸® ¼³Á¤
	// 1¹ø¸¸ ¼³Á¤ÇÏ°í ³ª¸ÓÁö´Â º¹»çÇÔ

	// ¸¶±¸¸® : 1 ÀÚ¹Ù¶ó : 2 Ãà°ü : 3 ¿ª»ð : 4
	// Æø : 5 °¢ ÁÂ : 11 °¢¿ì : 12 ÀÌ¹°Áú : 20~29
	// ¿¡Æ¼ÄÏ: 30

	// ±×¸®°í Çö ROI´Â 100ÀÚ¸®¼ö 

	AfxMessageBox("¸¶±¸¸® ¿µ¿ªÀ» 1¹ø »¡´ë ¿µ¿ª¿¡¼­ ¼³Á¤ÇÏ¼¼¿ä\n(ð¯ìéýåÎ·ÖÅæ´×ìàâïÒÓ®ÔéÖÅæ´)");

	m_imageView.SetInspectionROI(0,1);
}

void CPreInspection::OnChuck() 
{
	// TODO: Add your control notification handler code here
	AfxMessageBox("Ãà°ü ¿µ¿ªÀ» 1¹ø »¡´ë ¿µ¿ª¿¡¼­ ¼³Á¤ÇÏ¼¼¿ä\n(ð¯ìéýåÎ·ÖÅæ´×ìàâïÒõêÎ·ÖÅæ´)");

	m_imageView.SetInspectionROI(0,3);	
}

void CPreInspection::OnJabara() 
{
	// TODO: Add your control notification handler code here
	AfxMessageBox("ÀÚ¹Ù¶ó ¿µ¿ªÀ» 1¹ø »¡´ë ¿µ¿ª¿¡¼­ ¼³Á¤ÇÏ¼¼¿ä");

	m_imageView.SetInspectionROI(0,2);
}

void CPreInspection::OnInsert() 
{
	// TODO: Add your control notification handler code here
	AfxMessageBox("¿ª»ð ¿µ¿ªÀ» 1¹ø »¡´ë ¿µ¿ª¿¡¼­ ¼³Á¤ÇÏ¼¼¿ä\n(ð¯ìéýåÎ·ÖÅæ´×ìàâïÒßºìýÖÅæ´)");

	m_imageView.SetInspectionROI(0,4);

}

void CPreInspection::OnPok() 
{
	AfxMessageBox("Æø ¿µ¿ªÀ» 1¹ø »¡´ë ¿µ¿ª¿¡¼­ ¼³Á¤ÇÏ¼¼¿ä");
	m_imageView.SetInspectionROI(0,5);
}

void CPreInspection::OnAngleLeft() 
{
	AfxMessageBox("ÁÂÃø °¢µµ ¿µ¿ªÀ» 1¹ø »¡´ë ¿µ¿ª¿¡¼­ ¼³Á¤ÇÏ¼¼¿ä");	
	m_imageView.SetInspectionROI(0,11);	
}

void CPreInspection::OnAngleRight() 
{
	AfxMessageBox("¿ìÃø °¢µµ ¿µ¿ªÀ» 1¹ø »¡´ë ¿µ¿ª¿¡¼­ ¼³Á¤ÇÏ¼¼¿ä");
	m_imageView.SetInspectionROI(0,12);
}

void CPreInspection::OnEtiquette()
{
	AfxMessageBox(_T("¿¡Æ¼ÄÏ ¿µ¿ªÀ» 1¹ø »¡´ë ¿µ¿ª¿¡¼­ ¼³Á¤ÇÏ¼¼¿ä"));

	m_imageView.SetInspectionROI(0, 30);
}


void CPreInspection::OnBnClickedEtqLeft()
{
	AfxMessageBox(_T("ÁÂÃø ¿¡Æ¼ÄÏ ÀÌ¹° ¿µ¿ªÀ» 1¹ø »¡´ë ¿µ¿ª¿¡¼­ ¼³Á¤ÇÏ¼¼¿ä\n(ð¯ìéýåÎ·ÖÅæ´×ìàâïÒñ§Ü«èêðËÖÅæ´)"));
	m_imageView.SetInspectionROI(0, 40);
}

void CPreInspection::OnBnClickedEtqRight()
{
	AfxMessageBox(_T("¿ìÃø ¿¡Æ¼ÄÏ ÀÌ¹° ¿µ¿ªÀ» 1¹ø »¡´ë ¿µ¿ª¿¡¼­ ¼³Á¤ÇÏ¼¼¿ä\n(ð¯ìéýåÎ·ÖÅæ´×ìàâïÒéÓÜ«èêðËÖÅæ´)"));
	m_imageView.SetInspectionROI(0, 41);
}


//-------------------------------------------------------------------
void CPreInspection::OnBtnTtest()		// BHJ 20081210 ÀÌ¹ÌÁö 50Àå¾¿ Å×½ºÆ®. ½Ç½Ã. 
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	CFileDialog dlgopen(TRUE, "bmp", "*.bmp", NULL, _T("Files (*.bmp)|*.bmp|"));
	if(dlgopen.DoModal() == IDOK)
	{
		CString tempath, ReInspPath, ReInspData, startnum;

		tempath=dlgopen.GetPathName();
		ReInspPath = ReInspData = startnum = tempath;
		int a, b;
		a = ReInspPath.GetLength();
		b = ReInspPath.ReverseFind('\\');
		ReInspPath.Delete(b+1, a - b-1);
		
		ReInspData.Delete ( 0, tempath.ReverseFind('\\') + 1 );

		if ( ReInspData.GetLength() != 12 )
		{
			AfxMessageBox( _T("ÂüÁ¶¿ë ÀÌ¹ÌÁö¿¡ ´ëÇØ¼­¸¸ »ç¿ëÇÒ ¼ö ÀÖ½À´Ï´Ù! (Çü½Ä: 1_01_001.bmp)") );
			return;
		}

		ReInspData.Delete(ReInspData.GetLength()-7,7);

		startnum.Delete(0, tempath.GetLength()-6);
		startnum.Delete(startnum.GetLength()-4,4);

		CString Lstr;
		char line[128], chstart[128];
		int i=0, InspResult=0, numStart=1, numEnd=100;
		sprintf(chstart,startnum);
		sscanf(chstart,"%d", &numStart);

		if ( numStart + 50 > 100 ) 
			numEnd=100;
		else 
			numEnd = numStart+50;

		for ( i = numStart; i < numEnd ; i++ )
		{
			sprintf( line, ReInspPath + ReInspData + "%03d.bmp", i );
			Lstr = line;
			m_imageView.LoadImage( Lstr );
			
			// ÀÌ¹ÌÁö ¼ÂÆÃ
			m_imageView.m_process->SetImageSize(_CamWidth,_CamHeight);

			// °Ë»ç ÇÏ±â
			InspResult = m_imageView.DoInspection();
			if (InspResult > 0)
			{
				sprintf( line, _T("%d Image Error"), i );
				Lstr = line;
				AfxMessageBox( Lstr );
				break;
			}
		}
		if( InspResult == 0 )
			AfxMessageBox( _T("All Good") );
	}
}

BOOL CPreInspection::PreTranslateMessage(MSG* pMsg) 
{
	switch( pMsg->message){
		case WM_KEYDOWN:
		{
			int s;
			s = (int)pMsg->wParam;
			if (s==VK_RETURN)
			{
				return true;
			}
			else if (s==VK_TAB)
			{
				return true;
			}
			else if (s==VK_SPACE)
			{
				OnTest();
				return true;
			}
		}
    }
	return CDialog::PreTranslateMessage(pMsg);
}

void CPreInspection::OnSetParam()
{
	//AfxMessageBox("param hi");
	m_ptParam->ShowWindow(SW_SHOW);
}

void CPreInspection::WriteParam(INSP_PARAM ParamData)
{
	//AfxMessageBox("hi write");
	CString strpath;
	if((((m_imageView.m_process)->m_colorstraw)==0) && (((m_imageView.m_process)->m_tepe) ==0)){	// Èò»ö »¡´ë¿¡ ÀÚ¹Ù¶ó ¾øÀ½.
		strpath =  "c:\\ATI\\parameter_NC_NJB.txt";
	}
	else if((((m_imageView.m_process)->m_colorstraw)==0) && (((m_imageView.m_process)->m_tepe) ==1 )){	// Èò»ö »¡´ë¿¡ ÀÚ¹Ù¶ó ÀÖÀ½.
		strpath =  "c:\\ATI\\parameter_NC_YJB.txt";
	}
	else if((((m_imageView.m_process)->m_colorstraw) > 0) && (((m_imageView.m_process)->m_tepe) == 0)){	// À¯»ö »¡´ë¿¡ ÀÚ¹Ù¶ó ¾øÀ½.
		strpath =  "c:\\ATI\\parameter_YC_NJB.txt";
	}
	else if((((m_imageView.m_process)->m_colorstraw) > 0) && (((m_imageView.m_process)->m_tepe) ==1)){	// À¯»ö »¡´ë¿¡ ÀÚ¹Ù¶ó ÀÖÀ½.
		strpath =  "c:\\ATI\\parameter_YC_YJB.txt";
	}

//	int threshold, data[30];
//	float fdata[10];
//	char line[512];

		FILE *setting = fopen(strpath,"w");
		fprintf(setting,"[Main Threshold]\n");
			fprintf(setting,"%d\n", ParamData.nData[0]);
		
		fprintf(setting,"[Chuck Saturation GV]\n");
			fprintf(setting,"%d\n", ParamData.nData[4]);
		
		fprintf(setting,"------[¿¡Æ¼ÄÏ]------\n");
		fprintf(setting,"[ÃÖ¼Ò ¹à±â Â÷ÀÌ]\n");
			fprintf(setting,"%d\n", ParamData.nData[6]);
		
		fprintf(setting,"[¸î ÁÙ ÀÌ»ó?]\n");
			fprintf(setting,"%d\n", ParamData.nData[7]);
		
		float etqdev = (float)ParamData.fData[0];

		fprintf(setting,"[ÃÖ¼Ò Ç¥ÁØÆíÂ÷ ¼ÒÁÖÁ¡ 2ÀÚ¸®]\n");
			fprintf(setting,"%d\n", (int)((etqdev-(int)etqdev)*100) );
		
		fprintf(setting,"[ÃÖ¼Ò Ç¥ÁØÆíÂ÷]\n");
			fprintf(setting,"%d\n", (int)etqdev);
		
		fprintf(setting,"[ÀÚ¹Ù¶ó~¿¡Æ¼ÄÏ °Å¸®]\n");
			fprintf(setting,"%d\n", ParamData.nData[8]);
		
		fprintf(setting,"[¿¡Æ¼ÄÏ~½ºÅäÆÛ °Å¸®]\n");
			fprintf(setting,"%d\n", ParamData.nData[9]);

		fprintf(setting,"[¿¡Æ¼ÄÏ °ËÃâ ¹üÀ§]\n");
			fprintf(setting,"%d\n", ParamData.nData[10]);

		fprintf(setting,"[¿¡Æ¼ÄÏ ±æÀÌ]\n");
			fprintf(setting,"%d\n", ParamData.nData[11]);

		fprintf(setting,"-----[ÀÌ¹° ¹üÀ§ ¼³Á¤]----\n");
		fprintf(setting,"[¸¶±¸¸®¿¡¼­ Ã¹ ÀÌ¹° ¿µ¿ª±îÁö °Å¸®]\n");
			fprintf(setting,"%d\n", ParamData.nData[12]);
		
		fprintf(setting,"[ÀÚ¹Ù¶ó ÁÖº¯ ºñ °Ë»ç ¿µ¿ª]\n");
			fprintf(setting,"%d\n", ParamData.nData[13]);

		fprintf(setting,"[Ãà°ü¿¡¼­ ½ºÅäÆÛ±îÁö °Å¸®]\n");
			fprintf(setting,"%d\n", ParamData.nData[14]);	

		fprintf(setting,"[Ãà°ü¿¡¼­ ³»Ãø °Ë»ç¿µ¿ª±îÁö °Å¸®]\n");
			fprintf(setting,"%d\n", ParamData.nData[15]);

		fprintf(setting,"[Ãà°ü ¾ç ³¡´ÜÀ¸·ÎºÎÅÍ °Ë»ç¿µ¿ª±îÁö °Å¸®]\n");
			fprintf(setting,"%d\n", ParamData.nData[16]);

		fprintf(setting,"[¸¶±¸¸®, ÀÚ¹Ù¶ó, Ãà°ü~»óÇÏ ³¡ ´Ü±îÁö °Å¸®]\n");
			fprintf(setting,"%d\n", ParamData.nData[17]);

		fprintf(setting,"[³»Ãø »óÇÏ ³¡ ´Ü±îÁö °Å¸®]\n");
			fprintf(setting,"%d\n", ParamData.nData[18]);
			
		fprintf(setting,"[³»Ãø °Ë»ç ¿µ¿ª ³Êºñ]\n");
			fprintf(setting,"%d\n", ParamData.nData[19]);

		fprintf(setting,"-----[Åõ¸í »ðÀÔºÎ ¼³Á¤]----\n");
		fprintf(setting,"[Åõ¸í »ðÀÔºÎ Threshold °ª]\n");
			fprintf(setting,"%d\n", ParamData.nData[21]);	
		
		fprintf(setting,"-----[StrawExistance Offset]----\n");
			fprintf(setting,"%d\n", ParamData.nData[20]);	

		fprintf(setting,"-----[Æ¯º° Threshold]-------\n");
		fprintf(setting,"[¸¶±¸¸®½Ã ¸ÞÀÎ Threshold°ªÀÇ %]\n");
			fprintf(setting,"%d\n", ParamData.nData[1]);
		
		fprintf(setting,"[Ãà°ü½Ã ¸ÞÀÎ Threshold°ªÀÇ%]\n");
			fprintf(setting,"%d\n", ParamData.nData[2]);
		
		fprintf(setting,"[°¢ ÃøÁ¤½Ã ¸ÞÀÎ Threshold°ªÀÇ%]\n");
			fprintf(setting,"%d\n", ParamData.nData[3]);
		
		fprintf(setting,"----[Ä«¸Þ¶ó °ÔÀÎ ¼³Á¤]-----\n");
			fprintf(setting,"%f\n", ParamData.fData[1]);

		fprintf(setting,"----[Ãà°ü ÁÂ¿ì Æø Á¦ÇÑÄ¡]----\n");
			fprintf(setting,"%f\n", ParamData.fData[2]);

		fprintf(setting,"----[¸¶±¸¸® Åõ¸í ³»Ãø]----\n");
			fprintf(setting,"%d\n", ParamData.nData[5]);

		fprintf(setting,"----[Ä«¸Þ¶ó ¼ÅÅÍ ½ºÇÇÆ®]----\n");
			fprintf(setting,"%f\n", ParamData.fData[3]);

		fprintf(setting,"----[Ãà°ü ²©ÀÌ´Â ºÎºÐ ±â¿ï±â »óºÎ]----\n");
			fprintf(setting,"%f\n", ParamData.fData[4]);

		fprintf(setting,"----[Ãà°ü ²©ÀÌ´Â ºÎºÐ ±â¿ï±â ÇÏºÎ]----\n");
			fprintf(setting,"%f\n", ParamData.fData[5]);

		fprintf(setting,"----[Ä«¸Þ¶ó¸ðµå º¯°æ ¿©ºÎ]----\n");
			fprintf(setting,"%d\n", ParamData.nData[22]);
		fprintf(setting,"----[Ä«¸Þ¶ó Mode 14ÀÌ¸é [0], Mode 15ÀÌ¸é : [1]]----\n");
			fprintf(setting,"%d\n", ParamData.nData[24]);

		fprintf(setting,"----[Ãà°ü Æø Á¦ÇÑÇÏ´Â ¼öÄ¡]----\n");
			fprintf(setting,"%d\n", ParamData.nData[23]);
		
		fclose(setting);

		FILE *color = fopen("c:\\ATI\\colorthres.txt","w");
		for (int i=0;i<3;i++)
			fprintf(color,"%d\n", ParamData.nData[27+i]);
		fclose(color);

		m_MainPt->m_colorthres[0] = ParamData.nData[27];
		m_MainPt->m_colorthres[1] = ParamData.nData[28];
		m_MainPt->m_colorthres[2] = ParamData.nData[29];
		m_MainPt->ParameterRefresh();	
}

void CPreInspection::ReadParam(INSP_PARAM ParamData)
{
	m_ptParam->m_nMainThres				=	ParamData.nData[0];	
	m_ptParam->m_nMaguriThres			=	ParamData.nData[1];
	m_ptParam->m_nChuckThres			=	ParamData.nData[2];
	m_ptParam->m_nAngleThres			=	ParamData.nData[3];
	m_ptParam->m_nSatuthres				=	ParamData.nData[4];
	m_ptParam->m_nTransMaguriThres	=	ParamData.nData[5];
	m_ptParam->m_midNotInspArea			=	ParamData.nData[6];
	m_ptParam->m_chuckNotInspArea		=	ParamData.nData[7];
	m_ptParam->m_maguriSideDepth			=	ParamData.nData[8];
	m_ptParam->m_nEtqtoStopper			=	ParamData.nData[9];
	m_ptParam->m_nEtqInspArea			=	ParamData.nData[10];
	m_ptParam->m_nEtqLength				=	ParamData.nData[11];
	m_ptParam->m_nImulMaguriNoInsp	=	ParamData.nData[12];
	m_ptParam->m_nImulJabaraNoInsp	=	ParamData.nData[13];
	m_ptParam->m_nImulChuckToStopper	=	ParamData.nData[14];
	m_ptParam->m_nImulInsertNoInsp	=	ParamData.nData[15];
	m_ptParam->m_nImulMidToStopper	=	ParamData.nData[16];
	m_ptParam->m_transRatio	=	ParamData.nData[17];
	m_ptParam->m_nImulInsertUpDown	=	ParamData.nData[18];
	m_ptParam->m_nImulInsertHeight	=	ParamData.nData[19];
	m_ptParam->m_nImageCenter			=	ParamData.nData[20];
	m_ptParam->m_nTransInsertThres	=	ParamData.nData[21];
	if(ParamData.nData[22]==0)		m_ptParam->m_bUse14Trigger	= FALSE;
	else		m_ptParam->m_bUse14Trigger = TRUE;
	m_ptParam->m_nTriChangeMode		= ParamData.nData[24];

	m_ptParam->m_nImulReduce		=	ParamData.nData[23];
	

	m_ptParam->m_chuckLeft				=	(float)ParamData.fData[0];
	m_ptParam->m_fCameraGain			=	(float)ParamData.fData[1];
	m_ptParam->m_fChuckPokAverage		=	(float)ParamData.fData[2];
	m_ptParam->m_fCameraShutter		    =	(float)ParamData.fData[3];
	m_ptParam->m_fChuckUpScale			=	(float)ParamData.fData[4];
	m_ptParam->m_fChuckDownScale		=	(float)ParamData.fData[5];

	m_ptParam->UpdateData(false);

	_m_nImulMidToStopper = m_ptParam->m_nImulMidToStopper;
	_m_nImulChuckToStopper = m_ptParam->m_nImulChuckToStopper;
	_m_nImulReduce = m_ptParam->m_nImulReduce;
	_m_nImulMaguriNoInsp = m_ptParam->m_nImulMaguriNoInsp;
	g_midNotInspArea = m_ptParam->m_midNotInspArea;
	g_chuckNotInspArea = m_ptParam->m_chuckNotInspArea;
	g_chuckLeft = (int)(m_ptParam->m_chuckLeft);
	g_maguriSideDepth = (int)(m_ptParam->m_maguriSideDepth);
	g_transRatio = m_ptParam->m_transRatio;
}

void CPreInspection::SetPointer(CSeoilVisionDlg* CSpoint)
{
	m_MainPt = (CSeoilVisionDlg*)CSpoint;
}

void CPreInspection::ShowROIS( int roi, BOOL isChecked )
{
	UpdateData(true);
	m_imageView.m_bExistObj[roi] = isChecked;
	Invalidate(true);
}

void CPreInspection::OnChkRoi01()
{
	UpdateData(true);
	m_imageView.m_bExistObj[0] = m_bViewROI01;
	Invalidate(true);
}

void CPreInspection::OnChkRoi02()
{
	UpdateData(true);
	m_imageView.m_bExistObj[1] = m_bViewROI02;
	Invalidate(true);
}

void CPreInspection::OnChkRoi03()
{
	UpdateData(true);
	m_imageView.m_bExistObj[2] = m_bViewROI03;
	Invalidate(true);
}

void CPreInspection::OnChkRoi04()
{
	UpdateData(true);
	m_imageView.m_bExistObj[3] = m_bViewROI04;
	Invalidate(true);
}

void CPreInspection::OnChkRoi05()
{	
	UpdateData(true);
	m_imageView.m_bExistObj[4] = m_bViewROI05;
	Invalidate(true);
}


BOOL CPreInspection::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_dlgCopyBtn.OnMouseWheel(nFlags, zDelta, pt);

	//return CDialog::OnMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}

void CPreInspection::OnBnClickedButtonInspResult()
{
	CInspectResultDlg dlg;

	dlg.DoModal( m_imageView.m_result_Value );
}
