// InspectSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "InspectSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CInspectSettingDlg dialog


CInspectSettingDlg::CInspectSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectSettingDlg::IDD, pParent)
	, m_camWidth(0)
	, m_camHeight(0)
	, m_triggerPerPitch(0)
	, m_inspPitch(0)
	, m_jabara_margin_of_error(0)
	, m_imul_top_threshold(0)
	, m_imul_mid_threshold(0)
	, m_imul_jabara_threshold(0)
	, m_imul_chuck_threshold(0)
	, m_imul_insert_threshold(0)
	, m_etiquette_size(0)
	, m_imul_top_mask(0)
	, m_imul_top_mask_cols(0)
	, m_imul_mid_mask(0)
	, m_imul_jabara_mask(0)
	, m_imul_chuck_mask(0)
	, m_imul_insert_mask(0)
	, m_imul_insert_pos(0)
	, m_maxchuck(0)
	, m_maguri_angle_ok(FALSE)
	, m_mid_angle_ok(FALSE)
	, m_top_angle_ok(FALSE)
	, m_chuck_angle_ok(FALSE)
	, m_insert_angle_ok(FALSE)
	, m_maguri_diff_ratio(0)
	, m_maguri_side_ok(FALSE)
	, m_maguri_angle_gap(0)
	, m_maguri_angle_continuous(0)
	, m_maguri_angle_diff(0)
	, m_mid_angle_gap(0)
	, m_mid_angle_continuous(0)
	, m_mid_angle_diff(0)
	, m_mid_angle_height_diff(0)
	, m_top_angle_gap(0)
	, m_top_angle_continuous(0)
	, m_top_angle_diff(0)
	, m_top_angle_height_diff(0)
	, m_chuck_angle_gap(0)
	, m_chuck_angle_continuous(0)
	, m_chuck_angle_diff(0)
	, m_chuck_angle_height_diff(0)
	, m_insert_angle_gap(0)
	, m_insert_angle_continuous(0)
	, m_insert_angle_diff(0)
	, m_maguri_max_length(0)
	, m_imul_chuck_mask_cols(0)
	, m_imul_etq_left_ok(FALSE)
	, m_imul_etq_right_ok(FALSE)
	, m_imul_etq_left_pix(0)
	, m_imul_etq_left_threshold(0)
	, m_imul_etq_left_mask(0)
	, m_imul_etq_right_pix(0)
	, m_imul_etq_right_threshold(0)
	, m_imul_etq_right_mask(0)
	, m_imul_top_persent(0)
	, m_imul_mid_persent(0)
	, m_imul_top_size(0)
	, m_imul_mid_size(0)
	, m_imul_mid_notinspect(0)
	, m_stabbed_insert_notinspect(0)
	, m_stabbed_insert_pix(0)
{
	//{{AFX_DATA_INIT(CInspectSettingDlg)
	m_maguri_slide = 0.0;
	m_maguri_ok = FALSE;
	m_angle1_ok = FALSE;
	m_anglew_ok = FALSE;
	m_angle1_limit = 0.0;
	m_angle2_limit = 0.0;
	m_chuck_ok = FALSE;


	m_insert_ok = FALSE;
	m_insert_slide = 0.0;

	// jabara
	m_jabara_recognize = 0.0;
	m_jabara_num = 0;
	m_jabara_margin_of_error = 0;
	m_jabara_width_stdev = 0.0;
	m_jabara_distance_stdev = 0.0;
	m_jabara_gradient_stdev = 0.0;
	m_jabara_neighbor = 0;
	m_jabara_ok = FALSE;
	m_jabara_edge_ok = FALSE;


	// etiquette
	m_etiquette_ok = FALSE;
	m_etiquette_recognize = 0.0;
	m_etiquette_position = 0.0;
	m_etiquette_size = 0;

	m_minchuck = 0.0;
	m_maxchuck = 0.0;
	m_pok = 0.0;
	m_pok_limit = 0.0;
	m_pok_ok = FALSE;
	m_resol = 0.0;
	m_slide_length = 0;

	// 이물
	m_imul1_ok = FALSE;
	m_imul_mid_ok = FALSE;
	m_imul_jabara_ok = FALSE;
	m_imul_chuck_ok = FALSE;
	m_imul_insert_ok = FALSE;
	m_imul_etq_left_ok = FALSE;
	m_imul_etq_right_ok = FALSE;
	
	m_imul_pix = 0.0;
	m_imul_mid_pix = 0;
	m_imul_jabara_pix = 0;
	m_imul_chuck_pix = 0;
	m_imul_insert_pix = 0;
	m_imul_etq_left_pix = 0;
	m_imul_etq_right_pix = 0;

	m_imul_top_threshold = 0;
	m_imul_mid_threshold = 0;
	m_imul_jabara_threshold = 0;
	m_imul_chuck_threshold = 0;
	m_imul_insert_threshold = 0;
	m_imul_etq_left_threshold = 0;
	m_imul_etq_right_threshold = 0;

	m_imul_top_mask = 0;
	m_imul_top_mask_cols = 0;
	m_imul_mid_mask = 0;
	m_imul_jabara_mask = 0;
	m_imul_chuck_mask = 0;
	m_imul_chuck_mask_cols = 0;
	m_imul_insert_mask = 0;
	m_imul_insert_pos = 0;
	m_imul_etq_left_mask = 0;
	m_imul_etq_right_mask = 0;

	m_maguri_length = 0;
	m_maguri_max_length = 0;
	m_angle_diff = 0.0;
	m_imul_top_persent = 0.0;
	m_imul_mid_persent = 0.0;
	m_imul_top_size = 0;
	m_imul_mid_size = 0;

	m_imul_mid_notinspect = 0;

	m_stabbed_insert_notinspect = 0;
	m_stabbed_insert_pix = 0;

	// 피치, 트리거 추가

	m_camWidth = 0;
	m_camHeight = 0;
	m_triggerPerPitch = 0;
	m_inspPitch = 0;

	InitCheckBox();
	InitInspectionValues();

	//}}AFX_DATA_INIT
}

void CInspectSettingDlg::InitCheckBox()
{
	m_maguri_side_ok = FALSE;
	m_maguri_angle_ok = FALSE;
	m_mid_angle_ok = FALSE;
	m_top_angle_ok = FALSE;
	m_chuck_angle_ok = FALSE;
	m_insert_angle_ok = FALSE;
}

void CInspectSettingDlg::InitInspectionValues()
{
	m_maguri_diff_ratio = 0.0;

	m_maguri_angle_gap = 0;
	m_maguri_angle_continuous = 0;
	m_maguri_angle_diff = 0;

	m_mid_angle_gap = 0;
	m_mid_angle_continuous = 0;
	m_mid_angle_diff = 0;
	m_mid_angle_height_diff = 0;

	m_top_angle_gap = 0;
	m_top_angle_continuous = 0;
	m_top_angle_diff = 0;
	m_top_angle_height_diff = 0;

	m_chuck_angle_gap = 0;
	m_chuck_angle_continuous = 0;
	m_chuck_angle_diff = 0;
	m_chuck_angle_height_diff = 0;

	m_insert_angle_gap = 0;
	m_insert_angle_continuous = 0;
	m_insert_angle_diff = 0;
}

void CInspectSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInspectSettingDlg)
	DDX_Control(pDX, IDC_IMUL_PIX, c_imul_pix);
	DDX_Control(pDX, IDC_SLIDE_LENGTH, c_slide_length);
	DDX_Control(pDX, IDC_POK_LIMIT, c_pok_limit);
	DDX_Control(pDX, IDC_POK, c_pok);
	DDX_Control(pDX, IDC_MINCHUCK, c_minchuck);
	DDX_Control(pDX, IDC_MAXCHUCK, c_maxchuck);

	// jabara
	DDX_Control(pDX, IDC_JABARA_DISTANCE_STDEV, c_jabara_distance_stdev);
	DDX_Control(pDX, IDC_JABARA_WIDTH_STDEV, c_jabara_width_stdev);
	DDX_Control(pDX, IDC_JABARA_NUM, c_jabara_num);
	DDX_Control(pDX, IDC_JABARA_RECOG, c_jabara_recognize);
	DDX_Control(pDX, IDC_JABARA_GRADIENT_STDEV, c_jabara_gradient_stdev);
	DDX_Control(pDX, IDC_JABARA_NEIGHBOR, c_jabara_neighbor);
	DDX_Control(pDX, IDC_JABARA_MARGIN_OF_ERROR, c_jabara_margin_of_error);


	// etiquette
	DDX_Control(pDX, IDC_ETIQUETTE_RECOG, c_etiquette_recognize);
	DDX_Control(pDX, IDC_ETIQUETTE_POSITION, c_etiquette_position);

	// etc
	DDX_Control(pDX, IDC_INSERT_SLIDE, c_insert_slide);
	DDX_Control(pDX, IDC_ANGLELIMIT_2, c_anglelimit_2);
	DDX_Control(pDX, IDC_ANGLELIMIT_1, c_anglelimit_1);
	DDX_Control(pDX, IDC_ANGLE_DIFF, c_angle_diff);
	DDX_Control(pDX, IDC_MAGURI_LENGTH, c_maguri_length);
	DDX_Check(pDX, IDC_MAGURI_OK, m_maguri_ok);
	DDX_Check(pDX, IDC_ANGLE_1, m_angle1_ok);
	DDX_Check(pDX, IDC_ANGLE_2, m_anglew_ok);
	DDX_Text(pDX, IDC_ANGLELIMIT_1, m_angle1_limit);
	DDX_Text(pDX, IDC_ANGLELIMIT_2, m_angle2_limit);
	DDX_Check(pDX, IDC_CHUCK_OK, m_chuck_ok);

	// imul
	DDX_Text(pDX, IDC_IMUL_PIX, m_imul_pix);
	DDX_Text(pDX, IDC_IMUL_MID_PIX, m_imul_mid_pix);
	DDX_Text(pDX, IDC_IMUL_JABARA_PIX, m_imul_jabara_pix);
	DDX_Text(pDX, IDC_IMUL_CHUCK_PIX, m_imul_chuck_pix);
	DDX_Text(pDX, IDC_IMUL_INSERT_PIX, m_imul_insert_pix);


	// insert
	DDX_Check(pDX, IDC_INSERT_OK, m_insert_ok);
	DDX_Text(pDX, IDC_INSERT_SLIDE, m_insert_slide);

	// jabara
	DDX_Text(pDX, IDC_JABARA_RECOG, m_jabara_recognize);
	DDX_Text(pDX, IDC_JABARA_NUM, m_jabara_num);
	DDX_Text(pDX, IDC_JABARA_WIDTH_STDEV, m_jabara_width_stdev);
	DDX_Text(pDX, IDC_JABARA_DISTANCE_STDEV, m_jabara_distance_stdev);
	DDX_Text(pDX, IDC_JABARA_GRADIENT_STDEV, m_jabara_gradient_stdev);
	DDX_Text(pDX, IDC_JABARA_NEIGHBOR, m_jabara_neighbor);
	DDX_Text(pDX, IDC_JABARA_MARGIN_OF_ERROR, m_jabara_margin_of_error);
	DDV_MinMaxInt(pDX, m_jabara_margin_of_error, 0, 5);
	DDX_Check(pDX, IDC_JABARA_OK, m_jabara_ok);
	DDX_Check(pDX, IDC_JABARA_EDGE_OK, m_jabara_edge_ok);

	// etiquette
	DDX_Check(pDX, IDC_ETIQUETTE_OK, m_etiquette_ok);
	DDX_Text(pDX, IDC_ETIQUETTE_RECOG, m_etiquette_recognize);
	DDX_Text(pDX, IDC_ETIQUETTE_POSITION, m_etiquette_position);


	// 피치, 트리거 추가
	DDX_Text(pDX, IDC_CAM_WIDTH, m_camWidth);
	DDV_MinMaxInt(pDX, m_camWidth, 512, 1280);
	DDX_Text(pDX, IDC_CAM_HEIGHT, m_camHeight);
	DDV_MinMaxInt(pDX, m_camHeight, 64, 1024);
	DDX_Text(pDX, IDC_TRIGGER, m_triggerPerPitch);
	DDV_MinMaxInt(pDX, m_triggerPerPitch, 3, 5);
	DDX_Text(pDX, IDC_PITCH, m_inspPitch);
	DDV_MinMaxInt(pDX, m_inspPitch, 1, 3);


	// etc
	DDX_Text(pDX, IDC_MAXCHUCK, m_maxchuck);
	DDX_Text(pDX, IDC_MINCHUCK, m_minchuck);
	DDX_Text(pDX, IDC_POK, m_pok);
	DDX_Text(pDX, IDC_POK_LIMIT, m_pok_limit);
	DDX_Check(pDX, IDC_POK_OK, m_pok_ok);
	DDX_Text(pDX, IDC_RESOL, m_resol);
	DDX_Text(pDX, IDC_SLIDE_LENGTH, m_slide_length);

	DDX_Check(pDX, IDC_IMUL1_OK, m_imul1_ok);

	DDX_Text(pDX, IDC_MAGURI_LENGTH, m_maguri_length);
	DDX_Text(pDX, IDC_ANGLE_DIFF, m_angle_diff);
	//}}AFX_DATA_MAP


	// 이물
	DDX_Check(pDX, IDC_IMUL_MID_OK, m_imul_mid_ok);
	DDX_Check(pDX, IDC_IMUL_JABARA_OK, m_imul_jabara_ok);
	DDX_Check(pDX, IDC_IMUL_CHUCK_OK, m_imul_chuck_ok);
	DDX_Check(pDX, IDC_IMUL_INSERT_OK, m_imul_insert_ok);

	DDX_Control(pDX, IDC_IMUL_MID_PIX, c_imul_mid_pix);
	DDX_Control(pDX, IDC_IMUL_JABARA_PIX, c_imul_jabara_pix);
	DDX_Control(pDX, IDC_IMUL_CHUCK_PIX, c_imul_chuck_pix);
	DDX_Control(pDX, IDC_IMUL_INSERT_PIX, c_imul_insert_pix);

	DDX_Control(pDX, IDC_IMUL_TOP_THRESHOLD, c_imul_top_threshold);
	DDX_Control(pDX, IDC_IMUL_MID_THRESHOLD, c_imul_mid_threshold);
	DDX_Control(pDX, IDC_IMUL_JABARA_THRESHOLD, c_imul_jabara_threshold);
	DDX_Control(pDX, IDC_IMUL_CHUCK_THRESHOLD, c_imul_chuck_threshold);
	DDX_Control(pDX, IDC_IMUL_INSERT_THRESHOLD, c_imul_insert_threshold);

	DDX_Text(pDX, IDC_IMUL_TOP_THRESHOLD, m_imul_top_threshold);
	DDX_Text(pDX, IDC_IMUL_MID_THRESHOLD, m_imul_mid_threshold);
	DDX_Text(pDX, IDC_IMUL_JABARA_THRESHOLD, m_imul_jabara_threshold);
	DDX_Text(pDX, IDC_IMUL_CHUCK_THRESHOLD, m_imul_chuck_threshold);
	DDX_Text(pDX, IDC_IMUL_INSERT_THRESHOLD, m_imul_insert_threshold);
	DDX_Control(pDX, IDC_ETIQUETTE_SIZE, c_etiquette_size);
	DDX_Text(pDX, IDC_ETIQUETTE_SIZE, m_etiquette_size);
	DDX_Control(pDX, IDC_IMUL_TOP_MASK, c_imul_top_mask);
	DDX_Control(pDX, IDC_IMUL_TOP_MASK_COLS, c_imul_top_mask_cols);
	DDX_Control(pDX, IDC_IMUL_MID_MASK, c_imul_mid_mask);
	DDX_Control(pDX, IDC_IMUL_JABARA_MASK, c_imul_jabara_mask);
	DDX_Control(pDX, IDC_IMUL_CHUCK_MASK, c_imul_chuck_mask);
	DDX_Control(pDX, IDC_IMUL_INSERT_MASK, c_imul_insert_mask);
	DDX_Control(pDX, IDC_IMUL_INSERT_POS, c_imul_insert_pos);
	DDX_Text(pDX, IDC_IMUL_TOP_MASK, m_imul_top_mask);
	DDX_Text(pDX, IDC_IMUL_TOP_MASK_COLS, m_imul_top_mask_cols);
	DDX_Text(pDX, IDC_IMUL_MID_MASK, m_imul_mid_mask);
	DDX_Text(pDX, IDC_IMUL_JABARA_MASK, m_imul_jabara_mask);
	DDX_Text(pDX, IDC_IMUL_CHUCK_MASK, m_imul_chuck_mask);
	DDX_Text(pDX, IDC_IMUL_INSERT_MASK, m_imul_insert_mask);
	DDX_Text(pDX, IDC_IMUL_INSERT_POS, m_imul_insert_pos);
	DDX_Text(pDX, IDC_IMUL_TOP_PERSENT, m_imul_top_persent);
	DDX_Text(pDX, IDC_IMUL_MID_PERSENT, m_imul_mid_persent);
	DDX_Text(pDX, IDC_IMUL_TOP_SIZE, m_imul_top_size);
	DDX_Text(pDX, IDC_IMUL_MID_SIZE, m_imul_mid_size);

	DDX_Text(pDX, IDC_IMUL_MID_NOT, m_imul_mid_notinspect);

	// 찍힘 에디트 컨트롤 모음
	DDX_Check(pDX, IDC_MAGURI_ANGLE_OK, m_maguri_angle_ok);
	DDX_Check(pDX, IDC_MID_ANGLE_OK, m_mid_angle_ok);
	DDX_Check(pDX, IDC_TOP_ANGLE_OK, m_top_angle_ok);
	DDX_Check(pDX, IDC_CHUCK_ANGLE_OK, m_chuck_angle_ok);
	DDX_Check(pDX, IDC_INSERT_ANGLE_OK, m_insert_angle_ok);
	DDX_Control(pDX, IDC_MAGURI_DIFF_RATIO, c_maguri_diff_ratio);
	DDX_Text(pDX, IDC_MAGURI_DIFF_RATIO, m_maguri_diff_ratio);
	DDX_Check(pDX, IDC_MAGURI_SIDE_OK, m_maguri_side_ok);


	DDX_Control(pDX, IDC_MAGURI_ANGLE_GAP, c_maguri_angle_gap);
	DDX_Control(pDX, IDC_MAGURI_ANGLE_CONTINUOUS, c_maguri_angle_continuous);
	DDX_Control(pDX, IDC_MAGURI_ANGLE_DIFF, c_maguri_angle_diff);
	DDX_Control(pDX, IDC_MID_ANGLE_GAP, c_mid_angle_gap);
	DDX_Control(pDX, IDC_MID_ANGLE_CONTINUOUS, c_mid_angle_continuous);
	DDX_Control(pDX, IDC_MID_ANGLE_DIFF, c_mid_angle_diff);
	DDX_Control(pDX, IDC_MID_ANGLE_HEIGHT_DIFF, c_mid_angle_height_diff);
	DDX_Control(pDX, IDC_TOP_ANGLE_GAP, c_top_angle_gap);
	DDX_Control(pDX, IDC_TOP_ANGLE_CONTINUOUS, c_top_angle_continuous);
	DDX_Control(pDX, IDC_TOP_ANGLE_DIFF, c_top_angle_diff);
	DDX_Control(pDX, IDC_TOP_ANGLE_HEIGHT_DIFF, c_top_angle_height_diff);
	DDX_Control(pDX, IDC_CHUCK_ANGLE_GAP, c_chuck_angle_gap);
	DDX_Control(pDX, IDC_CHUCK_ANGLE_CONTINUOUS, c_chuck_angle_continuous);
	DDX_Control(pDX, IDC_CHUCK_ANGLE_DIFF, c_chuck_angle_diff);
	DDX_Control(pDX, IDC_CHUCK_ANGLE_HEIGHT_DIFF, c_chuck_angle_height_diff);
	DDX_Control(pDX, IDC_INSERT_ANGLE_GAP, c_insert_angle_gap);
	DDX_Control(pDX, IDC_INSERT_ANGLE_CONTINUOUS, c_insert_angle_continuous);
	DDX_Control(pDX, IDC_INSERT_ANGLE_DIFF, c_insert_angle_diff);

	// 찍힘 에디트 컨트롤 값 모음
	DDX_Text(pDX, IDC_MAGURI_ANGLE_GAP, m_maguri_angle_gap);
	DDX_Text(pDX, IDC_MAGURI_ANGLE_CONTINUOUS, m_maguri_angle_continuous);
	DDX_Text(pDX, IDC_MAGURI_ANGLE_DIFF, m_maguri_angle_diff);
	DDX_Text(pDX, IDC_MID_ANGLE_GAP, m_mid_angle_gap);
	DDX_Text(pDX, IDC_MID_ANGLE_CONTINUOUS, m_mid_angle_continuous);
	DDX_Text(pDX, IDC_MID_ANGLE_DIFF, m_mid_angle_diff);
	DDX_Text(pDX, IDC_MID_ANGLE_HEIGHT_DIFF, m_mid_angle_height_diff);
	DDX_Text(pDX, IDC_TOP_ANGLE_GAP, m_top_angle_gap);
	DDX_Text(pDX, IDC_TOP_ANGLE_CONTINUOUS, m_top_angle_continuous);
	DDX_Text(pDX, IDC_TOP_ANGLE_DIFF, m_top_angle_diff);
	DDX_Text(pDX, IDC_TOP_ANGLE_HEIGHT_DIFF, m_top_angle_height_diff);
	DDX_Text(pDX, IDC_CHUCK_ANGLE_GAP, m_chuck_angle_gap);
	DDX_Text(pDX, IDC_CHUCK_ANGLE_CONTINUOUS, m_chuck_angle_continuous);
	DDX_Text(pDX, IDC_CHUCK_ANGLE_DIFF, m_chuck_angle_diff);
	DDX_Text(pDX, IDC_CHUCK_ANGLE_HEIGHT_DIFF, m_chuck_angle_height_diff);
	DDX_Text(pDX, IDC_INSERT_ANGLE_GAP, m_insert_angle_gap);
	DDX_Text(pDX, IDC_INSERT_ANGLE_CONTINUOUS, m_insert_angle_continuous);
	DDX_Text(pDX, IDC_INSERT_ANGLE_DIFF, m_insert_angle_diff);
	DDX_Control(pDX, IDC_MAGURI_MAX_LENGTH, c_maguri_max_length);
	DDX_Text(pDX, IDC_MAGURI_MAX_LENGTH, m_maguri_max_length);
	DDX_Control(pDX, IDC_IMUL_CHUCK_MASK_COLS, c_imul_chuck_mask_cols);
	DDX_Text(pDX, IDC_IMUL_CHUCK_MASK_COLS, m_imul_chuck_mask_cols);

	// 기능 추가한 후 위치에 맞게 정리하기

	// 체크박스
	DDX_Check(pDX, IDC_IMUL_ETQ_LEFT_OK, m_imul_etq_left_ok);
	DDX_Check(pDX, IDC_IMUL_ETQ_RIGHT_OK, m_imul_etq_right_ok);

	// 좌측 에티켓 에딧박스
	DDX_Control(pDX, IDC_IMUL_ETQ_LEFT_PIX, c_imul_etq_left_pix);
	DDX_Control(pDX, IDC_IMUL_ETQ_LEFT_THRESHOLD, c_imul_etq_left_threshold);
	DDX_Control(pDX, IDC_IMUL_ETQ_LEFT_MASK, c_imul_etq_left_mask);

	// 우측 에티켓 에딧박스
	DDX_Control(pDX, IDC_IMUL_ETQ_RIGHT_PIX, c_imul_etq_right_pix);
	DDX_Control(pDX, IDC_IMUL_ETQ_RIGHT_THRESHOLD, c_imul_etq_right_threshold);
	DDX_Control(pDX, IDC_IMUL_ETQ_RIGHT_MASK, c_imul_etq_right_mask);

	// 좌측 이물 파라미터
	DDX_Text(pDX, IDC_IMUL_ETQ_LEFT_PIX, m_imul_etq_left_pix);
	DDX_Text(pDX, IDC_IMUL_ETQ_LEFT_THRESHOLD, m_imul_etq_left_threshold);
	DDX_Text(pDX, IDC_IMUL_ETQ_LEFT_MASK, m_imul_etq_left_mask);

	// 우측 이물 파라미터
	DDX_Text(pDX, IDC_IMUL_ETQ_RIGHT_PIX, m_imul_etq_right_pix);
	DDX_Text(pDX, IDC_IMUL_ETQ_RIGHT_THRESHOLD, m_imul_etq_right_threshold);
	DDX_Text(pDX, IDC_IMUL_ETQ_RIGHT_MASK, m_imul_etq_right_mask);
	//DDX_Control(pDX, IDC_IMUL_TOP_INSP_SIZE, c_imul_top_size);
	//DDX_Control(pDX, IDC_IMUL_TOP_INSP_PER, c_imul_top_per);
	DDX_Control(pDX, IDC_IMUL_TOP_PERSENT, c_imul_top_persent);
	DDX_Control(pDX, IDC_IMUL_MID_PERSENT, c_imul_mid_persent);
	DDX_Control(pDX, IDC_IMUL_TOP_SIZE, c_imul_top_size);
	DDX_Control(pDX, IDC_IMUL_MID_SIZE, c_imul_mid_size);

	DDX_Control(pDX, IDC_IMUL_MID_NOT, c_imul_mid_notinspect);
	DDX_Control(pDX, IDC_INSERT_NOT, c_stabbed_insert_notinspect);
	DDX_Text(pDX, IDC_INSERT_NOT, m_stabbed_insert_notinspect);
	DDX_Control(pDX, IDC_INSERT_PIX, c_stabbed_insert_pix);
	DDX_Text(pDX, IDC_INSERT_PIX, m_stabbed_insert_pix);
}


