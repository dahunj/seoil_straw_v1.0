// StrawProcess.cpp: implementation of the CStrawProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "StrawClasses.h"


// add
#include "../resource.h"
#include "../SeoilVisionDlg.h"

//

#include "StrawProcess.h"
#include "cv.h"
#include "highgui.h"

#include "opencv2/opencv.hpp"  // by walca0608
using namespace cv;
using namespace std;

#include "../Extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define PI 3.141592
#define STRAW_IMUL_TOP		111
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define SAVE_PATH_TEST_RESULT	_T("c:\\ATI\\Test\\")
#define FIXED_LENGTH	1280

bool Blob(IplImage *source, int minpix, int &m_nMaxBlob);

CStrawProcess::CStrawProcess()
{
	// 2008 06 17 ������ �̹��� ���μ��� �ʱ�ȭ

	// �̹��� ���� �ʱ�ȭ
	m_EdgedImage = NULL;
	m_OriginalImage = NULL;
	m_ThredImage = NULL;
	m_ThredImageCopy = NULL;
	m_ThredImageMaguri = NULL;
	m_ThredImageChuck = NULL;
	m_ThredImageInsert = NULL;

	
	m_SettedImageParam = 0;

	// �̹��� ����
	//--------------------------------------------------
	// cwlee �˰��� �׽�Ʈ
	//--------------------------------------------------
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	m_binaryImage = cvCreateImage(cvSize(_CamWidth, _CamHeight), 8, 1);
	m_binaryImage_band = cvCreateImage(cvSize(_CamWidth, _CamHeight), 8, 1);
	m_ThredImage = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,1);
	m_ThredImageCopy = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,1);
	m_ThredImageMaguri = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,1);
	m_ThredImageChuck = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,1);
	m_ThredImageInsert = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,1);

	m_YCrCbImage = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,3);
	m_grayImage = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,1);
	m_RGBImage = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,3);
	Image_Temp = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,1);
	m_OriginalImage = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,1);
	
	// ROI �̹��� ����
	m_ROI_imul = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,3);

	for (int x=0;x<3;x++)
		m_bin_imul[x] = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,1);

	// �ڹٶ� �ʱ�ȭ
	for (int i=0;i<1600;i++)
	{
		m_jabara.up[i] = -9999.9999;
		m_jabara.down[i] = -9999.9999;
	}

	// �÷� �ʱ�ȭ

	m_resultmap = NULL;
	m_colorsetted = false;
	memset(m_EtqOff, 0, sizeof(int)*4);
	memset(m_OffX, 0, sizeof(int)*6);
	memset(m_OffY, 0, sizeof(int)*3);

	// ������Ȧ�� ���� ��������

	int	threshold = 120;
		m_chuck_limit = 200;


	m_highlimit = threshold;
	m_lowlimit = 0;

	// ��Į�󻡴� ���� �о����
	FILE *color = fopen("c:\\ATI\\colorset.txt","r");
	fscanf(color,"%d",&m_trans);
	fscanf(color,"%d",&m_colorstraw);
	fscanf(color,"%d",&m_tepe);
	fclose(color);
	color = fopen("c:\\ATI\\colorthres.txt","r");
	for (int i=0;i<3;i++)
		fscanf(color,"%d",&m_colorthres[i]);
	fclose(color);

	//////////////////////////////////////////////////////////////////////
	// �̼����� ���� �Ӱ谪 �о���� added by walca061
	// ���� ��������, approximation epsilon �� ���� �����ϵ��� ����ȭ added by dahunj
	FILE *fTreshold = fopen("c:\\ATI\\para_threshold.txt","r");
	fscanf(fTreshold, "%d", &m_threshold);
	fscanf(fTreshold, "%d", &m_CirCount);
	fscanf(fTreshold, "%d", &m_Debug);
	//fscanf(fTreshold, "%d", &morphology_kernel_size);
	fscanf(fTreshold, "%d", &distance_original_to_approx);
	fclose(fTreshold);
	//////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	
	//////////////////////
	m_bUseInspectEtq = FALSE;
	s_inspect_temp = true;

	/*
	memset(m_ResultValue.Rmaguri, 0, sizeof(int)*4);
	memset(m_ResultValue.Rchuck, 0, sizeof(int)*3);
	memset(m_ResultValue.Rfchuck, 0, sizeof(double)*2);
	memset(m_ResultValue.Rinsult, 0, sizeof(int)*4);
	memset(m_ResultValue.Rimul, 0, sizeof(int)*7*2);
	memset(m_ResultValue.Rjabara, 0, sizeof(double)*4);
	memset(m_ResultValue.Rpok, 0, sizeof(double)*4);
	memset(m_ResultValue.Rangle, 0, sizeof(double)*5);
	memset(m_ResultValue.REtiquet, 0, sizeof(int)*4);
	memset(m_ResultValue.RfEtiquet, 0, sizeof(double)*2);

	m_ResultValue.jabara_width_stdev = 0.0;
	m_ResultValue.jabara_distance_stdev = 0.0;
	m_ResultValue.jabara_gradient_stdev = 0.0;
	m_ResultValue.jabara_neighbor = 0;
	m_ResultValue.jabara_num = 0;

	m_ResultValue.black_num = 0;
	*/

	ClearInspResult();

	//--------------------------------------------------
	// cwlee �˰��� �׽�Ʈ
	//--------------------------------------------------
	m_StrawEdge = new Edge[m_binaryImage->width]; // �ʱ⿡ �ѹ� �����ϰ� ������ �� �޸� �����ϵ��� ���� 2017-04-17

	//m_fpError = NULL;

	m_roi.x = m_roi.y = m_roi.width = m_roi.height = 0;
}


void CStrawProcess::ClearInspResult()
{
	for(int i = 0; i < 4; i++) m_ResultValue.Rmaguri[i] = 0;
	for(int i = 0; i < 3; i++) m_ResultValue.Rchuck[i]  = 0.0;
	for(int i = 0; i < 2; i++) m_ResultValue.Rfchuck[i] = 0.0;
	for(int i = 0; i < 6; i++) m_ResultValue.Rinsult[i] = 0.0;
	for(int i = 0; i < 4; i++) m_ResultValue.Rjabara[i] = 0.0;
	for(int i = 0; i < 4; i++) m_ResultValue.Rpok[i] = 0.0;
	for(int i = 0; i < 5; i++) m_ResultValue.Rangle[i] = 0.0;
	for(int i = 0; i < 4; i++) m_ResultValue.REtiquet[i] = 0;
	for(int i = 0; i < 4; i++) m_ResultValue.RfEtiquet[i] = 0.0;
	for(int i = 0; i < 7; i++)
		for(int j = 0; j < 2; j++)
			m_ResultValue.Rimul[i][j] = 0;
	m_ResultValue.jabara_width_stdev = 0.0;
	m_ResultValue.jabara_distance_stdev = 0.0;
	m_ResultValue.jabara_gradient_stdev = 0.0;
	m_ResultValue.jabara_neighbor = 0;
	m_ResultValue.jabara_num = 0;
	m_ResultValue.black_num = 0;
	m_ResultValue.maguri.inputMinLength = 0;
	m_ResultValue.maguri.inputMaxLength = 0;
	m_ResultValue.maguri.botLen = 0;
	m_ResultValue.maguri.topLen = 0;
	m_ResultValue.maguri.isSideOk = FALSE;
	m_ResultValue.maguri.isAngleOk = FALSE;
	m_ResultValue.mid.isAngleOk = FALSE;
	m_ResultValue.mid.isHeightOk = FALSE;
	m_ResultValue.mid.minHeight = 0;
	m_ResultValue.mid.maxHeight = 0;
	m_ResultValue.top.isAngleOk = FALSE;
	m_ResultValue.top.isHeightOk = FALSE;
	m_ResultValue.top.minHeight = 0;
	m_ResultValue.top.maxHeight = 0;
	m_ResultValue.chuck.isAngleOk = FALSE;
	m_ResultValue.chuck.isHeightOk = FALSE;
	m_ResultValue.chuck.minHeight = 0;
	m_ResultValue.chuck.maxHeight = 0;
	m_ResultValue.insert.isAngleOk = FALSE;
}

CStrawProcess::~CStrawProcess()
{
	// 2008 06 17 ������ �̹��� ���μ��� ����

	if (m_binaryImage != NULL) cvReleaseImage(&m_binaryImage);
	if (m_binaryImage_band != NULL) cvReleaseImage(&m_binaryImage_band);
	if (m_OriginalImage != NULL) cvReleaseImage(&m_OriginalImage);
	if (m_ThredImage != NULL) cvReleaseImage(&m_ThredImage);
	if (m_ThredImageCopy != NULL)cvReleaseImage(&m_ThredImageCopy);
	if (m_ThredImageMaguri != NULL)cvReleaseImage(&m_ThredImageMaguri);
	if (m_ThredImageChuck != NULL)cvReleaseImage(&m_ThredImageChuck);
	if (m_ThredImageInsert != NULL)cvReleaseImage(&m_ThredImageInsert);
	if (m_YCrCbImage != NULL)cvReleaseImage(&m_YCrCbImage);
	if (m_grayImage != NULL)cvReleaseImage(&m_grayImage);
	if (m_RGBImage != NULL)cvReleaseImage(&m_RGBImage);
	if (Image_Temp != NULL)cvReleaseImage(&Image_Temp);
	if (m_ROI_imul != NULL)cvReleaseImage(&m_ROI_imul);
	if (m_resultmap != NULL)cvReleaseImage(&m_resultmap);

	for(int i = 0; i < 3; i++)
		cvReleaseImage(&m_bin_imul[i]);

	if(m_StrawEdge != NULL)
	{
		delete [] m_StrawEdge;
		m_StrawEdge = NULL;
	}
}

bool CStrawProcess::SetImage(IplImage *image)
{
	// 2008 06 17 ������ �̹��� ���� -> ������Ȧ�� �� ��������
	

	// ó�� ���Ҷ� �Ķ���� ����

	// ��ձ��ϱ�
	bool avgdiff = GetImageAvgDiff();

	if (!avgdiff)
		// ������Ȧ�� ����!
		return false;

	if ( image->imageData == 0 || image->width != m_ThredImage->width || image->height != m_ThredImage->height ) 
	{
		return false;
	}

	cvThreshold(
		image, 
		m_ThredImage,
		(double)(m_highlimit + m_lowlimit)/2.0, // m_highlimit --> ���� Ư�� ���. �ۼ�Ʈ �ƴ�. GV��
		255.0,
		CV_THRESH_BINARY );

	if ( m_colorstraw == 0 && m_nSPThres[0] !=100 )
	{
		cvThreshold(
			image,
			m_ThredImageMaguri,
			(double)(m_highlimit+m_lowlimit) / 2.0 * m_nSPThres[0] * 0.01,
			255.0,
			CV_THRESH_BINARY );
	}
	else if (m_colorstraw == 0)
	{
		cvCopy(m_ThredImage, m_ThredImageMaguri);
	}
	
	if (m_colorstraw == 0 && m_nSPThres[1] !=100)
	{
		cvThreshold(
			image,
			m_ThredImageChuck,
			(double)(m_highlimit+m_lowlimit)/2.0*m_nSPThres[1]*0.01,
			255.0,
			CV_THRESH_BINARY );
	}
	else if (m_colorstraw == 0)
	{
		cvCopy(m_ThredImage, m_ThredImageChuck);
	}

	// ---------------------
	// Legacy
	// ---------------------
	//if (m_colorstraw == 0 && m_nSPThres[2] != 100)
	//{
	//	cvThreshold(
	//		image,
	//		m_ThredImageInsert,
	//		(double)(m_highlimit+m_lowlimit)/2.0*m_nSPThres[2]*0.01,
	//		255.0,
	//		CV_THRESH_BINARY );
	//}
	//else if (m_colorstraw == 0)
	//{
	//	cvCopy(m_ThredImage, m_ThredImageInsert);
	//}

	
	cvCopy(m_ThredImage, m_ThredImageCopy);

	// ���� ���� �ʱ�ȭ
	m_length.left_bottom = -1;
	m_length.left_top = -1;
	m_length.right_bottom = -1;
	m_length.right_top = -1;
	m_length.longs = -1.0;
	m_length.OK = 0;
	m_jabara.avg_down = -1.0;
	m_jabara.avg_up= -1.0;

	return true;

}

bool CStrawProcess::GetImageAvgDiff()
{
	return true;
}

bool CStrawProcess::SetMonoImage(IplImage *image)
{
	bool result;
	s_inspect_temp = true;
	if(image->imageData != 0 
		|| (image->colorModel[0] > 0 
		&& image->colorModel[1] > 0
		&& image->colorModel[2] > 0 
		&& image->colorModel[3] > 0)
	|| image->width == m_grayImage->width)
	{
		//cvCvtColor(image, m_grayImage, CV_RGB2GRAY);
		cvCopy(image, m_grayImage);

		// �˰��� �׽�Ʈ�� ����
		//cvCvtColor(image, m_binaryImage, CV_RGB2GRAY);
		cvCopy(image, m_binaryImage);

		//cvCvtColor(image, m_OriginalImage, CV_RGB2GRAY);
		cvCopy(image, m_OriginalImage);

		//cvCopy(image, m_RGBImage);
		cvCvtColor(image, m_RGBImage, CV_GRAY2RGB);

		result = SetImage(m_grayImage);

		if (result)
		{
			// 2008 08 01 �÷� ���� ������
			m_colorsetted = true;
			
			if(image->imageData != NULL)
			{
				if(m_YCrCbImage->roi != NULL)
				{
					TRACE("in Start4: m_YCrCb is not NULL\n\n");
					cvResetImageROI( m_YCrCbImage );
				}
				//cvCvtColor(image, m_YCrCbImage, CV_RGB2YCrCb);
				cvCvtColor(m_RGBImage, m_YCrCbImage, CV_RGB2YCrCb);
			}		
		}
	}
	else
	{
		s_inspect_temp = false;
	}

	return result;
}


bool CStrawProcess::SetColorImage(IplImage *image)
{
	bool result;
	s_inspect_temp = true;
	if(image->imageData != 0 
		|| (image->colorModel[0] > 0 
		&& image->colorModel[1] > 0
		&& image->colorModel[2] > 0 
		&& image->colorModel[3] > 0)
	|| image->width == m_grayImage->width)
	{
		cvCvtColor(image, m_grayImage, CV_RGB2GRAY);

		// �˰��� �׽�Ʈ�� ����
		cvCvtColor(image, m_binaryImage, CV_RGB2GRAY);

		cvCvtColor(image, m_OriginalImage, CV_RGB2GRAY);

		cvCopy(image, m_RGBImage);

		result = SetImage(m_grayImage);

		if (result)
		{
			// 2008 08 01 �÷� ���� ������
			m_colorsetted = true;
			
			if(image->imageData != 0)
			{
				if(m_YCrCbImage->roi != NULL)
				{
					TRACE("in Start4: m_YCrCb is not NULL\n\n");
					cvResetImageROI(m_YCrCbImage);
				}
				cvCvtColor(image, m_YCrCbImage, CV_RGB2YCrCb);
			}		
		}
	}
	else
	{
		s_inspect_temp = false;
	}

	return result;
}

bool CStrawProcess::CheckInsert(CvRect areaTmp, int step, bool bvirtual, double angle, int length, int notinspect, int pix)
{
	// 2017�� ����
	bool debugFlag = m_Debug; 
	
	bool detectAlgo1 = false; 	// ���� �˰������� �ҷ� Ž�� ������ ��� false, ������ ���(�Ǵ� ������ǰ) true
								// true �� �� �ű� �˰������� �߰� Ž��
	CvRect area = areaTmp;

	// by walca0608
	if(false){ 
		cvSetImageROI(m_OriginalImage,area);
		cvSaveImage("c:\\ATI\\Test\\ù��°_����.bmp",m_OriginalImage);
		cvResetImageROI(m_OriginalImage);
	}


	int ai;
	
	// ����� ���� -> �����غ���
	int searchline = 0;
	//////////////***////////////////

	if(m_colorstraw == FALSE)
	{
		if(m_trans == FALSE)
		{
			double threshold = (double)(m_highlimit+m_lowlimit)/2.0 * m_nSPThres[2] * 0.01;
			cvThreshold(m_OriginalImage, m_ThredImageInsert, threshold, 255.0, CV_THRESH_BINARY);
		}
		else
		{
			cvThreshold(m_OriginalImage,m_ThredImageInsert,m_nThresTransInsert,255.0,CV_THRESH_BINARY);
		}
		searchline = 10;
	}
	else // color
	{
		cvThreshold(m_OriginalImage,m_ThredImageInsert,m_nThresTransInsert,255.0,CV_THRESH_BINARY);
		searchline = 5;
	}

	if(searchline == 0)
		searchline = 7;

	int top[1600];
	int bottom[1600];
	// �ʱ�ȭ

	int i,j, maxTop=0, maxBot=0, maxHighTop=9999, maxHighBot=0;
	bool result= false;

	memset(top,-1,sizeof(int)*1600);
	memset(bottom,-1,sizeof(int)*1600);

	int max_top_index_y = 0;
	int max_top_index_x = 0;
	int max_value = 0;

	int max_bottom_index_y = 0;
	int max_bottom_index_x = 0;

	// ���� ���� ã�� (Park) <- �ִ밪 ã��?? by dahunj
	for(int y = area.y; y < area.y + area.height/2 - 10; y++)
	{
		max_value = 0;

		for(int x = area.x; x < area.x + area.width; x++)
		{
			if((unsigned char)m_ThredImageInsert->imageData[y*m_ThredImageInsert->widthStep+x] == 255)
			{
				max_value = x;
			}else{
				x = area.x + area.width;
			}
		}

		if(max_top_index_x <= max_value)
		{
			max_top_index_x = max_value;
			max_top_index_y = y;
		}
	}

	int temp_x = max_top_index_x;
	int temp_y = max_top_index_y;

	// ���� ���� �ּҰ� ã�� (Park)
	for(int y = temp_y - 10; y < temp_y + 10; y++)
	{
		max_value = 0;

		for(int x = temp_x - 10; x < temp_x + 10; x++)
		{
			if((unsigned char)m_ThredImageInsert->imageData[y*m_ThredImageInsert->widthStep+x] == 255)
			{
				max_value = x;
			}else{
				x = temp_x + 10;
			}
		}

		if(max_top_index_x >= max_value && max_value > temp_x - 5)
		{
			max_top_index_x = max_value;
			max_top_index_y = y;
		}
	}




	// ���� �Ʒ��� ã�� (Park)
	for(int y = area.y + area.height; y > area.y + area.height/2 + 10; y--)
	{
		max_value = 0;

		for(int x = area.x; x < area.x + area.width; x++)
		{
			if((unsigned char)m_ThredImageInsert->imageData[y*m_ThredImageInsert->widthStep+x] == 255)
			{
				max_value = x;
			}else{
				x = area.x + area.width;
			}
		}

		if(max_bottom_index_x <= max_value)
		{
			max_bottom_index_x = max_value;
			max_bottom_index_y = y;
		}
	}

	
	double gradient = 0;
	gradient = (double)(max_bottom_index_x - max_top_index_x)/(double)(max_bottom_index_y - max_top_index_y);

	int center_x = 0;
	int center_max_x = 0;
	bool center_over = false;
	int center_y = 0;

	for(int y = max_top_index_y; y < max_bottom_index_y; y++)
	{
		for(int x = area.x; x < area.x + area.width; x++)
		{
			if((unsigned char)m_ThredImageInsert->imageData[y*m_ThredImageInsert->widthStep+x] == 255)
			{
				center_x = x;
			}

			if(center_max_x < center_x)
			{
				center_max_x = center_x;
				center_y = y;
			}
		}
	}

	if(max_top_index_x + length < center_max_x && max_bottom_index_x + length < center_max_x)
	{
		center_over = true;
	}

	// ������ �ܱ�
	for (i=area.x;i<area.x+area.width;i+=step)
	{
		for (j=area.y+area.height/10;j<area.y+area.height-area.height/10;j+=step)
		{
			if (unsigned char(m_ThredImageInsert->imageData[j*m_ThredImageInsert->widthStep+i]) == 255
				&& (j>area.height/6+area.y || m_colorstraw==0))
			{
				// �� �� ������.
				if(i > area.x+step*3 && abs(top[i-area.x-step]-j)<15 && (maxHighTop-5) < j ) {
					top[i-area.x] = j;
					maxTop = i-area.x;
					break;
				}else if(i > area.x+step*3 && abs(top[i-area.x-step]-j)>=15){
					continue;
				}else if(i <= area.x+step*3){
					top[i-area.x] = j;
					maxTop = i-area.x;
					if(j < maxHighTop) maxHighTop=j;
					break;
				}

			}
		}
	}

	// �Ʒ����� �ܱ�
	for (i=area.x;i<area.x+area.width;i+=step)
	{
		for (j=area.y+area.height-1-area.height/10;j>=area.y+area.height/10;j-=step)
		{
			if (unsigned char(m_ThredImageInsert->imageData[j*m_ThredImageInsert->widthStep+i]) == 255 
				&& (j<area.y + area.height-1- area.height/6 || m_colorstraw==0))
			{
				// �� �� ������.
				if(i > area.x+step*3 && abs(bottom[i-area.x-step]-j)<15 && (maxHighBot+5)>j) {
					bottom[i-area.x] = j;
					maxBot = i-area.x;
					break;
				}else if(i > area.x+step*3 && abs(bottom[i-area.x-step]-j)>=15 ){
					continue;
				}else if(i <= area.x+step*3){
					bottom[i-area.x] = j;
					maxBot = i-area.x;
					if(j> maxHighBot) maxHighBot=j;
					break;
				}
			}
		}
	}	
	// ��� ���� ���� ���ؼ� Top ������ Bottom ������ ���Ѵ�.
	int sumtop =0 , sumbot =0, avgtop=0, avgbot=0, EndTop=0, EndBot=0;
	bool findtopmax = false, findbotmax = false;
	for (int ai=30; ai<maxTop; ai++)
	{
		sumtop = sumtop + top[ai];
		sumbot = sumbot + bottom[ai];
	}
	if (maxTop -30 == 0 || maxBot - 30 == 0)
		return true;
	avgtop = sumtop/(maxTop-30);
	avgbot = sumbot/(maxBot-30);
	for (ai=0; ai<maxTop; ai++)
	{
		if(ai > maxTop - 25 && top[ai] > avgtop+2 && findtopmax==false) {
			EndTop = ai;
			findtopmax = true;
		}
		if(ai > maxTop - 25 &&  bottom[ai] < avgbot-2 && findbotmax==false ){
			EndBot = ai;
			findbotmax = true;
		}
	}
	if (findtopmax==false)	EndTop = maxTop;
	if (findbotmax==false)	EndBot = maxBot;

	if(bvirtual==true){
		//cvSaveImage("c:\\ATI\\Test\\insert_original.bmp",m_OriginalImage);
		//cvSaveImage("c:\\ATI\\Test\\test_insert.bmp",m_ThredImageInsert);
		cvSetImageROI(m_ThredImageInsert,area);
		cvSaveImage("c:\\ATI\\Test\\����_����ȭ.bmp",m_ThredImageInsert);
		cvResetImageROI(m_ThredImageInsert);
	}


	// �������� ���� ������Ȧ�� -> ���󺹱�
	if (m_colorstraw == 0 && !m_trans){
	
	}
	else if (m_colorstraw==0 && m_trans)
	{
		cvThreshold(m_OriginalImage,m_ThredImageInsert,(double)(m_highlimit+m_lowlimit)/2.0,255.0,CV_THRESH_BINARY);
	}
	else if (m_colorstraw && m_trans){
		cvThreshold(m_grayImage,m_ThredImageInsert,m_nThresTransInsert,255.0,CV_THRESH_BINARY);
	}
	// ������������ üũ�Ͽ� ���� ����.
	// ����� ��� 5�ȼ� ������ ��� üũ��

	double slide[1600];
	double slide2[1600];
	for (i=area.x+(searchline-1);i<area.x+area.width;i+=step)
	{
		
		int sum = 0;
		int sum2 = 0;
	
		// 5ĭ ���� ���� ��
		int x1,x2;
		x2 = top[i-area.x];
		x1 = top[i-(searchline-1)-area.x];
		sum = x2-x1;
		
		x2 = bottom[i-area.x];
		x1 = bottom[i-(searchline-1)-area.x];
		sum2 = x2-x1;

		// �Ʒ��� ���� ����
		double slides = (double)sum / 10.0;
		slide[i-area.x-(searchline-1)] = slides;
		slides = (double) sum2 / 10.0;
		slide2[i-area.x-(searchline-1)] = slides;
		
	}

	// ���Ⱑ ����� �͵��� ���� ����. +- 1/2�� ���� ��
	int count1 = 0;
	int count2 = 0;

	for (i=0;i<area.width-(searchline);i++)
	{

		if ( slide[i] - m_jabara.up_slide >= m_Slide/ 2.0  && fabs(slide[i])< fabs(m_Slide * 3.0))
			count1++;
		
		if (slide2[i] - m_jabara.down_slide <= m_Slide/ -2.0 && fabs(slide2[i])< fabs(m_Slide * 3.0))
			count2++;

	}


	// ���� �Ʒ� �� ��ģ ���̰� ��ǰ���� �̻��̸�,
	m_nResultData[4] = EndTop;
	m_nResultData[5] = EndBot;
	m_nResultData[6] = count1;
	m_nResultData[7] = count2;
	m_nResultData[11] = m_SlideLength;

	

	m_ResultValue.Rinsult[0] = max_top_index_y;
	m_ResultValue.Rinsult[1] = max_bottom_index_y;
	m_ResultValue.Rinsult[4] = gradient;
	m_ResultValue.Rinsult[5] = center_over;
	/*
	if(abs(EndBot - EndTop) < 5 && EndTop+3 > maxTop && EndBot+3 > maxBot)
		result = false;
	else if ((count1 + count2) >= m_SlideLength)
		result = true;
	else
	{

	}
	*/

	bool gradientOK = false;
	
	if(gradient > angle || gradient < -angle)
	{
		gradientOK = true;
	}else{
		gradientOK = false;

		if(center_over == true)
		{
			gradientOK = true;
		}
	}

	
	if(gradientOK == true)
	{
		result = true;
	}else{
		result = false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���� �˰������� �ҷ� Ž�� ������ ��� false, ������ ���(�Ǵ� ������ǰ) true
	// true �� �� �ű� �˰������� �߰� Ž��
	detectAlgo1 = result; // result=true ���� ��ǰ, false�� �ҷ���ǰ

	//return result;
	if (1) { // ������ǰ �Ǵ� Ž�����н� �ű� �˰��� ����
		//////////////////////////////////////////////////////////////
		// by walca0608
		// �߰� �˰���
		double areaDiff_1_2 = 0.0;
		double areaDiff_1_3 = 0.0;
		double areaDiff_2_3 = 0.0;
		double areaCon1 = 0.0;
		double areaCon2 = 0.0;
		double areaCon3 = 0.0;

		double Distance_original_to_approx;

		Mat mat_OriImage0, mat_ClipImage0 ;
		mat_OriImage0 = cvarrToMat(m_grayImage/*m_OriginalImage*/);		 // �׷��̿���
		cv::Rect rect0(area);
		mat_ClipImage0 = mat_OriImage0(rect0);                  // �˻��� ������ ������ Mat ���� �Ҵ�
		if (debugFlag) imshow("mat_ClipImage0", mat_ClipImage0);

		Mat mat_OriImage, mat_ClipImage ;
		mat_OriImage = cvarrToMat(m_ThredImageInsert);       // 2������
		cv::Rect rect(area);
		mat_ClipImage = mat_OriImage(rect);                  // �˻��� ������ ������ Mat ���� �Ҵ�
		if (debugFlag) imshow("mat_ClipImage1", mat_ClipImage);

/*// �̹��� txt�� �����ϱ�, ������ ���� 
		FILE * pFile = fopen("c:\\ATI\\binaryData.txt", "w");

		for (int row = 0; row < mat_OriImage.rows; row++)
		{
			for (int col = 0; col < mat_OriImage.cols; col++) 
			{
				uchar data = mat_OriImage.at<uchar>(row, col);
				fprintf(pFile, "%3d ", data);
			}
			fprintf(pFile, "\n");
			//cout << "\n" << endl;
		}
		fclose(pFile);
*/
	
		// ó���� �̹��� �����Ҵ�
		//contour1�� ã�´�.
		vector<vector<cv::Point> > contours1, contours2, contours3, contours4, contours5;
		vector<Vec4i> hierarchy1, hierarchy2, hierarchy3, hierarchy4, hierarchy5;
		
		findContours(mat_ClipImage, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1); //CHAIN_APPROX_SIMPLE);
		findContours(mat_ClipImage, contours2, hierarchy2, RETR_EXTERNAL, CHAIN_APPROX_TC89_KCOS); //CV_CHAIN_APPROX_NONE);
		//findContours(mat_ClipImage, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); //CV_CHAIN_APPROX_NONE);
	
		/// Draw contours1 
		RNG rng(12345);
		Mat drawing1 = Mat::zeros(mat_ClipImage.size(), CV_8UC3); 
		for (int i = 0; i< contours1.size(); i++) { 
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); 
			drawContours(drawing1, contours1, i, color, 2, 8, hierarchy1, 0, Point2i()); 
		} 
		/// Show in a window 
		if (debugFlag) namedWindow("Contours1", CV_WINDOW_AUTOSIZE); 
		if (debugFlag)  imshow("Contours1", drawing1);

		/// Draw contours2 
		Mat drawing2 = Mat::zeros(mat_ClipImage.size(), CV_8UC3); 
		for (int i = 0; i< contours2.size(); i++) { 
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); 
			drawContours(drawing2, contours2, i, color, 2, 8, hierarchy2, 0, Point2i()); 
		} 
		/// Show in a window 
		if (debugFlag) namedWindow("Contours2", CV_WINDOW_AUTOSIZE); 
		if (debugFlag) imshow("Contours2", drawing2);

		TRACE(_T("\n-----------------------------------------------------------\n"));

		contours3.resize(contours1.size());

		// üũ ������1 ����
		int contourIndex1 = 0, contourCount1 = 0;
		for (size_t i = 0; i < contours1.size(); i++)  // ������ ��ü�� ���� ������ ���� ����?
		{
			if (contourCount1 < contours1[i].size()) {
				contourCount1 = contours1[i].size(); 
				contourIndex1 = i;
			}
		}

		// üũ ������2 ����
		int contourIndex2 = 0, contourCount2 = 0;
		for (size_t i = 0; i < contours2.size(); i++)  // ������ ��ü�� ���� ������ ���� ����?
		{
			if (contourCount2 < contours2[i].size()) {
				contourCount2 = contours2[i].size(); 
				contourIndex2 = i;
			}
		}
		
//		for (size_t i = 0; i < contours1.size(); i++)  // ������ ��ü�� ���� ������ ���� ����?
//		{
			contours3[0].resize(contours2[contourIndex1].size());
			// �ܰ��� �ܼ�ȭ�� 0.01 ��� ��. �Կ����� ���¿� ���� ������ �ʿ��� ��� �Ķ���ͷ� ó�� �ʿ�
			//contour1 �Ǵ� contour2�� �ٻ�ȭ�Ͽ� contours3�� �Ҵ�
			//approxPolyDP(Mat(contours2[contourIndex2]), contours3[0], arcLength(Mat(contours2[contourIndex2]), true)*0.01, true);
			Distance_original_to_approx = ((double)distance_original_to_approx)/10000;
			approxPolyDP(Mat(contours2[contourIndex2]), contours3[0], arcLength(Mat(contours2[contourIndex2]), true)*Distance_original_to_approx, true);
	
			int temp = contours3[0].size();
			if (temp < 3) return result; //continue;   // ���� ������ ���� ���� ��� ���� (��������)

			// ���̰� ���
			areaDiff_1_3 = contourArea( Mat(contours1[contourIndex1]) ) - contourArea( Mat(contours3[0]) );
			areaDiff_2_3 = contourArea( Mat(contours2[contourIndex2]) ) - contourArea( Mat(contours3[0]) );
			areaDiff_1_2 = contourArea( Mat(contours1[contourIndex1]) ) - contourArea( Mat(contours2[contourIndex2]) );
			
			//TRACE("countCont(1): %d\n", contours1[i].size());
			//TRACE("countCont(2): %d\n", contours2[i].size());
			//TRACE("countCont(3): %d\n", contours3[i].size());			
			//
			//TRACE("areaCont(1): %f\n", contourArea( Mat(contours1[i]) ));
			//TRACE("areaCont(2): %f\n", contourArea( Mat(contours2[i]) ));
			//TRACE("areaCont(3): %f\n", contourArea( Mat(contours3[i]) ));

			//TRACE("areaDiff(1-2): %f\n", areaDiff_1_2);
			//TRACE("areaDiff(2-3): %f\n", areaDiff_2_3);
			//TRACE("areaDiff(1-3): %f\n", areaDiff_1_3);

			/// Draw contours 
			Mat drawing3 = Mat::zeros(mat_ClipImage.size(), CV_8UC3); 
			for (int i = 0; i< contours3.size(); i++) { 
				Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); 
				drawContours(drawing3, contours3, i, color, 2, 8, hierarchy1, 0, Point2i()); 
			} 

			FileStorage file("approx_contour.ext", FileStorage::WRITE);
			file << "matName" << drawing3;
			/// Show in a window 
			if (debugFlag) namedWindow("approxContours", CV_WINDOW_AUTOSIZE); 
			if (debugFlag) imshow("approxContours", drawing3);

			//if (1){				
			//	TRACE("countCont(1): %d\n", contours1[i].size());
			//	TRACE("countCont(2): %d\n", contours2[i].size());
			//	TRACE("countCont(3): %d\n", contours3[i].size());			
			//	
			//	TRACE("areaCont(1): %f\n", contourArea( Mat(contours1[0]) ));
			//	TRACE("areaCont(2): %f\n", contourArea( Mat(contours2[0]) ));
			//	TRACE("areaCont(3): %f\n", contourArea( Mat(contours3[0]) ));

			//	TRACE("areaDiff(1-2): %f\n", areaDiff_1_2);
			//	TRACE("areaDiff(2-3): %f\n", areaDiff_2_3);
			//	TRACE("areaDiff(1-3): %f\n", areaDiff_1_3);

			//	string str1 = format("countCont(1): %d\n countCont(2): %d\n countCont(3): %d\n areaCont(1): %f\n areaCont(2): %f\n areaCont(3): %f\n areaDiff(1-2): %f\n areaDiff(1-3): %f\n areaDiff(2-3): %f\n",
			//						  contours1[i].size(), contours2[i].size(), contours3[i].size(), 
			//						  contourArea( Mat(contours1[0]) ), contourArea( Mat(contours2[0]) ), contourArea( Mat(contours3[0]) ),
			//						  areaDiff_1_2, areaDiff_1_3, areaDiff_2_3);
			//	//AfxMessageBox(str1.c_str());	
			//}

