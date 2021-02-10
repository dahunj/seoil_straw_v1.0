// StrawProcess.h: interface for the CStrawProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRAWPROCESS_H__B974BA69_E7A7_4EBE_8728_5856C4E6731A__INCLUDED_)
#define AFX_STRAWPROCESS_H__B974BA69_E7A7_4EBE_8728_5856C4E6731A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cxcore.h"
#include "cv.h"

//#include "ImageProcess.h"
//--------------------------------------------------------------
// Model Number
//#define MACHINE_16
//#define MACHINE_6
#define MACHINE_6_TRI
//--------------------------------------------------------------
// 2008 11 05 ������ ī�޶� ��ȣ Define -> 0 or 1
#define _CAM_NUM 0
//--------------------------------------------------------------
//#define _PC		0	// �ݴ��� �� PC 0��. 11ȣ��. �������� PC 1��, 11ȣ��.
//--------------------------------------------------------------
//	Machine�� Ư����.
#ifdef MACHINE_16
	// ���� ���� 16ȣ�� 2009.1. BHJ �ø��� �ѹ�.
	#define _SERIAL_NUM_L_SIDE 8510060		// Left 1
	#define _SERIAL_NUM_L_TOP  8510060		// Left 2
	#define _SERIAL_NUM_R_SIDE 8510060		// Right 1
	#define _SERIAL_NUM_R_TOP  8510060		// Right 2	
	
	// 16ȣ�� Settig
	#define _CamWidth	1024	// �⺻ ũ��� ����.
	#define _CamHeight	768		// �⺻ ũ��� ����.
	#define _SOL1	0
	#define _SOL2	0

	#define _OFFX6  20			// ���Ժ� Width.
	#define MODEL_16	1
	#define MODEL_6		0
#endif

#ifdef MACHINE_6
	// 2008 11 04 ������ �ø��� �ѹ� ������		// ���� ���� 5ȣ�� 2008.12. BHJ
	#define _SERIAL_NUM_R_SIDE	8510060 // Right Side	0 (side)	0 (CamNum)
	#define _SERIAL_NUM_R_TOP	8510060 // Right Top	1 (top)		1 (CamNum)
	#define _SERIAL_NUM_L_SIDE	8510060 // Left Side	0 (side)	2 (CamNum)
	#define _SERIAL_NUM_L_TOP	8510060 // Left	Top		1 (top)		3 (CamNum)
	
	#define _CamWidth	1280	// �⺻ ũ��� ����.
	#define _CamHeight	960		// �⺻ ũ��� ����.
	#define _SOL1	4
	#define _SOL2	5

	#define _OFFX6  90			// ���Ժ� Width.

	#define MODEL_16	0
	#define MODEL_6		1
#endif

