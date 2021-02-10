#pragma once
#include "afxpropertygridctrl.h"

#define ATIPROP_LONG		1
#define ATIPROP_CSTRING		2
#define ATIPROP_SHORT		3
#define ATIPROP_FLOAT		4
#define ATIPROP_DOUBLE		5
#define ATIPROP_INT			6
#define ATIPROP_UINT		7
#define ATIPROP_BOOL		8

//////////////////////////////////////////////////////////////////////////
// Prop�� ���� ����ϱ� ���� Ŭ����		v 2.0	2011/04/13
// UpdatePropData ȣ���Ͽ� MFC ��Ʈ�Ѱ� �����ϰ� ��밡��
//
///////		��� ���		//////////////////////////////////////////////
// 1. ��Ʈ�� �߰�
// CATIProperty m_PropList;
// 2. �ʱ�ȭ
// m_PropList.InitProp(this, CRect(0,0,100,100), Index);			// ��µ� ������ �ڵ�, ������ȣ
// m_PropList.AddNewProp(L"Prop1");									// Prop �̸�
// m_PropList.AddSubItem(L"�׸�1", L"�׸�1�Դϴ�.", &m_lVel);			// Sub Pro �߰�
// m_PropList.AddSubItem(L"�׸�2", L"�׸�2�Դϴ�.", &m_strVel);
// m_PropList.AddPropDone();										// Add/Done���� �ִ� 10������ �߰�����.
//
// 3. �� ���
// m_PropList.UpdatePropData(bSave);								// true : m_lVel�� ���
//																	// false : m_lVel ���� Prop��
// [ 2011-3-2 ] : YS-Park

#include "ATIProperty_Data.h"

class AFX_EXT_CLASS CATIProperty :
	public CMFCPropertyGridCtrl
{
public:
	CATIProperty(void);
	~CATIProperty(void);

	BOOL InitProp(CWnd *pParentWnd, CRect rtProp, UINT nID);
	BOOL CreateProp(CString strPropName);
	BOOL CreateSubProp(CString strPropName);
	void SetEnable(bool bEnable);
	void SetEnable(int nPropNum, bool bEnable);

	BOOL AddSubItem(CString strSubPropName, CString strSubDescription, long *plVel, BOOL bEnable=true);
	BOOL AddSubItem(CString strSubPropName, CString strSubDescription, CString *pstrVel, BOOL bEnable=true);
	BOOL AddSubItemCombo(CString strSubPropName, CString strSubDescription, CString *pstrVel, int nCombo, CString *pstrList, BOOL bEnable=true);

	BOOL AddSubItemSelectFile(CString strSubPropName, CString strSubDescription, CString *pstrVel, BOOL bEnable=true);
	BOOL AddSubItemSelectFolder(CString strSubPropName, CString strSubDescription, CString *pstrVel, BOOL bEnable=true);
	BOOL AddSubItem(CString strSubPropName, CString strSubDescription, short *psVel, BOOL bEnable=true);
	BOOL AddSubItem(CString strSubPropName, CString strSubDescription, float *pfVel, BOOL bEnable=true);
	BOOL AddSubItem(CString strSubPropName, CString strSubDescription, double *pdVel, BOOL bEnable=true);
	BOOL AddSubItem(CString strSubPropName, CString strSubDescription, int *pnVel, BOOL bEnable=true);
	BOOL AddSubItem(CString strSubPropName, CString strSubDescription, unsigned int *punVel, BOOL bEnable=true);
	BOOL AddSubItem(CString strSubPropName, CString strSubDescription, bool *pbVel, BOOL bEnable=true);

	BOOL PropDone();
	BOOL SubPropDone();
	void UpdatePropData(bool bSave);
	void DeleteAllProp();

protected:
	void UpdateSubPropData(CMFCPropertyGridProperty *pProp, bool bSave);
	void UpdateData(int nIndex, CMFCPropertyGridProperty *pProp, bool bSave);
	void SetEnableSubPropData(CMFCPropertyGridProperty *pProp, bool bEnable);

	CMFCPropertyGridProperty *m_pTempProp[10];
	int m_nStep;
	UINT m_nItemIndex;

	CATIProperty_Data* m_pDataTemp;
	vector<CATIProperty_Data*> m_Data;
};