/*
			/////////////////�̼������� �������� ������ (�ϴ� ����)//////////////////
			Mat morphology_roll = Mat::zeros(mat_ClipImage.size(), CV_8UC3);
			Mat bintemp;
			
			//threshold(mat_ClipImage0, bin, 120, 255, THRESH_BINARY);
			//threshold(mat_ClipImage, bintemp, 0, 255, THRESH_OTSU);
			threshold(mat_ClipImage0, bintemp, 0, 255, THRESH_OTSU);
			//threshold(drawing3, bintemp, 0, 255, THRESH_OTSU);

			Mat kernel(15,15,CV_8U,cv::Scalar(1));
			//morphologyEx(bintemp, morphology_roll, MORPH_OPEN, kernel);// ��������ó��
			morphologyEx(mat_ClipImage, morphology_roll, MORPH_OPEN, kernel);// ��������ó��
			
			findContours(morphology_roll, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1); 

			int contourIndex1 = 0, contourCount1 = 0;
			for (size_t i = 0; i < contours1.size(); i++)  // ������ ��ü�� ���� ������ ���� ����?
			{
				if (contourCount1 < contours1[i].size()) {
					contourCount1 = contours1[i].size(); 
					contourIndex1 = i;
				}
			}
			//contours5[0].resize(contours4[contourIndex4].size());
			// �ܰ��� �ܼ�ȭ�� 0.01 ��� ��. �Կ����� ���¿� ���� ������ �ʿ��� ��� �Ķ���ͷ� ó�� �ʿ�
			//contour4�� �ٻ�ȭ�Ͽ� contours5�� �Ҵ�
			//approxPolyDP(Mat(contours2[contourIndex2]), contours3[0], arcLength(Mat(contours2[contourIndex2]), true)*0.01, true);
			Distance_original_to_approx = ((double)distance_original_to_approx)/1000;
			approxPolyDP(Mat(contours1[contourIndex1]), contours5[0], arcLength(Mat(contours1[contourIndex1]), true)*Distance_original_to_approx, true);
		

			//erode(bintemp, erode_result, kernel);
			//erode(drawing3, morphology_roll, kernel);
			//GaussianBlur(drawing3, morphology_roll, cv::Size(3,3) , 0, 0);

			if (debugFlag) namedWindow("opening result", CV_WINDOW_AUTOSIZE); 
			if (debugFlag) imshow("opening result", morphology_roll);

			/// Draw contours2 
			Mat drawing5 = Mat::zeros(mat_ClipImage.size(), CV_8UC3); 
			for (int i = 0; i< contours5.size(); i++) { 
				Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); 
				drawContours(drawing5, contours5, i, color, 2, 8, hierarchy4, 0, Point2i()); 
			} 
			/// Show in a window 
			if (debugFlag) namedWindow("opening(morphology) result", CV_WINDOW_AUTOSIZE); 
			if (debugFlag) imshow("opening(morphology) result", drawing5);
*/
			/////////////////////////////////////////////////////////////////////////////////
			// ���̸��� ������ƾ
			bool rollFalse = false;  //���̸��� �÷��� = false(����)
			bool isConvex = isContourConvex(contours3[0]); 
			//bool isConvex2 = isContourConvex(contours4[0]);
			//bool isConvex2 = isContourConvex(ContourMorpoImg[0]);
			
			int size  = contours3[0].size();
			
			//if (isContourConvex(contours3[0]) == false && contours3[0].size() > 2) {  // �ܰ����� ���齺�� �ƴ� ��� �ܺ� ������
			if ((isConvex == false)  &&size > 2) {  // �ܰ����� ���齺�� �ƴ� ��� �ܺ� ������
				rollFalse = true;

				FILE *fTreshold2 = fopen("c:\\ATI\\para_threshold.txt","w");
				fprintf(fTreshold2, "%d\n", m_threshold);
				fprintf(fTreshold2, "%d\n", m_CirCount);
				fprintf(fTreshold2, "%d\n", m_Debug);
				//fprintf(fTreshold2, "%d\n", morphology_kernel_size);
				fprintf(fTreshold2, "%d\n", distance_original_to_approx);
				fprintf(fTreshold2, "%d\n", 0);
				fprintf(fTreshold2, "%d\n", int(areaDiff_2_3));
				
				fclose(fTreshold2);
			}
			/////////////////////////////////////////////////////////////////////////////////
			// ���̸��� & �̼��� ���̸��� ������ƾ
			// �������� ����� ������ ���̰� ������ �̼����� �κ��� ������,
			// ���̰� �������̻� �߻��ϸ� �̼��� ���̸������� ������

			bool roll2False = false;  //���̸��� �÷��� = false(����)
			Mat img_MorpoResult = Mat::zeros(mat_ClipImage.size(), CV_8UC3);
			vector<Vec4i> hierarchyMorpo;
			vector<vector<cv::Point> > ContourMorpoImg, ContourClipImg = contours2;
			Mat bin;
			//threshold(mat_ClipImage0, bin, 120, 255, THRESH_BINARY);
			threshold(mat_ClipImage0, bin, 0, 255, THRESH_OTSU);

			Mat ele(5, 5,CV_8U,cv::Scalar(1));
			morphologyEx(bin, img_MorpoResult, MORPH_OPEN, ele);// ��������ó��
			// Show in a window 			
			if (debugFlag) imshow("Morpology", img_MorpoResult);

			findContours(img_MorpoResult, ContourMorpoImg, hierarchyMorpo, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1); 
			//CHAIN_APPROX_TC89_KCOS; CV_CHAIN_APPROX_NONE; CHAIN_APPROX_TC89_L1
			// üũ ������M ����
			int contourIndexM = 0, contourCountM = 0;
			for (size_t i = 0; i < ContourMorpoImg.size(); i++)  // ������ ��ü�� ���� ������ ���� ����?
			{
				if (contourCountM < ContourMorpoImg[i].size()) {
					contourCountM = ContourMorpoImg[i].size(); 
					contourIndexM = i;
				}
			}
			
			Mat drawing4 = Mat::zeros(bin.size(), CV_8UC3); 
			for (int i = 0; i< ContourMorpoImg.size(); i++) {  
				Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); 
				drawContours(drawing4, ContourMorpoImg, i, color, 2, 8, hierarchyMorpo, 0, Point2i()); 
			} 
			if (debugFlag) namedWindow("Contours4", CV_WINDOW_AUTOSIZE); 
			if (debugFlag)  imshow("Contours4", drawing4);

			//double area0 = contourArea( Mat(contours3[0]));
			double area0 = contourArea( Mat(contours1[0]));
			double areaM = contourArea( Mat(ContourMorpoImg[contourIndexM]));
		
			double diff2 = area0 - areaM;
			if ((diff2 < -1.0*m_threshold || diff2 > m_threshold) && (abs(diff2) < 2.0*m_threshold) && (contours3[0].size() > 2)) {  // �̼��� ���̸� �̼�����
				roll2False = true;
				// üũ������ ������ ���̰��� ���Ͽ� ����(�׽�Ʈ ������ ����)
				FILE *fTreshold2 = fopen("c:\\ATI\\para_threshold.txt","w");
				fprintf(fTreshold2, "%d\n", m_threshold);
				fprintf(fTreshold2, "%d\n", m_CirCount);
				fprintf(fTreshold2, "%d\n", m_Debug);
				//fprintf(fTreshold2, "%d\n", morphology_kernel_size);
				fprintf(fTreshold2, "%d\n", distance_original_to_approx);
				fprintf(fTreshold2, "%d\n", int(diff2));
				fprintf(fTreshold2, "%d\n", int(areaDiff_2_3));
				
				fclose(fTreshold2);
			}
			
			/////////////////////////////////////////////////////////////////////////////////
			// ������ ������ƾ
			RotatedRect maRect = minAreaRect(contours3[0]);
			cv::Rect bRect = boundingRect(contours3[0]);

			double area1 = contourArea( Mat(contours3[0]));
			double area2 = bRect.height*bRect.width;
			double diff = area1 - area2;
			
			bool rvsFalse = false; //������ �÷��� = false(����)
			if ((diff > -1000 && diff < 1000) &&  (contours3[0].size() > 2)) {
				//�ݿ����¸� ������ �ƴ����� �з�
				vector<Vec3f> circles;
				//HoughCircles(mat_ClipImage0, circles, CV_HOUGH_GRADIENT, 2, 50, 150, 30, int(bRect.height/2*0.7), int(bRect.height/2*1.25));
				HoughCircles(mat_ClipImage0, circles, CV_HOUGH_GRADIENT, 2, 50, 150, m_CirCount, int(bRect.height/2*0.7), int(bRect.height/2*1.25));
				rvsFalse = true; //������ �ĺ�
				for( size_t ii = 0; ii < circles.size(); ii++ )
				{
					Vec3i c = circles[ii];
					cv::Point center(c[0], c[1]);
					int radius = c[2];
			 
					circle( mat_ClipImage0, center, radius, Scalar(0,255,0), 2);
					circle( mat_ClipImage0, center, 2, Scalar(0,0,255), 3);

					if ((center.x > bRect.x && center.x < (bRect.x + bRect.width)) && (center.y > bRect.y && center.y < (bRect.y + bRect.height))) {
						/*circle( mat_ClipImage0, center, radius, Scalar(0,255,0), 2);
						circle( mat_ClipImage0, center, 2, Scalar(0,0,255), 3);*/
						rvsFalse = false;  // ���ǿ� �´� ���� �ϳ��� �ִٸ� �������ĺ����� ����
					}
					if (debugFlag) imshow("mat_ClipImage_circle", mat_ClipImage0);
				}
			}

			////////////////////////////////////////////////////////////////////////////////////
			// ���� ����			
			if (rollFalse) {  //���̸��� ������
				string str4 = format("�ҷ����� : �����ҷ�");
				if (debugFlag) AfxMessageBox(str4.c_str());
				result = false;
			}
				
			if (roll2False) {  //�̼� ���̸��� ������
				string str4 = format("�ҷ����� : �̼������ҷ� (%f)", diff2);
				if (debugFlag) AfxMessageBox(str4.c_str());
				result = false;
			}

			if (rvsFalse) {  //������ ������
				//string str4 = format("�ҷ����� : ������ (%f, %d)", diff, circles.size());
				string str4 = format("�ҷ����� : ������");
				if (debugFlag) AfxMessageBox(str4.c_str());
				result = false;
			}
			////////////////////////////////////////////////////////////////////////////////////
//		}    // for (size_t i = 0; i < contours1.size(); i++)  // ������ ��ü�� ���� ������ ���� ����?
	}

	return result;
}

bool CStrawProcess::CheckInsertStabbed(CvRect areaTmp, int step, bool bvirtual, double angle, int length, int notinspect, int pix)
{
	CvRect area = areaTmp;

	// ����� ���� -> �����غ���
	int searchline = 0;
	//////////////***////////////////

	if(m_colorstraw == FALSE)
	{
		if(m_trans == FALSE)
		{
			double threshold = (double)(m_highlimit+m_lowlimit)/2.0 * m_nSPThres[2] * 0.01;
			cvThreshold(m_OriginalImage, m_ThredImageInsert, threshold, 255.0, CV_THRESH_BINARY);
		}
		else
		{
			cvThreshold(m_OriginalImage,m_ThredImageInsert,m_nThresTransInsert,255.0,CV_THRESH_BINARY);
		}
		searchline = 10;
	}
	else // color
	{
		cvThreshold(m_OriginalImage,m_ThredImageInsert,m_nThresTransInsert,255.0,CV_THRESH_BINARY);
		searchline = 5;
	}

	if(searchline == 0)
		searchline = 7;

	int top[1600];
	int bottom[1600];
	// �ʱ�ȭ

	int maxTop=0, maxBot=0, maxHighTop=9999, maxHighBot=0;
	bool result= false;

	memset(top,-1,sizeof(int)*1600);
	memset(bottom,-1,sizeof(int)*1600);

	int max_top_index_y = 0;
	int max_top_index_x = 0;
	int max_value = 0;

	int max_bottom_index_y = 0;
	int max_bottom_index_x = 0;

	// ���� ���� ã�� (Park)
	for(int y = area.y; y < area.y + area.height/2 - 10; y++)
	{
		max_value = 0;

		for(int x = area.x; x < area.x + area.width; x++)
		{
			if((unsigned char)m_ThredImageInsert->imageData[y*m_ThredImageInsert->widthStep+x] == 255)
			{
				max_value = x;
			}else{
				x = area.x + area.width;
			}
		}

		if(max_top_index_x <= max_value)
		{
			max_top_index_x = max_value;
			max_top_index_y = y;
		}
	}

	int temp_x = max_top_index_x;
	int temp_y = max_top_index_y;

	/*
	// ���� ���� �ּҰ� ã�� (Park)
	for(int y = temp_y - 10; y < temp_y + 10; y++)
	{
		max_value = 0;

		for(int x = temp_x - 10; x < temp_x + 10; x++)
		{
			if((unsigned char)m_ThredImageInsert->imageData[y*m_ThredImageInsert->widthStep+x] == 255)
			{
				max_value = x;
			}else{
				x = temp_x + 10;
			}
		}

		if(max_top_index_x >= max_value && max_value > temp_x - 5)
		{
			max_top_index_x = max_value;
			max_top_index_y = y;
		}
	}
	*/

	// ���� �Ʒ��� ã�� (Park)
	for(int y = area.y + area.height; y > area.y + area.height/2 + 10; y--)
	{
		max_value = 0;

		for(int x = area.x; x < area.x + area.width; x++)
		{
			if((unsigned char)m_ThredImageInsert->imageData[y*m_ThredImageInsert->widthStep+x] == 255)
			{
				max_value = x;
			}else{
				x = area.x + area.width;
			}
		}

		if(max_bottom_index_x <= max_value)
		{
			max_bottom_index_x = max_value;
			max_bottom_index_y = y;
		}
	}

	//���� �˻�
	//max_bottom_index_x, max_bottom_index_y
	//max_top_index_x, max_top_index_y
	int Stabbed_Up = 9999;
	int Stabbed_max = 0;

	int Stabbed_Down = 0;
	int Stabbed_min = 9999;

	for(int x = area.x + notinspect; x < area.x + area.width; x++)
	{
		for(int y = max_top_index_y; y > area.y; y--)
		{
			if((unsigned char)m_ThredImageInsert->imageData[y*m_ThredImageInsert->widthStep+x] == 255)
			{
				Stabbed_Up = y;
			}

			if(Stabbed_min > Stabbed_Up)
			{
				Stabbed_min = Stabbed_Up;
			}
		}
	}

	for(int x = area.x + notinspect; x < area.x + area.width; x++)
	{
		for(int y = max_bottom_index_y; y < area.y + area.height; y++)
		{
			if((unsigned char)m_ThredImageInsert->imageData[y*m_ThredImageInsert->widthStep+x] == 255)
			{
				Stabbed_Down = y;
			}

			if(Stabbed_max < Stabbed_Down)
			{
				Stabbed_max = Stabbed_Down;
			}
		}
	}

	m_ResultValue.Rinsult[0] = max_top_index_y;
	m_ResultValue.Rinsult[1] = max_bottom_index_y;
	m_ResultValue.Rinsult[2] = max_top_index_y - Stabbed_min;
	m_ResultValue.Rinsult[3] = Stabbed_max - max_bottom_index_y;

	/*
	if(abs(EndBot - EndTop) < 5 && EndTop+3 > maxTop && EndBot+3 > maxBot)
	result = false;
	else if ((count1 + count2) >= m_SlideLength)
	result = true;
	else
	{

	}
	*/

	bool StabbedOK = false;
	
	if(((max_top_index_y - Stabbed_min) > pix) || ((Stabbed_max - max_bottom_index_y) > pix))
	{
		StabbedOK = false;
	}else{
		StabbedOK = true;;
	}

	if(StabbedOK == true)
	{
		result = true;
	}else{
		result = false;
	}

	return result;
}

void CStrawProcess::SetImageSize(int width, int height)
{
	// 2008 06 17 ������ �뷮 ���� ���� ����
	m_ImageWidth = width;
	m_ImageHeight = height;
	m_SettedImageParam = 1;


	return;

}

bool CStrawProcess::ChechMagunus(CvRect area, bool bvirtual)
{

	CvRect volatile areas = area;
	// �������� ���� ������Ȧ��
	


	int top[1600];
	int bottom[1600];

	// 2008 08 04 ������ -> ������ ���� (����������) flag
	int top_flag = 0;
	int bottom_flag = 0;

	// 2008 08 11 ������ -> �Ʒ� �� ������ üũ
	int top_meet = 0;
	int bottom_meet = 0;


	// �ʱ�ȭ

	int i,j;
	bool result= true;
	int step =1;
	float finspvalue = 2.3f;
	int meetmax=7;
	if(m_colorstraw>=1) finspvalue = 200;
	if(m_colorstraw==1)	meetmax=12;	// �����迭.

	memset(top,-1,sizeof(int)*1600);
	memset(bottom,-1,sizeof(int)*1600);

	// -1�ΰ� ������ X
	int up = 0;
	int down = 0;
	for (i=areas.x;i<areas.width;i++)
	{
		if (top[i] == -1)
			up = 1;
		if (bottom[i] == -1)
			down = 1;

		if (up&&down)
			break;

	}

	int topmax=9999, bottommax=0, topMinpos =0, topMaxpos=0, bottomMaxpos=0, bottomMinpos=0;
	int m_cntmeetmax=-1;
	// ������ �ܱ�
	if (m_colorstraw==0 && m_trans==0)
	{
		for (i=areas.x;i<areas.x+areas.width;i+=step)
		{
			for (j=areas.y;j<areas.y+areas.height;j+=step)
			{
				if (unsigned char(m_ThredImageMaguri->imageData[j*m_ThredImageMaguri->widthStep+i]) == 255)
				{
					// �� �� ������.
					top[i-areas.x] = j;
					if (topMinpos==0){
						topMinpos=i;
						m_cntmeetmax =0;
					}
					break;
				}
			}
		}

		// �Ʒ����� �ܱ�
		for (i=areas.x;i<areas.x+areas.width;i+=step)
		{
			for (j=areas.y+areas.height-1;j>=areas.y;j-=step)
			{
				if (unsigned char(m_ThredImageMaguri->imageData[j*m_ThredImageMaguri->widthStep+i]) == 255)
				{
					// �� �� ������.
					bottom[i-areas.x] = j;
					if (bottomMinpos==0)
					{
						bottomMinpos=i;
						m_cntmeetmax =0;
					}
					break;
				}
			}
		}
	}
	else {	// color straw�̰ų� ������ ������ ���.
		for (i=areas.x;i<areas.x+areas.width;i+=step)
		{
			for (j=areas.y;j<areas.y+areas.height;j+=step)
			{
				if (unsigned char(m_ThredImageMaguri->imageData[j*m_ThredImageMaguri->widthStep+i]) == 255)
				{  
					if(unsigned char(m_ThredImageMaguri->imageData[(j+8)*m_ThredImageMaguri->widthStep+i]) == 255 && 
						unsigned char(m_ThredImageMaguri->imageData[(j+10)*m_ThredImageMaguri->widthStep+i]) == 255){
						// �� �� ������.
							top[i-areas.x] = j;
							if (topMinpos==0){
								topMinpos=i;
								m_cntmeetmax =0;
							}
							if(topmax > j && m_cntmeetmax<meetmax) {
								topmax=j;
								topMaxpos = i;
								m_cntmeetmax =0;
							}
							if (m_cntmeetmax>=0 && top[i-area.x] <= topmax)
							{
								m_cntmeetmax++;
							}
							break;
					}
				}
			}
		}
		m_cntmeetmax =-1;
		// �Ʒ����� �ܱ�
		for (i=areas.x;i<areas.x+areas.width;i+=step)
		{
			for (j=areas.y+areas.height-1;j>=areas.y;j-=step)
			{
				if (unsigned char(m_ThredImageMaguri->imageData[j*m_ThredImageMaguri->widthStep+i]) == 255)
				{
					if (unsigned char(m_ThredImageMaguri->imageData[(j-8)*m_ThredImageMaguri->widthStep+i]) == 255 && 
						unsigned char(m_ThredImageMaguri->imageData[(j-10)*m_ThredImageMaguri->widthStep+i]) == 255)
					{
							// �� �� ������.
						bottom[i-areas.x] = j;
						if (bottomMinpos==0)
						{
							bottomMinpos=i;
							m_cntmeetmax =0;
						}

						if(bottommax < j && m_cntmeetmax<meetmax) {
							bottommax=j;
							bottomMaxpos = i;
							m_cntmeetmax =0;
						}

						if (m_cntmeetmax>=0 && bottom[i-area.x] >= bottommax)
						{
							m_cntmeetmax++;
						}
						break;
					}
				}
			}
		}	
	}

	if(bvirtual==true){
		cvSetImageROI(m_ThredImageMaguri,area);
		cvSaveImage("c:\\ATI\\Test\\test_maguri.bmp",m_ThredImageMaguri);
		cvResetImageROI(m_ThredImageMaguri);
	}
	// �������� ���� ������Ȧ�� -> ���󺹱�

	// ���� ���� �ʱ�ȭ
	double slide[1600];
	double slide2[1600];

	memset(slide, 999,sizeof(double)*1600);
	memset(slide2, 999,sizeof(double)*1600);
	
	// ������ �����ʺ��� �ܾ Check
	for (i=areas.x+3;i<areas.x+areas.width;i+=step)
	{
		
		int sum = 0;
		int sum2 = 0;
	
		// 5ĭ ���� ���� ��
		int x1,x2;
		x2 = top[i-areas.x];
		x1 = top[i-3-areas.x];
		sum = x2-x1;
		
		x2 = bottom[i-areas.x];
		x1 = bottom[i-3-areas.x];
		sum2 = x2-x1;

		// �Ʒ��� ���� ����
		double slides = (double)sum / 3.0;
		slide[i-areas.x-3] = slides;
		slides = (double) sum2 / 3.0;
		slide2[i-areas.x-3] = slides;
		
		// �׽�Ʈ ���
	}

	// ���Ⱑ ����� �͵��� ���� ����. +- 1/2�� ���� ��
	int count1 = 0;
	int count2 = 0;

	int top_neg = 0;
	int bot_neg = 0;
	int top_neg2 = 0;
	int bot_neg2 = 0;

	for (i=area.width-4;i>=0;i--)
	{
	
			// ������ �� �� ������ �ϰ��� ���� (����)
			int x3,x2,x1;
			x3 = top[i+2];
			x2 = top[i+1];
			x1 = top[i];
		int d32, d21, d31;
			d32 = x3-x2;
			d21 = x2-x1;
			d31 = x3-x1; 

		
		// ���ʿ����� �Ʒ��� ������ ��. -> ���� +
		if (fabs(slide[i])>fabs(m_Magunus_Slide) && slide[i] < 0)

		{
			if ( (d32 * d21 >= 0) && (d21 * d31 >=0) && (d32 * d31 >=0))
			{
				if (top_meet != 2)
					count1++;
				top_neg2 = 0;
				
				// ������ ������ ��� ���� �κ�. 5����̸� ������ �������� �ν�. BHJ 20081204
				if (count1==m_Magunus_SlideLength/2)
				{
					int x,y;
					x = i + areas.x + m_Magunus_SlideLength/2;
					y = top[i+m_Magunus_SlideLength/2];
					m_jabara.up[1550] = x;	// jabara 1550, 1551 : ������ ���� ����. 
					m_jabara.up[1551] = y;
				}

				// 2008 08 11 ������ �������� ���
				if (count1 >= m_Magunus_SlideLength/2)
				{
					top_flag = i+area.x;
					top_neg = 0;
					if (!top_meet)
					{
						top_meet = 1;
						
					}
				}

				// 2008 09 09 ������ -> ����ġ�� ���� �� -> �ʱ� 2.5�� ����
				if (i>0)
					if (slide[i] != 0 || slide[i-1] != 0)

						if ((slide[i] * slide[i-1] < -2.0 ||  fabs(slide[i-1]) > 3.0) && top_meet != 2)
					top_meet = 2;



			}


			//}

		}
		else if (fabs(slide[i] ) < fabs(m_Magunus_Slide/3.0) &&((d32 * d21 == 0) && (d21 * d31 ==0) && (d32 * d31 ==0)) )
		{
			// �Թ��� ���
			if (top_meet == 2)
			{
				
				top_neg++;
				// ���� 2ĭ �̻� ������ -> ��
				if (top_neg >= 4)
					if (top[i] != -1){
						result =false;
						return false;
					}
			}
			else if (count1)
			{
				top_neg2 ++;
				if (top_neg2 >= 3)
				{
					top_meet = 0;
					count1 = 0;
				}
			}
		}
		else if (fabs(slide[i]+m_jabara.up_slide)>finspvalue *fabs(m_Magunus_Slide) && slide[i] > 0 && top_meet ==1)
		{
			// 2008 09 09 ������ -> ���� ��׷��� -> ������ ��׷��� �� ���� -> 2.3��
			result = false;
			return false;
		}


			// ������ �� �� ������ �ϰ������� (����)
			x3 = bottom[i+2];
			x2 = bottom[i+1];
			x1 = bottom[i];
			d32 = x3-x2;
			d21 = x2-x1;
			d31 = x3-x1;


		// �Ʒ��ʿ����� ���� ������ ��. -> ���� -
		if (fabs(slide2[i])>fabs(m_Magunus_Slide) && slide2[i] > 0)
		{


			if ( (d32 * d21 >= 0) && (d21 * d31 >=0) && (d32 * d31 >=0))
			{
				if (bottom_meet != 2)
					count2++;
					// ��ġ ���
				bot_neg = 0;
				bot_neg2 = 0;
				bottom_flag = i + area.x;
				// ������ ������ ��� ���� �κ�. m_Magunus_SlideLength/2 ����̸� ������ �������� �ν�. BHJ 20081204
				if (count2==m_Magunus_SlideLength/2)
				{
					int x,y;
					x = i + areas.x + m_Magunus_SlideLength/2;
					y = bottom[i+m_Magunus_SlideLength/2];
					m_jabara.down[1550] = x;	// jabara 1550, 1551 : ������ �Ʒ��� ����. 
					m_jabara.down[1551] = y;
				}

				if (count2 >= m_Magunus_SlideLength/2)
				{
					bottom_flag = i+area.x;
					bot_neg = 0;
					if (!bottom_meet)
					{
						bottom_meet = 1;
						
					}
				}
				
				if (i>0)
					if (slide2[i] != 0 || slide2[i-1] != 0)
				if ((slide2[i] * slide2[i-1] < -2.0 ||  fabs(slide2[i-1]) > 3.0 ) && bottom_meet != 2)
					bottom_meet = 2;
			}
		
		}
		else if (fabs(slide2[i] ) < fabs(m_Magunus_Slide/3.0) &&( (d32 * d21 == 0) && (d21 * d31 ==0) && (d32 * d31 ==0)))
			{
				if (bottom_meet == 2)
				{
					bot_neg++;
					if (bot_neg >= 4)
						if (bottom[i] != -1){
							result = false;
							return false;
						}
				}
				//else if (bottom_meet == 1)
				else if (count2)
				{

					bot_neg2++;
					if (bot_neg2 >= 3)
					{
						count2 = 0;
						bottom_meet = 0;
					}
				}
			}
		else if (fabs(slide2[i]-m_jabara.down_slide)>finspvalue *fabs(m_Magunus_Slide) && slide2[i] < 0 && bottom_meet==1)
		{
			// 2008 09 09 ������ -> ���� ��׷��� -> ������ ��׷��� �� ���� -> 2.3��
			result = false;
			return false;
		}


	}
	// Test

	// ���� �� �Ʒ� ��� ��������
	// ������ ��������.
	m_jabara.up[1562] = topMinpos;
	m_jabara.down[1562] = bottomMinpos;
	
	if (m_colorstraw==0 && m_trans==0)
	{
		m_nResultData[0] = count1;
		m_nResultData[1] = count2;
		m_nResultData[12] = m_Magunus_SlideLength;
		m_ResultValue.Rmaguri[0] = count1;
		m_ResultValue.Rmaguri[1] = count2;
		m_ResultValue.Rmaguri[2] = m_Magunus_SlideLength;
		

		if (count1 + count2 >= m_Magunus_SlideLength && result==TRUE)
			result = true;
		else result = false;

		if (result)
		{
			// ���� ����� ���� ��츸,	
			m_length.left_bottom = bottom_flag;
			m_length.left_top = top_flag;
		
		}

		else
		{

		}
	}	// �������� ���.
	else
	{
		m_nResultData[0] = abs(topMaxpos-topMinpos);
		m_nResultData[1] = abs(bottomMaxpos-bottomMinpos);
		m_nResultData[12] = m_Magunus_SlideLength;

		m_ResultValue.Rmaguri[0] = abs(topMaxpos-topMinpos);
		m_ResultValue.Rmaguri[1] = abs(bottomMaxpos-bottomMinpos);
		m_ResultValue.Rmaguri[2] = m_Magunus_SlideLength;

		int summinmax;
		summinmax = m_nResultData[0] + m_nResultData[1];
		
		if (m_nResultData[0] > 100 || m_nResultData[1] > 100)
		{
			result = false;
		}
		else if (summinmax >= m_Magunus_SlideLength && result==TRUE)
			result = true;
		else result = false;

		if (result)
		{
			// ���� ����� ���� ��츸,	
			m_length.left_bottom = bottom_flag;
			m_length.left_top = top_flag;

		}

		else
		{

		}
	}

	return result;
}

