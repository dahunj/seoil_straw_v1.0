#pragma once

class AFX_EXT_CLASS CGeneralLib
{
public:
	CGeneralLib(void);
	~CGeneralLib(void);

	static CString FindExtension(const CString& name);
	static CString FindFileName(const CString& name);
	static CString FindFilePath(const CString& name);
	static HWND StartProcess(CString program, CString m_strClassName, HWND pParent, int nWaitMinute);
	static LRESULT SendDataMessage(HWND parm_wnd, HWND parm_this_wnd, int parm_message_type, char *parm_data, int parm_size);
	static bool WriteFile( CFile *pFile, int nItemCode, int nSize, void *pdata);
	static bool WriteFile(CFile *pFile, int nItemCode, int nSizeW, int nSizeH, void **pdata);
	static int ReadItemCode(CFile *pFile);
	static bool ReadFile(CFile *pFile, void *pdata);
	static bool ReadFile(CFile *pFile, int nSizeW, void **pdata);
	static bool TrushFile(CFile *pFile);
	static CString ReadStringFile(CFile *pFile, char *pBuffer);
	static void ClockDelay(int msec);
	static CString TimeToString(CTime time);
	static CRect SortRect(CRect rt);
	static bool InFence(CPoint pt, CPoint* pptPLine, int nPLineNum);
	static bool InFence(int x, int y, CPoint* pptPLine, int nPLineNum);
	static CPoint CutFence(CRect rt, CPoint pt);
	static void swap(int a, int b);
	static void SaveRawImage(CString strName, LPBYTE *ppImage, CRect ROI);
	static void SaveRawImage(CString strName, LPBYTE pImage, int nSize);
	static int PopData(CString str, long *nData, CString *strName);
	static CString PushData(CString str, UINT nData);
	static CString PushData(CString str, long nData);
	static bool IsOverLap(CRect rt1, CRect rt2);
	static CRect MergeRect(CRect rt1, CRect rt2);
	static CRect CutOffRect(CRect rtRef, CRect rt);
	static bool WriteFile( CFile *pFile, int nItemCode, CString str);
	static bool ReadFile(CFile *pFile, CString *pstr);
	static int RoundingUp(float f);

};