BEGIN_MESSAGE_MAP(CInspectSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CInspectSettingDlg)
	ON_BN_CLICKED(IDC_MAGURI_OK, OnMaguriOk)
	ON_BN_CLICKED(IDC_Clear, OnClear)
	ON_BN_CLICKED(IDC_CHUCK_OK, OnChuckOk)
	ON_BN_CLICKED(IDC_JABARA_OK, OnJabaraOk)
	ON_BN_CLICKED(IDC_POK_OK, OnPokOk)
	ON_BN_CLICKED(IDC_INSERT_OK, OnInsertOk)
	ON_BN_CLICKED(IDC_ANGLE_1, OnAngle1)
	ON_BN_CLICKED(IDC_ANGLE_2, OnAngle2)
	ON_BN_CLICKED(IDC_IMUL1_OK, OnImul1Ok)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ETIQUETTE_OK, &CInspectSettingDlg::OnEtiquetteOk)
	ON_BN_CLICKED(IDC_IMUL_MID_OK, &CInspectSettingDlg::OnImulMidOk)
	ON_BN_CLICKED(IDC_IMUL_JABARA_OK, &CInspectSettingDlg::OnImulJabaraOk)
	ON_BN_CLICKED(IDC_IMUL_CHUCK_OK, &CInspectSettingDlg::OnImulChuckOk)
	ON_BN_CLICKED(IDC_IMUL_INSERT_OK, &CInspectSettingDlg::OnImulInsertOk)
	ON_BN_CLICKED(IDC_JABARA_EDGE_OK, &CInspectSettingDlg::OnJabaraEdgeOk)
	
	ON_BN_CLICKED(IDC_MAGURI_ANGLE_OK, &CInspectSettingDlg::OnBnClickedMaguriAngleOk)
	ON_BN_CLICKED(IDC_MID_ANGLE_OK, &CInspectSettingDlg::OnBnClickedMidAngleOk)
	ON_BN_CLICKED(IDC_TOP_ANGLE_OK, &CInspectSettingDlg::OnBnClickedTopAngleOk)
	ON_BN_CLICKED(IDC_CHUCK_ANGLE_OK, &CInspectSettingDlg::OnBnClickedChuckAngleOk)
	ON_BN_CLICKED(IDC_INSERT_ANGLE_OK, &CInspectSettingDlg::OnBnClickedInsertAngleOk)
	ON_BN_CLICKED(IDC_MAGURI_SIDE_OK, &CInspectSettingDlg::OnBnClickedMaguriSideOk)
	ON_BN_CLICKED(IDC_IMUL_ETQ_LEFT_OK, &CInspectSettingDlg::OnImulEtqLeftOk)
	ON_BN_CLICKED(IDC_IMUL_ETQ_RIGHT_OK, &CInspectSettingDlg::OnImulEtqRightOk)
	ON_BN_CLICKED(IDRESET, &CInspectSettingDlg::OnBnClickedReset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectSettingDlg message handlers

void CInspectSettingDlg::OnMaguriOk() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	if (m_maguri_ok)
	{
		c_maguri_length.EnableWindow(true);
		c_maguri_diff_ratio.EnableWindow(true);
		c_maguri_max_length.EnableWindow(true);

		if(m_maguri_angle_ok) this->OnBnClickedMaguriAngleOk();
		if(m_mid_angle_ok)	  this->OnBnClickedMidAngleOk();	
		if(m_top_angle_ok)	  this->OnBnClickedTopAngleOk();

		// legacy
		//c_maguri_slide.EnableWindow(true);
	}
	else
	{
		c_maguri_length.EnableWindow(false);
		c_maguri_diff_ratio.EnableWindow(false);
		c_maguri_max_length.EnableWindow(false);
		
		this->OnBnClickedMaguriAngleOk();
		this->OnBnClickedMidAngleOk();
		this->OnBnClickedTopAngleOk();

		// legacy
		//c_maguri_slide.EnableWindow(false);
	}
	UpdateData(FALSE);
}