bool CStrawProcess::CheckStopperSide(CvRect area)
{
	// 2008 06 17 ������ ������ �����۸� üũ
	// ���Ʒ� ����� ����

	int top[1600];
	int bottom[1600];


	// �ʱ�ȭ

	int i,j;
	bool result= false;
	int step =1;
	memset(top,-1,sizeof(int)*1600);
	memset(bottom,-1,sizeof(int)*1600);


	// ������ �ܱ�
	for (i=area.x;i<area.x+area.width;i+=step)
	{
		for (j=area.y;j<area.y+area.height;j+=step)
		{
			if (unsigned char(m_ThredImage->imageData[j*m_ThredImage->widthStep+i]) == 255)
			{
				// �� �� ������.
				top[i-area.x] = j;
				break;
			}
		}
	}

	// �Ʒ����� �ܱ�
	for (i=area.x;i<area.x+area.width;i+=step)
	{
		for (j=area.y+area.height-1;j>=area.y;j-=step)
		{
			if (unsigned char(m_ThredImage->imageData[j*m_ThredImage->widthStep+i]) == 255)
			{
				// �� �� ������.
				bottom[i-area.x] = j;
				break;
			}
		}
	}	

	
	// ���� �� ���� �̻� ������ �� �κ��� �� �ȴ�.
	// ������ ���� ����
	int zero  = 0;
	for (i=0;i<area.width;i++)
	{
		if ((top[i] >= area.height/2)|| (top[i] == -1))
		{
			zero = 1;
			break;
		}
		if ((bottom[i] <=area.height/2) || (bottom[i] == -1))
		{
			zero = 1;
			break;
		}
	}

	// �̻� �������� ǥ������ üũ
	if (!zero)
	{
		// ���� ����
		int top_min = 999;
		int top_max = -2;
		int top_sum = 0;
		long top_sum2 = 0;
		int top_count = 0;

		// ��հ� ǥ������, �ִ�, �ּ� ��� (��)
		for (i=0;i<area.width;i++)
		{
			top_count++;
			top_sum+=top[i];
			top_sum2 += (long)pow((double)top[i],2.0);
			if (top_max < top[i])
				top_max = top[i];
			if (top_min > top[i])
				top_min = top[i];
		}
		double top_avg = (double)top_sum / (double)top_count;
		double top_diff = sqrt((double)top_sum2/(double)top_count -pow(top_avg,2));

		// �Ʒ��� ����
		int bot_min = 999;
		int bot_max = -2;
		int bot_sum = 0;
		long bot_sum2 = 0;
		int bot_count = 0;

		// ��հ� ǥ������, �ִ�, �ּ� ��� (��)
		for (i=0;i<area.width;i++)
		{
			bot_count++;
			bot_sum+=bottom[i];
			bot_sum2 += (long)pow((double)bottom[i],2.0);
			if (bot_max < bottom[i])
				bot_max = bottom[i];
			if (bot_min > bottom[i])
				bot_min = bottom[i];
		}
		double bot_avg = (double)bot_sum / (double)bot_count;
		double bot_diff = sqrt((double)bot_sum2/(double)bot_count -pow(bot_avg,2));

		// ���� ��ġ�� 1 diff �̻��̾�� �� (1sigma ->���� 75%)
		// ���� �ݴ�������� 1diff �̻� Ƣ����� ����
		if (((double)top_max >= top_avg + 1.0 * top_diff)  && ( (double)bot_min <= bot_avg - 1.0 * bot_diff) )
		{
			// �׸��� �� ǥ�������� ���� �������� �̻��̾�� ��
			// ���̰� +-�̹Ƿ� ������� ������ 1/2�� ��
			if ( ((top_max-top_avg) > m_SideStopperLimit/2.0) && (bot_avg - bot_min > m_SideStopperLimit/2.0 ))
				result = 1;
		}

	}

	return result;
}

bool CStrawProcess::CheckStopperUp(CvRect area)
{
	// ���ʿ� ������ �� �� ã�´�.
	// ���� Ȱ��


	bool result = false;


	int sx,sy,ex,ey;
	// 1/3���� 2/3���� �˻� x,y
	sx = area.x+ cvRound(1.0/3.0 *(double)area.width);
	ex = area.x + cvRound(2.0/3.0 * (double)area.width);
	sy = area.y + cvRound(1.0/3.0 * (double)area.height);
	ey = area.y + cvRound(2.0/3.0 * (double)area.height);

	int height = ey - sy;
	
	// �� ���� ���� 1/3 �̻��� �־�� ��
	int thres = cvRound(1.0/3.0 * (double)height);

	int i,j;
	int count = 0;
	for (i=sy;i<ey;i++)
	{
		
		int index = sy * m_EdgedImage->widthStep;
		for (j=sx;j<ex;j++)
		{
			int index1 = index + j;
			if ((unsigned char)m_EdgedImage->imageData[index1] == 255)
			{
				// ������.
				count++;
				break;
			}
		}
	}

	// 1/3 �̻��ΰ�?
	if (thres <= count)
		result = true;

	return result;
}

bool CStrawProcess::CheckChuck(CvRect areaTmp, double resol, bool bvirtual)
{
	CvRect area = areaTmp;

	// ��� �˻�
	int top[1600], bottom[1600];
	memset(top,-1,sizeof(int)*1600);
	memset(bottom,-1,sizeof(int)*1600);
	int i,j;

	// �ʱ�ȭ
	int sx = area.x;
	int sy = area.y;
	int ex = area.x+area.width;
	int ey = area.y+area.height;


	// �� ���� ��� ���ΰ� ä���ֱ�.	(1/2 ��� ���� ���ʺκи� �ش�.)
	bool bgab=false;
	int ngabCnt = 0;
	if(m_colorstraw>0){		// Color Straw�� ��츸 ����.
		for (j=(sy+ey)/2-20; j<(sy+ey)/2+20; j++)
		{
			for (i=sx; i<(sx+ex)/2; i++)
			{	
				if((unsigned char)m_ThredImageChuck->imageData[j*m_ThredImageChuck->widthStep+i] == 0){
					bgab=true;
					ngabCnt++;
				}	
				else if (bgab==TRUE)
				{
					for(int k=0; k<ngabCnt+1; k++){
						TRACE("m_ThredImageChuck changed!\n");
						m_ThredImageChuck->imageData[j*m_ThredImageChuck->widthStep+i-k] = 255;	
					}
					ngabCnt=0;
					bgab=false;
				}
			}
		}
	}
	bool bTransEndPos=false;
	int nTransEndPosition=0, nTECnt=0, nTETop=0, nTEBot=0;
	

	if(m_colorstraw>0 && m_trans==TRUE){		// Color Straw�̰� ���Ժ� ������ ��츸 ����.
		for (i=ex-5;i>sx;i--)						// ���� ã��.
		{
			for (j=sy+3;j<ey-3;j++)
			{	
				if((unsigned char)m_ThredImageChuck->imageData[j*m_ThredImageChuck->widthStep+i]==255){
					nTECnt++;
				}
			}
			if(nTECnt > 20) {
				nTransEndPosition=i;
				nTECnt=0;	
				for (int k=(sy+ey)/2;k>=sy+3;k--)
				{
				// ������ �ܱ�
					if ((unsigned char)m_ThredImageChuck->imageData[k*m_ThredImageChuck->widthStep+i-3] == 0
						&& (unsigned char)m_ThredImageChuck->imageData[(k-3)*m_ThredImageChuck->widthStep+i-3] == 0
						&& (unsigned char)m_ThredImageChuck->imageData[(k-2)*m_ThredImageChuck->widthStep+i-3] == 0
						&& (unsigned char)m_ThredImageChuck->imageData[(k-1)*m_ThredImageChuck->widthStep+i-3] == 0
						){
						nTETop = k;
						break;
					}
				}
				for (int k=(sy+ey)/2;k<ey-3;k++)
				{
					// �Ʒ����� �ܱ�
					// ������ �ܱ�
					if ((unsigned char)m_ThredImageChuck->imageData[k*m_ThredImageChuck->widthStep+i-3] == 0 
						&& (unsigned char)m_ThredImageChuck->imageData[(k+3)*m_ThredImageChuck->widthStep+i-3] == 0
						&& (unsigned char)m_ThredImageChuck->imageData[(k+1)*m_ThredImageChuck->widthStep+i-3] == 0
						&& (unsigned char)m_ThredImageChuck->imageData[(k+2)*m_ThredImageChuck->widthStep+i-3] == 0
						)
					{
						nTEBot = k;
						break;
					}
				}
				break;
			}
		}
		// ������ ã�� ���� ���� �� ä���.
		for (j=nTETop+3; j<nTEBot-3; j++)
		{
			for (i=sx; i<nTransEndPosition-3; i++)
			{	
				TRACE("m_ThredImageChuck changed!\n");
				m_ThredImageChuck->imageData[j*m_ThredImageChuck->widthStep+i] = 255;	
			}
		}
	}

	//2009 01 14 ������ -> ������� �ұ�
	bool binitposition=false; 

	for (i=sx;i<ex;i++)
	{
		for (j=(sy+ey)/2;j>=sy+3;j--)
		{
			// ������ �ܱ�
			if ((unsigned char)m_ThredImageChuck->imageData[j*m_ThredImageChuck->widthStep+i] == 0
				&& (unsigned char)m_ThredImageChuck->imageData[(j-3)*m_ThredImageChuck->widthStep+i] == 0
				&& (unsigned char)m_ThredImageChuck->imageData[(j-2)*m_ThredImageChuck->widthStep+i] == 0
				&& (unsigned char)m_ThredImageChuck->imageData[(j-1)*m_ThredImageChuck->widthStep+i] == 0
				)
			{
				if (j==(sy+ey)/2 
					&& (unsigned char)m_ThredImageChuck->imageData[(j-10)*m_ThredImageChuck->widthStep+i] == 0
					&& (unsigned char)m_ThredImageChuck->imageData[(j+10)*m_ThredImageChuck->widthStep+i] == 0)
				{
					top[i] = j+60;
				}else{
					top[i] = j;
				}
				
				break;
			}
		}
		for (j=(sy+ey)/2;j<ey-3;j++)
		{
			// �Ʒ����� �ܱ�
			// ������ �ܱ�
			if ((unsigned char)m_ThredImageChuck->imageData[j*m_ThredImageChuck->widthStep+i] == 0 
				&& (unsigned char)m_ThredImageChuck->imageData[(j+3)*m_ThredImageChuck->widthStep+i] == 0
				&& (unsigned char)m_ThredImageChuck->imageData[(j+1)*m_ThredImageChuck->widthStep+i] == 0
				&& (unsigned char)m_ThredImageChuck->imageData[(j+2)*m_ThredImageChuck->widthStep+i] == 0
				)
			{
				if (j==(sy+ey)/2
					&& (unsigned char)m_ThredImageChuck->imageData[(j-10)*m_ThredImageChuck->widthStep+i] == 0
					&& (unsigned char)m_ThredImageChuck->imageData[(j+10)*m_ThredImageChuck->widthStep+i] == 0)
				{
					bottom[i] = j-60;
				}
				else bottom[i] = j;
				break;
			}
		}

	}

	int sumTop=0, sumBot=0, sumAvgTop=0, sumAvgBot=0;
	int avgCha[500];
	int nCntCha=0;
	int up_Topmax=9999, bot_Topmax=0, up_Topmin=0, bot_Topmin=9999, up_TopPos=0, bot_TopPos=0;
	int up_TopminPos=0, bot_TopminPos=0;
	int inspMinPos=sx;
	memset(avgCha,0, sizeof(int)*500);
	
		for (i=sx; i<ex-4; i=i+5)
		{
			if(up_Topmax >= top[i] && i< (sx+ex)/2) {
				up_Topmax = top[i];
				up_TopPos = i;
			}
			if(bot_Topmax < bottom[i] && i< (sx+ex)/2) {
				bot_Topmax = bottom[i];
				bot_TopPos = i;
			}
			if (up_Topmin < top[i] && i< (sx+ex)/2)
			{
				up_Topmin = top[i];
				up_TopminPos = i;
			}
			if(bot_Topmin >= bottom[i] && i< (sx+ex)/2)
			{
				bot_Topmin = bottom[i];
				bot_TopminPos = i;
			}
			for (j=0; j<5; j++)
			{
				sumTop = sumTop + top[i+j];
				sumBot = sumBot + bottom[i+j];
			}
			sumAvgTop = (int)floor(sumTop/5.0 + 0.5); 
			sumAvgBot = (int)floor(sumBot/5.0 + 0.5); 
			for (j=0; j<5; j++)
			{
				top[i+j] = sumAvgTop;
				bottom[i+j] = sumAvgBot;
//				fprintf(test2,"i+j : %d top : %d bottom : %d, [cha] : %d\n",i+j,top[i+j], bottom[i+j], bottom[i+j]-top[i+j]);
				avgCha[i-sx+j] = bottom[i+j]-top[i+j];
				nCntCha ++;
			}
			sumTop =0;
			sumBot =0;
		}

		if ((up_Topmin - up_Topmax > 5) || (bot_Topmax - bot_Topmin > 5))
		{
			if(up_Topmin - up_Topmax > 5 && up_TopPos > up_TopminPos) inspMinPos = max(up_TopminPos, up_TopPos) + 20;
			if(bot_Topmax - bot_Topmin > 5 && bot_TopPos > bot_TopminPos) inspMinPos = max(bot_TopminPos, bot_TopPos) + 20;
		}
//	}
//	fclose(test2);
	if(bvirtual==true){
		//cvSaveImage("c:\\ATI\\Test\\test_chuck.bmp",m_ThredImageChuck);
		cvSetImageROI(m_ThredImageChuck,area);
		cvSaveImage("c:\\ATI\\Test\\���_����ȭ.bmp",m_ThredImageChuck);
		cvResetImageROI(m_ThredImageChuck);
	}
	
	// �������� ���� ������Ȧ�� -> ���󺹱�
	//if (m_colorstraw == 0 && m_nSPThres[1] !=100)
	//	cvThreshold(m_OriginalImage,m_ThredImage,(double)(m_highlimit+m_lowlimit)/2.0,255.0,CV_THRESH_BINARY);

// ���� 15ĭ ���� 10ĭ ������ ����
int up_left = 0;
int up_right = 0;
int down_left = 0;
int down_right = 0;
int upLeft_OK =0 ;
int downLeft_OK =0 ;
int upRight_OK =0 ;
int downRight_OK =0 ;

// 2008 10 07 ������ ��� ������ �� ã��
int find_up = 0;
int find_down = 0;
bool bErr=false;
//double pastSlide, pastSlide_d;
int nErrCntUp=0, nErrCntDn=0;
int slidegab =19;

// ���� �ִ� �ּ�
double up_min = 9999.9999;
double up_max = -9999.9999;
double down_min = 9999.9999;
double down_max = -9999.9999;
double findratio = 0.2;
if(m_colorstraw>0)findratio=m_fChuckSlideBottom;

//FILE *test4 = fopen("c:\\chuck_slidecha.txt","w");
int findleft=0;

if (slidegab == 0)
return true;

for (i=sx;i<ex-slidegab-1;i++)
{
	double slide = ((double)top[i+slidegab]-(double)top[i])/slidegab - m_jabara.up_slide ;
	double slide_d = ((double)bottom[i+slidegab]-(double)bottom[i])/slidegab - m_jabara.down_slide ;

	double slidecha = ((double)avgCha[i-sx+19]-(double)avgCha[i-sx])/slidegab - m_jabara.up_slide ;

//	fprintf(test4,"%d %lf\n",i,slidecha);
	
	if (upLeft_OK < 2)
	{
		if (slide >= 0.09)
//			up_left = i+19;
			upLeft_OK = 1;

		if (slide < 0.09 && upLeft_OK==1)
			upLeft_OK = i;
	}
	
	if (!up_left && i>sx+10)
	{
		if (slidecha < -findratio && !find_up)
		{
			find_up = 1;
			up_max = slidecha;
			up_min = slidecha;
		}
		
		if (slidecha < up_min&& find_up)
		{
			up_min = slidecha;
		}
		if (slidecha> up_max&& find_up)
		{
			up_max = slidecha;
		}
		
		if(fabs(slidecha-up_min)>m_fChuckSlideTop && find_up && !up_left && fabs(slidecha) < 0.3
			&& i>up_Topmax && i>inspMinPos)
		{
			up_left=i;
			findleft = up_left;
		}	
	}



	// down.
	if (downLeft_OK < 2 )
	{
		if ((slide_d) <= -0.09)
		
			downLeft_OK = 1;

		if (slide_d > -0.09 && downLeft_OK==1)
			downLeft_OK = i;
	}

	if (!down_left && i>sx+10)
	{
		if ((slide_d) <= -0.07 && !find_down)
		{
			find_down = 1;
			down_max = slide_d;
			down_min = slide_d;
		}

		if (slide_d < down_min&& find_down)
		{
			down_min = slide_d;
		}
		if (slide_d> down_max&& find_down)
		{
			down_max = slide_d;
		}

		// 1���� �̻�	//20090522
		if (fabs(slide_d-down_min)>m_fChuckSlideBottom && find_down && !down_left
			&& i>bot_TopPos && i>inspMinPos){
				down_left=i;
		}
	}
}

//fclose(test4);

// 2009 03 23 ������ -> ���� �� ��� �и�

	// ����� ��쿡�� �״�� ó�� (�Ǵ� �����뿡 ���Ժ� �������� ���� ���.)
	if (m_colorstraw==0 && m_trans==0)
	{ 

		for (i=ex-1;i>sx+9;i--)
		{
			double slide = ((double)top[i]-(double)top[i-9])/9.0 - m_jabara.up_slide ;
			double slide_d = ((double)bottom[i]-(double)bottom[i-9])/9.0 - m_jabara.down_slide ;

			if (upRight_OK < 2)
			{
				if (slide >= 0.15)
					upRight_OK = i-9;
			}

			if (!up_right)
			{
				if (slide >= 0.14)
					up_right = i-9;
			}
			
			if (downRight_OK < 2)
			{
				if ((slide_d) <= -0.15)
		//			up_left = i+19;
					downRight_OK = i-9;
				// ���Ⱑ �ݴ�� �ö󰡸� ����.
			}

			if (!down_right)
			{
				if ((slide_d) <= -0.14)
					down_right = i-9;
			}

			if (up_right && down_right)
				break;
		}
	}

	else	// �� �����̰ų� ���Ժΰ� ������ ���.
	{
		// �������κ��� �Ÿ�
		const int dist = 10;
		// �� �ȼ� �̻� ���̸� ��?
		const int limit_cha = 30;

		// ������ & ���� -> �׳� ���ʿ��� ã��
		for (i=up_left+dist;i<ex;i++)
		{
			if (upRight_OK < 2)
			{
				// �޺��� ���ؼ� ����
				int cha = abs(top[i]-top[i-1]);
				if (cha > limit_cha)
				{
					up_right = i;
					upRight_OK = i;
				}
			}

			if (up_right)
				break;
		}

		for (i=down_left+dist;i<ex;i++)
		{
			if (downRight_OK < 2)
			{
				// �޺��� ���ؼ� ����
				int cha = abs(bottom[i]-bottom[i-1]);
				if (cha > limit_cha)
				{
					down_right = i;
					downRight_OK = i;
				}
			}

			if (down_right)
				break;

		}
	}

	// �׽�Ʈ ���
//	FILE *test = fopen("c:\\magunus.txt","w");


	// 2008 09 09 ������ -> �յ� ���� ���غ��� (5��)
	double fb_up[1600], fb_down[1600];
	for (i=sx+2;i<ex-2;i++)
	{
		double nu=0;
		double nd = 0;
		for (j=-2;j<3;j++)
		{
			if (j==0) continue;
			nu += fabs((double)top[i+j]-(double)top[i]);
			nd += fabs((double)bottom[i+j]-(double)bottom[i]);
		}
		fb_up[i] = nu / 4.0;
		fb_down[i] = nd / 4.0;
	}
	

	up_left = findleft;
	down_left = findleft;
	up_right = max(up_right,down_right);
	down_right = up_right;
	// ���
  	int up = abs(up_right- up_left);//-12;
	int down = abs(down_right- down_left);//-12;

	// ����� ������, ���� ����Ʈ ������.	BHJ
 	if (downRight_OK==0) downRight_OK = down_right;
	if (upRight_OK==0) upRight_OK = up_right;
	m_jabara.up[1558] = upLeft_OK;
	m_jabara.up[1559] = top[upLeft_OK];
	m_jabara.down[1558] = downLeft_OK;
	m_jabara.down[1559] = bottom[downLeft_OK];
	m_jabara.up[1560] = upRight_OK;
	m_jabara.up[1561] = top[upRight_OK-3];
	m_jabara.down[1560] = downRight_OK;
	m_jabara.down[1561] = bottom[downRight_OK-3];


	m_nResultData[2] = up;
	m_nResultData[3] = down;
	m_nResultData[10] = (int)m_minChuck;
	m_ResultValue.Rchuck[0] = ( up + down ) / 2.0 * resol;
	m_ResultValue.Rchuck[1] = m_minChuck * resol;
	m_ResultValue.Rchuck[2] = m_maxChuck * resol;

	if ( m_colorstraw >= 1  || m_colorstraw == 0 )
	{
		if (!( (up + down)/2 >= m_minChuck)
			|| (up + down)/2 >= m_maxChuck )
		{
			return false;
		}
		if (up <= 0 || down <= 0 || up >= area.width || down >=area.width)
		{
			return false;
		}
	}
	else{
		if (!((double)up >= (double)m_minChuck) )
		{
			return false;
		}

		if (!((double)down >= (double)m_minChuck))
		{
			return false;
		}
	}


	m_length.right_bottom = down_right;
	m_length.right_top = up_right;
	
	if( bErr == true )
		return false;

	// 2009 05 29 ������ �������̸� �¿��� �Ʒ��� ���� �� 
 	if (m_colorstraw)
	{
		double top_left_avg = 0;
		double top_right_avg = 0;
		double bottom_left_avg = 0;
		double bottom_right_avg = 0;
		int count = 0;
		
		for (int i=sx+2;i<up_left-2;i++)
		{
			count++;
			top_left_avg += top[i];
		}
		top_left_avg/= (double)count;
		count = 0;

		for (int i=up_left+2;i<up_right-2;i++)
		{
			count++;
			top_right_avg += top[i];
		}
		top_right_avg /= (double)count;
		count = 0;

		for (int i=sx+2;i<down_left-2;i++)
		{
			count ++;
			bottom_left_avg+=bottom[i];

		}
		bottom_left_avg /= (double)count;
		count = 0;

		for (int i=down_left +2 ;i< down_right -2;i++)
		{
			count++;
			bottom_right_avg+=bottom[i];
		}
		bottom_right_avg/=(double)count;
		count = 0;

		double pok_left = fabs(top_left_avg -bottom_left_avg)/2.0;
		double pok_right = fabs(top_right_avg - bottom_right_avg)/2.0;

		m_ResultValue.Rfchuck[0] = fabs(pok_left-pok_right);
		m_ResultValue.Rfchuck[1] = m_fPokDiff;
		
		if (fabs(pok_left-pok_right) < m_fPokDiff)
			return false;
	}

	return true;
}