#ifdef MACHINE_6_TRI
	// 2009 0331 -> PC 1���� ���. _SERIAL_NUM_R_SIDE ����.
	// 2008 11 04 ������ �ø��� �ѹ� ������		// ���� ���� 5ȣ�� 2008.12. BHJ
	#define _SERIAL_NUM_10	111  // 1ȣ��
	#define _SERIAL_NUM_15	111  // 1ȣ��--------------------
	#define _SERIAL_NUM_20	222  // 2ȣ��
	#define _SERIAL_NUM_25	222  // 2ȣ��--------------------
	#define _SERIAL_NUM_30	333  // 3ȣ��
	#define _SERIAL_NUM_35	333  // 3ȣ��--------------------
	#define _SERIAL_NUM_40	444  // 4ȣ��	
	#define _SERIAL_NUM_45	444  // 4ȣ��--------------------
	#define _SERIAL_NUM_50	9201513  // 5ȣ��
	#define _SERIAL_NUM_55	9201508  // 5ȣ��--------------------
	#define _SERIAL_NUM_60	9160375  // 6ȣ��
	#define _SERIAL_NUM_65	9051255  // 6ȣ��--------------------
	#define _SERIAL_NUM_70	777  // 7ȣ��
	#define _SERIAL_NUM_75	777  // 7ȣ��--------------------
	#define _SERIAL_NUM_80	888  // 8ȣ��
	#define _SERIAL_NUM_85	888  // 8ȣ��--------------------
	#define _SERIAL_NUM_90	999  // 9ȣ��
	#define _SERIAL_NUM_95	999  // 9ȣ��--------------------
	#define _SERIAL_NUM_100	9160382  // 10ȣ��
	#define _SERIAL_NUM_105	9160329  // 10ȣ��--------------------
	#define _SERIAL_NUM_110	8510068  // 11ȣ��
	#define _SERIAL_NUM_115	8510060  // 11ȣ��--------------------
	#define _SERIAL_NUM_120	9160383  // 12ȣ�� ����.
	#define _SERIAL_NUM_125	9160321  // 12ȣ�� �ݴ�.--------------------
	#define _SERIAL_NUM_130	9051253  // 13ȣ��
	#define _SERIAL_NUM_135	8510066  // 13ȣ��--------------------
	#define _SERIAL_NUM_140	9160316  // 14ȣ��
	#define _SERIAL_NUM_145	9160386  // 14ȣ��--------------------
	#define _SERIAL_NUM_150	111  // 15ȣ��
	#define _SERIAL_NUM_155	111  // 15ȣ��--------------------
	//#define _SERIAL_NUM_160	9160375  // 16ȣ�� ���� ī�޶� ��ü.
	#define _SERIAL_NUM_160	9160267  // 16ȣ�� ����.
	#define _SERIAL_NUM_165	9160380  // 16ȣ�� �ݴ�--------------------
	#define _SERIAL_NUM_170	111  // 17ȣ��
	#define _SERIAL_NUM_175	111  // 17ȣ��--------------------
	#define _SERIAL_NUM_180	111  // 18ȣ��
	#define _SERIAL_NUM_185	111  // 18ȣ��--------------------
	#define _SERIAL_NUM_190	111  // 19ȣ��
	#define _SERIAL_NUM_195	111  // 19ȣ��--------------------
	#define _SERIAL_NUM_200	111  // 20ȣ��
	#define _SERIAL_NUM_205	111  // 20ȣ��--------------------
	#define _SERIAL_NUM_210	111  // 21ȣ��
	#define _SERIAL_NUM_215	111  // 21ȣ��--------------------
	#define _SERIAL_NUM_220	111  // 22ȣ��
	#define _SERIAL_NUM_225	111  // 22ȣ��--------------------
	#define _SERIAL_NUM_230	111  // 23ȣ��
	#define _SERIAL_NUM_235	111  // 23ȣ��--------------------
	#define _SERIAL_NUM_240	111  // 24ȣ��
	#define _SERIAL_NUM_245	111  // 24ȣ��--------------------
	#define _SERIAL_NUM_250	111  // 25ȣ��
	#define _SERIAL_NUM_255	111  // 25ȣ��--------------------
	#define _SERIAL_NUM_260	111  // 26ȣ��
	#define _SERIAL_NUM_265	9160377  // 26ȣ��--------------------

	// 3���� 2010. 04. 09. ������
#define _SERIAL_NUM_300	9240662	//3-1
#define _SERIAL_NUM_305	9240678	//3-2
#define _SERIAL_NUM_310	9270006	//3-3
#define _SERIAL_NUM_315	9270007	//3-4
#define _SERIAL_NUM_320	9270009	//3-5
#define _SERIAL_NUM_325	9270013	//3-6


// cwlee add
#define _SERIAL_NUM_400	8401200


//	#define _SERIAL_NUM_60	8510068  // Right Side	0 (side)	0 (CamNum)	
	// 2009 0331 -> PC 0���� ���. _SERIAL_NUM_L_SIDE ����.
//	#define _SERIAL_NUM_65	8510060 // Left Side	0 (side)	2 (CamNum)	
#define _MAX_FRAME_RATE_COUNTS 128
#define _MAX_CAMERAS 1
#define _IMAGE_BUFFER_SIZE 40
#define _STATUS_BUFFER_SIZE	200
#define _REJECT_BUFFER_SIZE 500
	

	#define _SOL1	4
	#define _SOL2	5

	#define _OFFX6  90			// ���Ժ� Width.

	#define MODEL_16	0
	#define MODEL_6		1
#endif
//--------------------------------------------------------------
// ���� Define �κ�.