void CInspectSettingDlg::OnClear() 
{
	// 모든 변수 초기화시킴
	UpdateData(TRUE);
	m_angle1_limit = 0.0;
	m_angle1_ok = false;
	m_angle2_limit = 0.0;
	m_anglew_ok = false;
	m_chuck_ok = false;
	
	m_imul1_ok		 = FALSE;
	m_imul_mid_ok	 = FALSE;
	m_imul_jabara_ok = FALSE;
	m_imul_chuck_ok  = FALSE;
	m_imul_insert_ok = FALSE;
	m_imul_etq_left_ok = FALSE;
	m_imul_etq_right_ok = FALSE;

	m_imul_pix = 0.0;
	m_imul_mid_pix = 0;
	m_imul_jabara_pix = 0;
	m_imul_chuck_pix = 0;
	m_imul_insert_pix = 0;
	m_imul_etq_left_pix = 0;
	m_imul_etq_right_pix = 0;

	m_imul_top_threshold = 0;
	m_imul_mid_threshold = 0;
	m_imul_jabara_threshold = 0;
	m_imul_chuck_threshold = 0;
	m_imul_insert_threshold = 0;
	m_imul_etq_left_threshold = 0;
	m_imul_etq_right_threshold = 0;

	m_imul_top_mask = 0;
	m_imul_mid_mask = 0;
	m_imul_jabara_mask = 0;
	m_imul_chuck_mask = 0;
	m_imul_chuck_mask_cols = 0;
	m_imul_insert_mask = 0;
	m_imul_insert_pos = 0;
	m_imul_etq_left_mask = 0;
	m_imul_etq_right_mask = 0;

	m_insert_ok = false;
	m_insert_slide = 0.0;

	m_jabara_recognize = 0.0;
	m_jabara_num = 0;
	m_jabara_margin_of_error = 0;
	m_jabara_width_stdev = 0.0;
	m_jabara_distance_stdev = 0.0;
	m_jabara_gradient_stdev = 0.0;
	m_jabara_neighbor = 0;
	m_jabara_ok = FALSE;
	m_jabara_edge_ok = FALSE;

	m_etiquette_ok = false;
	m_etiquette_recognize = 0.0;
	m_etiquette_position = 0.0;
	m_etiquette_size = 0;

	m_maguri_ok = false;
	m_maguri_length = 0;
	m_maguri_max_length = 0;
	m_maguri_slide = 0.0;
	m_minchuck = 0.0;
	m_maxchuck = 0.0;
	m_pok = 0.0;
	m_pok_limit = 0.0;
	m_pok_ok = false;
	m_resol = 0.12;
	m_slide_length = 0;

	// 추가된 사항
	m_camWidth = 0;
	m_camHeight = 0;
	m_triggerPerPitch = 0;
	m_inspPitch = 0;

	m_imul_top_persent = 0;
	m_imul_mid_persent = 0;

	m_imul_top_size = 0;
	m_imul_mid_size = 0;

	m_imul_mid_notinspect = 0;

	m_stabbed_insert_notinspect = 0;
	m_stabbed_insert_pix = 0;

	UpdateData(FALSE);
}