double CStrawProcess::CalculateSlide(CvRect area, double limit, int nLRSelect, bool bvirtual)
{
	return 0.0;
	// 2008 07 31 ������ ���� ���ϴ� �Լ�
	// area ������ �� �Ʒ��� �ܰ� ���� ���Ѵ�.
	// limit(degree) �̻� ����� -999.999(����) ����
/*
	CvRect volatile areas = area;

	double angle=-999.999;

	// ���� �Ʒ� ���ϱ�
	int top[1600], bottom[1600];
	memset(top,-1,sizeof(int)*1600);
	memset(bottom,-1,sizeof(int)*1600);
	int i,j;

	// �ʱ�ȭ
	int sx = areas.x;
	int sy = areas.y;
	int ex = areas.x+areas.width;
	int ey = areas.y+areas.height;


	if (m_colorstraw == 0)
	{
		for (i=sx;i<ex;i++)
		{
			for (j=(sy+ey)/2;j>=sy;j--)
			{
				// ������ �ܱ�
				if ((unsigned char)m_ThredImageAngle->imageData[j*m_ThredImageAngle->widthStep+i] == 0 )
		
				if ((unsigned char)m_ThredImageAngle->imageData[j*m_ThredImageAngle->widthStep+i] == 0 )
				{
					top[i] = j;
					break;
				}
			}
			for (j=(sy+ey)/2;j<ey;j++)
			{
				// �Ʒ����� �ܱ�
				// ������ �ܱ�
				if ((unsigned char)m_ThredImageAngle->imageData[j*m_ThredImageAngle->widthStep+i] == 0 )
				{
					bottom[i] = j;
					break;
				}

			}
		}
	}
	else
	{
		for (i=sx;i<ex;i++)
		{
			for (j=sy+3;j<ey-3;j++)
			{
				// ������ �ܱ�
				if ((unsigned char)m_ThredImageAngle->imageData[j*m_ThredImageAngle->widthStep+i] != 0 )
				{
					top[i] = j;
					break;
				}
			}
			for (j=ey-3;j>=sy+3;j--)
			{
				// �Ʒ����� �ܱ�
				// ������ �ܱ�
				if ((unsigned char)m_ThredImageAngle->imageData[j*m_ThredImageAngle->widthStep+i] != 0 )
				{
					bottom[i] = j;
					break;
				}

			}
		}
	}


	if(nLRSelect==0)
		memset(TopBot,0,sizeof(int)*2*2);
	TopBot[nLRSelect][0] = top[(sx+ex)/2];
	TopBot[nLRSelect][1] = bottom[(sx+ex)/2];

	int count = 0;

	double sxx_1 = 0.0;
	double sxy_1 = 0.0;
	double sxx_2 = 0.0;
	double sxy_2 = 0.0;
	double xy_1 = 0.0;
	double xy_2 = 0.0;
	double x2_1 = 0.0;
	double x2_2 = 0.0;
	double sumx2_1 = 0.0;
	double sumx2_2 = 0.0;
	double sumx_1 = 0.0;
	double sumx_2 = 0.0;
	double sumy_1 = 0.0;
	double sumy_2 = 0.0;

	for (i=sx;i<ex;i++)
	{
		count++;

		// ���� ó��
		sumx_1 += (double) i;
		sumx2_1 += (double)pow((double)i,2);
		sumy_1 += (double) top[i];	
		xy_1 += (double)i * (double)top[i];

		// �Ʒ��� ó��
		sumx_2 += (double) i;
		sumx2_2 += (double)pow((double)i,2);
		sumy_2 += (double) bottom[i];	
		xy_2 += (double)i * (double)bottom[i];
		
	}	

	// ���� ���� ���ϱ�
	sxx_1 = sumx2_1 - (double)(pow(sumx_1,2)/(double)count);
	sxy_1 = xy_1 - sumx_1 * sumy_1 / (double)count;
	sxx_2 = sumx2_2 - (double)(pow(sumx_2,2)/(double)count);
	sxy_2 = xy_2 - sumx_2 * sumy_2 / (double)count;
	double ang1 = sxy_1/sxx_1;
	double ang2 = sxy_2 / sxx_2;

	if (abs(ex-sx)>15)
	{
	m_jabara.slide_up = ang1;
		m_jabara.up_slide = ang1;

	ang1 = atan(ang1);

	ang1 = ang1 / PI * 180.0;


	m_jabara.slide_down = ang2;
		m_jabara.down_slide = ang2;

	ang2 = atan(ang2);

	ang2 = ang2 / PI * 180.0;
	}


	// ǥ�� ���� �ſ� ������ (15 ����) -> �׳� ���ư�
	// 2009 04 22 ������ ǥ������ ������ ���� ���� �ſ� ŭ
	if (abs(ex-sx) < 16)
	{
		m_jabara.slide_up = 0.01;
		m_jabara.up_slide = 0.01;
		m_jabara.slide_down = 0.01;
		m_jabara.down_slide = 0.01;
		return 0.01;
	}

	// ���� ���ϱ�
	// ���� �� ���̰� limit �̻��̸� ����Ʈ
	// ���� ���� ���̸� ����
	double cha = fabs(ang1-ang2);
	if ((cha < limit) && (ang1 < limit ) && (ang2 < limit)){
		angle = (ang1+ang2)/ 2.0*-1.0;
		m_ResultValue.Rangle[0] = ang1;
		m_ResultValue.Rangle[1] = ang2;
		m_ResultValue.Rangle[2] = limit;
	}
	else
	{
		m_ResultValue.Rangle[0] = ang1;
		m_ResultValue.Rangle[1] = ang2;
		m_ResultValue.Rangle[2] = limit;

		angle = -999.999;
	}

	if(bvirtual==true)
		cvSaveImage("c:\\ATI\\Test\\test_slide.bmp",m_ThredImageAngle);

	// ���� ���
	m_jabara.spoint = (area.x + area.x + area.width)/2;


// ������� ���� ��׷��� ���� 2008. 9. 16. ������

	double top2[1600], bottom2[1600];
	memset(top2,-1,sizeof(int)*1600);
	memset(bottom2,-1,sizeof(int)*1600);

	for (i=sx;i<ex;i++)
	{
		for (j=(sy+ey)/2;j>=sy-5;j--)
		{
			// ������ �ܱ�
			if ((unsigned char)m_ThredImageAngle->imageData[j*m_ThredImageAngle->widthStep+i] == 0)
			{
				top2[i] = j;
				break;
			}
		}
		for (j=(sy+ey)/2;j<ey+5;j++)
		{
			// �Ʒ����� �ܱ�
			// ������ �ܱ�
			if ((unsigned char)m_ThredImageAngle->imageData[j*m_ThredImageAngle->widthStep+i] == 0)
			{
				bottom2[i] = j;
				break;
			}

		}
	}

	// 2008 09 09 ������ -> �յ� ���� ���غ��� (5��)
	double fb_up[1600], fb_down[1600];

	int up_min ,down_min,up_max,down_max;
	up_max = down_max = -1;
	up_min = down_min = 9999;

	double err_up = 0.0;
	double err_down = 0.0;


	for (i=sx+2;i<ex-2;i++)
	{
		double nu=0;
		double nd = 0;
		for (j=-2;j<3;j++)
		{
			if (j==0) continue;
			nu += fabs((double)top[i+j]-(double)top[i]);
			nd += fabs((double)bottom[i+j]-(double)bottom[i]);
		}
		fb_up[i] = nu / 4.0;
		fb_down[i] = nd / 4.0;
	
		// �ּ� �ִ� ã��
		if (up_min > top2[i])
			up_min = (int)top2[i];
		if (up_max < top2[i])
			up_max = (int)top2[i];
		if (down_min > bottom2[i])
			down_min = (int)bottom2[i];
		if (down_max < bottom2[i])
			down_max = (int)bottom2[i];

		// ��ȭġ �ްݽ� ����
		if(m_colorstraw > 0 && (fb_up[i] > 12 || fb_down[i] >12)){

			m_ResultValue.Rangle[3] = fb_up[i];
			m_ResultValue.Rangle[4] = fb_down[i];
			return -999.999;
		}
		else if (m_colorstraw==0 && (fb_up[i] > 1.20 || fb_down[i] >1.20))
		{

			m_ResultValue.Rangle[3] = fb_up[i];
			m_ResultValue.Rangle[4] = fb_down[i];
		 	return -999.999;
		}
	}


	// �ִ� �ּ� üũ
	int width = area.width;
	double cha1 = (double)width * tan(angle*PI/180.0);
	double cha2 = (double)width * tan(angle*PI/180.0);

	if ((int) cha1 == 0)
		cha1 = 3.0;
	if ((int) cha2 == 0)
		cha2 = 3.0;


	return angle;
*/
}


bool CStrawProcess::CalculateJabaraDiff(CvRect area, int masksize)
{
	// �ڹٶ� ���� ���ϱ�
	// ���� �Ʒ� ���ϱ�
	int top[1600], bottom[1600];
	memset(top,-1,sizeof(int)*1600);
	memset(bottom,-1,sizeof(int)*1600);
	int i,j;

	// �ʱ�ȭ
	int sx = area.x;
	int sy = area.y;
	int ex = area.x+area.width;
	int ey = area.y+area.height;


	for (i=sx;i<ex;i++)
	{
		for (j=sy;j<ey;j++)
		{
			// ������ �ܱ�
			if ((unsigned char)m_ThredImage->imageData[j*m_ThredImage->widthStep+i] == 255)
			{
				top[i] = j;
				break;
			}
		}
		for (j=ey-1;j>=sy;j--)
		{
			// �Ʒ����� �ܱ�
			// ������ �ܱ�
			if ((unsigned char)m_ThredImage->imageData[j*m_ThredImage->widthStep+i] == 255)
			{
				bottom[i] = j;
				break;
			}

		}
	}

	// �ʱ�ȭ
	for (i=0;i<1500;i++)
	{
		m_jabara.up[i] =  -9999.9999;
		m_jabara.down[i] = -9999.9999;
	}
	
	// �� �Ʒ����� ���� �ܴ´�.


	// ���� ���� ��� (����ũ ������ ����-> ����ũ ������� �ݵ�� Ȧ����!)

	if (masksize %2 == 0)
		return false;
	
	// �׽�Ʈ ���


	for (i=sx;i<ex-masksize;i++)
	{
		double nowsum = 0.0;
		double nowsum_d = 0.0;
		double n2_u = 0.0;
		double n2_d = 0.0;
		for (j=0;j<masksize;j++)
		{
			// �� ��ġ���� ��� ����
			nowsum += (double) top[i+j];
			nowsum_d += (double) bottom[i+j];
			n2_u += pow((double)top[i+j],2);
			n2_d += pow((double)bottom[i+j],2);
		}

		double avg_up = nowsum/ (double)masksize;
		double avg_down = nowsum_d/(double)masksize;
		double avg2_up = n2_u / (double)masksize;
		double avg2_down = n2_d / (double)masksize;
		
		m_jabara.up[i] = sqrt(avg2_up-pow(avg_up,2));
		m_jabara.down[i] = sqrt(avg2_down-pow(avg_down,2));

	}

	return true;
}

bool CStrawProcess::CalculateColorDiff(CvRect area, double colorlimit, double brightlimit, int masksize)
{
	// 2008 08 01 ������ ����ũ ���� �� �÷� ���� ���
	if(area.width <= 0 || area.height <=0) return false;
	if (!m_colorsetted)
		return false;

	// ��� ���� ����
	if (m_resultmap != NULL)
	{
		cvReleaseImage(&m_resultmap);
	}

	// ��� �̹��� ����
	m_resultmap = cvCreateImage(cvSize(area.width-masksize,area.height-masksize),8,1);


	int i,j,k,l;
	for (i=0;i<m_resultmap->height;i++)
	{
		for (j=0;j<m_resultmap->width;j++)
		{
			m_resultmap->imageData[(i*m_resultmap->widthStep+j)] = (unsigned char) 0;
		}
	}
	// �ʱ�ȭ
	int sx = area.x;
	int sy = area.y;
	int ex = area.x+area.width;
	int ey = area.y+area.height;

	for (i=sx;i<ex-masksize;i++)
	{
		for (j=sy;j<ey-masksize;j++)
		{
			// ���Ÿ� ���ϱ�
			unsigned char min_Y = 255;
			unsigned char max_Y = 0;
			unsigned char min_Cr = 255;
			unsigned char max_Cr = 0;
			unsigned char min_Cb = 255;
			unsigned char max_Cb = 0;

			for (k = 0;k<masksize;k++)
			{
				for (l = 0;l<masksize;l++)
				{
					// min max ã��
					unsigned char Y = m_YCrCbImage->imageData[(j+l)*m_YCrCbImage->widthStep+(i+k)*3];
					unsigned char Cr = m_YCrCbImage->imageData[(j+l)*m_YCrCbImage->widthStep+(i+k)*3+2];
					unsigned char Cb = m_YCrCbImage->imageData[(j+l)*m_YCrCbImage->widthStep+(i+k)*3+1];

					if (Y<min_Y)
						min_Y = Y;
					if (Y > max_Y)
						max_Y = Y;
					if (Cr < min_Cr)
						min_Cr = Cr;
					if (Cr > max_Cr)
						max_Cr = Cr;
					if (Cb < min_Cb)
						min_Cb = Cb;
					if (Cb > max_Cb)
						max_Cb = Cb;



				}
			}

			// �̷��� �ؼ� ã�� �ִ� - �ּ� �� limit ������ �� ĭ�� ĥ�Ѵ�.
			int flag = 0;
			if ( abs((int)min_Y-(int)max_Y) >(int) brightlimit)
				flag = 1;
			if (abs((int)min_Cb - (int) max_Cb) > (int) colorlimit)
				flag = 1;
			if (abs((int)min_Cr - (int) max_Cr) > (int) colorlimit)
				flag = 1;
			
			if (flag)
				// ��ĥ�ϱ�
				m_resultmap->imageData[((j-sy)*m_resultmap->widthStep+(i-sx))] = (unsigned char) 255;

		}
	}

	return true;
}

double CStrawProcess::InspectPok(CvRect area, double resol, double limit)
{
	// 2008 08 02 ������ - �ܰ� ����
	// fov �޾Ƽ� ���ؼ� ��ȯ��.
	CvRect volatile areas = area;
	if(area.width <= 0 || area.height <=0) return false;

	// ���� �Ʒ� ���ϱ�
	int top[1600], bottom[1600];

	int i,j;

	// �ʱ�ȭ
	int sx = areas.x;
	int sy = areas.y;
	int ex = areas.x+areas.width;
	int ey = areas.y+areas.height;

	int count = 0;
	int sum = 0;
	int min = 9999;
	int max = -1;

	memset(top,-1,sizeof(int)*1600);
	memset(bottom,-1,sizeof(int)*1600);

	double up_sum = 0.0;
	double down_sum = 0.0;

	for (i=sx;i<ex;i++)
	{
		for (j=sy;j<ey;j++)
		{
			// ������ �ܱ�
			if ((unsigned char)m_ThredImage->imageData[j*m_ThredImage->widthStep+i] == 255)
			{
				top[i] = j;
				break;
			}
		}
		for (j=ey-1;j>=sy;j--)
		{
			// �Ʒ����� �ܱ�
			// ������ �ܱ�
			if ((unsigned char)m_ThredImage->imageData[j*m_ThredImage->widthStep+i] == 255)
			{
				bottom[i] = j;
				break;
			}

		}
		// ����
		sum += bottom[i] - top[i];
		up_sum += (double)top[i];
		down_sum += (double)bottom[i];
		count++;

		// �ִ��ּ� ���ϱ� (��׷��� üũ)
		if ((bottom[i] - top[i]) > max)
			max = bottom[i] - top[i];

		if ((bottom[i] - top[i]) < min)
			min = bottom[i] - top[i];
		
	}

	// ��ձ��ϱ�

	double avg = (double) sum / (double) count;

	// �ִ� �ּ� ���� üũ
	// limit ����� ����
	m_dResultData[0] = abs(max-min) * resol;

	m_ResultValue.Rpok[0] = abs(max-min) * resol;
	m_ResultValue.Rpok[1] = limit;

	if (limit < (double) abs(max-min) * resol)
		return -1.0;
	else
	{
		// �ڹٶ�� ��� 2008 08 04 ������
		m_jabara.avg_up = up_sum / (double) count;
		m_jabara.avg_down = down_sum / (double) count;
		return avg * resol;
	}

}

double CStrawProcess::InspectLength(double resol, double limit)
{
	// 2008 08 04 ������
	// ���̸� ��� -> �̹� ������, ������ ã�ƾ� ��

	// ���� ��ã������ ����
	if (m_length.left_bottom == -1 || m_length.left_top == -1 || m_length.right_bottom == -1 || m_length.right_top == -1)
	{
		m_length.longs = -1.0;
		m_length.OK = -1;
		return -1.0;
	}

	// �� �� �ִٸ� ��� ����
	int up_l;
	int bottom_l;
	up_l = m_length.right_top - m_length.left_top;
	bottom_l = m_length.right_bottom - m_length.left_bottom;

	if (up_l <0 || bottom_l < 0)
	{
		m_length.longs = -1.0;
		m_length.OK = -1;
		return -1.0;

	}

	double length_top;
	double length_down;

	length_top = (double) up_l * resol;
	length_down = (double) bottom_l * resol;

	if (limit < fabs(length_down - length_top))
	{
		// �� ���̰� ���� ���� �̻��̸�
		m_length.longs = -1.0;
		m_length.OK = -1;
		return -1.0;

	}

	// ���ݱ��� �������� ��ճ��� ��
	m_length.longs = (length_top+length_down) / 2.0;
	m_length.OK = 1;
	return m_length.longs;
	

}

bool CStrawProcess::CheckJabara(CvRect area, double height, double height_limit, double length, double length_limit)
{

	int top[1600], bottom[1600];

	int i,j;

	// �ʱ�ȭ
	int sx = area.x;
	int sy = area.y;
	int ex = area.x+area.width;
	int ey = area.y+area.height;


	memset(top,-1,sizeof(int)*1600);
	memset(bottom,-1,sizeof(int)*1600);

	int count = 0;
	double avg1 = 0.0;
	double avg2 = 0.0;
	for (i=sx;i<ex;i++)
	{
		for (j=sy;j<ey;j++)
		{
			// ������ �ܱ�
			if ((unsigned char)m_ThredImageCopy->imageData[j*m_ThredImageCopy->widthStep+i] == 255)
			{
				top[i] = j;
				avg1+= (double) j;
				break;
			}
		}
		for (j=ey-1;j>=sy;j--)
		{
			// �Ʒ����� �ܱ�
			// ������ �ܱ�
			if ((unsigned char)m_ThredImageCopy->imageData[j*m_ThredImageCopy->widthStep+i] == 255)
			{
				bottom[i] = j;
				avg2+= (double)j;
				break;
			}

		}
		count ++;
	}

	bool result = false;

	int up_flag = 0;
	int down_flag = 0;

	if (m_jabara.avg_down == -1.0 || m_jabara.avg_up == 1.0)
	{
		// �̻�����
		return false;
	}


	// 2008 08 07 ������ ���� �ۼ� -> 5�ȼ� �̵����
	for (i=sx;i<ex-abs(sx-ex)/10;i++)
	{
		// ���������� ���鼭 �̻� üũ �� ������ ã��

		int min = 9999;
		int max = -1;
		int d_min = 9999;
		int d_max = -1;

		// ��ġ���� ����
		int min_pos = -1;
		int max_pos = -1;
		int dmin_pos = -1;
		int dmax_pos = -1;

		for (j= 0; j<abs(sx-ex)/10;j++)
		{
			if (top[i+j] > max)
			{
				max = top[i+j];
				max_pos = i+j;
			}
			if (top[i+j] < min)
			{
				min = top[i+j];
				min_pos = i+j;
			}
			if (bottom[i+j] > d_max)
			{
				d_max = bottom[i+j];
				dmax_pos = i+j;
			}
			if (bottom[i+j] < d_min)
			{
				d_min = bottom[i+j];
				dmin_pos = i+j;
			}
		}

		// ���ʿ��� ���̰� �̻� ���� �ִ°�?
		if (!up_flag)
		{
			if ((abs(min-max) >= (cvRound(height - height_limit + cvRound(5.0 * m_jabara.up_slide)))) && max_pos < min_pos)
				up_flag = i;
		}

		// �Ʒ��ʿ��� ���̰� �̻� ���� �ִ°�?
		if (!down_flag)
		{
			if ((abs(d_max-d_min) >= (cvRound(height - height_limit + cvRound(5.0 * m_jabara.down_slide)))) && dmax_pos > dmin_pos)
				down_flag = i;

		}

		// �Ѱ� �̻��� ����ó�� (2�� �̻�)
		if (((double)abs(min-max)/3.0) >= (height + height_limit + (5.0 * m_jabara.up_slide)))
			return false;
		if (((double)abs(d_max-d_min)/3.0)>= (height + height_limit + (5.0 * m_jabara.down_slide))) // BHJ 20081128
			return false;

	}

	int rup_flag = 0;
	int rdown_flag = 0;

	// �쿡�� �·� �ܱ�
	for (i=ex-abs(sx-ex)/10-1;i>sx+abs(sx-ex)/10;i--)
	{
		int min = 9999;
		int max = -1;
		int d_min = 9999;
		int d_max = -1;

		// ��ġ���� ����
		int min_pos = -1;
		int max_pos = -1;
		int dmin_pos = -1;
		int dmax_pos = -1;

		for (j= 0; j<abs(sx-ex)/10;j++)
		{

			if (top[i+j] > max)
			{
				max = top[i+j];
				max_pos = i+j;
			}
			if (top[i+j] < min)
			{
				min = top[i+j];
				min_pos = i+j;
			}
			if (bottom[i+j] > d_max)
			{
				d_max = bottom[i+j];
				dmax_pos = i+j;
			}
			if (bottom[i+j] < d_min)
			{
				d_min = bottom[i+j];
				dmin_pos = i+j;
			}

		}

		// ���ʿ��� ���̰� �̻� ���� �ִ°�?
		if (!rup_flag)
		{
			if ((abs(min-max) >= (cvRound((double)height - height_limit + 5.0 * m_jabara.up_slide))) && (max_pos > min_pos))
				rup_flag = i;
		}

		// �Ʒ��ʿ��� ���̰� �̻� ���� �ִ°�?
		if (!rdown_flag)
		{ 
			if ((abs(d_max-d_min) >= (cvRound((double)height - height_limit + 5.0 * m_jabara.down_slide))) && (dmax_pos < dmin_pos))
				rdown_flag = i;

		}

		// �Ѱ� �̻��� ����ó�� (2�� �̻�)
		if (((double)abs(min-max)/3.0) >= fabs((double)height + (double)height_limit + (5.0 * m_jabara.up_slide)))
			return false;
		if (((double)abs(d_max-d_min)/3.0) >= fabs((double)height + (double)height_limit + (5.0 * m_jabara.down_slide))) // BHJ 20081128
			return false;

	}

	double up_length, down_length; 
	up_length = (double)(abs(up_flag-rup_flag) - abs(sx-ex)/10) ;
	down_length  = (double)(abs(down_flag-rdown_flag) - abs(sx-ex)/10) ;

	// Jabara ������, ���� ����Ʈ ������. BHJ
	m_jabara.up[1553] = up_flag;
	m_jabara.up[1554] = top[up_flag];
	m_jabara.down[1553] = down_flag;
	m_jabara.down[1554] = bottom[down_flag];
	m_jabara.up[1556] = rup_flag;
	m_jabara.up[1557] = top[rup_flag];
	m_jabara.down[1556] = rdown_flag;
	m_jabara.down[1557] = bottom[rdown_flag];

	// ���� ����
	m_dResultData[1] = up_length;
	m_dResultData[2] = down_length;
	m_dResultData[3] = fabs((up_length+ down_length)/2.0 - length);
	m_dResultData[4] = length_limit;

	m_ResultValue.Rjabara[0] = up_length;
	m_ResultValue.Rjabara[1] = down_length;
	m_ResultValue.Rjabara[2] = fabs((up_length+ down_length)/2.0 - length);
	m_ResultValue.Rjabara[3] = length_limit;
	if (fabs((up_length+ down_length)/2.0 - length) > length_limit)
	{
			return false;
	}
	
	// ���� ���̸� ��� ����
	double average = (up_length + down_length) / 2.0;
	result = true;


	return result;
}

bool CStrawProcess::CheckEtiq(CvRect area, double length, double limit)
{
	// ���ʿ� ������ �� �� ã�´�.
	// ���� Ȱ��


	bool result = false;



	// �ʱ�ȭ
	int sx = area.x;
	int sy = area.y;
	int ex = area.x+area.width;
	int ey = area.y+area.height;
	int height = ey - sy;
	
	// �� ���� ���� 1/3 �̻��� �־�� ��
	int thres = cvRound(1.0/3.0 * (double)height);

	int i,j;
	int count = 0;
	for (i=sy;i<ey;i++)
	{
		
		int index = sy * m_EdgedImage->widthStep;
		for (j=sx;j<ex;j++)
		{
			int index1 = index + j;
			if ((unsigned char)m_EdgedImage->imageData[index1] == 255)
			{
				// ������.
				count++;
				break;
			}
		}
	}

	int up_count = count;
	count = 0;

	// �Ʒ� �ܱ�
	for (i=ey-1;i>=sy;i--)
	{
		
		int index = sy * m_EdgedImage->widthStep;
		for (j=sx;j<ex;j++)
		{
			int index1 = index + j;
			if ((unsigned char)m_EdgedImage->imageData[index1] == 255)
			{
				// ������.
				count++;
				break;
			}
		}
	}

	int down_count = count;

	// ������� �����Ͽ� ���� �ȼ����� ����Ʈ
	
	

	// 1/3 �̻��ΰ�?

	// ���� ���ΰ�?
	if ( fabs((double)up_count - length) < limit && fabs((double)down_count - length) < limit)
		result = true;

	return result;
}

bool CStrawProcess::CheckColor(CvRect area, double colorlimit, double brightlimit, int masksize, int pix)
{

	bool result = true;

	

	// ����ũ ũ��� Default 5x5 ���
	masksize= 3;
	bool res;
	res = CalculateColorDiff(area,colorlimit,brightlimit,masksize);

	if (res)
	{
	


		// �ٸ� ������� ����
		// �� ���� ������
		int count = 0;
		int limit = pix * 255;


		CvScalar temp = cvSum(m_resultmap);
		double temps = temp.val[0];

		

		if (temps > (double)limit)
	//	if (count > pix)
			result = false;
	}
	else
		return false;

	return result;


}


// ������ 2008 08 05 ���� �����
// Blob Coloring���� ����� stack function

// pix �̻��� �ҷ� �߰� �� OUT
int push(short *stackx, short *stacky, short vx, short vy, int *top)
{
	if(*top>=300000) return(-1);
	(*top)++;
	stackx[*top]=vx;
	stacky[*top]=vy;
	return(1);
}

int pop(short *stackx, short *stacky, short *vx, short *vy, int *top)
{
	if(*top==0) return(-1);
	*vx=stackx[*top];
	*vy=stacky[*top];
	(*top)--;
	return(1);
}


bool CStrawProcess::BlobError(int width, int height, int pix)
{
	bool error = true;
	int i,j,m,n,top,BlobArea[1000];
	short curColor = 0, r, c, area;

	// �������� ����� �޸� �Ҵ� 
	short* stackx=new short [height*width];
	short* stacky=new short [height*width];

	// �󺧸��� �ȼ��� �����ϱ� ���� �޸� �Ҵ� 
	short *coloring = new short [height*width];
	for(i=0; i<height*width; i++) coloring[i]=0; //�޸� �ʱ�ȭ 

	for(i=this_area.y; i<this_area.y+height; i++) 
	{
		for(j=this_area.x; j<this_area.x+width; j++) 
		{
			if(coloring[(i-this_area.y)*width+j-this_area.x]!=0 || m_resultmap->imageData[i*m_resultmap->widthStep+j]!=255) continue;
			r=i; 
			c=j;
			top=0; area=1;
			curColor++;

			while(1) 
			{
GRASSFIRE:
				for(m=r-1; m<=r+1; m++)
				{
					for(n=c-1; n<=c+1; n++) 
					{
						// ���� �ȼ��� �����踦 ����� ó�� ����  
						if(m<0 || m>=height || n<0 || n>=width) continue;

						if((int)m_resultmap->imageData[m*m_resultmap->widthStep+n]==255 && coloring[(m-this_area.height)*width+n-this_area.x]==0)
						{
							//coloring[m*width+n]=curColor;  // ���� �󺧷� ��ũ 
							if(push(stackx,stacky,(short)m,(short)n,&top)==-1) continue;
							r=m; 
							c=n;
							area++;
							goto GRASSFIRE;
						}
					}
				}
				if(pop(stackx,stacky,&r,&c,&top)==-1) break;
			}
			if(curColor<1000) BlobArea[curColor] = area;				
		}
	}

	for (i=0;i<curColor;i++)
	{
		if (BlobArea[i] >= pix)
			error = false;
	}

	delete [] stackx;
	delete [] stacky;
	delete [] coloring;
	return error;
}

bool CStrawProcess::SetColorROIImage(IplImage *image, CvRect area)
{
	// 2008 08 18 ������ �÷��� ��� �̹��� �б�
	// ���� ������ ����
	IplImage *gray ;
	gray= cvCreateImage(cvSize(area.width,area.height),8,1);
	if(image==NULL || area.width <= 0 || area.height <=0) return false;
	if( image->imageData == 0 ) return false;
	// ROI ����
	cvSetImageROI(image,area);
	// ��ȯ�ϱ�
		TRACE("Start5:Cvt\n");
	cvCvtColor(image,gray,CV_RGB2GRAY);TRACE("End5:Cvt\n");

	bool result = SetImage(gray);

	// ������
	cvReleaseImage(&gray);

	if (result)
	{
		// 2008 08 01 �÷� ���� ������
		m_colorsetted = true;

		if(m_RGBImage != NULL)
			cvReleaseImage(&m_RGBImage);

		m_RGBImage =cvCreateImage(cvSize(area.width,area.height),8,3);
		cvCopyImage(image,m_RGBImage);

		if (m_YCrCbImage == NULL)
		{
			m_YCrCbImage = cvCreateImage(cvSize(area.width,area.height),8,3);
		}
		if( m_RGBImage->imageData == 0 ) return false;

		cvCvtColor(m_RGBImage,m_YCrCbImage,CV_RGB2YCrCb);
	}


	cvResetImageROI(image);
	return result;
}

bool CStrawProcess::CheckColor2(CvRect area, double colorlimit, double brightlimit, int pix)
{
	// ���ο� ����� ��
	// �ʱ�ȭ
	int sx = area.x;
	int sy = area.y;
	int ex = area.x+area.width;
	int ey = area.y+area.height;

	int i,j;

	int count1 = 0;
	int count2 = 0;


	bool result = true;

	// �ϴ� ó�� �˻�
	for (i=sy;i<ey;i+=3)
	{
		for (j=sx;j<ex;j+=3)
		{
			// ���Ÿ� ���ϱ�
			unsigned char min_Y = 255;
			unsigned char max_Y = 0;
			unsigned char min_Cr = 255;
			unsigned char max_Cr = 0;
			unsigned char min_Cb = 255;
			unsigned char max_Cb = 0;

			// �ִ� �ּ� ã�´�.
			for (int k=0;k<3;k++)
			{
				// ���� ���� �Ѿ�� continue
				int index = (i+k)*m_YCrCbImage->widthStep;
				if ((i+k) >= m_YCrCbImage->height)
					break;;
				for (int l=0;l<3;l++)
				{
					// ���� �Ѿ�� �Ѿ��
					if ((j+l) >= m_YCrCbImage->width)
						continue;
					// min max ã��
					unsigned char Y = m_YCrCbImage->imageData[index+(j+l)*3];
					unsigned char Cr = m_YCrCbImage->imageData[index+(j+l)*3+2];
					unsigned char Cb = m_YCrCbImage->imageData[index+(j+l)*3+1];

					if (Y<min_Y)
						min_Y = Y;
					if (Y > max_Y)
						max_Y = Y;
					if (Cr < min_Cr)
						min_Cr = Cr;
					if (Cr > max_Cr)
						max_Cr = Cr;
					if (Cb < min_Cb)
						min_Cb = Cb;
					if (Cb > max_Cb)
						max_Cb = Cb;


				}
			}

			// �ִ� �ּ� �Ǻ�
			int flag = 0;
			if ( abs((int)min_Y-(int)max_Y) >(int) brightlimit)
				flag = 1;
			if (abs((int)min_Cb - (int) max_Cb) > (int) colorlimit)
				flag = 1;
			if (abs((int)min_Cr - (int) max_Cr) > (int) colorlimit)
				flag = 1;
			
			if (flag)
				// ��ĥ�ϱ�
				count1++;
		}
	}

	// ��° �˻� (+1 +1)
	for (i=sy+1;i<ey;i+=3)
	{
		for (j=sx+1;j<ex;j+=3)
		{
			// ���Ÿ� ���ϱ�
			unsigned char min_Y = 255;
			unsigned char max_Y = 0;
			unsigned char min_Cr = 255;
			unsigned char max_Cr = 0;
			unsigned char min_Cb = 255;
			unsigned char max_Cb = 0;

			// �ִ� �ּ� ã�´�.
			for (int k=0;k<3;k++)
			{
				// ���� ���� �Ѿ�� continue
				int index = (i+k)*m_YCrCbImage->widthStep;
				if ((i+k) >= m_YCrCbImage->height)
					break;;
				for (int l=0;l<3;l++)
				{
					// ���� �Ѿ�� �Ѿ��
					if ((j+l) >= m_YCrCbImage->width)
						continue;
					// min max ã��
					unsigned char Y = m_YCrCbImage->imageData[index+(j+l)*3];
					unsigned char Cr = m_YCrCbImage->imageData[index+(j+l)*3+2];
					unsigned char Cb = m_YCrCbImage->imageData[index+(j+l)*3+1];

					if (Y<min_Y)
						min_Y = Y;
					if (Y > max_Y)
						max_Y = Y;
					if (Cr < min_Cr)
						min_Cr = Cr;
					if (Cr > max_Cr)
						max_Cr = Cr;
					if (Cb < min_Cb)
						min_Cb = Cb;
					if (Cb > max_Cb)
						max_Cb = Cb;


				}
			}

			// �ִ� �ּ� �Ǻ�
			int flag = 0;
			if ( abs((int)min_Y-(int)max_Y) >(int) brightlimit)
				flag = 1;
			if (abs((int)min_Cb - (int) max_Cb) > (int) colorlimit)
				flag = 1;
			if (abs((int)min_Cr - (int) max_Cr) > (int) colorlimit)
				flag = 1;
			
			if (flag)
				// ��ĥ�ϱ�
				count2++;
		}
	}

	// pix ���� ���
	if ( (double)(count1+count2) >= (double)pix/9.0)
	{
		result = false;
		// ���� �׽�Ʈ
//		cvSaveImage("c:\\imul.bmp",m_RGBImage);
	}

	return result;

}