// �̹� ���� ������ ���� Offset
#define _OFFX1	3			// ������ ���ܿ��� ���� offset
#define _OFFX2	10			// �ڹٶ� ���ܿ��� ���� offset
#define _OFFX3	60			// ������� ����� ���ϴ� Offset
#define _OFFX4	20			// ���Ժ� X Offset
#define _OFFX5	25			// ����� minus X Offset

#define _OFFY1	5			// ������, �ڹٶ� ��� Y Offset
#define _OFFY2	15			// ���Ժ� Y Offset
#define _OFFY3	27			// ���Ժ� Y Width

#define _OFFXETQ1	50		// ��Ƽ�� ����. (�ڹٶ󿡼� ��Ƽ�� ���۱���)
#define _OFFXETQ2	50		// ��Ƽ�� ����. (��Ƽ�� ������ �̹� ���� ������)
#define _OFFXETQLENGTH	50	// ��Ƽ�� ���� ���� ũ��.
#define _ETQLENGTH		260	// ��Ƽ�� ���� ����.

// �̹� ���� ����.
#define _IMULGV			240			// �̹��� �̰ͺ��� �۴ٰ� ����.
#define _IMULGV_CHUCK	240		// �̹��� �̰ͺ��� �۴ٰ� ����.
//--------------------------------------------------------------


//#define _ShowWidth	1024
//#define _ShowHeight	168

//#define _CamWidth	1280
//#define _CamHeight	256

//#define _MAXROI	5

#define _TEST_DELAY	20

enum ErrorType
{
	SUCCESS = 1,
	TYPE1 = -1,   // �ȼ��� ���ġ ����
	TYPE2 = -2,   // ��迡 ���� or �߰��� �����
	TYPE3 = -3    // �߰��� ���� ���� or Edge�� �� �ٲ�
};

typedef struct _maguriResult{
	int inputMinLength;
	int inputMaxLength;
	int botLen;
	int topLen;
	BOOL isSideOk;
	BOOL isAngleOk;
} MaguriResult;

typedef struct _midResult{
	int minHeight;
	int maxHeight;
	BOOL isAngleOk;
	BOOL isHeightOk;
} MidResult;

typedef struct _topResult{
	int minHeight;
	int maxHeight;
	BOOL isAngleOk;
	BOOL isHeightOk;
} TopResult;

typedef struct _chuckResult{
	int minHeight;
	int maxHeight;
	BOOL isAngleOk;
	BOOL isHeightOk;
} ChuckResult;

typedef struct _insertResult
{
	BOOL isAngleOk;
} InsertResult;

struct RESULT_INSPVALUE
{
	MaguriResult maguri;
	MidResult mid;
	TopResult top;
	ChuckResult chuck;
	InsertResult insert;

	int Rmaguri[4];
	double Rchuck[3];
	double Rfchuck[2];
	double Rinsult[6];
	double Rangle[5];
	double Rjabara[4];
	int Rimul[7][2];
	double Rpok[4];
	int REtiquet[4];
	double RfEtiquet[2];

	int jabara_num;
	int jabara_neighbor;
	double jabara_width_stdev;
	double jabara_distance_stdev;
	double jabara_gradient_stdev;

	int black_num;
};


// �˻簪 ���� ����ü
struct INSPECT_SETTING
{
	// ���� �˻�� -> ��絵, ��絵 ���� �ּ� ����
	double m_Slide; int m_SlideLength;
	// ������ �˻�� -> ��絵, ��絵 ���� �ּ� ����
	double m_Magunus_Slide;
	double m_Magunus_Slide2;
	int m_Magunus_SlideLength;
	int m_maguri_max_length;

	// ������ ���� ����;

	// ĳ�� ���� ������Ȧ��, ����
	int m_CannyLow, m_CannyHigh, m_CannyApp;
	// ��� ������
	double m_minChuck;
	double m_maxChuck;

	// �ػ� �� ī�޶� ����
	double  resol;
	int     camWidth;
	int		camHeight;
	int		triggerPerPitch;
	int		inspPitch;

	// �÷��˻�
	double colorlimit, brightlimit;
	int masksize;

	// �̹�
	int pixsize;
	int mid_pixsize;
	int jabara_pixsize;
	int chuck_pixsize;
	int insert_pixsize;
	int imul_etq_left_pix;
	int imul_etq_right_pix;

