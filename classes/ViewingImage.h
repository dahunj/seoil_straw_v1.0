// ViewingImage.h: interface for the CViewingImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWINGIMAGE_H__61F3F564_274E_4416_A457_F055896BF3B5__INCLUDED_)
#define AFX_VIEWINGIMAGE_H__61F3F564_274E_4416_A457_F055896BF3B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cxcore.h"
#include "cv.h"
#include "highgui.h"

class CViewingImage  
{
public:
	bool LoadImage(LPCSTR filename);
	CViewingImage();
	virtual ~CViewingImage();

	// 그림 변수 -> 지금 보는 것 1024x768 컬러
	IplImage *m_nowView;

	// ROI들 1번~3번 + 예비 4번 빨대
	CvRect m_ROI[4];

	// 구조체
	struct INSPECT_ROI{
		CvRect maguri;	// 마구리
		CvRect insert;	// 역삽
		CvRect pok;		// 폭
		CvRect angle[2];	// 각도 2개 (좌, 우)
		CvRect jabara;	// 자바라
		CvRect chuck;	// 축관
		CvRect imul[10];	// 이물질은 예비로 10개까지
		// 이물질 예상 -> 자바라 좌측, 자바라, 자바라 우측, 축관 좌부, 축관 우부
	};

	// ROI별로 검사 구조체도 예비 4개
	struct INSPECT_ROI m_inspect_ROI[4];

};

#endif // !defined(AFX_VIEWINGIMAGE_H__61F3F564_274E_4416_A457_F055896BF3B5__INCLUDED_)