int CStrawProcess::StrawExistanceCheckForTrans(CvRect partialArea, CvRect totalArea, bool bPrint)
{
	const CvRect insertArea = partialArea;
	const CvRect strawArea = totalArea;
	
	int errType = StrawExistanceCheckForTrans_Total(strawArea, 0);

	if(errType == SUCCESS)
	{
		return true;
	}
	else if(errType != TYPE1) // ��迡 ���� or �߰��� �� or Edge�� �� �ٲ�
	{
		return false;
	}

	// �������� ��� �߰��� �ִ��� Ȯ���� ����� ����
	// ���� ������ �� �� ����

	// �� �ܿ� �ȼ��� ���ġ ����
	int nonZero;
	cvSetImageROI(m_ThredImage, partialArea);
	nonZero = cvCountNonZero(m_ThredImage);
	if(bPrint)
	{
		cvSaveImage(_T("C:\\ATI\\Test\\Count_partial.bmp"), m_ThredImage);
	}
	cvResetImageROI(m_ThredImage);

	int pixels = partialArea.width * partialArea.height;
	double ratio = ((double)nonZero) / pixels;

	if(ratio > (g_transRatio / 100.0))
	{
		return -1;
	}

	return false;
}

int CStrawProcess::StrawExistanceCheckForTrans_Total(CvRect area, int type)
{
	CvRect areas = area;
	// ���� ������ üũ
	// ��ü ������ 1/4 �̻� �� �� ������ ���� ������ �ľ�

	int result = SUCCESS, m_bFirst = false, m_bLast = false;
	double gijun;
	int offset=0;
	if (areas.y < 50)
	{
		m_bFirst = true;
	}
	if (areas.y > _CamHeight - 50)
	{
		m_bLast = true;
	}

	if (m_trans)
	{
		offset = m_nStrawExistOffset;
	}

	// ������� �ٲ�
	if (m_colorstraw && type==1)	// test��� 
	{
		SetColorImageByCOI2by1(m_RGBImage,m_colorthres[m_colorstraw-1]);
	}

	// ROI ����	
	cvSetImageROI(m_ThredImage,areas);
	// NonZero ���ϱ�
	int nonZero = cvCountNonZero(m_ThredImage);
	cvSaveImage(_T("C:\\ATI\\Test\\Count_total.bmp"), m_ThredImage);
	if (m_trans){
		if(m_colorstraw==TRUE) gijun =4.0;
		else gijun = 5.0;
	}
	else
		gijun = 3.0;

	cvResetImageROI(m_ThredImage);

	if (nonZero < areas.width * areas.height / (int)gijun ){
		
		result = TYPE1;
	}
	else
	{
		// ��迡 ��Ҵ��� Ȯ��
		
		if ( m_ThredImage->imageData[ m_ThredImage->widthStep * areas.y + areas.x + areas.width / 3 - 100] != 0 ||
			 m_ThredImage->imageData[ m_ThredImage->widthStep * areas.y + areas.x + areas.width / 3]       != 0 ||
			 m_ThredImage->imageData[ m_ThredImage->widthStep * areas.y + areas.x + areas.width / 3 + 100] != 0 ||
			 m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height - 1) + areas.x + areas.width / 3 - 100] != 0 ||
			 m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height - 1) + areas.x + areas.width / 3]       != 0 ||
			 m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height - 1) + areas.x + areas.width / 3 + 100] != 0)
		{
			return TYPE2;
		}

		/*
		cvCircle(m_ThredImage, cvPoint(areas.x + areas.width / 3 - 100,  areas.y + areas.height / 2 - 5), 3, cvScalar(0), 1, 8, 0);
		cvCircle(m_ThredImage, cvPoint(areas.x + areas.width / 3,		 areas.y + areas.height / 2 - 5), 3, cvScalar(0), 1, 8, 0);
		cvCircle(m_ThredImage, cvPoint(areas.x + areas.width / 3 + 100,  areas.y + areas.height / 2 - 5), 3, cvScalar(0), 1, 8, 0);

		cvCircle(m_ThredImage, cvPoint(areas.x + areas.width / 3 - 100,  areas.y + areas.height / 2), 3, cvScalar(0), 1, 8, 0);
		cvCircle(m_ThredImage, cvPoint(areas.x + areas.width / 3,		 areas.y + areas.height / 2), 3, cvScalar(0), 1, 8, 0);
		cvCircle(m_ThredImage, cvPoint(areas.x + areas.width / 3 + 100,  areas.y + areas.height / 2), 3, cvScalar(0), 1, 8, 0);

		cvCircle(m_ThredImage, cvPoint(areas.x + areas.width / 3 - 100,  areas.y + areas.height / 2 + 5), 3, cvScalar(0), 1, 8, 0);
		cvCircle(m_ThredImage, cvPoint(areas.x + areas.width / 3,		 areas.y + areas.height / 2 + 5), 3, cvScalar(0), 1, 8, 0);
		cvCircle(m_ThredImage, cvPoint(areas.x + areas.width / 3 + 100,  areas.y + areas.height / 2 + 5), 3, cvScalar(0), 1, 8, 0);

		cvShowImage("����", m_ThredImage);
		cvWaitKey(0);
		*/

		// �߰��� ������� Ȯ��
		if ( (m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 - 5) + areas.x + areas.width / 3 - 100] == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 - 5) + areas.x + areas.width / 3]       == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 - 5) + areas.x + areas.width / 3 + 100] == 0)   &&
			 (m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2    ) + areas.x + areas.width / 3 - 100] == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2    ) + areas.x + areas.width / 3]       == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2    ) + areas.x + areas.width / 3 + 100] == 0)   &&
			 (m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 + 5) + areas.x + areas.width / 3 - 100] == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 + 5) + areas.x + areas.width / 3]       == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 + 5) + areas.x + areas.width / 3 + 100] == 0)   )
		{
			return TYPE2;
		}


		int j;
		int numChange[3]={0,0,0};
		int upEdge[3]={0,0,0}, DownEdge[3]={0,0,0};
		bool bDark[3]={1,1,1}, bBright[3]={0,0,0};
		for (j = areas.y ; j < areas.y + areas.height; j++)
		{
			if(bDark[0] == TRUE && bBright[0] == FALSE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset] !=0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset+1] !=0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset-1] !=0){
				bDark[0] = FALSE;
				bBright[0] = TRUE;
				upEdge[0] = j;
				numChange[0]++;
			} 
			else if (bDark[0] == FALSE && bBright[0] == TRUE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset+1] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset-1] ==0)
			{
				bDark[0] = TRUE;
				bBright[0] = FALSE;
				DownEdge[0] = j;
				numChange[0]++;
			}

			if(bDark[1] == TRUE && bBright[1] == FALSE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset] !=0 
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset-1] !=0 
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset+1] !=0 ){
				bDark[1] = FALSE;
				bBright[1] = TRUE;
				upEdge[1] = j;
				numChange[1]++;
			} 
			else if (bDark[1] == FALSE && bBright[1] == TRUE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset-1] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset+1] ==0)
			{
				bDark[1] = TRUE;
				bBright[1] = FALSE;
				DownEdge[1] = j;
				numChange[1]++;
			}

			if(bDark[2] == TRUE && bBright[2] == FALSE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +200 + offset] !=0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +100 + offset-1] !=0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +100 + offset+1] !=0){
				bDark[2] = FALSE;
				bBright[2] = TRUE;
				upEdge[2] = j;
				numChange[2]++;
			} 
			else if (bDark[2] == FALSE && bBright[2] == TRUE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +200 + offset] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +100 + offset-1] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +100 + offset+1] ==0)
			{
				bDark[2] = TRUE;
				bBright[2] = FALSE;
				DownEdge[2] = j;
				numChange[2]++;
			}

		}
		int arealimit1, arealimit2;
		
		if (areas.y<20) 
			arealimit1=50;
		else 
			arealimit1 = 45;

		if(upEdge[0] < 1 || upEdge[1] <1 || upEdge[2] < 1 
			|| DownEdge[0] <1 || DownEdge[1] <1 || DownEdge[2] <1 ) 
			result = TYPE3;

		// ���밡 �߰��� �־�� ��. 
		if (numChange[1] >= 2)
		{
			if(abs((upEdge[1] - areas.y) - (areas.y + areas.height - DownEdge[1])) > arealimit1 ) 
				result = TYPE3;
		}
		else 
			result = TYPE3;
		
		if (numChange[0] >= 2){
			if(abs((upEdge[0] - areas.y) - (areas.y + areas.height - DownEdge[0])) > arealimit1 ) 
				result=TYPE3;
		}else 
			result = TYPE3;

		// ���� ����
		if (numChange[2] >= 2 && !m_trans){
			if(abs((upEdge[2] - areas.y) - (areas.y + areas.height - DownEdge[2])) > arealimit1 ) 
				result=TYPE3;
		}else if (!m_trans)
			result = TYPE3;

		if(areas.y < 20) arealimit2 = areas.height/11;
		else arealimit2 = (areas.height/11);

		if (numChange[1] >= 2)
		{
			if((abs(upEdge[1] - areas.y)< arealimit2 ) || 
				(abs(areas.y + areas.height - DownEdge[1]) < arealimit2) ) 
			{
				result=TYPE3;
			}	
		}
		else 
			result = TYPE3;
		
		if (numChange[0] >= 2){
			if((abs(upEdge[0] - areas.y)< arealimit2) || 
				(abs(areas.y + areas.height - DownEdge[0]) < arealimit2) 
				/*&& abs(DownEdge[1] - upEdge[1]) < 30 */){
				result=TYPE3;
			}
		}
		else 
			result = TYPE3;
	}
	
	return result;

}

bool CStrawProcess::StrawExistanceCheck(CvRect area, int type)
{
	CvRect areas = area;
	// ���� ������ üũ
	// ��ü ������ 1/4 �̻� �� �� ������ ���� ������ �ľ�

	bool result = true, m_bFirst = false, m_bLast = false;
	double gijun;
	int offset=0;
	if (areas.y < 50)
	{
		m_bFirst = true;
	}
	if (areas.y > _CamHeight - 50)
	{
		m_bLast = true;
	}

	if (m_trans)
	{
		offset = m_nStrawExistOffset;
	}

	// ������� �ٲ�
	if (m_colorstraw && type==1)	// test��� 
	{
		SetColorImageByCOI2by1(m_RGBImage,m_colorthres[m_colorstraw-1]);
	}

	// ROI ����	
	cvSetImageROI(m_ThredImage,areas);
	// NonZero ���ϱ�
	int nonZero = cvCountNonZero(m_ThredImage);
	if (m_trans){
		if(m_colorstraw==TRUE) gijun =4.0;
		else gijun = 5.0;
	}
	else
		gijun = 3.0;

	cvResetImageROI(m_ThredImage);

	if (nonZero < areas.width * areas.height / (int)gijun ){//39382
		
		result = false;
	}
	else
	{
		// ��迡 ��Ҵ��� Ȯ��
		if ( m_ThredImage->imageData[ m_ThredImage->widthStep * areas.y + areas.x + areas.width / 3 - 100] != 0 ||
			 m_ThredImage->imageData[ m_ThredImage->widthStep * areas.y + areas.x + areas.width / 3]       != 0 ||
			 m_ThredImage->imageData[ m_ThredImage->widthStep * areas.y + areas.x + areas.width / 3 + 100] != 0 ||
			 m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height - 1) + areas.x + areas.width / 3 - 100] != 0 ||
			 m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height - 1) + areas.x + areas.width / 3]       != 0 ||
			 m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height - 1) + areas.x + areas.width / 3 + 100] != 0)
		{
			return false;
		}

		// �߰��� ������� Ȯ��
		if ( (m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 - 5) + _CamWidth / 2 - 100] == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 - 5) + _CamWidth / 2]       == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 - 5) + _CamWidth / 2 + 100] == 0)   &&
			 (m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2    ) + _CamWidth / 2 - 100] == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2    ) + _CamWidth / 2]       == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2    ) + _CamWidth / 2 + 100] == 0)   &&
			 (m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 + 5) + _CamWidth / 2 - 100] == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 + 5) + _CamWidth / 2]       == 0 ||
			  m_ThredImage->imageData[ m_ThredImage->widthStep * (areas.y + areas.height / 2 + 5) + _CamWidth / 2 + 100] == 0)   )
		{
			return false;
		}


		int j;
		int numChange[3]={0,0,0};
		int upEdge[3]={0,0,0}, DownEdge[3]={0,0,0};
		bool bDark[3]={1,1,1}, bBright[3]={0,0,0};
		for (j = areas.y ; j < areas.y + areas.height; j++)
		{
			if(bDark[0] == TRUE && bBright[0] == FALSE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset] !=0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset+1] !=0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset-1] !=0){
				bDark[0] = FALSE;
				bBright[0] = TRUE;
				upEdge[0] = j;
				numChange[0]++;
			} 
			else if (bDark[0] == FALSE && bBright[0] == TRUE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset+1] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 -100 + offset-1] ==0)
			{
				bDark[0] = TRUE;
				bBright[0] = FALSE;
				DownEdge[0] = j;
				numChange[0]++;
			}

			if(bDark[1] == TRUE && bBright[1] == FALSE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset] !=0 
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset-1] !=0 
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset+1] !=0 ){
				bDark[1] = FALSE;
				bBright[1] = TRUE;
				upEdge[1] = j;
				numChange[1]++;
			} 
			else if (bDark[1] == FALSE && bBright[1] == TRUE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset-1] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 + offset+1] ==0)
			{
				bDark[1] = TRUE;
				bBright[1] = FALSE;
				DownEdge[1] = j;
				numChange[1]++;
			}

			if(bDark[2] == TRUE && bBright[2] == FALSE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +200 + offset] !=0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +150 + offset-1] !=0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +150 + offset+1] !=0){
				bDark[2] = FALSE;
				bBright[2] = TRUE;
				upEdge[2] = j;
				numChange[2]++;
			} 
			else if (bDark[2] == FALSE && bBright[2] == TRUE && m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +200 + offset] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +150 + offset-1] ==0
				&& m_ThredImage->imageData[m_ThredImage->widthStep * j + m_ThredImage->widthStep/2 +150 + offset+1] ==0)
			{
				bDark[2] = TRUE;
				bBright[2] = FALSE;
				DownEdge[2] = j;
				numChange[2]++;
			}

		}
		int arealimit1, arealimit2;
		
		if (areas.y<20) 
			arealimit1=50;
		else 
			arealimit1 = 45;

		if(upEdge[0] < 1 || upEdge[1] <1 || upEdge[2] < 1 
			|| DownEdge[0] <1 || DownEdge[1] <1 || DownEdge[2] <1 ) 
			result = false;

		// ���밡 �߰��� �־�� ��. 
		if (numChange[1] >= 2)
		{
			if(abs((upEdge[1] - areas.y) - (areas.y + areas.height - DownEdge[1])) > arealimit1 ) 
				result = false;
		}
		else 
			result = false;
		
		if (numChange[0] >= 2){
			if(abs((upEdge[0] - areas.y) - (areas.y + areas.height - DownEdge[0])) > arealimit1 ) 
				result=false;
		}else 
			result = false;

		// ���� ����
		if (numChange[2] >= 2 && !m_trans){
			if(abs((upEdge[2] - areas.y) - (areas.y + areas.height - DownEdge[2])) > arealimit1 ) 
				result=false;
		}else if (!m_trans)
			result = false;

		if(areas.y < 20) arealimit2 = areas.height/11;
		else arealimit2 = (areas.height/11);

		if (numChange[1] >= 2)
		{
			if((abs(upEdge[1] - areas.y)< arealimit2 ) || 
				(abs(areas.y + areas.height - DownEdge[1]) < arealimit2) ) 
			{
				result=false;
			}	
		}
		else 
			result = false;
		
		if (numChange[0] >= 2){
			if((abs(upEdge[0] - areas.y)< arealimit2) || 
				(abs(areas.y + areas.height - DownEdge[0]) < arealimit2) 
				/*&& abs(DownEdge[1] - upEdge[1]) < 30 */){
				result=false;
			}
		}
		else 
			result = false;
	}
	
	return result;

}

bool CStrawProcess::CheckPokArea(double pok, double pok_mobum, double limit)
{
	// �������� �������� ���� �������� ����
	bool result = true;

	double cha = fabs(pok-pok_mobum);

	if (cha > limit)
		result = false;

	return result;
}

bool CStrawProcess::CheckTwoGak(double angle1, double angle2, double limit)
{
	// �� �� �� ���ؼ� ���� ���� �Ǻ�
	bool result = true;
	// ���� ǥ������ ������ ������
//	if (angle2 > 999.999 && angle1 >-900.0)
//	{
//		return true;
//	}

	if (angle1 <= -900.0 || angle2 <= -900.0)
	{

		return false;
	}

	double cha = fabs(angle1-angle2);
	if (cha > limit)
	{

		result = false;
	}

	return result;

}

int CStrawProcess::GVData(int x, int y, int nRGB)
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return 0;

	int result=0;
	double dRatioX = (double)_ShowWidth/_CamWidth;
	double dRatioY = (double)_ShowHeight/_CamHeight;
	
	x = (int)(x/dRatioX);
	y = (int)(y/dRatioY);

	if (x>0 && y>0 && x< m_ThredImage->widthStep && y<m_ThredImage->height)
	{
		if (nRGB == 0)
		{	// Blue
			result = m_RGBImage->imageData[y*m_ThredImage->widthStep*3+x*3];
		}
		else if(nRGB ==1){		// Green
			result = m_RGBImage->imageData[y*m_ThredImage->widthStep*3+x*3+1];
		}
		else if(nRGB ==2){		// Red
			result = m_RGBImage->imageData[y*m_ThredImage->widthStep*3+x*3+2];
		}
	}
	if(result==-1) result = 255;
	return abs(result);
}

bool CStrawProcess::CheckColor3(CvRect area, double colorlimit, double brightlimit, int pix)
{
	// ���ο� ���� �˰���
	bool result;


	// ROI üũ�غ�
	if (area.x < 0 || area.x+area.width >= m_ThredImage->width ||area.y < 0 || area.y+area.height >= m_ThredImage->height )
		return false;

	cvReleaseImage(&m_ROI_imul);

	for (int x=0;x<3;x++)
	{
		cvReleaseImage(&m_bin_imul[x]);

		m_bin_imul[x]= cvCreateImage(cvSize(area.width,area.height),8,1);
	}

	m_ROI_imul = cvCreateImage(cvSize(area.width,area.height),8,3);
	

	// �� ROI�ϰ� ����
	cvSetImageROI(m_YCrCbImage,area);
	cvCopy(m_YCrCbImage,m_ROI_imul);

	// ��� ���ϱ�
	CvScalar Avg;

	Avg = cvAvg(m_ROI_imul);


	// ������ Ȧ�� (����ϰ� ����)
	for (int i=0;i<area.height;i++)
	{
		for (int j=0;j<area.width;j++)
		{
			if (abs((unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+j*3] - Avg.val[0]) > brightlimit)
			{
				m_bin_imul[0]->imageData[i*m_bin_imul[0]->widthStep+j] = (unsigned char)255;
			}
			else
			{
				m_bin_imul[0]->imageData[i*m_bin_imul[0]->widthStep+j] = (unsigned char)0;
			}

			if (abs((unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+j*3+1] - (unsigned char)Avg.val[1]) > colorlimit)
			{
				m_bin_imul[1]->imageData[i*m_bin_imul[0]->widthStep+j] = (unsigned char)255;
			}
			else
			{
				m_bin_imul[1]->imageData[i*m_bin_imul[0]->widthStep+j] = (unsigned char)0;
			}

			if (abs((unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+j*3+2] - Avg.val[2]) > colorlimit)
			{
				m_bin_imul[2]->imageData[i*m_bin_imul[0]->widthStep+j] = (unsigned char)255;
			}
			else
			{
				m_bin_imul[2]->imageData[i*m_bin_imul[0]->widthStep+j] = (unsigned char)0;
			}

		}
	}


	bool results= true;
	// Blob �ǽ���
	for (int i=0;i<3;i++)
	{
		results = results && Blob(m_bin_imul[i],pix, this->m_nMaxBlob);
	}
	result = results;


	cvResetImageROI(m_YCrCbImage);


	return result;

}

// Blob Coloring���� ����� stack function
int push(int *stackx, int *stacky, int arr_size, int x, int y, int *top)
{
	if(*top >= arr_size)
		return -1;

	(*top)++;
	stackx[*top] = x;
	stacky[*top] = y;
	return 1;
}

int pop(int *stackx, int *stacky, int *x, int *y, int *top)
{
	if(*top == 0)
		return -1;

	*x = stackx[*top];
	*y = stacky[*top];
	(*top)--;
	return 1;
}
int CStrawProcess::GetMaxBlob()
{
	return m_nMaxBlob;
}
void CStrawProcess::SetMaxBlob(int blob)
{
	m_nMaxBlob = blob;
}

// Blob ���� �Լ�
bool Blob(IplImage *source, int minpix, int &m_nMaxBlob)
{
	int BlobArea[256];
	int curColor = 0;
	int *stackx = new int[source->width * source->height];
	int *stacky = new int[source->width * source->height];
	int arr_size = source->height*source->width;
	m_nMaxBlob = 1;

	// ��ǥ ����
	unsigned char *dst = new unsigned char[source->widthStep*source->height];
	memset(dst,0x00,source->widthStep*source->height);

	// ���� ����
	for (int i=0;i<source->height;i++)
	{
		for (int j=0;j<source->width;j++)
		{
			int index = i*source->widthStep+j;

			// �̹� �湮�� ���̰ų� 255 �ƴϸ� ���
			if (((unsigned char)source->imageData[index] < (unsigned char)255) || ((unsigned char)dst[index] >(unsigned char)0))
				continue;

			int y=i;
			int x=j;
			int top = 0;
			int area = 1;
			curColor++;

			// Grassfire ����
			if (curColor < 256)
			{
				while(1)
				{
GRASSFIRE:
					for (int m=y-1;m<=y+1;m++)
					{
						for (int n=x-1;n<x+1;n++)
						{
							if ((m<0)|| (m>=source->height) || (n<0) || (n>=source->width))
								continue;

							index = m*source->widthStep+n;
							if (((unsigned char)source->imageData[index] == (unsigned char)255) && ((unsigned char)dst[index] == (unsigned char)0))
							{
								dst[index] = curColor;

								if (push(stackx,stacky,arr_size,n,m,&top) == -1)
									continue;

								y=m;
								x=n;
								area++;
								goto GRASSFIRE;
							}

						}

						

					}
					if (pop(stackx,stacky,&x,&y,&top) == -1)
						break;
					
				}	// while ����.	
				BlobArea[curColor] = area;
				if(m_nMaxBlob < area)
				{
					m_nMaxBlob = area;		// �ִ� ũ�� ���ϱ�.
				}
			}	// curColor <256 ����.
			else
			{
				break;
			}
		}
	}

	// ó������ ������ ���� ������ ã��
	bool result = true;
	for (int i=0;i<curColor;i++)
	{
		if (BlobArea[i] >= minpix)
		{
			result = false;
			break;
		}
	}

	// ����Ʈ
	delete [] stackx;
	delete [] stacky;
	delete [] dst;
//	delete [] BlobArea;

	return result;
}


#define MASK_COLOR4 10

bool CStrawProcess::CheckColor4(CvRect area, double colorlimit, double brightlimit, int pix, int type, bool test)
{
	// ���پ� ���� �̵���� ���� �����
	// ���ο� ���� �˰���%


	bool result=TRUE;
	int i;

	// ROI üũ�غ�
	if (area.x < 0 || area.x+area.width >= m_ThredImage->width ||area.y < 0 || area.y+area.height >= m_ThredImage->height
		|| area.width <= 0 || area.height <= 0){
		AfxMessageBox("area ����");
		return false;
	}else{
	

	for (int x=0;x<3;x++)
	{
		cvReleaseImage(&m_bin_imul[x]);

		m_bin_imul[x]= cvCreateImage(cvSize(area.width,area.height),8,1);

		cvSetZero(m_bin_imul[x]);
	}
	if(m_ROI_imul)
	{
		cvReleaseImage(&m_ROI_imul);
	}

	if(m_ROI_imul != NULL)
		cvReleaseImage(&m_ROI_imul);

	m_ROI_imul = cvCreateImage(cvSize(area.width,area.height),8,3); //DEL cwlee

	// �� ROI�ϰ� ����
	cvSetImageROI(m_YCrCbImage,area);

	cvCopy(m_YCrCbImage, m_ROI_imul);

	int width = area.width;
	int height = area.height;
	int nColor1=0, nColor2=0, nimulGV;
	int gabvalue = 0, gabimulTop = 0, gabbackimul = 0, spimul1 =0, spimul2 = 0;		// �̹� ó�� ����.
	
	if(area.width > 15) gabvalue=10;
	else gabvalue = 5;
	if (type==0) gabimulTop = area.width/3;		// ��� �̹� ó�� ���� Ư�� ����.

	if(type==3)
		//nimulGV = _IMULGV_CHUCK;
		nimulGV = m_chuck_limit;
	else nimulGV = m_chuck_limit;
		

	// ���κ��� �ִ� �ּ� ����
	// ��� ���ϴ� Mask�� �ϴ� 5���� �� -> MASK ������
	for (i=0;i<height;i++)
	{
		for (int k=0;k<width-gabvalue;k++)
		{
			int min[5], max[5];
			memset(min,255,5);
			memset(max,0,5);
			unsigned char GVmin=255, GVmax=0, GVmax2=0;
	

			for (int c=0;c<2;c++)
			{	
				
				if(c==0){
					for (int j=0;j<gabvalue;j++)
					{
						// �� �÷����� �ܴ�

							if ((unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j)*3+c] > (unsigned char)GVmax)
							{
								if(abs((unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j-1)*3+c]-(unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j)*3+c]) >(brightlimit*0.6) ||
									abs((unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j+1)*3+c]-(unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j)*3+c]) >(brightlimit*0.6)) {}
								else GVmax = (unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j)*3+c];
							}
							
							if ((unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j)*3+c] < (unsigned char)GVmin){
								if(abs((unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j-1)*3+c]-(unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j)*3+c]) >(brightlimit*0.6) ||
									abs((unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j+1)*3+c]-(unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j)*3+c]) >(brightlimit*0.6)) {}
								else	GVmin = (unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+j)*3+c];
							}
							
							if (k-gabbackimul < 0) gabbackimul = 0;
							else gabbackimul = 5;
							spimul1 =  (unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k-gabbackimul)*3+c];
							spimul2 = (unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k)*3+c];
						
					}
				}
			
				else if(c==1){
					
					if(k+gabvalue <width){
						min[c] = (unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k)*3+c] ;
						max[c] = (unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+gabvalue)*3+c];
						min[c+1] = (unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k)*3+c+1];
						max[c+1] = (unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+gabvalue)*3+c+1];	

						min[c+2] = (unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+1)*3+c];
						max[c+2] = (unsigned char)m_ROI_imul->imageData[i*m_ROI_imul->widthStep+(k+1)*3+c+1];	

					
					}
					else {
						min[c] = 0;
						max[c] = 0;
						min[c+1] = 0;
						max[c+1] = 0;
						min[c+2] = 0;
						max[c+2] = 0;
						min[c+3] = 0;
						max[c+3] = 0;
					}
				}
			}
				// ������
			if (abs(GVmin-GVmax) > brightlimit && GVmax < nimulGV){
				
				if(type==0 && k < gabimulTop && abs(spimul1 - spimul2) > brightlimit && k>5 ){	// ���� �����.
					m_bin_imul[0]->imageData[i*m_bin_imul[0]->widthStep+k] = (unsigned char)255;
					nColor2++;
				}else if (type==0 && k>=gabimulTop)
				{
					m_bin_imul[0]->imageData[i*m_bin_imul[0]->widthStep+k] = (unsigned char)255;
					nColor2++;
				}else if (type !=0)
				{
					m_bin_imul[0]->imageData[i*m_bin_imul[0]->widthStep+k] = (unsigned char)255;
					nColor2++;
				}
				
			}
			else
				m_bin_imul[0]->imageData[i*m_bin_imul[0]->widthStep+k] = (unsigned char)0;
			
			if (abs(min[1]-max[1]) + abs(min[2]-max[2]) > colorlimit ){			// ������1
				if(abs(min[3]-max[1]) + abs(max[3]-max[2]) > colorlimit /*&& abs(min[4]-max[1]) + abs(min[4]-max[2]) > colorlimit */){
					if( (min[1] - max[1]) * (min[2] - max[2]) < 0  && (min[3] - max[1]) * (max[3] - max[2]) < 0){
						if ( abs(min[1] - max[1]) >= abs(min[2] - max[2]))
						{
							m_bin_imul[1]->imageData[i*m_bin_imul[1]->widthStep+k] = (unsigned char)255;
							nColor1 ++;
						}
					}
				}
			}
			else
				m_bin_imul[1]->imageData[i*m_bin_imul[1]->widthStep+k] = (unsigned char)0;
			
		}
	}

	bool results= true;
	// Blob �ǽ���
	for (i=0;i<3;i++)
	{
		if(i==0 && nColor2 >= pix) results = FALSE; 
		
		if(i==1 && nColor1 >= pix) results = FALSE;
		
		if(results==FALSE) break;
	}
	result = results;
	m_ResultValue.Rimul[type][0] = nColor2;
	m_ResultValue.Rimul[type][1] = nColor1;

	if (test==TRUE && result==false)
	{
		// �׽�Ʈ ���
		CString test;
  		for (i=0;i<2;i++)
  		{
  			test.Format("C:\\ATI\\Test\\test_%d_%d_%d_%d.bmp",i, m_nMaxBlob, nColor1, nColor2);
  			cvSaveImage(test,m_bin_imul[i]);
  		}
 		cvSaveImage("C:\\ATI\\Test\\YCBCR.bmp",m_YCrCbImage);
	}

	cvResetImageROI(m_YCrCbImage);

	return result;
	}

}