	int imul_top_threshold;
	int imul_mid_threshold;
	int imul_jabara_threshold;
	int imul_chuck_threshold;
	int imul_insert_threshold;
	int imul_etq_left_threshold;
	int imul_etq_right_threshold;

	int imul_top_mask;
	int imul_top_mask_cols;
	int imul_mid_mask;
	int imul_jabara_mask;
	int imul_chuck_mask;
	int imul_chuck_mask_cols;
	int imul_insert_mask;
	int imul_insert_pos;
	int imul_etq_left_mask;
	int imul_etq_right_mask;

	// �ڹٶ� ����
	double	jabara_recognize;
	int		jabara_num;
	int		jabara_margin_of_error;
	double	jabara_width_stdev;
	double	jabara_distance_stdev;
	double	jabara_gradient_stdev;
	int		jabara_neighbor;

	// ��Ƽ�� ����
	double etiquette_recognize;
	double etiquette_position;
	int	   etiquette_size;

	// ���� ����
	double m_length_limit;

	// �� ����
	double m_pok_limit;
	double m_pok;

	// ���� ����
	double m_angle_limit;

	// ���� ���˻�
	double angle_limit[2];
	// �¿� ����
	double angle_diff;



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

	double imul_top_persent;
	double imul_mid_persent;

	int imul_top_size;
	int imul_mid_size;

	int imul_mid_notinspect;

	int stabbed_insert_notinspect;
	int stabbed_insert_pix;
};



typedef struct _ObjSpec
{
	int label;
	int pixels;
	int distance;
	CvRect rect;
}ObjectSpec;

typedef struct _Edge
{
	int up;
	int bottom;
}Edge;

class CStrawProcess  
{
public:
	CStrawProcess();
	virtual ~CStrawProcess();

	// ������Ȧ���� �� ��� ���� �뵵�� Ŭ���� 2008 06 17 ������
public:
	RESULT_INSPVALUE m_ResultValue;
	int m_lowlimit, m_highlimit, m_avgdiffgot;
	double m_average, m_diff;
	float m_fCamGain;
	float m_fCamShutter;
	float m_fChuckSlideTop;
	float m_fChuckSlideBottom;
//	CImageProcess *m_ImageProcess;

	// �̹��� ���� ������ 2008 06 17 ������
public:

	void ClearInspResult();

	bool CheckInsert(CvRect area, int step, bool bvirtual, double angle, int length, int notinspect, int pix);
	bool CheckInsertStabbed(CvRect area, int step, bool bvirtual, double angle, int length, int notinspect, int pix);
	bool SetColorImage(IplImage *image);
	bool SetMonoImage(IplImage *image);
	bool GetImageAvgDiff();
	bool SetImage(IplImage *image);

	CvRect this_area;
	// ��ǰ ���� ���� ������ 2008 06 17 ������
public:
	// ���� �˻�� -> ��絵, ��絵 ���� �ּ� ����
	double m_Slide; int m_SlideLength;
	// ������ �˻�� -> ��絵, ��絵 ���� �ּ� ����
	double m_Magunus_Slide;
	double m_Magunus_Slide2;
	int m_Magunus_SlideLength;
	// ����Ʈ �����۰˻�� -> �ּ� �󸶳� �п��� �ϳ�
	int m_SideStopperLimit;
	// ĳ�� ���� ������Ȧ��, ����
	int m_CannyLow, m_CannyHigh, m_CannyApp;
	// ��� ������
	double m_minChuck;
	double m_maxChuck;
	// �뷮 ���� ���� �̹��� ������ �� ����
public :
	bool CalculateJabaraDiff(CvRect area, int masksize);

	double CalculateSlide(CvRect area, double limit, int nLRSelect, bool bvirtual);
	bool CheckChuck(CvRect area, double resol, bool bvirtual);
	bool CheckStopperUp(CvRect area);
	bool CheckStopperSide(CvRect area);
	bool ChechMagunus(CvRect area, bool bvirtual);
	void SetImageSize(int width, int height);
	int m_SettedImageParam;
	int m_ImageWidth;
	int m_ImageHeight;