INT_PTR CInspectSettingDlg::DoModal() 
{
	// 구조체에 복사
	m_backup.m_angle1_limit = m_angle1_limit;
	m_backup.m_angle1_ok = m_angle1_ok;
	m_backup.m_anglew_ok = m_anglew_ok;
	m_backup.m_chuck_ok = m_chuck_ok;
	m_backup.m_angle2_limit = m_angle2_limit;

	// 이물
	m_backup.m_imul1_ok = m_imul1_ok;
	m_backup.m_imul_mid_ok = m_imul_mid_ok;
	m_backup.m_imul_jabara_ok = m_imul_jabara_ok;
	m_backup.m_imul_chuck_ok = m_imul_chuck_ok;
	m_backup.m_imul_insert_ok = m_imul_insert_ok;
	m_backup.m_imul_etq_left_ok = m_imul_etq_left_ok;
	m_backup.m_imul_etq_right_ok = m_imul_etq_right_ok;

	m_backup.m_imul_pix = m_imul_pix;
	m_backup.m_imul_mid_pix = m_imul_mid_pix;
	m_backup.m_imul_jabara_pix = m_imul_jabara_pix;
	m_backup.m_imul_chuck_pix = m_imul_chuck_pix;
	m_backup.m_imul_insert_pix = m_imul_insert_pix;
	m_backup.m_imul_etq_left_pix = m_imul_etq_left_pix;
	m_backup.m_imul_etq_right_pix = m_imul_etq_right_pix;
	
	m_backup.m_imul_top_threshold = m_imul_top_threshold;
	m_backup.m_imul_mid_threshold = m_imul_mid_threshold;
	m_backup.m_imul_jabara_threshold = m_imul_jabara_threshold;
	m_backup.m_imul_chuck_threshold = m_imul_chuck_threshold;
	m_backup.m_imul_insert_threshold = m_imul_insert_threshold;
	m_backup.m_imul_etq_left_threshold = m_imul_etq_left_threshold;
	m_backup.m_imul_etq_right_threshold = m_imul_etq_right_threshold;
	
	m_backup.m_imul_top_mask = m_imul_top_mask;
	m_backup.m_imul_top_mask_cols = m_imul_top_mask_cols;
	m_backup.m_imul_mid_mask = m_imul_mid_mask;
	m_backup.m_imul_jabara_mask = m_imul_jabara_mask;
	m_backup.m_imul_chuck_mask = m_imul_chuck_mask;
	m_backup.m_imul_chuck_mask_cols = m_imul_chuck_mask_cols;
	m_backup.m_imul_insert_mask = m_imul_insert_mask;
	m_backup.m_imul_insert_pos = m_imul_insert_pos;
	m_backup.m_imul_etq_left_mask = m_imul_etq_left_mask;
	m_backup.m_imul_etq_right_mask = m_imul_etq_right_mask;

	m_backup.m_insert_ok = m_insert_ok;
	m_backup.m_insert_slide = m_insert_slide;

	m_backup.m_jabara_ok = m_jabara_ok;
	m_backup.m_jabara_edge_ok = m_jabara_edge_ok;
	m_backup.m_jabara_recognize = m_jabara_recognize;
	m_backup.m_jabara_num = m_jabara_num;
	m_backup.m_jabara_margin_of_error = m_jabara_margin_of_error;
	m_backup.m_jabara_width_stdev = m_jabara_width_stdev;
	m_backup.m_jabara_distance_stdev = m_jabara_distance_stdev;
	m_backup.m_jabara_gradient_stdev = m_jabara_gradient_stdev;
	m_backup.m_jabara_neighbor = m_jabara_neighbor;

	m_backup.m_etiquette_ok = m_etiquette_ok;
	m_backup.m_etiquette_recognize = m_etiquette_recognize;
	m_backup.m_etiquette_position = m_etiquette_position;
	m_backup.m_etiquette_size = m_etiquette_size;

	m_backup.m_maguri_length = m_maguri_length;
	m_backup.m_maguri_max_length = m_maguri_max_length;
	m_backup.m_maguri_ok = m_maguri_ok;
	m_backup.m_maguri_slide = m_maguri_slide;
	m_backup.m_minchuck = m_minchuck;
	m_backup.m_maxchuck = m_maxchuck;
	m_backup.m_pok = m_pok;
	m_backup.m_pok_limit = m_pok_limit;
	m_backup.m_pok_ok = m_pok_ok;
	m_backup.m_resol = m_resol;
	m_backup.m_slide_length = m_slide_length;

	m_backup.m_camWidth = m_camWidth;
	m_backup.m_camHeight = m_camHeight;
	m_backup.m_triggerPerPitch = m_triggerPerPitch;
	m_backup.m_inspPitch = m_inspPitch;

	m_backup.m_maguri_diff_ratio = m_maguri_diff_ratio;
	m_backup.m_maguri_angle_gap = m_maguri_angle_gap;
	m_backup.m_maguri_angle_continuous = m_maguri_angle_continuous;
	m_backup.m_maguri_angle_diff = m_maguri_angle_diff;
	m_backup.m_mid_angle_gap = m_mid_angle_gap;
	m_backup.m_mid_angle_continuous = m_mid_angle_continuous;
	m_backup.m_mid_angle_diff = m_mid_angle_diff;
	m_backup.m_mid_angle_height_diff = m_mid_angle_height_diff;
	m_backup.m_top_angle_gap = m_top_angle_gap;
	m_backup.m_top_angle_continuous = m_top_angle_continuous;
	m_backup.m_top_angle_diff = m_top_angle_diff;
	m_backup.m_top_angle_height_diff = m_top_angle_height_diff;
	m_backup.m_chuck_angle_gap = m_chuck_angle_gap;
	m_backup.m_chuck_angle_continuous = m_chuck_angle_continuous;
	m_backup.m_chuck_angle_diff = m_chuck_angle_diff;
	m_backup.m_chuck_angle_height_diff = m_chuck_angle_height_diff;
	m_backup.m_insert_angle_gap = m_insert_angle_gap;
	m_backup.m_insert_angle_continuous = m_insert_angle_continuous;
	m_backup.m_insert_angle_diff = m_insert_angle_diff;

	m_backup.m_imul_top_persent = m_imul_top_persent;
	m_backup.m_imul_mid_persent = m_imul_mid_persent;
	m_backup.m_imul_top_size = m_imul_top_size;
	m_backup.m_imul_mid_size = m_imul_mid_size;

	m_backup.m_imul_mid_notinspect = m_imul_mid_notinspect;

	m_backup.m_stabbed_insert_notinspect = m_stabbed_insert_notinspect;
	m_backup.m_stabbed_insert_pix = m_stabbed_insert_pix;

	return CDialog::DoModal();
}

void CInspectSettingDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	UpdateData(TRUE);

	// 원래대로 되돌림
	m_angle1_limit = m_backup.m_angle1_limit;
	m_angle1_ok = m_backup.m_angle1_ok;
	m_anglew_ok = m_backup.m_anglew_ok;
	m_chuck_ok = m_backup.m_chuck_ok;

	m_imul1_ok = m_backup.m_imul1_ok;
	m_imul_mid_ok = m_backup.m_imul_mid_ok;
	m_imul_jabara_ok = m_backup.m_imul_jabara_ok;
	m_imul_chuck_ok = m_backup.m_imul_chuck_ok;
	m_imul_insert_ok = m_backup.m_imul_insert_ok;
	m_imul_etq_left_ok = m_backup.m_imul_etq_left_ok;
	m_imul_etq_right_ok = m_backup.m_imul_etq_right_ok;

	m_imul_pix = m_backup.m_imul_pix;
	m_imul_mid_pix = m_backup.m_imul_mid_pix;
	m_imul_jabara_pix = m_backup.m_imul_jabara_pix;
	m_imul_chuck_pix = m_backup.m_imul_chuck_pix;
	m_imul_insert_pix = m_backup.m_imul_insert_pix;
	m_imul_etq_left_pix = m_backup.m_imul_etq_left_pix;
	m_imul_etq_right_pix = m_backup.m_imul_etq_right_pix;

	m_imul_top_threshold = m_backup.m_imul_top_threshold;
	m_imul_mid_threshold = m_backup.m_imul_mid_threshold;
	m_imul_jabara_threshold = m_backup.m_imul_jabara_threshold;
	m_imul_chuck_threshold = m_backup.m_imul_chuck_threshold;
	m_imul_insert_threshold = m_backup.m_imul_insert_threshold;
	m_imul_etq_left_threshold = m_backup.m_imul_etq_left_threshold;
	m_imul_etq_right_threshold = m_backup.m_imul_etq_right_threshold;

	m_imul_top_mask = m_backup.m_imul_top_mask;
	m_imul_top_mask_cols = m_backup.m_imul_top_mask_cols;
	m_imul_mid_mask = m_backup.m_imul_mid_mask;
	m_imul_jabara_mask = m_backup.m_imul_jabara_mask;
	m_imul_chuck_mask = m_backup.m_imul_chuck_mask;
	m_imul_chuck_mask_cols = m_backup.m_imul_chuck_mask_cols;
	m_imul_insert_mask = m_backup.m_imul_insert_mask;
	m_imul_insert_pos = m_backup.m_imul_insert_pos;
	m_imul_etq_left_mask = m_backup.m_imul_etq_left_mask;
	m_imul_etq_right_mask = m_backup.m_imul_etq_right_mask;
	
	m_insert_ok = m_backup.m_insert_ok;
	m_insert_slide = m_backup.m_insert_slide;

	m_etiquette_ok = m_backup.m_etiquette_ok;
	m_etiquette_recognize = m_backup.m_etiquette_recognize;
	m_etiquette_position = m_backup.m_etiquette_position;
	m_etiquette_size = m_backup.m_etiquette_size;

	m_jabara_ok = m_backup.m_jabara_ok;
	m_jabara_edge_ok = m_backup.m_jabara_edge_ok;
	m_jabara_recognize = m_backup.m_jabara_recognize;
	m_jabara_num = m_backup.m_jabara_num;
	m_jabara_margin_of_error = m_backup.m_jabara_margin_of_error;
	m_jabara_width_stdev = m_backup.m_jabara_width_stdev;
	m_jabara_distance_stdev = m_backup.m_jabara_distance_stdev;
	m_jabara_gradient_stdev = m_backup.m_jabara_gradient_stdev;
	m_jabara_neighbor = m_backup.m_jabara_neighbor;

	m_maguri_length = m_backup.m_maguri_length;
	m_maguri_max_length = m_backup.m_maguri_max_length;
	m_maguri_ok = m_backup.m_maguri_ok;
	m_maguri_slide = m_backup.m_maguri_slide;
	m_minchuck = m_backup.m_minchuck;
	m_maxchuck = m_backup.m_maxchuck;
	m_pok = m_backup.m_pok;
	m_pok_limit = m_backup.m_pok_limit;
	m_pok_ok = m_backup.m_pok_ok;
	m_resol = m_backup.m_resol;
	m_slide_length = m_backup.m_slide_length;
	
	
	m_camWidth = m_backup.m_camWidth;
	m_camHeight = m_backup.m_camHeight;
	m_triggerPerPitch = m_backup.m_triggerPerPitch;
	m_inspPitch = m_backup.m_inspPitch;

	m_maguri_diff_ratio = m_backup.m_maguri_diff_ratio;
	m_maguri_angle_gap = m_backup.m_maguri_angle_gap;
	m_maguri_angle_continuous = m_backup.m_maguri_angle_continuous;
	m_maguri_angle_diff = m_backup.m_maguri_angle_diff;
	m_mid_angle_gap = m_backup.m_mid_angle_gap;
	m_mid_angle_continuous = m_backup.m_mid_angle_continuous;
	m_mid_angle_diff = m_backup.m_mid_angle_diff;
	m_mid_angle_height_diff = m_backup.m_mid_angle_height_diff;
	m_top_angle_gap = m_backup.m_top_angle_gap;
	m_top_angle_continuous = m_backup.m_top_angle_continuous;
	m_top_angle_diff = m_backup.m_top_angle_diff;
	m_top_angle_height_diff = m_backup.m_top_angle_height_diff;
	m_chuck_angle_gap = m_backup.m_chuck_angle_gap;
	m_chuck_angle_continuous = m_backup.m_chuck_angle_continuous;
	m_chuck_angle_diff = m_backup.m_chuck_angle_diff;
	m_chuck_angle_height_diff = m_backup.m_chuck_angle_height_diff;
	m_insert_angle_gap = m_backup.m_insert_angle_gap;
	m_insert_angle_continuous = m_backup.m_insert_angle_continuous;
	m_insert_angle_diff = m_backup.m_insert_angle_diff;

	m_imul_top_persent = m_backup.m_imul_top_persent;
	m_imul_mid_persent = m_backup.m_imul_mid_persent;
	m_imul_top_size = m_backup.m_imul_top_size;
	m_imul_mid_size = m_backup.m_imul_mid_size;

	m_imul_mid_notinspect = m_backup.m_imul_mid_notinspect;
	
	m_stabbed_insert_notinspect = m_backup.m_stabbed_insert_notinspect;
	m_stabbed_insert_pix = m_backup.m_stabbed_insert_pix;
	
	UpdateData(FALSE);

	CDialog::OnCancel();
}