bool CStrawProcess::SetColorImageByCOI2by1(IplImage *image, int thres)
{
	bool result;
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return false;
	// ������Ȧ���� �ϵ�, COI �ϳ� �޾Ƽ� ��
	// R -> 2, G -> 1, B -> 0
	// �ΰ� �޾� ����
	// �׸��� �ΰ� �� ������ �Ѱ��� ������ ����
	// ������ 2009 03 06
	m_length.left_bottom = -1;
	m_length.left_top = -1;
	m_length.right_bottom = -1;
	m_length.right_top = -1;
	m_length.longs = -1.0;
	m_length.OK = 0;
	m_jabara.avg_down = -1.0;
	m_jabara.avg_up= -1.0;

	int COI1, COI2, COIanti1, COIanti2;
	float ratio1, ratio2;
	if(image->imageData !=0  ){
	// ��û�� �迭
	if (m_colorstraw == 2 || m_colorstraw == 3)
	{
		COI1 = 0;				//		0 : blue, 1 : Green, 2 : Red.
		COI2 = 1;				//
		COIanti1 = 2;			// 
		COIanti2 = 2;
		ratio1 = 1.5;
		ratio2 = 1.0;
	}
	else
	{
		// �����迭
		COI1 = 2;
		COI2 = 2;
		COIanti1 = 0;
		COIanti2 = 1;
		ratio1 = 1;
		ratio2 = 1;
	}

	for (int i=0;i<_CamHeight;i++)
	{
		for (int j=0;j<_CamWidth;j++)

		{
			// �� ä�ΰ� ���ϰ� 2�� ���� ��ճ��� ó����
			double gv, gv1;
			gv = ((double)((unsigned char)image->imageData[i*image->widthStep+j*3+COI1] - (unsigned char)image->imageData[i*image->widthStep+j*3+COIanti1])) ;
			gv1 = ((double)((unsigned char)image->imageData[i*image->widthStep+j*3+COI2] - (unsigned char)image->imageData[i*image->widthStep+j*3+COIanti2])) ;
			gv += gv1;
	
			if (gv < 0)
				gv = 0.0;
			if (gv1 < thres*ratio1/2.0 || gv*ratio2/2.0 < thres/2.0)
				gv = 0.0;


			if (j>300 && (
				(unsigned char)image->imageData[i*image->widthStep+j*3+0]+(unsigned char)image->imageData[i*image->widthStep+j*3+1]+(unsigned char)image->imageData[i*image->widthStep+j*3+2]>270))
			{		// �߰� ���� �̹������� ��� ������ �κ��� ����.
				gv=0;
			}
			
			m_grayImage->imageData[i*m_grayImage->widthStep+j] = (unsigned char)(cvRound(gv));

		}
	}

	m_colorsetted = true;
	// ������Ȧ��

	m_RGBImage = image;
	

	cvCvtColor(image,m_OriginalImage,CV_RGB2GRAY);
	
	cvThreshold(m_grayImage,m_ThredImage,thres,255.0,CV_THRESH_BINARY);

	cvCvtColor(m_RGBImage,m_YCrCbImage,CV_RGB2YCrCb);



	// ī��
	cvCopy(m_ThredImage, m_ThredImageMaguri);

	cvCopy(m_ThredImage, m_ThredImageChuck);
	cvCopy(m_ThredImage, m_ThredImageInsert);
	cvCopy(m_ThredImage, m_ThredImageCopy);
	result = true;
	}
	else{
		s_inspect_temp = false;
	}

	return result;
}

bool CStrawProcess::InspectJabara(const CvRect roiTmp, const int numOfObj, const int marginOfErr, const int minSize, const double ratio, const double cDistStdev, const double cWidthStdev, const bool bPrintout)
{
	CvRect roi = roiTmp;

	bool error;
	
	CMap<int, int, ObjectSpec, ObjectSpec> objSpecMap;

	const int pixels = roi.width * roi.height;

	int* label = new int[pixels];

	// ���� ������׷�: ���� n %��ŭ ī��Ʈ ��
	cvCopy(m_OriginalImage, m_binaryImage);
	RatioHistogram(roi, ratio, bPrintout);
	
	// Labeling
	Labeling(roi, label, false, bPrintout);
	
	// ������Ʈ ���� ȹ�� �� ���� ������Ʈ ����
	GetObjectSpec(roi, label, objSpecMap, minSize, bPrintout);
	
	// ���� x��ǥ�� ������ ��� ��ħ
	GroupObject(objSpecMap, label, roi, bPrintout);
	
	// ������Ʈ�� ���� Ȯ�� �� �ҷ� üũ
	GetDistanceBetweenObject(objSpecMap);
	error = JabaraErrorCheck(
		objSpecMap, 
		numOfObj, 
		marginOfErr,
		cDistStdev, 
		cWidthStdev, 
		bPrintout );


	delete [] label;

	return error;
}

bool CStrawProcess::InspectJabaraEdge(const CvRect roiTmp, const int numOfObj, const int marginOfErr, const double cGradStdev, const int cDiff, const bool bPrintout)
{
	CvRect roi = roiTmp;

	bool error = true;

	// Thresholding	
	// src: m_OriginalImage
	// dst: m_binaryImage
	OtsuThreshold(roi, m_OriginalImage, bPrintout);

	// Get Jabara Edge
	GetStrawEdge(roi);

	// Get Ridges
	int *upRidges = new int[numOfObj];
	int *downRidges = new int[numOfObj];

	GetRidges2(roi, numOfObj, upRidges, downRidges);

	error = JabaraEdgeErrorCheck(
		upRidges, 
		downRidges, 
		numOfObj, 
		marginOfErr,
		cGradStdev, 
		cDiff, 
		bPrintout );

	// �Ʒ� ������ ��� Ȯ�ο�
	// Output
	if(bPrintout == true)
	{
		for ( int h = roi.y; h < roi.y + roi.height; h++ )
		{
			for( int w = roi.x; w < roi.x + roi.width; w++ )
			{
				int idx = h * roi.width + w;
				int imgIdx = (h + roi.y) * (m_RGBImage->widthStep) + (w + roi.x) * m_RGBImage->nChannels;
				
				if ( m_StrawEdge[w].up != -1 )
				{
					// Change Color
					int tmpX = w;
					int tmpY = m_StrawEdge[w].up;
					int tmpIdx = tmpY * m_RGBImage->widthStep + tmpX * m_RGBImage->nChannels;
					
					m_RGBImage->imageData[tmpIdx + 2] = 255;
					m_RGBImage->imageData[tmpIdx + 1] = 255;
					m_RGBImage->imageData[tmpIdx + 0] = 0;
				}
				if ( m_StrawEdge[w].bottom != -1 )
				{
					// Change Color
					int tmpX = w;
					int tmpY = m_StrawEdge[w].bottom;
					int tmpIdx = tmpY * m_RGBImage->widthStep + tmpX * m_RGBImage->nChannels;
					
					m_RGBImage->imageData[tmpIdx + 2] = 255;
					m_RGBImage->imageData[tmpIdx + 1] = 255;
					m_RGBImage->imageData[tmpIdx + 0] = 0;
				}
			}
		}

		int idx;
		for(idx = 0; idx < numOfObj; idx++)
		{
			if(upRidges[idx] != -1)
			{
				//upRidges[idx] => m_StrawEdges�� x �ε���
				// m_StrawEdges[~] -> �̹��� ���� y
				int x, y;
				x = upRidges[idx];
				y = m_StrawEdge[x].up;

				int imgIdx = y * m_RGBImage->widthStep + x * m_RGBImage->nChannels;

				m_RGBImage->imageData[imgIdx + 2] = 255;
				m_RGBImage->imageData[imgIdx + 1] = 0;
				m_RGBImage->imageData[imgIdx + 0] = 0;
			}

			if(downRidges[idx] != -1)
			{
				int x, y;
				x = downRidges[idx];
				y = m_StrawEdge[x].bottom;
				
				int imgIdx = y * m_RGBImage->widthStep + x * m_RGBImage->nChannels;

				m_RGBImage->imageData[imgIdx + 2] = 255;
				m_RGBImage->imageData[imgIdx + 1] = 0;
				m_RGBImage->imageData[imgIdx + 0] = 0;
			}
		}

		cvSaveImage(_T("c:\\ATI\\Test\\�ڹٶ�_������.bmp"), m_RGBImage);
	}


	if(upRidges != NULL)
	{
		delete [] upRidges;
	}
	if(downRidges != NULL)
	{
		delete [] downRidges;
	}
//	if(m_StrawEdge != NULL)
//	{
//		delete [] m_StrawEdge;
//		m_StrawEdge = NULL;
//	}

	return error;
}

// m_binaryImage: ����ȭ�� �̹���, ������� ����
void CStrawProcess::Labeling(const CvRect roiTmp, int label[], bool isEdge, const bool bPrintout)
{
	CvRect roi = roiTmp;

	if ( roi.width  <= 0 ||
		 roi.height <= 0 ) 
	{
		return;
	}

	CMap<int, int, int, int> equalLabelMap;
	equalLabelMap.RemoveAll();

	memset(label, 0, sizeof(int) * roi.width * roi.height);

	char* binImg = m_binaryImage->imageData;
	
	const int startX = roi.x + 1;
	const int startY = roi.y + 1;
	const int endX = roi.x + roi.width - 1;
	const int endY = roi.y + roi.height - 1;
	const int widthStep = m_binaryImage->widthStep;

	// -----------
	// LT	T	RT
	// L	C	R
	// LB	B	RB
	// -----------
	// L = Left, T = Top, R = Right, B = Bottom
	// C = Current

	int left, topLeft, top, rightTop;
	int labelX, labelY;
	int connectedLabel[4];

	int minLabel, minPos;
	int i;

	int currentLabel = 1;

	// First Pass
	for( int y = startY; y < endY; y++ )
	{
		for( int x = startX; x < endX; x++ )
		{
			if(isEdge == true)
			{
				if(m_StrawEdge[x].up >= y || m_StrawEdge[x].up == -1)
					continue;

				if(m_StrawEdge[x].bottom <= y || m_StrawEdge[x].bottom == -1)
					continue;
			}

			if ( binImg[y * widthStep + x] == 0 )
				continue;

			left     = (unsigned char)binImg[y       * widthStep + x - 1];
			topLeft  = (unsigned char)binImg[(y - 1) * widthStep + x - 1];
			top      = (unsigned char)binImg[(y - 1) * widthStep + x    ];
			rightTop = (unsigned char)binImg[(y - 1) * widthStep + x + 1];

			labelX = x - roi.x;
			labelY = y - roi.y;

			// 8-connectivity
			if ( left	  == 0 &&
				 topLeft  == 0 &&
				 top	  == 0 &&
				 rightTop == 0)
			{
				// Labeling
				label[labelY * roi.width + labelX] = currentLabel;
				
				// For Grouping Label
				equalLabelMap.SetAt(currentLabel, currentLabel);
				
				currentLabel++;
			}
			else // Connected
			{
				// Find The Smallest Label
				connectedLabel[0] = label[labelY       * roi.width + labelX - 1]; // left
				connectedLabel[1] = label[(labelY - 1) * roi.width + labelX - 1]; // left top
				connectedLabel[2] = label[(labelY - 1) * roi.width + labelX    ]; // top
				connectedLabel[3] = label[(labelY - 1) * roi.width + labelX + 1]; // right top

				minLabel = INT_MAX;
				minPos = -1;

				for ( i = 0; i < 4; i ++ )
				{
					if (connectedLabel[i] != 0)
					{
						int val;
						equalLabelMap.Lookup(connectedLabel[i], val);

						if(minLabel > val)
						{
							minLabel = val;
							minPos = i;
						}
					}
				}

				if (minPos == -1) // Only One
				{
					equalLabelMap.SetAt(currentLabel, currentLabel);
					label[labelY * roi.width + labelX] = currentLabel;

					currentLabel++;
				}
				else
				{
					label[labelY * roi.width + labelX] = minLabel;

					for ( i = 0; i < 4; i++)
					{
						if (connectedLabel[i] != 0 && connectedLabel[i] != minLabel)
						{
							equalLabelMap.SetAt(connectedLabel[i], minLabel);
						}
					}
				}
			}
		}
	}

	// Second Pass
	for ( int h = 0; h < roi.height; h++ )
	{
		for( int w = 0; w < roi.width; w++ )
		{
			int idx = h * roi.width + w;
			
			if ( label[idx] != 0 )
			{
				int key = label[idx];
				int value;

				while(true)
				{
					equalLabelMap.Lookup(key, value);
					
					if(key == value)
						break;
					
					key = value;
				}

				label[idx] = value;
			}
		}
	}

	// �Ʒ��� �߰� ���
	if(bPrintout == true)
		WriteToFile(roi, label, "c:\\ATI\\Test\\labeling.txt");

	//POSITION pos = equalLabelMap.GetStartPosition();
	//int key;
	//int val;

	//while(pos != NULL)
	//{
	//	equalLabelMap.GetNextAssoc(pos, key, val);
	//	TRACE("%d->%d\n", key, val);
	//}
}

void CStrawProcess::GetMeanWeight(int hist[], const int numOfPixel, const int start, const int end, double& mean, double& weight)
{
	mean = weight = 0.0;

	for ( int i = start; i < end; i++ )
	{
		weight += hist[i];
		mean += ( i * hist[i] );
	}

	if ( weight == 0.0 )
	{
		mean = 0.0;
	}
	else
	{
		mean /= weight;
		weight /= numOfPixel;
	}
}

void CStrawProcess::RatioHistogram(const CvRect roiTmp, const double r, const bool bPrintout)
{
	CvRect roi = roiTmp;

	const int numOfPixels = roi.width * roi.height;
	const int ratio = (int)(numOfPixels / 100 * r);

	const int histLen = 256;
	int hist[histLen];
	memset(hist, 0, sizeof(int) * histLen);

	const int startX = roi.x;
	const int startY = roi.y;
	const int endX = roi.x + roi.width;
	const int endY = roi.y + roi.height;

	int x, y;

	// Initialize Histogram
	for ( x = startX; x < endX; x++ )
	{
		for ( y = startY; y < endY; y++ )
		{
			++hist[((unsigned char)(m_binaryImage->imageData[y * (m_binaryImage->widthStep) + x]))];
		}
	}
	cvSetImageROI( m_binaryImage, roi );
	
	cvResetImageROI( m_binaryImage );

	int sum = 0;
	int threshold;

	// Count Histogram elements
	for ( threshold = histLen - 1; threshold >= 0; threshold-- )
	{
		sum += hist[threshold];

		if ( sum >= ratio )
			break;
	}

	unsigned char pixel;

	// Thresholding
	for ( x = startX; x < endX; x++ )
	{
		for ( y = startY; y < endY; y++ )
		{
			pixel = ((unsigned char)(m_binaryImage->imageData[y * (m_binaryImage->widthStep) + x]));

			if ( pixel >= threshold )
				m_binaryImage->imageData[y * (m_binaryImage->widthStep) + x] = 255;
			else
				m_binaryImage->imageData[y * (m_binaryImage->widthStep) + x] = 0;
		}
	}
	
	if(bPrintout == true)
		cvSaveImage("c:\\ATI\\Test\\�ڹٶ�.bmp", m_binaryImage);
}

void CStrawProcess::GetObjectSpec(const CvRect roiTmp, int label[], CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap, const int minSize, const bool bPrintout)
{
	CvRect roi = roiTmp;

	objSpecMap.RemoveAll();

	ObjectSpec objSpec;

	int x, y;
	int labelVal;
	int len;
	
	for (y = 0; y < roi.height; y++)
	{
		for (x = 0; x < roi.width; x++)
		{
			labelVal = label[y * roi.width + x];
			
			if (labelVal != 0)
			{
				if (objSpecMap.Lookup(labelVal, objSpec) == FALSE)
				{
					objSpec.label    = labelVal;
					objSpec.distance = 0;
					objSpec.pixels   = 1;
					objSpec.rect     = cvRect(x, y, 0, 0);
				}
				else
				{
					objSpec.pixels++;

					// width
					if(x < objSpec.rect.x)
						objSpec.rect.width += (objSpec.rect.x - x);
					else if(x > objSpec.rect.x + objSpec.rect.width)
						objSpec.rect.width = x - objSpec.rect.x;
		
					// height
					len = abs(objSpec.rect.y - y);					
					if (objSpec.rect.height < len)
						objSpec.rect.height = len;

					// Left
					objSpec.rect.x = (x < objSpec.rect.x) ? x : objSpec.rect.x;
					
					// top
					objSpec.rect.y = (y < objSpec.rect.y) ? y : objSpec.rect.y;	
				}
				objSpecMap.SetAt(labelVal, objSpec);
			}
		}
	}

	POSITION pos = objSpecMap.GetStartPosition();

	int key;
	ObjectSpec obj;

	// Delete Small Object
	while(pos != NULL)
	{
		objSpecMap.GetNextAssoc(pos, key, obj);

		// Just Check
		//TRACE("\n\nObject %d's Size: %d, (x: %d, y:%d, width: %d, height:%d)\n\n", key, obj.pixels, obj.rect.x, obj.rect.y, obj.rect.width + 1, obj.rect.height + 1);

		if (obj.pixels <= minSize)
		{
			for ( x = obj.rect.x; x <= (obj.rect.x + obj.rect.width); x++ )
			{
				for ( y = obj.rect.y; y <= (obj.rect.y + obj.rect.height); y++)
					label[y * roi.width + x] = 0;
			}
			objSpecMap.RemoveKey(key);
		}
	}

	// Just Check
	if(bPrintout == true)
		WriteToFile(roi, label, "c:\\ATI\\Test\\GetObjectSpec_labeling.txt");
}

void CStrawProcess::GetDistanceBetweenObject(CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap)
{
	const int len = (int)objSpecMap.GetCount();

	ObjectSpec* objArr = new ObjectSpec[len];

	int idx = 0;
	int i;

	int key;
	ObjectSpec obj;

	POSITION pos = objSpecMap.GetStartPosition();

	// Copy to Array
	while (pos != NULL)
	{
		objSpecMap.GetNextAssoc(pos, key, objArr[idx++]);
	}

	// Sort Ascent Order
	for (idx = 0; idx < len; idx++)
	{
		for (i = 0; i < len - idx - 1; i++)
		{
			if (objArr[i].rect.x > objArr[i + 1].rect.x)
			{
				obj = objArr[i + 1];
				objArr[i + 1] = objArr[i];
				objArr[i] =obj;
			}
		}
	}

	//TRACE("\n\n");
	// Calculate Distance
	for (idx = 0; idx < len - 1; idx++)
	{
		// obj[idx + 1].left - obj[idx].right
		objArr[idx].distance = objArr[idx + 1].rect.x - (objArr[idx].rect.x + objArr[idx].rect.width) - 1;
		objSpecMap.SetAt(objArr[idx].label, objArr[idx]);

		// Just Check
		//TRACE("Distance %d-%d: %d\n", objArr[idx].label, objArr[idx + 1].label, objArr[idx].distance);
	}

	delete [] objArr;
}

void CStrawProcess::GetRidges(const CvRect roiTmp, const int numOfObj, int upRidges[], int downRidges[])
{
	CvRect roi = roiTmp;

	memset(upRidges, -1, sizeof(int) * roi.width);
	memset(downRidges, -1, sizeof(int) * roi.width);

	const int steps = 4;

	int edgeIdx;
	int upIdx = 0;
	int downIdx = 0;
	int step;
	int diff = 0;

	// ���� �ȼ��� ���� ��


	for(edgeIdx = roi.x + steps; edgeIdx < roi.x + roi.width - 2 - steps; edgeIdx++)
	{
		// Up Ridges
		if(m_StrawEdge[edgeIdx].up != -1)
		{
			diff = 0;
			for(step = 1; step <= steps; step++)
			{
				if(m_StrawEdge[edgeIdx].up >= m_StrawEdge[edgeIdx + step].up)
					break;
				if(diff < (m_StrawEdge[edgeIdx + step].up - m_StrawEdge[edgeIdx].up))
					diff = m_StrawEdge[edgeIdx + step].up - m_StrawEdge[edgeIdx].up;
			}
			
			if(step == steps + 1 && diff >= 2)
			{
				upRidges[upIdx++] = edgeIdx;
				edgeIdx += 3;
			}
		}

		if(upIdx == numOfObj)
			break;
	}

	for(edgeIdx = roi.x; edgeIdx < roi.x + roi.width - 2; edgeIdx++)
	{
		// Down Ridges
		if(m_StrawEdge[edgeIdx].bottom != -1)
		{
			diff = 0;
			for(step = 1; step <= steps; step++)
			{
				if(m_StrawEdge[edgeIdx].bottom <= m_StrawEdge[edgeIdx + step].bottom)
					break;
				if(diff < (m_StrawEdge[edgeIdx].bottom - m_StrawEdge[edgeIdx + step].bottom))
					diff = m_StrawEdge[edgeIdx].bottom - m_StrawEdge[edgeIdx + step].bottom;
			}
			
			if(step == steps + 1 && diff >= 2)
			{
				downRidges[downIdx++] = edgeIdx;
				edgeIdx += 3;
			}
		}

		if(downIdx == numOfObj)
			break;
	}

	TRACE("\n\nNumber of up ridges: %d, down ridges: %d\n\n", upIdx, downIdx);
}

void CStrawProcess::GetRidges2(const CvRect roiTmp, const int numOfObj, int upRidges[], int downRidges[])
{	
	CvRect roi = roiTmp;

	memset(upRidges, -1, sizeof(int) * numOfObj);
	memset(downRidges, -1, sizeof(int) * numOfObj);

	const int steps = 4;
	const int goahead = 3;
	
	int edgeIdx;
	int upIdx = 0;
	int downIdx = 0;
	int step;
	
	int lDiff = 0;
	int rDiff = 0;
	
	// Up Ridges
	for(edgeIdx = roi.x + steps; edgeIdx < roi.x + roi.width - 2 - steps; edgeIdx++)
	{
		if(m_StrawEdge[edgeIdx].up != -1)
		{
			lDiff = 0;
			rDiff = 0;

			for(step = -steps; step < 0; step++) // left
			{
				if(m_StrawEdge[edgeIdx].up > m_StrawEdge[edgeIdx + step].up)
					break;

				if(lDiff < (m_StrawEdge[edgeIdx + step].up - m_StrawEdge[edgeIdx].up))
					lDiff = m_StrawEdge[edgeIdx + step].up - m_StrawEdge[edgeIdx].up;
			}
			
			if(lDiff <= 0 || step < 0)
				continue;

			for(step = 1; step <= steps; step++) // right
			{
				if(m_StrawEdge[edgeIdx].up > m_StrawEdge[edgeIdx + step].up)
					break;
				
				if(rDiff < (m_StrawEdge[edgeIdx + step].up - m_StrawEdge[edgeIdx].up))
					rDiff = m_StrawEdge[edgeIdx + step].up - m_StrawEdge[edgeIdx].up;
			}
			
			if(rDiff <= 0 || step <= steps)
				continue;

			if(rDiff >= 2 || lDiff >= 2)
			{
				upRidges[upIdx++] = edgeIdx;
				edgeIdx += goahead;
			}
		}
		
		if(upIdx == numOfObj)
			break;
	}
	
	// Down Ridges
	for(edgeIdx = roi.x + steps; edgeIdx < roi.x + roi.width - 2 - steps; edgeIdx++)
	{
		if(m_StrawEdge[edgeIdx].bottom != -1)
		{
			lDiff = 0;
			rDiff = 0;

			for(step = -steps; step < 0; step++) // left
			{
				if(m_StrawEdge[edgeIdx].bottom < m_StrawEdge[edgeIdx + step].bottom)
					break;

				if(lDiff < (m_StrawEdge[edgeIdx].bottom - m_StrawEdge[edgeIdx + step].bottom))
					lDiff = m_StrawEdge[edgeIdx].bottom - m_StrawEdge[edgeIdx + step].bottom;
			}
			
			if(lDiff <= 0 || step < 0)
				continue;

			for(step = 1; step <= steps; step++) // right
			{
				if(m_StrawEdge[edgeIdx].bottom < m_StrawEdge[edgeIdx + step].bottom)
					break;
				
				if(rDiff < (m_StrawEdge[edgeIdx].bottom - m_StrawEdge[edgeIdx + step].bottom))
					rDiff = m_StrawEdge[edgeIdx].bottom - m_StrawEdge[edgeIdx + step].bottom;
			}
			
			if(rDiff <= 0 || step <= steps)
				continue;
			
			if(rDiff >= 2 || lDiff >= 2)
			{
				downRidges[downIdx++] = edgeIdx;
				edgeIdx += goahead;
			}
		}
		
		if(downIdx == numOfObj)
			break;
	}

	//TRACE("\n\nNumber of up ridges: %d, down ridges: %d\n\n", upIdx, downIdx);
}

bool CStrawProcess::JabaraErrorCheck(CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap, const int numOfObj, const int marginOfErr, const double cDistStdev, const double cWidthStdev, const bool bPrintout)
{	
	bool error = true;

	POSITION pos = objSpecMap.GetStartPosition();
	
	int min, max;
	min = INT_MAX;
	max = INT_MIN;
	
	int minLabel, maxLabel;
	minLabel = -1;
	maxLabel = -1;
	
	int key;
	ObjectSpec obj;
	
	double distMean;
	double distStdev;
	double widthMean;
	double widthStdev;

	int width;

	double distSum = 0;
	double distSqrsum = 0;
	double widthSum = 0;
	double widthSqrsum = 0;

	while(pos != NULL)
	{
		objSpecMap.GetNextAssoc(pos, key, obj);
		
		width = obj.rect.width;

		if (width < min)
		{
			min = width;
			minLabel = obj.label;
		}
		if (width > max)
		{
			max = width;
			maxLabel = obj.label;
		}

		distSum += obj.distance;
		distSqrsum += (obj.distance * obj.distance);

		widthSum += (width + 1);
		widthSqrsum += ((width + 1) * (width + 1));

		//TRACE("Width %d: %d\n", obj.label, width + 1);
	}

	min++;
	max++;

	distMean = distSum / (objSpecMap.GetCount() - 1);
	distStdev = sqrt((distSqrsum / (objSpecMap.GetCount() - 1)) - distMean * distMean);
	
	widthMean = widthSum / objSpecMap.GetCount();
	widthStdev = sqrt((widthSqrsum / objSpecMap.GetCount()) - widthMean * widthMean);
	
	if (distStdev >= cDistStdev) // default: 1.5, 0.9
	{
		error = false;
	}
	
	if (widthStdev >= cWidthStdev) // default: 1.8, 1.1
	{
		error = false;
	}
	
	int diff = abs( (int)(objSpecMap.GetCount()) - numOfObj );

	if ( diff > marginOfErr )
	{
		error = false;
	}

	m_ResultValue.jabara_num = (int)objSpecMap.GetCount();
	m_ResultValue.jabara_distance_stdev = distStdev;
	m_ResultValue.jabara_width_stdev = widthStdev;

	if(bPrintout == true)
	{
		// ���� ���� ���
		//fprintf(m_fpError, _T("\n--------------�ڹٶ�----------------\n"));
		//fprintf(m_fpError, _T("�ڹٶ� �Ÿ� ���: %.3lf, ǥ������: %.5lf\n"), distMean, distStdev);
		//fprintf(m_fpError, _T("�ڹٶ� �β� ���: %.3lf, ǥ������: %.5lf\n"), widthMean, widthStdev);
		//fprintf(m_fpError, _T("�ڹٶ� ����: %d\n"), objSpecMap.GetCount());
		//fprintf(m_fpError, _T("--------------------------------------\n"));
	}

	return error;
}

bool CStrawProcess::JabaraEdgeErrorCheck(int upRidges[], int downRidges[], const int numOfObj, const int marginOfErr, const double cGradStdev, const int cDiff, const bool bPrintout)
{
	// Counting
	int idx;
	int upCnt = 0, downCnt = 0;
	for(idx = 0; idx < numOfObj; idx++)
	{
		if(upRidges[idx] != -1)
			upCnt++;
		if(downRidges[idx] != -1)
			downCnt++;
	}

	if(bPrintout == true)
	{
		//fprintf(m_fpError, _T("\n----------�ڹٶ� ���-------------\n"));
		//fprintf(m_fpError, _T("�� ������ ����: %d, �Ʒ� ������ ����: %d\n\n"), upCnt, downCnt);
	}

	int upDownDiff = abs(upCnt - downCnt);
	int upDownAvg = (upCnt + downCnt) / 2;
	int checkDiff = abs(numOfObj - upDownAvg);

	// �� �Ʒ� �ڹٶ� ���� ���
	// ������ ������� �ʰ� �ִ�, �ּ� ���� ���̸� �� ��
	//if(upDownDiff > marginOfErr || checkDiff > marginOfErr) // del
	//{ // del
	//	return false; // del
	//} // del

	// 1. ��, �Ʒ� �� ���� x�� ��ġ ����(����) ���, ǥ������
	// 2. ������ ���� min, max, �Ʒ����� ���� min, max
	// 3(����). �� ������ �Ÿ� -> ���� �˻� ����

	double gradAvg = 0.0;
	double gradStdev = 0.0;

	int upMin = m_StrawEdge[(upRidges[0])].up;
	int upMax = m_StrawEdge[(upRidges[0])].up;
	int downMin = m_StrawEdge[(downRidges[0])].bottom;
	int downMax = m_StrawEdge[(downRidges[0])].bottom;;

	for(idx = 0; idx < numOfObj; idx++)
	{
		gradAvg += abs(upRidges[idx] - downRidges[idx]);

		if(upMin  > m_StrawEdge[(upRidges[idx])].up)
		{
			if(m_StrawEdge[(upRidges[idx])].up <= 1024 && m_StrawEdge[(upRidges[idx])].up >= 0)
				upMin = m_StrawEdge[(upRidges[idx])].up;
		}
		if(upMax  < m_StrawEdge[(upRidges[idx])].up)
		{
			if(m_StrawEdge[(upRidges[idx])].up <= 1024 && m_StrawEdge[(upRidges[idx])].up >= 0)
				upMax = m_StrawEdge[(upRidges[idx])].up;
		}

		if(downMin  > m_StrawEdge[(downRidges[idx])].bottom)
		{
			if(m_StrawEdge[(downRidges[idx])].bottom <= 1024 && m_StrawEdge[(downRidges[idx])].bottom >= 0)
				downMin = m_StrawEdge[(downRidges[idx])].bottom;
		}
		if(downMax  < m_StrawEdge[(downRidges[idx])].bottom)
		{
			if(m_StrawEdge[(downRidges[idx])].bottom <= 1024 && m_StrawEdge[(downRidges[idx])].bottom >= 0)
				downMax = m_StrawEdge[(downRidges[idx])].bottom;
		}
	}

	//gradAvg /= numOfObj;

	//for(idx = 0; idx < numOfObj; idx++)
	//	gradStdev += ((abs(upRidges[idx] - downRidges[idx]) - gradAvg) * (abs(upRidges[idx] - downRidges[idx]) - gradAvg));
	
	//gradStdev /= numOfObj;


	int upDiff = upMax - upMin;
	int downDiff = downMax - downMin;

	m_ResultValue.jabara_neighbor = (upDiff > downDiff) ? upDiff : downDiff;
	m_ResultValue.jabara_gradient_stdev = 0.0;//gradStdev; // del

	if(bPrintout == true)
	{
		//fprintf(m_fpError, _T("������ ���� ���: %.3lf, ǥ������: %.3lf\n"), gradAvg, gradStdev);
		//fprintf(m_fpError, _T("�� �ּ�: %d, �ִ�: %d, ����: %d\n"), upMin, upMax, upDiff);
		//fprintf(m_fpError, _T("�Ʒ� �ּ�: %d, �ִ�: %d, ����: %d\n"), downMin, downMax, downDiff);
		//fprintf(m_fpError, _T("--------------------------\n"));
	}

	//if(gradStdev > cGradStdev) // del : ��ħ �˻��̹Ƿ� ����� �˻����� ����
	//	return false;
	if(upDiff > cDiff)
		return false;
	if(downDiff > cDiff)
		return false;

	return true;
}