	// ������ ����
// 	int m_colorstraw,	m_trans;

	// �ӵ� ����� ���� �̹��� ROI�� �׻� ����� ��
	IplImage *m_ROI_imul;//ADD cwlee
	IplImage *m_bin_imul[3]; //ADD cwlee

	// ���� �ڹٶ� �� ���� ����ü
	struct jabara{
		double up[1600];
		double down[1600];

		// 2008 08 04 ���� ��տ�
		double avg_up;
		double avg_down;

		double slide_up;
		double slide_down;

		double up_slide;
		double down_slide;

		int spoint;

	};

	struct jabara m_jabara;

	// ���� ������ ����ü
	struct length{
		double longs;
		int left_top;
		int left_bottom;
		int right_top;
		int right_bottom;
		int OK;
	public:
	};

	struct length m_length;

public:
	int m_UseCam14mode;
	int m_SelectCamMode;
	int m_transMaguri;
	float m_fPokDiff;
	int m_tepe;
	BOOL m_bUseInspectEtq;
	bool SetColorImageByCOI2by1(IplImage *image, int thres);
	bool CheckColor4(CvRect area, double colorlimit, double brightlimit, int pix, int type, bool test);
	bool CheckColor3(CvRect area, double colorlimit, double brightlimit, int pix);
	int GVData(int x, int y, int nRGB);
	bool CheckTwoGak(double angle1, double angle2, double limit);
	bool CheckPokArea(double pok, double pok_mobum, double limit);
	bool StrawExistanceCheck(CvRect area, int type);
	int StrawExistanceCheckForTrans(CvRect partialArea, CvRect totalArea, bool bPrint);
	int  StrawExistanceCheckForTrans_Total(CvRect area, int type);
	bool CheckColor2(CvRect area, double colorlimit, double brightlimit, int pix);
	
	
	
	bool SetColorROIImage(IplImage *image, CvRect area);
	bool BlobError(int width, int height, int pix);
	bool CheckEtiq(CvRect area, double length, double limit);
	double InspectLength(double resol, double limit);
	double InspectPok(CvRect area, double resol, double limit);
	// �÷�����
	bool m_colorsetted;
	bool s_inspect_temp;

	// 2008 08 18 ��� �� �̸� �����
	
	IplImage *m_grayImage;

	bool CalculateColorDiff(CvRect area,double colorlimit, double brightlimit, int masksize);
	bool CheckJabara(CvRect area, double height, double height_limit, double length, double length_limit);
	bool CheckColor(CvRect area, double colorlimit, double brightlimit, int masksize, int pix);

	// ����� ��� ����
	int m_chuck_limit;
	// ��Ƽ�� ���� ������.
	int GV_limit;				// GV Difference
	int GV_line_limit;			// min line
	int GV_maxline_limit;		// max line
	int dev_limit;				// ���� 
	int m_EtqOff[4];			// Etiquette offset.
	// Offset
	int m_OffX[6];				// OffX1, OffX2, OffX3, OffX4, OffX5
	int m_OffY[3];				// OffY1, OffY2, OffY3
	// ���� ���Ժο� ���� Threshold value.
	int m_nThresTransInsert;
	int m_nThresInsert;
	//
	int m_nStrawExistOffset;
	// Ư�� Threshold ���� Threshold �� % ��
	int m_nSPThres[5];
	// ������ ����
	int m_colorstraw;
	int m_trans;
	int m_colorthres[3];

	////////////////////////////////////////////////////
	// �̼����� ���� �Ķ���� added by walca 0615
	int m_threshold;
	int m_Debug; // ����� ���� 1�̸� â�� ������
	int m_CirCount;
	int morphology_kernel_size;
	int distance_original_to_approx;

	// �̹� �˻� ����.(Te : �ڹٶ� ���� ���)
	int TopBot[2][2];	// TopBot[Left or Right] [Top or Bottom]

	// ��� data.
	int m_nResultData[20];
	double m_dResultData[20];

public://///////////////////////////////////////////// ADD cwlee (global to local)