void CInspectSettingDlg::OnChuckOk() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_chuck_ok)
	{
		c_minchuck.EnableWindow( true );
		c_maxchuck.EnableWindow( true );

		OnBnClickedMidAngleOk();
		OnBnClickedTopAngleOk();
		OnBnClickedChuckAngleOk();
	}
	else
	{
		c_minchuck.EnableWindow( false );
		c_maxchuck.EnableWindow( false );

		OnBnClickedMidAngleOk();
		OnBnClickedTopAngleOk();
		OnBnClickedChuckAngleOk();
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnJabaraOk() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_jabara_ok)
	{
		c_jabara_recognize.EnableWindow( true );
		c_jabara_num.EnableWindow( true );
		c_jabara_width_stdev.EnableWindow( true );
		c_jabara_distance_stdev.EnableWindow( true );
		c_jabara_margin_of_error.EnableWindow( true );

		if ( m_jabara_edge_ok )
		{
			c_jabara_gradient_stdev.EnableWindow( true );
			c_jabara_neighbor.EnableWindow( true );
		}
		else
		{
			c_jabara_gradient_stdev.EnableWindow( false );
			c_jabara_neighbor.EnableWindow( false );
		}

		OnBnClickedMidAngleOk();
		OnBnClickedTopAngleOk();
	}
	else
	{
		c_jabara_recognize.EnableWindow( false );
		c_jabara_num.EnableWindow( false );
		c_jabara_width_stdev.EnableWindow( false);
		c_jabara_distance_stdev.EnableWindow(false);
		c_jabara_gradient_stdev.EnableWindow(false);
		c_jabara_neighbor.EnableWindow(false);
		c_jabara_margin_of_error.EnableWindow(false);

		OnBnClickedMidAngleOk();
		OnBnClickedTopAngleOk();
	}

	UpdateData(FALSE);	
}

void CInspectSettingDlg::OnPokOk() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_pok_ok)
	{
		c_pok.EnableWindow(true);
		c_pok_limit.EnableWindow(true);
	}

	else
	{
		c_pok.EnableWindow(false);
		c_pok_limit.EnableWindow(false);
	}

	UpdateData(FALSE);		
}

void CInspectSettingDlg::OnInsertOk() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_insert_ok)
	{
		c_insert_slide.EnableWindow(true);
		c_slide_length.EnableWindow(true);

		OnBnClickedInsertAngleOk();
	}
	else
	{
		c_insert_slide.EnableWindow(false);
		c_slide_length.EnableWindow(false);
		
		OnBnClickedInsertAngleOk();
	}

	UpdateData(FALSE);			
}

void CInspectSettingDlg::OnAngle1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_angle1_ok)
	{
		c_anglelimit_1.EnableWindow(true);
	}
	else
	{
		c_anglelimit_1.EnableWindow(false);
	}

	if (m_angle1_ok && m_anglew_ok)
	{
		c_angle_diff.EnableWindow(true);
	}
	else
	{
		c_angle_diff.EnableWindow(false);
	}

	UpdateData(FALSE);	
}

void CInspectSettingDlg::OnAngle2() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_anglew_ok)
	{
		c_anglelimit_2.EnableWindow(true);
	}
	else
	{
		c_anglelimit_2.EnableWindow(false);
	}

	if (m_angle1_ok && m_anglew_ok)
	{
		c_angle_diff.EnableWindow(true);
	}
	else
	{
		c_angle_diff.EnableWindow(false);
	}

	UpdateData(FALSE);	
}


void CInspectSettingDlg::OnEtiquetteOk()
{
	UpdateData(TRUE);

	if(this->m_etiquette_ok)
	{
		this->c_etiquette_recognize.EnableWindow( true );
		this->c_etiquette_position.EnableWindow( true );
		this->c_etiquette_size.EnableWindow( true );
	}
	else
	{
		this->c_etiquette_recognize.EnableWindow( false );
		this->c_etiquette_position.EnableWindow( false );
		this->c_etiquette_size.EnableWindow( false );
	}

	UpdateData(FALSE);
}


void CInspectSettingDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	OnMaguriOk();
	OnInsertOk();
	OnChuckOk();
	OnJabaraOk();
	OnPokOk();
	OnAngle1();
	OnAngle2();
	OnEtiquetteOk();

	OnImul1Ok();
	OnImulMidOk();
	OnImulJabaraOk();
	OnImulChuckOk();
	OnImulInsertOk();
	OnImulEtqLeftOk();
	OnImulEtqRightOk();

	OnBnClickedMaguriAngleOk();
	OnBnClickedMidAngleOk();
	OnBnClickedTopAngleOk();
	OnBnClickedChuckAngleOk();
	OnBnClickedInsertAngleOk();
}