void CStrawProcess::GroupObject(CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap, int label[], const CvRect roiTmp, const bool bPrintout)
{
	CvRect roi = roiTmp;

	// Object�� �����ϰ�
	// map�� array�� ��ȯ
	// ���� x��ǥ�� 50%�̻� �����Ѵٸ� ��ħ
	int x, y;
	int sx, ex;
	int sy, ey;

	int sutureLabel;
	int shareWidth;
	
	int key;
	ObjectSpec obj;
	
	int chkKey;
	ObjectSpec chkObj;

	POSITION pos = objSpecMap.GetStartPosition();
	POSITION chkPos;

	int left, right, top, bottom;

	//TRACE("\n\n");
	while ( pos != NULL )
	{
		objSpecMap.GetNextAssoc(pos, key, obj);

		chkPos = objSpecMap.GetStartPosition();

		while( chkPos != NULL )
		{
			objSpecMap.GetNextAssoc(chkPos, chkKey, chkObj);

			if ( obj.label == chkObj.label )
				continue;
			
			// left: �� �߿� ū ��
			if ( obj.rect.x <= chkObj.rect.x )
				left = chkObj.rect.x;
			else
				left = obj.rect.x;

			// right: �� �߿� ���� ��
			if ( obj.rect.x + obj.rect.width <= chkObj.rect.x + chkObj.rect.width )
				right = obj.rect.x + obj.rect.width;
			else
				right = chkObj.rect.x + chkObj.rect.width;

			shareWidth = right - left;

			if ( (shareWidth >= (obj.rect.width / 2)) && (shareWidth >= (chkObj.rect.width / 2)) )
			{
				//TRACE("Share(%d-%d): %d\n", obj.label, chkObj.label, shareWidth + 1);

				// ����
				// 1. objSpecMap���� ���յ� object ����
				// 2. pixel counting
				// 3. labeling
				// 4. width, height
				ObjectSpec* pBotSpec;
				ObjectSpec* pTopSpec;

				if ( obj.rect.y <= chkObj.rect.y )
				{
					top = obj.rect.y + obj.rect.height;
					bottom = chkObj.rect.y;

					sutureLabel = obj.label;
					
					pTopSpec = &obj;
					pBotSpec = &chkObj; // ���� object
				}
				else
				{
					top = chkObj.rect.y + chkObj.rect.height;
					bottom = obj.rect.y;

					sutureLabel = chkObj.label;

					pTopSpec = &chkObj;
					pBotSpec = &obj;
				}

				for ( y = top + 1; y < bottom; y++ )
				{
					for ( x = left; x <= right; x++ )
					{
						label[y * roi.width + x] = sutureLabel;
						
						pTopSpec->pixels++;
					}
				}

				sx = pBotSpec->rect.x;
				ex = sx + pBotSpec->rect.width;
				sy = pBotSpec->rect.y;
				ey = sy + pBotSpec->rect.height;
				
				for ( y = sy; y <= ey; y++ )
				{
					for ( x = sx; x <= ex; x++ )
					{
						if (label[y * roi.width + x] != 0)
						{
							label[y * roi.width + x] = sutureLabel;

							pTopSpec->pixels++;
						}
					}
				}
				
				if ( obj.rect.x <= chkObj.rect.x )
					left = obj.rect.x;
				else
					left = chkObj.rect.x;

				if ( obj.rect.x + obj.rect.width <= chkObj.rect.x + chkObj.rect.width )
					right = chkObj.rect.x + chkObj.rect.width;
				else
					right = obj.rect.x + obj.rect.width;

				if ( obj.rect.y <= chkObj.rect.y )
					top = obj.rect.y;
				else
					top = chkObj.rect.y;
				
				if ( obj.rect.y + obj.rect.height <= chkObj.rect.y + chkObj.rect.height )
					bottom = chkObj.rect.y + chkObj.rect.height;
				else
					bottom = obj.rect.y + obj.rect.height;

				pTopSpec->rect.x = left;
				pTopSpec->rect.width = right - left;
				pTopSpec->rect.y = top;
				pTopSpec->rect.height = bottom - top;

				objSpecMap.SetAt(pTopSpec->label, *pTopSpec);
				objSpecMap.RemoveKey(pBotSpec->label);
				break;
			}
		}
	}
	
	if(bPrintout == true)
		WriteToFile(roi, label, "c:\\ATI\\Test\\suture.txt");
}

void CStrawProcess::WriteToImage(const CvRect roiTmp, int label[], const char* fileName)
{
	CvRect roi = roiTmp;

	IplImage *img = cvCreateImage(cvSize(roi.width, roi.height), 8, 1);

	int w, h;

	for ( h = 0; h < roi.height; h ++ )
	{
		for ( w = 0; w < roi.width; w++ )
		{
			img->imageData[h * img->widthStep + w] = label[h * roi.width + w];
		}
	}
	
	cvSaveImage(fileName, img);
	cvReleaseImage(&img);
}

void CStrawProcess::WriteToFile(const CvRect roiTmp, int label[], const char* fileName)
{
	CvRect roi = roiTmp;

	FILE* fp;
	char buf[20];
	int i;
	
	fp = fopen(fileName, "wt");
	
	for ( i = 0; i < roi.width * roi.height; i++ )
	{
		if ( i % (roi.width) == 0)
		{
			sprintf(buf, "\n");
			fwrite(buf, sizeof(char), strlen(buf), fp);
		}
		
		if ( label[i] == 0 )
			sprintf(buf, "..");
		else
			sprintf(buf, "%02d", label[i]);
		
		fwrite(buf, sizeof(char), strlen(buf), fp);
	}
	
	fclose(fp);
}

bool CStrawProcess::OtsuThreshold(const CvRect roiTmp, IplImage *img, const bool bPrintout)
{
	CvRect roi = roiTmp;

	const int startX = roi.x;
	const int startY = roi.y;
	const int endX = roi.x + roi.width;
	const int endY = roi.y + roi.height;
	const int numOfPixels = roi.width * roi.height;

	const int histLen = 256;
	int hist[histLen];
	memset(hist, 0, sizeof(int) * histLen);

	int x, y;

	// Get Histogram
	for ( x = startX; x < endX; x++ ) {
		for ( y = startY; y < endY; y++) {
			++hist[((unsigned char)(img->imageData[y * (img->widthStep) + x]))];
		}
	}

	int threshold;
	double wF;   // Weight of Foreground;
	double wB;   // Weight of Background;
	double meanB;
	double meanF;
	double varBetween;
	double varMax = 0.0;

	for ( int histIdx = 0; histIdx < histLen; histIdx++ )
	{
		// Background
		GetMeanWeight(hist, numOfPixels, 
			0, histIdx + 1, 
			meanB, wB);

		// Foreground
		GetMeanWeight(hist, numOfPixels, 
			histIdx + 1, histLen, 
			meanF, wF);

		varBetween = wF * wB * (meanF - meanB) * (meanF - meanB);

		if ( varBetween > varMax )
		{
			threshold = histIdx;
			varMax = varBetween;
		}
	}

	//cvZero(m_binaryImage);

	for ( x = startX; x < endX; x++ ) {
		for ( y = startY; y < endY; y++ ) {
			if ( ((unsigned char)(img->imageData[y * (img->widthStep) + x])) > ((unsigned char)threshold)) {
				m_binaryImage->imageData[y * (m_binaryImage->widthStep) + x] = 255;
			}
			else
				m_binaryImage->imageData[y * (m_binaryImage->widthStep) + x] = 0;
		}
	}
	
	if(bPrintout == true)
	{
		cvSetImageROI(m_binaryImage, roi);
		cvSaveImage("c:\\ATI\\Test\\����_����ȭ.bmp", m_binaryImage);
		cvResetImageROI(m_binaryImage);
		cvCopy(m_binaryImage, m_binaryImage_band);
	}
	//TRACE("\nThreshold: %d\n", threshold);

	return true;
}

void CStrawProcess::GaussianFilter(const CvRect roiTmp, double sigma)
{
	CvRect roi = roiTmp;

	const int width = roi.width;
	const int height = roi.height;
	const int widthStep = m_binaryImage->widthStep;

	char *img = m_binaryImage->imageData;
	
	//Gaussian mask
	int dim = (int)max(3.0, 8 * sigma + 1.0);
	if(dim % 2 == 0)
		dim++; // odd
	int dim2 = (int) dim / 2;
	
	double *pMask = new double[dim];
	
	int w, h, k, i;

	int x;
	for(i=0; i<dim; i++)
	{
		x = i - dim2;
		pMask[i] = exp(-(x*x) / (2*sigma*sigma)) / (sqrt(2.0*PI)*sigma);
	}
	
	double **buf = new double*[height];
	for(i=0; i<height; i++)
	{
		buf[i] = new double[width];
		memset(buf[i], 255, sizeof(double) * width);
	}
	
	double sum1, sum2;

	//vertical convolution
	for(w=0; w<width; w++)
	{
		for(h=0; h<height; h++)
		{
			sum1 = sum2 = 0.0;

			for(k=0; k<dim; k++)//0 1 2
			{
				x = k - dim2 + h; // 0: -1, 1: 0, 2: 1
				if(x >= 0 && x < height)
				{
					sum1 += pMask[k];
					sum2 += (pMask[k] * (unsigned char)img[(x + roi.y) * widthStep + w + roi.x]);
				}
			}
			
			buf[h][w] = sum2 / sum1;
		}
	}
	
	//horizontal convolution
	for(h=0; h<height; h++)
	{
		for(w=0; w<width; w++)
		{
			sum1 = sum2 = 0.0;
			
			for(k=0; k<dim; k++)
			{
				x = k - dim2 + w;
				if(x >= 0 && x < width)
				{
					sum1 += pMask[k];
					sum2 += (pMask[k] * buf[h][x]);
				}
			}
			
			double tmp = sum2 / sum1;

			img[(h + roi.y) * widthStep + (w + roi.x)] = (unsigned char)(tmp >= 255.0 ? 255 : tmp);
		}
	}
	
	delete[] pMask;
	
	for(i=0; i<height; i++)
		delete[] buf[i];
	delete[] buf;

	cvSaveImage("c:\\gaussian.bmp", m_binaryImage);
}