	IplImage *m_RGBImage;
	IplImage *Image_Temp;
	IplImage *m_YCrCbImage;
	IplImage *m_resultmap;
	// ���� �÷� �̹���
	//IplImage *m_ColorImage;
	// ���� �̹���
	IplImage *m_OriginalImage;
	// ������Ȧ���� �̹���
	IplImage *m_ThredImage;
	IplImage *m_ThredImageCopy;
	IplImage *m_ThredImageMaguri;
	IplImage *m_ThredImageChuck;
	IplImage *m_ThredImageInsert;

	IplImage* m_binaryImage; // �浹 ������ ���� ����
	IplImage* m_binaryImage_band; // �浹 ������ ���� ����

	// ������ �̹���
	IplImage *m_EdgedImage;

	int m_nMaxBlob;

	int GetMaxBlob();
	void SetMaxBlob(int blob);

public:
	//--------------------------------------------------------------
	// Algorithm Test Start
	//--------------------------------------------------------------

	//FILE *m_fpError;
	void ErrorFileOpen();
	void ErrorFileClose();

	// -----------------------------------------------------
	// cwlee Jabara Test (Fin.)
	// -----------------------------------------------------
	bool InspectJabara(const CvRect roi, const int numOfObj, const int marginOfErr, const int minSize, const double ratio, const double cDistStdev, const double cWidthStdev, const bool bPrintout); // ok
	
	bool InspectJabaraEdge(const CvRect roi, const int numOfObj, const int marginOfErr, const double cGradStdev, const int cDiff, const bool bPrintout); // ok
	
	void Labeling(const CvRect roi, int label[], bool isEdge, const bool bPrintout); // ok
	
	void GetMeanWeight(int hist[], const int numOfPixel, const int start, const int end, double& mean, double& weight);// ok
	void RatioHistogram(const CvRect roi, const double r, const bool bPrintout); // ok
	void GetObjectSpec(const CvRect roi, int label[], CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap, const int minSize, const bool bPrintout); // ok
	void GetDistanceBetweenObject(CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap); // ok
	void GetRidges(const CvRect roi, const int numOfObj, int upRidges[], int downRidges[]); // ok
	void GetRidges2(const CvRect roi, const int numOfObj, int upRidges[], int downRidges[]); // ok
	bool JabaraErrorCheck(CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap, const int numOfObj, const int marginOfErr, const double cDistStdev, const double cWidthStdev, const bool bPrintout); // ok
	bool JabaraEdgeErrorCheck(int upRidges[], int downRidges[], const int numOfObj, const int marginOfErr, const double cGradStdev, const int cDiff, const bool bPrintout); // ok
	void GroupObject(CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap, int label[], const CvRect roi, const bool bPrintout); // ok
	
	void WriteToFile(const CvRect roi, int label[], const char* fileName); // ok
	void WriteToImage(const CvRect roi, int label[], const char* fileName); // ok
	
	bool OtsuThreshold(const CvRect roi, IplImage *img, const bool bPrintout); // ok
	void GaussianFilter(const CvRect roi, double sigma); // ok

	// -----------------------------------------------------
	// cwlee Black Point Test (cont.)
	// -----------------------------------------------------
	//bool InspectBlackPoint(const CvRect roi, const int elemSize, const int threshold, const int minSize, const bool bPrintout, CString part, int partNum); // ok
	bool InspectBlackPoint(int now_count, const CvRect roi, int cols, int rows, const int threshold, const int minSize, const bool bPrintout, CString part, int partNum, const CvRect roiTemp, const double persent, int minSizeband,int notinspect); // ok
	bool InspectBlackBand(int now_count, const CvRect roi, int cols, int rows, const int threshold, const int minSize, const bool bPrintout, CString part, int partNum, const CvRect roiTemp, const double persent, int notinspect); // ok
	void BasicThreshold(const CvRect roi, const int threshold, const bool bPrintout); // ok
	void ReInspectBlackPoint(
		const CvRect tmpOrigRoi,
		CPoint rect[],  
		int minSize, 
		int threshold, 
		int mask,        
		int label[],	  
		CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap); // ���� ���
	
	BOOL IsIncluded(const CvRect *orig, ObjectSpec *obj, const CvRect *rect);

	//void InspectEtqRightBlackPoint(const cvRect roi, int label[], CMap<int, int, ObjectSpec, ObjectSpec>& objSpecMap);

	void EqualizeHistogram(const CvRect roi); // ok