void CInspectSettingDlg::OnImul1Ok() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	if (m_imul1_ok)
	{
		this->c_imul_pix.EnableWindow( true );
		this->c_imul_top_threshold.EnableWindow( true );
		this->c_imul_top_mask.EnableWindow( true );
		this->c_imul_top_mask_cols.EnableWindow( true );
		this->c_imul_top_persent.EnableWindow( true );
		this->c_imul_top_size.EnableWindow( true );
	}
	else
	{
		this->c_imul_pix.EnableWindow( false );
		this->c_imul_top_threshold.EnableWindow( false );
		this->c_imul_top_mask.EnableWindow( false );
		this->c_imul_top_mask_cols.EnableWindow( false );
		this->c_imul_top_persent.EnableWindow( false );
		this->c_imul_top_size.EnableWindow( false );
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnImulMidOk()
{
	UpdateData(TRUE);

	if ( m_imul_mid_ok )
	{
		this->c_imul_mid_pix.EnableWindow( true );
		this->c_imul_mid_threshold.EnableWindow( true );
		this->c_imul_mid_mask.EnableWindow( true );
		this->c_imul_mid_persent.EnableWindow( true );
		this->c_imul_mid_size.EnableWindow( true );
		this->c_imul_mid_notinspect.EnableWindow( true );

	}
	else
	{
		this->c_imul_mid_pix.EnableWindow( false );
		this->c_imul_mid_threshold.EnableWindow( false );
		this->c_imul_mid_mask.EnableWindow( false );
		this->c_imul_mid_persent.EnableWindow( false );
		this->c_imul_mid_size.EnableWindow( false );
		this->c_imul_mid_notinspect.EnableWindow( false );
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnImulJabaraOk()
{
	UpdateData(TRUE);

	if ( m_imul_jabara_ok )
	{
		this->c_imul_jabara_pix.EnableWindow( true );
		this->c_imul_jabara_threshold.EnableWindow( true );
		this->c_imul_jabara_mask.EnableWindow( true );
	}
	else
	{
		this->c_imul_jabara_pix.EnableWindow( false );
		this->c_imul_jabara_threshold.EnableWindow( false );
		this->c_imul_jabara_mask.EnableWindow( false );
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnImulChuckOk()
{
	UpdateData(TRUE);

	if ( m_imul_chuck_ok )
	{
		this->c_imul_chuck_pix.EnableWindow( true );
		this->c_imul_chuck_threshold.EnableWindow( true );
		this->c_imul_chuck_mask.EnableWindow( true );
		this->c_imul_chuck_mask_cols.EnableWindow( true );
	}
	else
	{
		this->c_imul_chuck_pix.EnableWindow( false );
		this->c_imul_chuck_threshold.EnableWindow( false );
		this->c_imul_chuck_mask.EnableWindow( false );
		this->c_imul_chuck_mask_cols.EnableWindow( false );
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnImulInsertOk()
{
	UpdateData(TRUE);

	if ( m_imul_insert_ok )
	{
		this->c_imul_insert_pix.EnableWindow( true );
		this->c_imul_insert_threshold.EnableWindow( true );
		this->c_imul_insert_mask.EnableWindow( true );
		this->c_imul_insert_pos.EnableWindow( true );
	}
	else
	{
		this->c_imul_insert_pix.EnableWindow( false );
		this->c_imul_insert_threshold.EnableWindow( false );
		this->c_imul_insert_mask.EnableWindow( false );
		this->c_imul_insert_pos.EnableWindow( false );
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnJabaraEdgeOk()
{
	UpdateData( TRUE );

	if ( m_jabara_ok && m_jabara_edge_ok )
	{
		this->c_jabara_gradient_stdev.EnableWindow( true );
		this->c_jabara_neighbor.EnableWindow( true );
	}
	else
	{
		this->c_jabara_gradient_stdev.EnableWindow( false );
		this->c_jabara_neighbor.EnableWindow( false );
	}

	UpdateData( FALSE );
}
void CInspectSettingDlg::OnBnClickedMaguriSideOk()
{
	UpdateData(TRUE);

	if(!m_maguri_ok)
	{
		m_maguri_side_ok = FALSE;
	}

	UpdateData( FALSE );
}

void CInspectSettingDlg::OnBnClickedMaguriAngleOk()
{
	UpdateData(TRUE);

	if(m_maguri_angle_ok && m_maguri_ok)
	{
		c_maguri_angle_continuous.EnableWindow(TRUE);
		c_maguri_angle_diff.EnableWindow(TRUE);
		c_maguri_angle_gap.EnableWindow(TRUE);
	}
	else
	{
		c_maguri_angle_continuous.EnableWindow(FALSE);
		c_maguri_angle_diff.EnableWindow(FALSE);
		c_maguri_angle_gap.EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnBnClickedMidAngleOk()
{
	UpdateData(TRUE);

	if(m_mid_angle_ok && m_chuck_ok && (m_maguri_ok || m_jabara_ok))
	{
		c_mid_angle_continuous.EnableWindow(TRUE);
		c_mid_angle_diff.EnableWindow(TRUE);
		c_mid_angle_gap.EnableWindow(TRUE);
		c_mid_angle_height_diff.EnableWindow(TRUE);
	}
	else
	{
		c_mid_angle_continuous.EnableWindow(FALSE);
		c_mid_angle_diff.EnableWindow(FALSE);
		c_mid_angle_gap.EnableWindow(FALSE);
		c_mid_angle_height_diff.EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnBnClickedTopAngleOk()
{
	UpdateData(TRUE);

	if(m_top_angle_ok && m_maguri_ok && (m_jabara_ok || m_chuck_ok))
	{
		c_top_angle_continuous.EnableWindow(TRUE);
		c_top_angle_diff.EnableWindow(TRUE);
		c_top_angle_gap.EnableWindow(TRUE);
		c_top_angle_height_diff.EnableWindow(TRUE);
	}
	else
	{
		c_top_angle_continuous.EnableWindow(FALSE);
		c_top_angle_diff.EnableWindow(FALSE);
		c_top_angle_gap.EnableWindow(FALSE);
		c_top_angle_height_diff.EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnBnClickedChuckAngleOk()
{
	UpdateData(TRUE);

	if(m_chuck_angle_ok && m_chuck_ok)
	{
		c_chuck_angle_continuous.EnableWindow(TRUE);
		c_chuck_angle_diff.EnableWindow(TRUE);
		c_chuck_angle_gap.EnableWindow(TRUE);
		c_chuck_angle_height_diff.EnableWindow(TRUE);
	}
	else
	{
		c_chuck_angle_continuous.EnableWindow(FALSE);
		c_chuck_angle_diff.EnableWindow(FALSE);
		c_chuck_angle_gap.EnableWindow(FALSE);
		c_chuck_angle_height_diff.EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnBnClickedInsertAngleOk()
{
	UpdateData(TRUE);

	if(m_insert_angle_ok && m_insert_ok)
	{
		c_insert_angle_continuous.EnableWindow(TRUE);
		c_insert_angle_diff.EnableWindow(TRUE);
		c_insert_angle_gap.EnableWindow(TRUE);
		c_stabbed_insert_notinspect.EnableWindow(TRUE);
		c_stabbed_insert_pix.EnableWindow(TRUE);
	}
	else
	{
		c_insert_angle_continuous.EnableWindow(FALSE);
		c_insert_angle_diff.EnableWindow(FALSE);
		c_insert_angle_gap.EnableWindow(FALSE);
		c_stabbed_insert_notinspect.EnableWindow(FALSE);
		c_stabbed_insert_pix.EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnImulEtqLeftOk()
{
	UpdateData(TRUE);

	if(m_imul_etq_left_ok)
	{
		c_imul_etq_left_pix.EnableWindow(TRUE);
		c_imul_etq_left_threshold.EnableWindow(TRUE);
		c_imul_etq_left_mask.EnableWindow(TRUE);
	}
	else
	{
		c_imul_etq_left_pix.EnableWindow(FALSE);
		c_imul_etq_left_threshold.EnableWindow(FALSE);
		c_imul_etq_left_mask.EnableWindow(FALSE);
	}


	UpdateData(FALSE);
}

void CInspectSettingDlg::OnImulEtqRightOk()
{
	UpdateData(TRUE);

	if(m_imul_etq_right_ok)
	{
		c_imul_etq_right_pix.EnableWindow(TRUE);
		c_imul_etq_right_threshold.EnableWindow(TRUE);
		c_imul_etq_right_mask.EnableWindow(TRUE);
	}
	else
	{
		c_imul_etq_right_pix.EnableWindow(FALSE);
		c_imul_etq_right_threshold.EnableWindow(FALSE);
		c_imul_etq_right_mask.EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}

void CInspectSettingDlg::OnBnClickedReset()
{
	OnClear();
}