BOOL CStrawProcess::IsIncluded(const CvRect *orig, ObjectSpec *obj, const CvRect *rect)
{
	// �̹��� Ŀ�� ��迡 ���� ���
	// �ڽ� �ȿ� ���Ե� ���
	// ��� ���
	if( (obj->rect.x + orig->x) > (rect->x + rect->width)     ||
		(obj->rect.x + obj->rect.width + orig->x) < (rect->x) ||
		(obj->rect.y + orig->y) > (rect->y + rect->height)    ||
		(obj->rect.y + obj->rect.height + orig->y) < (rect->y))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CStrawProcess::ReInspectBlackPoint(
		const CvRect tmpOrigRoi,
		CPoint rect[],      // ����, ���� ��Ƽ�� ����
		int minSize,       // �ּ� ������
		int threshold,     // ���
		int mask,          // ����ũ ���� ����
		int label[],	   // ���� label
		CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap)
{
	POSITION pos = objSpecMap.GetStartPosition();
	int key;
	ObjectSpec obj;
	CvRect reInspArea = cvRect(rect[0].x, rect[0].y, abs(rect[0].x - rect[1].x), abs(rect[0].y - rect[1].y));
	CvRect origArea = tmpOrigRoi;

	//////////////////////////// ���� ���� ���ϱ�
	CvRect intersectArea;
	if ((origArea.x + origArea.width) > (reInspArea.x) &&
		(origArea.x) < (reInspArea.x + reInspArea.width) &&
		(origArea.y + origArea.height) > (reInspArea.y) &&
		(origArea.y) < (reInspArea.y + reInspArea.height))
	{
		intersectArea.x = origArea.x > reInspArea.x ? origArea.x : reInspArea.x;
		intersectArea.y = origArea.y > reInspArea.y ? origArea.y : reInspArea.y;
		intersectArea.width = (origArea.x + origArea.width) > (reInspArea.x + reInspArea.width) ? (reInspArea.x + reInspArea.width - intersectArea.x) : (origArea.x + origArea.width - intersectArea.x);
		intersectArea.height = (origArea.y + origArea.height) > (reInspArea.y + reInspArea.height) ? (reInspArea.y + reInspArea.height - intersectArea.y) : (origArea.y + origArea.height - intersectArea.y);
	}
	else // ���� ���� �ۿ� ����
	{
		return;
	}
	
	/////////////////////////////// ���� ���� �� object ���ֱ�
	while(pos != NULL)
	{
		objSpecMap.GetNextAssoc(pos, key, obj);
		
		if(IsIncluded(&origArea, &obj, &intersectArea))
		{
			// Delete
			for(int h = obj.rect.y; h <= obj.rect.y + obj.rect.height; h++)
			{
				for(int w = obj.rect.x; w <= obj.rect.x + obj.rect.width; w++)
				{
					int idx = h * origArea.width + w;
					label[idx] = 0;
				}
			}
			objSpecMap.RemoveKey(key);
		}
	}
	
	/////////////////////////////// �ٽ� �˻��ϱ�
	IplConvKernel *elem = cvCreateStructuringElementEx(1, mask, 0, mask / 2, CV_SHAPE_RECT);
	const int re_pixels = intersectArea.width * intersectArea.height;
	int* re_label = new int[re_pixels];
	CMap<int, int, ObjectSpec, ObjectSpec> re_objSpecMap;

	cvSetImageROI( m_OriginalImage, intersectArea );
	cvSetImageROI( m_binaryImage, intersectArea );

	cvMorphologyEx(m_OriginalImage, m_binaryImage, NULL, elem, CV_MOP_BLACKHAT); // Top Hat Transform
	BasicThreshold(intersectArea, threshold, false);
	Labeling(intersectArea, re_label, true, false);
	GetObjectSpec(intersectArea, re_label, re_objSpecMap, minSize, false); // Object�� spec�� ������, ���� object�� ������

	cvResetImageROI( m_OriginalImage );
	cvResetImageROI( m_binaryImage );

	/////////////////////// �ҷ��� ������ ���
	if(re_objSpecMap.GetCount() > 0)
	{
		// ���� label�� re_label ���� �߰�
		// objSpecMap�� �߰�
		int width_diff = intersectArea.x - origArea.x;
		int height_diff = intersectArea.y - origArea.y;
		
		for(int h = 0; h < intersectArea.height; h++)
		{
			for(int w = 0; w < intersectArea.width; w++)
			{
				int re_idx = h * intersectArea.width + w;
				int orig_idx = (h + height_diff) * origArea.width + (w + width_diff);
				if(re_label[re_idx] != 0)
				{
					// label�� �ش� �κ� ����
					label[orig_idx] = re_label[re_idx];
				}
			}
		}

		POSITION pos = re_objSpecMap.GetStartPosition();
		int key;
		ObjectSpec obj;
		ObjectSpec retrieve_obj;
		bool exit_flag = false;
		while(pos != NULL)
		{
			re_objSpecMap.GetNextAssoc(pos, key, obj);
			int id = obj.label;
			while(objSpecMap.Lookup(id, retrieve_obj))
			{
				if(INT_MAX > id)
					id++;
				else
				{
					exit_flag = true;
					break;
				}
			}
			if(exit_flag == false)
			{
				obj.label = id;
				objSpecMap.SetAt(id, obj);
			}
		}
	}

	re_objSpecMap.RemoveAll();
	delete [] re_label;
}

// 1. Minimum Object Size
// 2. Structuring Element Size
// 3. Threshold Value
bool CStrawProcess::InspectBlackPoint(int now_count, CvRect roiTmp,  int cols, int rows, const int threshold, const int minSize, const bool bPrintout, CString part, int partNum, const CvRect roiT, const double persent, int minSizeband, int notinspect)
{
	CString output = _T("");
	CvRect roi = roiTmp;
	
	IplConvKernel *elem = cvCreateStructuringElementEx(cols, rows, cols / 2, rows / 2, CV_SHAPE_RECT);

	const int pixels = roi.width * roi.height;
	int* label = new int[pixels];
	CMap<int, int, ObjectSpec, ObjectSpec> objSpecMap;

	// ���� ��ü ���� ����
	GetStrawEdge(roi);
	
	if ( partNum == STRAW_IMUL_TOP )
	{
		int sx = roi.x + 1;
		int ex = roi.x + roi.width - 1;
		int cnt = 0;

		int maguriGap = _m_nImulMaguriNoInsp;

		for ( int i = sx; i < ex && cnt <= maguriGap; i++ )
		{
			if ( m_StrawEdge[i].up != -1 || m_StrawEdge[i].bottom != -1 )
			{
				m_StrawEdge[i].up = 0;
				m_StrawEdge[i].bottom = 0;
				
				cnt++;
			}
		}
	}
	else if ( partNum == STRAW_IMUL_MID)
	{
		int sx, ex;

		sx = roi.x;
		ex = roi.x + roi.width;

		int reduce = _m_nImulMidToStopper;

		for(int i = sx; i < ex; i++)
		{
			m_StrawEdge[i].up += reduce;
			m_StrawEdge[i].bottom -= reduce;
		}
	}
	else if ( partNum == STRAW_IMUL_CHUCK )
	{
		int cnt = _m_nImulChuckToStopper;

		for(int i = roi.x + roi.width - 1; cnt > 0; i--)
		{
			if(m_StrawEdge[i].up != -1 || m_StrawEdge[i].bottom != -1)
			{
				m_StrawEdge[i].up = 0;
				m_StrawEdge[i].bottom = 0;
				cnt--;
			}
		}
	}

	//dahunj
	cvSetImageROI( m_OriginalImage, roi );
	cvSetImageROI( m_binaryImage, roi );

	// Black top hat transform
	//  - Structuring element���� �۰� ��ο� �κ� �ĺ�
	//  - ROI ���� �ʿ�
	cvMorphologyEx(m_OriginalImage, m_binaryImage, NULL, elem, CV_MOP_BLACKHAT);

	//cvShowImage("2", m_binaryImage);
	//cvWaitKey(0);

	if(bPrintout == true)
	{
		output.Format( _T("c:\\ATI\\Test\\%s_����.bmp"), part.GetBuffer() );
		cvSaveImage( output.GetBuffer(), m_binaryImage );
	}

	// Top Hat transform�� ������ �̹����� ���� ����ȭ ����
	BasicThreshold(roi, threshold, bPrintout); // Default Threshold: 30
	Labeling(roi, label, true, bPrintout); // Labeling ���� �� ROI�� Edge�� �����ϸ�?
	GetObjectSpec(roi, label, objSpecMap, minSize, bPrintout); // Object�� spec�� ������, ���� object�� ������

	cvResetImageROI( m_OriginalImage );
	cvResetImageROI( m_binaryImage );

	if(objSpecMap.GetCount() > 0)
	{
		CSeoilVisionDlg *pMainDlg = (CSeoilVisionDlg*)(AfxGetApp()->GetMainWnd());
		
		if(pMainDlg->m_picture->m_pre->m_imageView.m_inspect_ox.imul_etq_left) // 12. �̹� ���� ��Ƽ�� �˻� ����
		{
			ReInspectBlackPoint(
				roi,
				pMainDlg->m_picture->m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left,
				pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_left_pix,
				pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_left_threshold,
				pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_left_mask,
				label,
				objSpecMap);
		}
		if(pMainDlg->m_picture->m_pre->m_imageView.m_inspect_ox.imul_etq_right) // 13. �̹� ���� ��Ƽ�� �˻� ����
		{
			ReInspectBlackPoint(
				roi,
				pMainDlg->m_picture->m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right,
				pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_right_pix,
				pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_right_threshold,
				pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_right_mask,
				label,
				objSpecMap);
		}
	}

	// ��� ���
	m_ResultValue.Rimul[partNum - STRAW_IMUL_TOP][0] = (int)objSpecMap.GetCount();

	
	
	if ( m_ResultValue.Rimul[partNum - STRAW_IMUL_TOP][0] == 0 )
	{
		cvResetImageROI( m_OriginalImage );
		cvResetImageROI( m_binaryImage );

		double average = 0;
		int Range = 0;

		int Slice_Width = 0;
		int Slice_Num = 0;

		int start_x = 0;
		int end_x = 0;
		int start_y = 0;
		int end_y = 0;

		///New Park
		if ( partNum == STRAW_IMUL_TOP )//////////2
		{
			int max = -9999;
			int min = 9999;

			start_x = roiT.x + roiT.width;
			end_x = roi.x + roi.width;

			for(int j = roi.y; j < roi.y + roi.height; j++)
			{
				for(int i = roi.x; i < roi.x + roi.width; i++)
				{
					int imgIdx = (j) * (m_OriginalImage->widthStep) + (i) * m_OriginalImage->nChannels;

					m_binaryImage->imageData[imgIdx] = 0;
				}
			}

			for(int i = roi.x; i < roi.x + roi.width; i++)
			{
				if(m_StrawEdge[i].up != -1 && m_StrawEdge[i].bottom != -1)
				{
					if(m_StrawEdge[i].bottom > max)
					{
						max = m_StrawEdge[i].bottom;
					}

					if(m_StrawEdge[i].up < min)
					{
						min = m_StrawEdge[i].up;
					}
				}
			}

			Range = (max - min)/3;

			start_y = (max+min)/2 - (Range/2);
			end_y = (max+min)/2 + (Range/2);

			Slice_Num = 10;

			Slice_Width = abs((start_x) - (end_x))/Slice_Num;

			double sum = 0;
			double num = 0;
			int Start = start_x;
			int End = start_x + abs((start_x) - (end_x))/Slice_Num;

			for(int x = Start ; x < End ; x++)
			{
				for(int y = start_y; y < end_y; y++)
				{
					int imgIdx = (y) * (m_OriginalImage->widthStep) + (x) * m_OriginalImage->nChannels;

					sum = sum + (unsigned char)m_OriginalImage->imageData[imgIdx];

					num++;
				}
			}

			average = sum / num;

			for(int x = start_x ; x < end_x ; x++)
			{
				for(int y = start_y; y < end_y; y++)
				{
					int imgIdx = (y) * (m_OriginalImage->widthStep) + (x) * m_OriginalImage->nChannels;

					if((unsigned char)m_OriginalImage->imageData[imgIdx] < average * persent)///////0.95
					{
						m_binaryImage->imageData[imgIdx] = 255;
					}
				}
			}
		} // end of STRAW_IMUL_TOP

		if ( partNum == STRAW_IMUL_MID)//////////3
		{
			int Etiquette_start_y = 0;
			int Etiquette_end_y = 0;

			int count = 0;
			bool Ettiquet = false;

			int max = -9999;
			int min = 9999;

			start_x = roi.x;
			end_x = roi.x + roi.width - notinspect;

			for(int j = roi.y; j < roi.y + roi.height; j++)
			{
				for(int i = roi.x; i < roi.x + roi.width; i++)
				{
					int imgIdx = (j) * (m_OriginalImage->widthStep) + (i) * m_OriginalImage->nChannels;

					m_binaryImage->imageData[imgIdx] = 0;
				}
			}

			for(int i = start_x; i < end_x; i++)
			{
				if(m_StrawEdge[i].up != -1 && m_StrawEdge[i].bottom != -1)
				{
					if(m_StrawEdge[i].bottom > max)
					{
						max = m_StrawEdge[i].bottom;
					}

					if(m_StrawEdge[i].up < min)
					{
						min = m_StrawEdge[i].up;
					}
				}
			}

			Range = (max - min);

			start_y = (max+min)/2;
			end_y = (max+min)/2;

			int num = 0;
			int sum = 0;

			int gv_average = 0;

			Slice_Num = 10;
			Slice_Width = abs((start_x) - (end_x))/Slice_Num;

			start_y = start_y - (Range/3)/2;
			end_y = end_y + (Range/3)/2;

			int Start = start_x;
			int End = start_x + abs((start_x) - (end_x))/Slice_Num;

			for(int x = Start ; x < End ; x++)
			{
				for(int y = start_y; y < end_y; y++)
				{
					int imgIdx = (y) * (m_OriginalImage->widthStep) + (x) * m_OriginalImage->nChannels;

					sum = sum + (unsigned char)m_OriginalImage->imageData[imgIdx];

					num++;
				}
			}

			average = sum / num;

			for(int x = start_x ; x < end_x ; x++)
			{
				for(int y = start_y; y < end_y; y++)
				{
					int imgIdx = (y) * (m_OriginalImage->widthStep) + (x) * m_OriginalImage->nChannels;

					if((unsigned char)m_OriginalImage->imageData[imgIdx] < average * persent)///////0.95
					{
						m_binaryImage->imageData[imgIdx] = 255;
					}
				}
			}
		} // end of STRAW_IMUL_MID

		//cvShowImage("m_OriginalImage", m_OriginalImage);
		//cvShowImage("m_binaryImage", m_binaryImage);
		//cvWaitKey(0);

		if(bPrintout == true)
		{
			output.Format( _T("c:\\ATI\\Test\\%s_�⸧��.bmp"), part.GetBuffer() );
			cvSaveImage( output.GetBuffer(), m_binaryImage );
		}

		cvSetImageROI( m_OriginalImage, roi );
		cvSetImageROI( m_binaryImage, roi );

		// Top Hat transform�� ������ �̹����� ���� ����ȭ ����
		BasicThreshold(roi, threshold, bPrintout); // Default Threshold: 30
		Labeling(roi, label, true, bPrintout); // Labeling ���� �� ROI�� Edge�� �����ϸ�?
		GetObjectSpec(roi, label, objSpecMap, minSizeband, bPrintout); // Object�� spec�� ������, ���� object�� ������

		cvResetImageROI( m_OriginalImage );
		cvResetImageROI( m_binaryImage );

		//cvShowImage("temp", m_binaryImage);
		//cvWaitKey(0);

		if(objSpecMap.GetCount() > 0)
		{
			CSeoilVisionDlg *pMainDlg = (CSeoilVisionDlg*)(AfxGetApp()->GetMainWnd());

			if(pMainDlg->m_picture->m_pre->m_imageView.m_inspect_ox.imul_etq_left)
			{
				ReInspectBlackPoint(
					roi,
					pMainDlg->m_picture->m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left,
					pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_left_pix,
					pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_left_threshold,
					pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_left_mask,
					label,
					objSpecMap);
			}
			if(pMainDlg->m_picture->m_pre->m_imageView.m_inspect_ox.imul_etq_right)
			{
				ReInspectBlackPoint(
					roi,
					pMainDlg->m_picture->m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right,
					pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_right_pix,
					pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_right_threshold,
					pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.imul_etq_right_mask,
					label,
					objSpecMap);
			}
		}


		m_ResultValue.Rimul[partNum - STRAW_IMUL_TOP][0] = (int)objSpecMap.GetCount();
	}
	
	

	// �Ʒ� ������ ��� Ȯ�ο�
	// - start - //////////////////////////////////////////////////////////////////////////	
	if(bPrintout == true)
	{
		for ( int h = 0; h < roi.height; h++ )
		{
			for( int w = 0; w < roi.width; w++ )
			{
				int idx = h * roi.width + w;
				int imgIdx = (h + roi.y) * (m_RGBImage->widthStep) + (w + roi.x) * m_RGBImage->nChannels;
				
				if ( label[idx] != 0 )
				{
					//TRACE("(%d, %d): %d\n", w, h, label[idx]);
					m_RGBImage->imageData[imgIdx + 2] = 255;
					m_RGBImage->imageData[imgIdx + 1] = 0;
					m_RGBImage->imageData[imgIdx + 0] = 0;
				}

				if ( m_StrawEdge[w + roi.x].up != -1 )
				{
					// Change Color
					int tmpX = w + roi.x;
					int tmpY = m_StrawEdge[w + roi.x].up;
					int tmpIdx;
					if(tmpY < 0) {
						tmpY = 0;
					}
					tmpIdx = tmpY * m_RGBImage->widthStep + tmpX * m_RGBImage->nChannels;

					m_RGBImage->imageData[tmpIdx + 2] = 0;
					m_RGBImage->imageData[tmpIdx + 1] = 255;
					m_RGBImage->imageData[tmpIdx + 0] = 0;
				}
				if ( m_StrawEdge[w + roi.x].bottom != -1 )
				{
					// Change Color
					int tmpX = w + roi.x;
					int tmpY = m_StrawEdge[w + roi.x].bottom;
					int tmpIdx;

					if(tmpY < 0) {
						tmpY = 0;
					}
					tmpIdx = tmpY * m_RGBImage->widthStep + tmpX * m_RGBImage->nChannels;
					
					m_RGBImage->imageData[tmpIdx + 2] = 0;
					m_RGBImage->imageData[tmpIdx + 1] = 255;
					m_RGBImage->imageData[tmpIdx + 0] = 0;
				}
			}
		}

		output.Format( _T("c:\\ATI\\Test\\%s_�̹��ҷ�.bmp"), part.GetBuffer() );
		cvSaveImage( output.GetBuffer(), m_RGBImage);
	}
	// - end - //////////////////////////////////////////////////////////////////////////

	//int i;
	//for ( i = 0; i < pixels; i++ )
	//{
	//	if ( label[i] != 0)
	//		label[i] = 255;
	//}
	//WriteToImage(roi, label, "c:\\result.bmp");

	// Release
	cvReleaseStructuringElement(&elem);

//	if(m_StrawEdge != NULL)
//	{
//		delete [] m_StrawEdge;
//		m_StrawEdge = NULL;
//	}
	if(label != NULL)
	{
		delete [] label;
	}


	if ( m_ResultValue.Rimul[partNum - STRAW_IMUL_TOP][0] > 0 )
		return false;

	return true;
}

bool CStrawProcess::InspectInsert(const CvRect tmpRoi, const int threshold, const double stdev)
{
	const CvRect roi = tmpRoi;

	cvSetImageROI(m_OriginalImage, roi);
	cvSetImageROI(m_binaryImage, roi);

	cvThreshold(m_OriginalImage, m_binaryImage, m_nThresTransInsert, 255.0, CV_THRESH_BINARY);

	cvSaveImage(_T("c:\\insert_threshold_image.bmp"), m_binaryImage);

	cvResetImageROI(m_OriginalImage);
	cvResetImageROI(m_binaryImage);

	return true;
}

void CStrawProcess::BasicThreshold(const CvRect roiTmp, const int threshold, const bool bPrintout)
{
	CvRect roi = roiTmp;

	const int histLen = 256;
	int hist[histLen];
	memset(hist, 0, sizeof(int) * histLen);

	const int startX = roi.x;
	const int startY = roi.y;
	const int endX = roi.x + roi.width;
	const int endY = roi.y + roi.height;

	int x, y;

	for ( y = startY; y < endY; y++ )
	{
		for ( x = startX; x < endX; x++ )
		{
			++hist[((unsigned char)(m_binaryImage->imageData[y * (m_binaryImage->widthStep) + x]))];
		}
	}

	unsigned char pixel;

	for ( x = startX; x < endX; x++ )
	{
		for ( y = startY; y < endY; y++ )
		{
			pixel = ((unsigned char)(m_binaryImage->imageData[y * (m_binaryImage->widthStep) + x]));

			if ( pixel > threshold )
				m_binaryImage->imageData[y * (m_binaryImage->widthStep) + x] = 255;
			else
				m_binaryImage->imageData[y * (m_binaryImage->widthStep) + x] = 0;
		}
	}
	
	if(bPrintout == true)
		cvSaveImage(_T("C:\\ATI\\Test\\BasicThreshold.bmp"), m_binaryImage);
}

void CStrawProcess::EqualizeHistogram(const CvRect roiTmp)
{
	CvRect roi = roiTmp;

	cvSaveImage("c:\\ATI\\Test\\before_equalization.bmp", m_OriginalImage);
	const int sx = roi.x;
	const int ex = roi.x + roi.width;
	const int sy = roi.y;
	const int ey = roi.y + roi.height;


	char *img = m_OriginalImage->imageData;
	const int widthStep = m_OriginalImage->widthStep;
	const int width = m_OriginalImage->width;
	const int height = m_OriginalImage->height;
	const int pixels = width * height;

	int hist[256];
	double cdf[256];

	memset(hist, -1, sizeof(int) * 256);
	memset(cdf,  -1, sizeof(double) * 256);

	int x, y;

	// initialize histogram
	for(y = sy; y < ey; y++)
	{
		for(x = sx; x < ex; x++)
		{
			hist[img[y * widthStep + x]]++;
		}
	}

	cdf[0] = hist[0];
	int idx;
	for(idx = 1; idx < 256; idx++)
	{
		cdf[idx] = cdf[idx - 1] + hist[idx];
	}

	for(idx = 0; idx < 256; idx++)
	{
		hist[idx] = (int)((cdf[idx] - cdf[0]) * 255.0 / (cdf[255] - cdf[0]));

		if(hist[idx] > 255)
			hist[idx] = 255;
		else if(hist[idx] < 0)
			hist[idx] = 0;
	}

	for(y = sy; y < ey; y++)
	{
		for(x = sx; x < ex; x++)
		{
			img[y * widthStep + x] = (unsigned char)hist[img[y * widthStep + x]];
		}
	}
	
	cvSaveImage("c:\\ATI\\Test\\equalization.bmp", m_OriginalImage);
}

void CStrawProcess::GetStrawEdge(const CvRect roiTmp)
{
	CvRect roi = roiTmp;

//	m_StrawEdge = new Edge[m_binaryImage->width];  // �ʱ⿡ �ѹ��� �����ϰ� init ���ϵ��� ���� 2017-04-17

	// init
	for(int i = 0; i < m_binaryImage->width; i++)
	{
		m_StrawEdge[i].up = -1;
		m_StrawEdge[i].bottom = -1;
	}

	const int sx = roi.x;
	const int sy = roi.y;
	const int ex = roi.x + roi.width;
	const int ey = roi.y + roi.height;

	const int widthStep = m_binaryImage->widthStep;

	const int checkRange = 3;

	char *img = m_binaryImage->imageData;

	int x, y, rangeIdx;
	
	// Up Edge
	//for(y = (ey + sy) / 2; y >= checkRange; y--)
	for ( y = ( ey + sy ) / 2; y >= sy; y-- ) // �߰����� ���� ��ĵ
	{
		for(x = sx; x < ex; x++) // ���ʿ��� ���������� ��ĵ
		{
			if((unsigned char)img[y * widthStep + x] == 0 || m_StrawEdge[x].up != -1) // ��� �κ� �н�
				continue;

			// �� ��ġ���� ���� �� �ȼ��� ����� �ƴ� ���. ��, ������ ���
			for(rangeIdx = 1; rangeIdx <= checkRange; rangeIdx++)
			{
				if(img[(y - rangeIdx) * widthStep + x] != 0)
					rangeIdx = INT_MAX - 1;
			}

			// �� ��ġ���� ���� �� �ȼ��� ����� ��� �ε��� ����
			if(rangeIdx == checkRange + 1)
			{
				m_StrawEdge[x].up = y;
			}
		}
	}

	// Bottom Edge
	for(y = (ey + sy) / 2; y < ey - checkRange; y++)
	{
		for(x = sx; x < ex; x++)
		{
			if((unsigned char)img[y * widthStep + x] == 0 || m_StrawEdge[x].bottom != -1)
				continue;
			
			for(rangeIdx = 1; rangeIdx <= checkRange; rangeIdx++)
			{
				if(img[(y + rangeIdx) * widthStep + x] != 0)
					rangeIdx = INT_MAX - 1;
			}
			
			if(rangeIdx == checkRange + 1)
			{
				m_StrawEdge[x].bottom = y;
			}
		}
	}

}

bool CStrawProcess::InspectEtiquette(const CvRect roiTmp, const int minSize, const double ratio, const double position, const bool bPrintout)
{
	CvRect roi = roiTmp;

	bool isError = false;

	const int pixels = roi.width * roi.height;
	int *label = new int[pixels];

	CMap<int, int, ObjectSpec, ObjectSpec> objSpecMap;

	// 1. ����ȭ
	cvCopy(m_OriginalImage, m_binaryImage);
	RatioHistogram(roi, ratio, bPrintout);

	// 2. �󺧸�
	Labeling(roi, label, false, bPrintout);

	// 3. ������Ʈ ���� ȹ�� �� ���� ������Ʈ ����
	GetObjectSpec(roi, label, objSpecMap, minSize, bPrintout); // Default: 50

	// 4. ���� üũ
	isError = EtiquetteErrorCheck(objSpecMap, position, roi, bPrintout);

	delete [] label;

	return isError;
}

void CStrawProcess::MeanFilter(const CvRect roiTmp, const int width, const int height)
{
	CvRect roi = roiTmp;

	IplImage *tmpIplImage = cvCreateImage(cvSize(m_binaryImage->width, m_binaryImage->height), 8, 1);
	cvCopy(m_binaryImage, tmpIplImage);

	int x, y, mx, my;
	int sum, val;

	const int div = width * height;

	for(y = roi.y; y < (roi.y + roi.height); y++)
	{
		for(x = roi.x; x < (roi.x + roi.width); x++)
		{
			sum = 0;
			for(mx = x - 1; mx <= x + 1; mx++)
			{
				for(my = y - 1; my <= y + 1; my++)
				{
					sum += ((unsigned char)(m_binaryImage->imageData[my * (m_binaryImage->widthStep) + mx]));
				}
			}
			val = (int)(sum / div + 0.5);

			tmpIplImage->imageData[y * tmpIplImage->widthStep + x] = (unsigned char)(val > 255 ? 255 : val);
		}
	}
	
	cvResetImageROI(m_binaryImage);
	cvResetImageROI(tmpIplImage);

	cvSetImageROI(m_binaryImage, roi);
	cvSetImageROI(tmpIplImage, roi);

	cvCopy(tmpIplImage, m_binaryImage);

	cvResetImageROI(m_binaryImage);
	cvResetImageROI(tmpIplImage);

	cvReleaseImage(&tmpIplImage);

	cvSaveImage("c:\\ATI\\Test\\hey.bmp", m_binaryImage);
}

bool CStrawProcess::EtiquetteErrorCheck(CMap<int, int, ObjectSpec, ObjectSpec> &objSpecMap, const double ratio, const CvRect roiTmp, const bool bPrintout)
{
	CvRect roi = roiTmp;

	bool isError = false;

	const int rightPart = (int)(roi.width * ratio / 100.0);

	int key;
	ObjectSpec obj;

	int objPos = 0;

	POSITION pos = objSpecMap.GetStartPosition();

	while(pos != NULL)
	{
		objSpecMap.GetNextAssoc(pos, key, obj);

		objPos = obj.rect.x + obj.rect.width / 2; // middle of object

		if(objPos >= rightPart)
		{
			isError = true;
			break;
		}
	}

	m_ResultValue.REtiquet[0] = ( int )isError;
	m_ResultValue.REtiquet[1] = ( int )( objPos * 100.0 / roi.width );

	if(bPrintout == true)
	{
		//fprintf(m_fpError, _T("\n------------ ��Ƽ�� -------------\n"));
		//fprintf(m_fpError, _T("��Ƽ�� ��ġ: %d%%\n"), (int)(objPos * 100.0 / roi.width));
		//fprintf(m_fpError, _T("---------------------------\n"));
	}

	return isError;
}

void CStrawProcess::ErrorFileOpen()
{
}

void CStrawProcess::ErrorFileClose()
{
	//if(m_fpError == NULL)
	//	return;

	//fclose(m_fpError);

	//m_fpError = NULL;
}

// ��� �˻�
bool CStrawProcess::InspectMaguri(bool bPrintout)
{
	const CvRect roi = m_roi;
	int top[FIXED_LENGTH];
	int bot[FIXED_LENGTH];

	// ���� ����
	GetTopBotPoints(m_ThredImageMaguri, top, bot);

	if(bPrintout)
	{
		PrintoutBinaryImage(m_ThredImageMaguri, _T("������_����ȭ.bmp"));
		PrintoutTopBot(top, bot, _T("������_��輱.bmp"), 255, 255, 0);
	}

	int topLen;
	int botLen;
	GetMaguriTopBotLength(top, bot, &topLen, &botLen);
	
	CSeoilVisionDlg *pMainDlg = (CSeoilVisionDlg*)(AfxGetApp()->GetMainWnd());
	
	int minLength = pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_Magunus_SlideLength;
	int maxLength = pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_maguri_max_length;
	double maxRatio = pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_maguri_diff_ratio;
	int maguriLength = topLen + botLen;
	double ratio = topLen > botLen? ((double)topLen / botLen) : ((double)botLen / topLen);

	SetInspResultMaguri(minLength, maxLength, maxRatio, topLen, botLen);

	if(maguriLength >= minLength && maguriLength <= maxLength)
	{
		if(maxRatio > ratio)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

int CStrawProcess::GetMaguriTopStartPoint(int side[])
{
	const CvRect roi = m_roi;
	int startPoint = -1;

	const int ahead = 4;
	for(int y = roi.y; y < roi.y + roi.height - ahead; y++)
	{
		if((side[y] - side[y + ahead] == 0) && side[y] != -1)
		{
			startPoint = y;
			break;
		}
	}

	return startPoint;
}
int CStrawProcess::GetMaguriBotStartPoint(int side[])
{
	const CvRect roi = m_roi;
	int startPoint = -1;

	const int ahead = 4;
	for(int y = roi.y + roi.height; y > roi.y + ahead; y--)
	{
		if((side[y] - side[y - ahead] == 0) && side[y] != -1)
		{
			startPoint = y;
			break;
		}
	}

	return startPoint;
}

bool CStrawProcess::InspectMaguriSide(bool bPrintout)
{
	const CvRect roi = m_roi;
	int side[FIXED_LENGTH];
	
	GetMaguriSidePoints(side);
	if(bPrintout)
		PrintoutMaguriSide(side);

	int upCnt = 0;
	int downCnt = 0;
	int state = -1;

	int topStartPoint = GetMaguriTopStartPoint(side);
	int botStartPoint = GetMaguriBotStartPoint(side);

	const int ahead = 3;
	m_ResultValue.maguri.isSideOk = true;
	for(int y = topStartPoint; y < botStartPoint - ahead; y++)
	{
		if(abs(side[y] - side[y + ahead]) > g_maguriSideDepth)
		{
			m_ResultValue.maguri.isSideOk = false;
		}
	}

	bool bFlag = (m_ResultValue.maguri.isSideOk != 0);

	return bFlag;
}

bool CStrawProcess::InspectMaguriAngle(bool bPrintout)
{
	int top[FIXED_LENGTH];
	int bot[FIXED_LENGTH];
	
	GetTopBotPoints(m_ThredImageMaguri, top, bot);

	CSeoilVisionDlg *pMainDlg = (CSeoilVisionDlg*)(AfxGetApp()->GetMainWnd());

	bool isOk = IsTopBotAngleOk(
		bot, // �������� �������� �ִ� ��츦 �˻��ϹǷ� top, bot�� �Ųٰ� ��!
		top,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_maguri_angle_gap,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_maguri_angle_continuous,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_maguri_angle_diff);

	m_ResultValue.maguri.isAngleOk = isOk;

	return isOk;
}

void CStrawProcess::PrintoutTopBot(int *top, int *bot, CString fileName, unsigned char r, unsigned char g, unsigned char b)
{
	const CvRect roi = m_roi;
	unsigned char *img = (unsigned char*)m_RGBImage->imageData;
	const int widthStep = m_RGBImage->widthStep;
	const int channels = m_RGBImage->nChannels;
	int imgIdx;

	for(int x = roi.x; x < roi.x + roi.width; x++)
	{
		if(top[x] != -1)
		{
			imgIdx = top[x] * widthStep + x * channels;
			img[imgIdx + 2] = r;
			img[imgIdx + 1] = g;
			img[imgIdx + 0] = b;
		}

		if(bot[x] != -1)
		{
			imgIdx = bot[x] * widthStep + x * channels;
			img[imgIdx + 2] = r;
			img[imgIdx + 1] = g;
			img[imgIdx + 0] = b;
		}
	}

	cvSetImageROI(m_RGBImage, roi);
	fileName = SAVE_PATH_TEST_RESULT + fileName;
	cvSaveImage(fileName.GetBuffer(), m_RGBImage);
	cvResetImageROI(m_RGBImage);
}

void CStrawProcess::PrintoutMaguriSide(int *side)
{
	const CvRect roi = m_roi;

	for(int y = roi.y; y < roi.y + roi.height; y++)
	{
		if(side[y] != -1)
		{
			int imgIdx = y * m_RGBImage->widthStep + side[y] * m_RGBImage->nChannels;

			m_RGBImage->imageData[imgIdx + 2] = 128;
			m_RGBImage->imageData[imgIdx + 1] = 0;
			m_RGBImage->imageData[imgIdx + 0] = 255;
		}
	}

	CString output = SAVE_PATH_TEST_RESULT;
	output += _T("������_����.bmp");
	cvSetImageROI(m_RGBImage, roi);
	cvSaveImage(output.GetBuffer(), m_RGBImage);
	cvResetImageROI(m_RGBImage);
}

void CStrawProcess::PrintoutBinaryImage(IplImage *img, CString fileName)
{
	const CvRect roi = m_roi;
	fileName = SAVE_PATH_TEST_RESULT + fileName;

	cvSetImageROI(img, roi);
	cvSaveImage(fileName.GetBuffer(), img);
	cvResetImageROI(img);
}

void CStrawProcess::GetTopBotPoints(IplImage *iplImg, int *top, int *bot)
{
	if(iplImg->nChannels != 1)
		return;

	memset(top, -1, sizeof(int) * FIXED_LENGTH);
	memset(bot, -1, sizeof(int) * FIXED_LENGTH);

	const CvRect roi = m_roi;
	unsigned char *img = (unsigned char*)iplImg->imageData;
	const int widthStep = iplImg->widthStep;

	int sx = roi.x;
	int ex = roi.x + roi.width;
	int sy = roi.y + roi.height / 2;
	int top_ey = roi.y;
	int bot_ey = roi.y + roi.height;

	for(int x = sx; x < ex; x++)
	{
		// top
		for(int y = sy; y > top_ey; y--)
		{
			if( img[(y - 0) * widthStep + x] == 255 &&
				img[(y - 1) * widthStep + x] == 0 &&
				img[(y - 2) * widthStep + x] == 0 &&
				img[(y - 3) * widthStep + x] == 0)
			{
				top[x] = y;
				break;
			}
		}

		// bot
		for(int y = sy; y < bot_ey; y++)
		{
			if( img[(y + 0) * widthStep + x] == 255 &&
				img[(y + 1) * widthStep + x] == 0 &&
				img[(y + 2) * widthStep + x] == 0 &&
				img[(y + 3) * widthStep + x] == 0)
			{
				bot[x] = y;
				break;
			}
		}
	}
}

void CStrawProcess::GetMaguriSidePoints(int *side)
{
	memset(side, -1, sizeof(int) * FIXED_LENGTH);

	const CvRect roi = m_roi;
	unsigned char *img = (unsigned char*)(m_ThredImageMaguri->imageData);
	int widthStep = m_ThredImageMaguri->widthStep;

	for(int y = roi.y; y < roi.y + roi.height; y++)
	{
		for(int x = roi.x + 3; x < roi.x + roi.width - 3; x++)
		{
			if( (img[y * widthStep + x + 2]) == 255 &&
				(img[y * widthStep + x + 1]) == 255 &&
				(img[y * widthStep + x + 0]) == 255 &&
				(img[y * widthStep + x - 1]) == 0 &&
				(img[y * widthStep + x - 2]) == 0 &&
				(img[y * widthStep + x - 3]) == 0)
			{
				side[y] = x;
				break;
			}
		}
	}
}

void CStrawProcess::SetROI(int x, int y, int width, int height)
{
	m_roi.x = x;
	m_roi.y = y;
	m_roi.width = width;
	m_roi.height = height;
}

CvRect CStrawProcess::GetROI()
{
	return m_roi;
}

void CStrawProcess::SetInspResultMaguri(int inputMinLen, int inputMaxLen, double ratio, int topLen, int botLen)
{
	m_ResultValue.maguri.inputMinLength = inputMinLen;
	m_ResultValue.maguri.inputMaxLength = inputMaxLen;
	m_ResultValue.maguri.topLen = topLen;
	m_ResultValue.maguri.botLen = botLen;
	m_ResultValue.maguri.isAngleOk = FALSE;
	m_ResultValue.maguri.isSideOk = FALSE;
}

void CStrawProcess::GetMaguriTopBotLength(int top[], int bot[], int *topLen, int *botLen)
{
	const CvRect roi = m_roi;

	const int next = 10; // 6
	const int cont = 3; // 3

	int topCnt = 0;
	int botCnt = 0;
	int topMaxCnt = 0;
	int botMaxCnt = 0;

	for(int x = roi.x; x < roi.x + roi.width - next - cont; x++) {
		int checkIdx;
		for(checkIdx = 0; checkIdx < cont; checkIdx++) {
			if(top[x + checkIdx] != -1 && top[x + checkIdx + next] != -1) {
				if(top[x + checkIdx] - top[x + checkIdx + next] <= 0) {
					break;
				}
			} else {
				break;
			}
		} // end of for

		if(checkIdx == cont) {
			topCnt++;
			if(topCnt > topMaxCnt)
				topMaxCnt = topCnt;
		} else {
			topCnt = 0;
		}

		for(checkIdx = 0; checkIdx < cont; checkIdx++) {
			if(bot[x + checkIdx] != -1 && bot[x + checkIdx + next] != -1) {
				if(bot[x + checkIdx] - bot[x + checkIdx + next] >= 0)
					break;
			} else {
				break;
			}
		}

		if(checkIdx == cont) {
			botCnt++;
			if(botCnt > botMaxCnt)
				botMaxCnt = botCnt;
		} else {
			botCnt = 0;
		}
	} // end of for

	*topLen = topMaxCnt;
	*botLen = botMaxCnt;
}

bool CStrawProcess::IsTopBotAngleOk(int *top, int *bot, const int next, const int cont, const int inputDiff)
{
	const CvRect roi = m_roi;

	for(int x = roi.x; x < roi.x + roi.width; x++)
	{
		int checkIdx;
		for(checkIdx = 0; checkIdx < cont; checkIdx++)
		{
			if(top[x + checkIdx] != -1 && top[x + checkIdx + next] != -1)
			{
				int diff = top[x + checkIdx] - top[x + checkIdx + next];

				if(diff >= inputDiff) {}
				else break;
			}
			else
				 break;
		}
		if(checkIdx == cont)
		{
			return false;
		}

		for(checkIdx = 0; checkIdx < cont; checkIdx++)
		{
			if(bot[x + checkIdx] != -1 && bot[x + checkIdx + next] != -1)
			{
				int diff = bot[x + checkIdx] - bot[x + checkIdx + next];

				if(diff <= -inputDiff) {}
				else break;
			}
			else break;

			
		}
		if(checkIdx == cont)
		{
			return false;
		}
	}

	return true;
}

bool CStrawProcess::InspectMidAngle(bool bPrintout)
{
	int top[FIXED_LENGTH];
	int bot[FIXED_LENGTH];

	GetTopBotPoints(m_ThredImageMaguri, top, bot);

	if(bPrintout)
		PrintoutTopBot(top, bot, _T("�ߴ�_��輱.bmp"), 255, 255, 0);

	CSeoilVisionDlg *pMainDlg = (CSeoilVisionDlg*)(AfxGetApp()->GetMainWnd());

	bool isAngleOk;
	bool isHeightOk;

	int minHeight;
	int maxHeight;

	IsTopBotAngleHeightOk(
		top,
		bot,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_mid_angle_gap,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_mid_angle_continuous,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_mid_angle_diff,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_mid_angle_height_diff,
		&isAngleOk,
		&isHeightOk,
		&minHeight,
		&maxHeight);

	if(isAngleOk)
	{
		isAngleOk = IsTopBotAngleOk(
			bot,
			top,
			pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_mid_angle_gap,
			pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_mid_angle_continuous,
			pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_mid_angle_diff);
	}

	m_ResultValue.mid.isAngleOk = isAngleOk;
	m_ResultValue.mid.isHeightOk = isHeightOk;
	m_ResultValue.mid.minHeight = minHeight;
	m_ResultValue.mid.maxHeight = maxHeight;

	return (isAngleOk && isHeightOk);
}

void CStrawProcess::IsTopBotAngleHeightOk(int *top, int *bot, const int next, const int cont, const int inputDiff, const int inputHeightDiff, bool *isAngleOk, bool *isHeightOk, int *outMinHeight, int *outMaxHeight)
{
	const CvRect roi = m_roi;

	int maxHeight = 0;
	int minHeight = 1000;

	*isAngleOk = true;

	for(int x = roi.x; x < roi.x + roi.width; x++)
	{
		if(top[x] != -1 && bot[x] != -1)
		{
			int height = bot[x] - top[x];

			if(height > maxHeight) maxHeight = height;
			if(height < minHeight) minHeight = height;
		}

		int checkIdx;
		for(checkIdx = 0; checkIdx < cont; checkIdx++)
		{
			if(top[x + checkIdx] != -1 && top[x + checkIdx + next] != -1)
			{
				int diff = top[x + checkIdx] - top[x + checkIdx + next];

				if(diff >= inputDiff) {}
				else break;
			}
			else
				 break;
		}
		if(checkIdx == cont)
		{
			*isAngleOk = false;
		}

		for(checkIdx = 0; checkIdx < cont; checkIdx++)
		{
			if(bot[x + checkIdx] != -1 && bot[x + checkIdx + next] != -1)
			{
				int diff = bot[x + checkIdx] - bot[x + checkIdx + next];

				if(diff <= -inputDiff) {}
				else break;
			}
			else break;
		}
		if(checkIdx == cont)
		{
			*isAngleOk = false;
		}
	}

	*outMinHeight = minHeight;
	*outMaxHeight = maxHeight;

	if(maxHeight - minHeight >= inputHeightDiff)
	{
		*isHeightOk = false;
	}
	else
	{
		*isHeightOk = true;
	}
}

bool CStrawProcess::InspectTopAngle(bool bPrintout)
{
	int top[FIXED_LENGTH];
	int bot[FIXED_LENGTH];

	GetTopBotPoints(m_ThredImageMaguri, top, bot);

	if(bPrintout)
		PrintoutTopBot(top, bot, _T("���_��輱.bmp"), 255, 255, 0);

	CSeoilVisionDlg *pMainDlg = (CSeoilVisionDlg*)(AfxGetApp()->GetMainWnd());

	bool isAngleOk;
	bool isHeightOk;

	int minHeight;
	int maxHeight;

	IsTopBotAngleHeightOk(
		top,
		bot,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_top_angle_gap,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_top_angle_continuous,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_top_angle_diff,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_top_angle_height_diff,
		&isAngleOk,
		&isHeightOk,
		&minHeight,
		&maxHeight);

	if(isAngleOk)
	{
		isAngleOk = IsTopBotAngleOk(
			bot,
			top,
			pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_top_angle_gap,
			pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_top_angle_continuous,
			pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_top_angle_diff);
	}

	m_ResultValue.top.isAngleOk = isAngleOk;
	m_ResultValue.top.isHeightOk = isHeightOk;
	m_ResultValue.top.minHeight = minHeight;
	m_ResultValue.top.maxHeight = maxHeight;

	return (isAngleOk && isHeightOk);
}

bool CStrawProcess::InspectChuckAngle(bool bPrintout)
{
	int top[FIXED_LENGTH];
	int bot[FIXED_LENGTH];

	GetTopBotPoints(m_ThredImageChuck, top, bot);
	
	if(bPrintout)
		PrintoutTopBot(top, bot, _T("���_��輱.bmp"), 255, 255, 0);

	bool isAngleOk = false;
	bool isHeightOk = false;
	int minHeight = 0;
	int maxHeight = 0;

	CSeoilVisionDlg *pMainDlg = (CSeoilVisionDlg*)(AfxGetApp()->GetMainWnd());
	
	isAngleOk = IsTopBotAngleOk(
		top,
		bot,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_chuck_angle_gap,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_chuck_angle_continuous,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_chuck_angle_diff);

	ReduceTopBotRightSide(top, bot, g_chuckNotInspArea);
	
	if(bPrintout)
		PrintoutTopBot(top, bot, _T("���_������.bmp"), 128, 0, 255);

	// ���� ����
	isHeightOk = IsChucKTopBotHeight(
		top,
		bot,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_chuck_angle_height_diff,
		&minHeight,
		&maxHeight);

	m_ResultValue.chuck.isAngleOk = isAngleOk;
	m_ResultValue.chuck.isHeightOk = isHeightOk;
	m_ResultValue.chuck.minHeight = minHeight;
	m_ResultValue.chuck.maxHeight = maxHeight;

	return (isAngleOk && isHeightOk);
}

void CStrawProcess::ReduceTopBotRightSide(int *top, int *bot, const int pixels)
{
	const CvRect roi = m_roi;

	int topCnt = 0;
	int botCnt = 0;

	for(int x = roi.x + roi.width; x > roi.x; x--)
	{
		if(top[x] != -1)
		{
			top[x] = -1;
			topCnt++;
		}
		if(bot[x] != -1)
		{
			bot[x] = -1;
			botCnt++;
		}
		if(topCnt >= pixels && botCnt >= pixels)
			break;
	}
}

bool CStrawProcess::IsChucKTopBotHeight(int top[], int bot[], const int inputHeightDiff, int *outMinHeight, int *outMaxHeight)
{
	const CvRect roi = m_roi;

	int minHeight = 9999;
	int maxHeight = 0;

	for(int x = roi.x; x < roi.x + roi.width; x++)
	{
		if(top[x] != -1 && bot[x] != -1)
		{
			int height = bot[x] - top[x];

			if(height > maxHeight)
				maxHeight = height;
			if(height < minHeight)
				minHeight = height;
		}
	}

	*outMinHeight = minHeight;
	*outMaxHeight = maxHeight;

	if(maxHeight - minHeight >= inputHeightDiff)
		return false;
	else
		return true;
}

bool CStrawProcess::InspectInsertAngle(bool bPrintout)
{
	if(m_trans == TRUE)
	{
		m_ResultValue.insert.isAngleOk = TRUE;
		return true;
	}

	int top[FIXED_LENGTH];
	int bot[FIXED_LENGTH];

	GetTopBotPoints(m_ThredImageInsert, top, bot);
	if(bPrintout)
		PrintoutTopBot(top, bot, _T("����_��輱.bmp"), 255, 255, 0);

	CSeoilVisionDlg *pMainDlg = (CSeoilVisionDlg*)(AfxGetApp()->GetMainWnd());

	bool isAngleOk = IsTopBotAngleOk(
		top,
		bot,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_insert_angle_gap,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_insert_angle_continuous,
		pMainDlg->m_picture->m_pre->m_imageView.m_inspect_setting.m_insert_angle_diff);

	m_ResultValue.insert.isAngleOk = isAngleOk;

	return isAngleOk;
}