	void GetStrawEdge(const CvRect roi); // ok

	Edge *m_StrawEdge;

	// -----------------------------------------------------
	// cwlee Etiquette Test (cont.)
	// -----------------------------------------------------
	bool InspectEtiquette(const CvRect roi, const int minSize, const double ratio, const double position, const bool bPrintout); // ok
	void MeanFilter(const CvRect roi, const int width, const int height); // ok
	bool EtiquetteErrorCheck(CMap<int, int, ObjectSpec, ObjectSpec> &objSpecMap, const double ratio, const CvRect roi, const bool bPrintout);

	// -----------------------------------------------------
	// cwlee Insert Test (cont.)
	// -----------------------------------------------------
	bool InspectInsert(const CvRect roi, const int threshold, const double stdev);
	

	// -----------------------------------------------------
	// cwlee Angle Test (cont.) - 2016.03.18
	// -----------------------------------------------------
	bool InspectTopAngle(bool bPrintout);
	bool InspectMidAngle(bool bPrintout);
	bool InspectMaguri(bool bPrintout);
	bool InspectMaguriSide(bool bPrintout);
	bool InspectMaguriAngle(bool bPrintout);
	bool InspectChuckAngle(bool bPrintout);
	bool InspectInsertAngle(bool bPrintout);

	bool IsTopBotAngleOk(int *top, int *bot, const int next, const int cont, const int inputDiff);
	void IsTopBotAngleHeightOk(int *top, int *bot, const int next, const int cont, const int inputDiff, const int inputHeightDiff, bool *isAngleOk, bool *isHeightOk, int *outMinHeight, int *outMaxHeight);
	bool IsChucKTopBotHeight(int top[], int bot[], int inputHeightDiff, int *minHeight, int *maxHeight);

	void GetTopBotPoints(IplImage *iplImg, int *top, int *bot);
	void GetMaguriSidePoints(int *side);
	void GetMaguriTopBotLength(int top[], int bot[], int *topLen, int *botLen);
	int  GetMaguriTopStartPoint(int side[]);
	int  GetMaguriBotStartPoint(int side[]);

	void PrintoutBinaryImage(IplImage *img, CString fileName);
	void PrintoutTopBot(int *top, int *bot, CString fileName, unsigned char r, unsigned char g, unsigned char b);
	void PrintoutMaguriSide(int *side);
	//--------------------------------------------------------------
	// Algorithm Test End
	//--------------------------------------------------------------
	CvRect m_roi;
	void SetROI(int x, int y, int width, int height);
	CvRect GetROI();
	void SetInspResultMaguri(int inputMinLen, int inputMaxLen, double ratio, int topLen, int botLen);
	void ReduceTopBotRightSide(int *top, int *bot, const int pixels);
};

// �˻� ��� ����ü
struct RESULT_INSPECT
{
	bool maguri;
	bool chuck;
	double angle[2];
	
	bool imul;
	bool imul_mid;
	bool imul_jabara;
	bool imul_chuck;
	bool imul_insert;
	
	bool insert;
	bool jabara;
	double pok;
	bool etiquette;
};
// �Ķ���� ������ ����ü
struct INSP_PARAM
{
	int nData[30];
	double fData[10];
};

/**********************************************************


static	IplImage *m_RGBImage;
static	IplImage *m_YCrCbImage;
static	IplImage *m_resultmap;
	// ���� �÷� �̹���
static	IplImage *m_ColorImage;
	// ���� �̹���
static	IplImage *m_OriginalImage;
	// ������Ȧ���� �̹���
static	IplImage *m_ThredImage;
static	IplImage *m_ThredImageCopy;
static	IplImage *m_ThredImageMaguri;
static	IplImage *m_ThredImageChuck;
static	IplImage *m_ThredImageAngle;

	// ������ �̹���
static	IplImage *m_EdgedImage;

static int m_nMaxBlob;

******************************************************/


// ROI �̹���

//static IplImage *m_ROI_imul; // DEL cwlee
//static IplImage *m_bin_imul[3]; // DEL cwlee


#endif // !defined(AFX_STRAWPROCESS_H__B974BA69_E7A7_4EBE_8728_5856C4E6731A__INCLUDED_)
