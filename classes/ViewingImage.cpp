// ViewingImage.cpp: implementation of the CViewingImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CViewerTest.h"
#include "ViewingImage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewingImage::CViewingImage()
{
	// 컬러 이미지 초기화
	m_nowView = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,3);
}

CViewingImage::~CViewingImage()
{
	// 이미지 삭제
	cvReleaseImage(&m_nowView);
	delete [] m_nowView;

}

bool CViewingImage::LoadImage(LPCSTR filename)
{
	// 파일 불러오기
	// 1024x768의 컬러이여만 된다.

	IplImage *temp;
	temp = cvLoadImage(filename,CV_LOAD_IMAGE_COLOR);
	
	bool ok = true;
	// 크기, 색상 등 테스트
	if (temp->width != _CamWidth)
		ok = false;
	if (temp->height != _CamHeight)
		ok = false;
	if (temp->nChannels != 3)
		ok = false;

	// 다 괜찮으면 이미지 복사
	if (ok)
	{
		m_nowView = cvCloneImage(temp);

	}

	// 릴리즈
	cvReleaseImage(&temp);
	delete [] temp;
	
	return ok;

}
