// InspectResultDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SeoilVision.h"
#include "InspectResultDlg.h"
#include "Extern.h"


// CInspectResultDlg dialog

IMPLEMENT_DYNAMIC(CInspectResultDlg, CDialog)

CInspectResultDlg::CInspectResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInspectResultDlg::IDD, pParent)
{
	::AfxInitRichEdit2();
}

CInspectResultDlg::~CInspectResultDlg()
{
	
}

INT_PTR CInspectResultDlg::DoModal(RESULT_INSPVALUE *pInspResult)
{
	for ( int i = 0; i < _MAXPIT; i++ )
	{
		memcpy( &m_inspResult[i], &pInspResult[i], sizeof( RESULT_INSPVALUE ) );
	}

	return CDialog::DoModal();
}

void CInspectResultDlg::ClearInspResult()
{
}

void CInspectResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_INSP_RESULT, m_ctrREditInspResult);
}


BEGIN_MESSAGE_MAP(CInspectResultDlg, CDialog)
END_MESSAGE_MAP()

BOOL CInspectResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strMsg = _T("");
	CString tmpMsg;

	for ( int i = 0; i < _MAXPIT; i++ )
	{
		tmpMsg.Format( _T("--------%d번 검사 결과--------\n"), i + 1 );
		strMsg += tmpMsg;

		//tmpMsg.Format( _T("[좌측각] : %.3f\n"), 
		//	m_inspResult[i].Rangle[0] );
		//strMsg += tmpMsg;
		
		//tmpMsg.Format( _T("[우측각] : %.3f\n"), 
		//	m_inspResult[i].Rangle[1]);
		//strMsg += tmpMsg;

		//tmpMsg.Format( _T("[폭] : %.3f(%.3f)\n"), 
		//	m_inspResult[i].Rpok[0], 
		//	m_inspResult[i].Rpok[1]);
		//strMsg += tmpMsg;
		
		//tmpMsg.Format( _T("[마구리] Up count : %d, Down count : %d, Up+Down > 검사 수치 비교 : %d, 투명 내측 삐짐 %d(한도 5)\n"),
		//	m_inspResult[i].Rmaguri[0], 
		//	m_inspResult[i].Rmaguri[1],
		//	m_inspResult[i].Rmaguri[2], 
		//	m_inspResult[i].Rmaguri[3] );
		//strMsg += tmpMsg;

		tmpMsg.Format(_T("[상단 각도上端角度] 폭寬 : %d, 각도角度 : %d, 최소 폭最小寬度 : %d, 최대 폭最大寬度 : %d\n"),
			m_inspResult[i].top.isHeightOk,
			m_inspResult[i].top.isAngleOk,
			m_inspResult[i].top.minHeight,
			m_inspResult[i].top.maxHeight);
		strMsg += tmpMsg;

		tmpMsg.Format(_T("[중단 각도中端角度] 폭寬 : %d, 각도角度 : %d, 최소 폭最小寬度 : %d, 최대 폭最大寬度 : %d\n"),
			m_inspResult[i].mid.isHeightOk,
			m_inspResult[i].mid.isAngleOk,
			m_inspResult[i].mid.minHeight,
			m_inspResult[i].mid.maxHeight);
		strMsg += tmpMsg;

		tmpMsg.Format(_T("[마구리端口] 위上: %d, 아래下: %d, 합合: %d, 최소最小: %d, 최대最大: %d, 옆면側面: %d, 각도角度: %d\n"), 
			m_inspResult[i].maguri.topLen,
			m_inspResult[i].maguri.botLen,
			m_inspResult[i].maguri.topLen + m_inspResult[i].maguri.botLen,
			m_inspResult[i].maguri.inputMinLength,
			m_inspResult[i].maguri.inputMaxLength,
			m_inspResult[i].maguri.isSideOk,
			m_inspResult[i].maguri.isAngleOk);
		strMsg += tmpMsg;

		tmpMsg.Format( _T("[축관缩管] 축관 길이長度 : %.3f mm, (최소最小: %.3f mm, 최대最大: %3.f mm), 각도角度 : %d, 폭寬 : %d, 최소 폭最小寬度: %d, 최대 폭最大寬度: %d\n"), 
			m_inspResult[i].Rchuck[0], 
			m_inspResult[i].Rchuck[1],
			m_inspResult[i].Rchuck[2],
			m_inspResult[i].chuck.isAngleOk,
			m_inspResult[i].chuck.isHeightOk,
			m_inspResult[i].chuck.minHeight,
			m_inspResult[i].chuck.maxHeight);
		strMsg += tmpMsg;

		tmpMsg.Format( _T("[삽입插入] 상단 끝 좌표上端尖座標 : %d, 하단 끝 좌표下端尖座標 : %d(상단 찍힘 측정 좌표上端壓傷測量座標 : %d, 하단 찍힘 측정 좌표下端壓傷測量座標 : %d) 측정 각도測定角度 : %lf, Center Over: %d\n"), 
			(int)m_inspResult[i].Rinsult[0], 
			(int)m_inspResult[i].Rinsult[1], 
			(int)m_inspResult[i].Rinsult[2], 
			(int)m_inspResult[i].Rinsult[3],
			m_inspResult[i].Rinsult[4], 
			(int)m_inspResult[i].Rinsult[5]);
		strMsg += tmpMsg;

		tmpMsg.Format( _T("[자바라褶皱] 개수數量: %d, 두께 표준편차厚度標準偏差: %.3lf, 거리 표준편차距離標準偏差: %.3lf, 기울기 표준편차斜度標準偏差: %.3lf, 높이 차이高度差異: %d\n"), 
			m_inspResult[i].jabara_num, 
			m_inspResult[i].jabara_width_stdev,
			m_inspResult[i].jabara_distance_stdev, 
			m_inspResult[i].jabara_gradient_stdev,
			m_inspResult[i].jabara_neighbor);
		strMsg += tmpMsg;

		tmpMsg.Format( _T("[에티켓 검출 여부使用凹槽检查] - (1:유是, 0:무否) : %d,   위치: %d\n"), 
			m_inspResult[i].REtiquet[0],
			m_inspResult[i].REtiquet[1]
			);
		strMsg += tmpMsg;

		tmpMsg.Format( _T("[이물 개수異物數量] - 상단上端: %d, 중단中端: %d, 자바라褶皱: %d, 축관缩管: %d, 내측內管: %d\n"), 
			m_inspResult[i].Rimul[0][0], 
			m_inspResult[i].Rimul[1][0], 
			m_inspResult[i].Rimul[2][0],
			m_inspResult[i].Rimul[3][0],
			m_inspResult[i].Rimul[4][0] );
		strMsg += tmpMsg;

		tmpMsg.Format( _T("\n") );
		strMsg += tmpMsg;
	}

	m_ctrREditInspResult.ReplaceSel( strMsg.GetBuffer() );

	return TRUE;  
}
