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

	// �׸� ���� -> ���� ���� �� 1024x768 �÷�
	IplImage *m_nowView;

	// ROI�� 1��~3�� + ���� 4�� ����
	CvRect m_ROI[4];

	// ����ü
	struct INSPECT_ROI{
		CvRect maguri;	// ������
		CvRect insert;	// ����
		CvRect pok;		// ��
		CvRect angle[2];	// ���� 2�� (��, ��)
		CvRect jabara;	// �ڹٶ�
		CvRect chuck;	// ���
		CvRect imul[10];	// �̹����� ����� 10������
		// �̹��� ���� -> �ڹٶ� ����, �ڹٶ�, �ڹٶ� ����, ��� �º�, ��� ���
	};

	// ROI���� �˻� ����ü�� ���� 4��
	struct INSPECT_ROI m_inspect_ROI[4];

};

#endif // !defined(AFX_VIEWINGIMAGE_H__61F3F564_274E_4416_A457_F055896BF3B5__INCLUDED_)
