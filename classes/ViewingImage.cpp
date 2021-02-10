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
	// �÷� �̹��� �ʱ�ȭ
	m_nowView = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,3);
}

CViewingImage::~CViewingImage()
{
	// �̹��� ����
	cvReleaseImage(&m_nowView);
	delete [] m_nowView;

}

bool CViewingImage::LoadImage(LPCSTR filename)
{
	// ���� �ҷ�����
	// 1024x768�� �÷��̿��� �ȴ�.

	IplImage *temp;
	temp = cvLoadImage(filename,CV_LOAD_IMAGE_COLOR);
	
	bool ok = true;
	// ũ��, ���� �� �׽�Ʈ
	if (temp->width != _CamWidth)
		ok = false;
	if (temp->height != _CamHeight)
		ok = false;
	if (temp->nChannels != 3)
		ok = false;

	// �� �������� �̹��� ����
	if (ok)
	{
		m_nowView = cvCloneImage(temp);

	}

	// ������
	cvReleaseImage(&temp);
	delete [] temp;
	
	return ok;

}
