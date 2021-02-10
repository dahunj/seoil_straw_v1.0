#pragma once

#include "VectorData_Bool.h"
#include "VectorData_Int.h"
#include "VectorData_Float.h"

// ���� ������ �����ϸ� push�Ͽ� �÷���.
// ����� ���� �ʿ����.
// ���� �ӵ��� 3~4��, �бⰡ 2�� ������ (10,000,000�� ���� 70ms, �б� 40ms)
// [ 2012-2-3 ] : YS-Park

enum VectorDataType{
	VectorDataType_bool,
	VectorDataType_int,
	VectorDataType_float,
};

class AFX_EXT_CLASS CVectorDataManager
{
public:
	CVectorDataManager(void);
	~CVectorDataManager(void);

	BOOL Save(CString strName);
	BOOL Read(CString strName);

	void ResetAll();
	int CreateData(CString sName, VectorDataType type);
	void ResetData(CString sName);
	int GetDataNum(CString sName);

	void AddData(int nDataIndex, int nData);
	void AddData(int nDataIndex, bool bData);
	void AddData(int nDataIndex, float fData);

	int GetData_int(int nDataIndex, int nItemIndex);
	bool GetData_bool(int nDataIndex, int nItemIndex);
	float GetData_float(int nDataIndex, int nItemIndex);

	int FindDataIndex(CString sName);
	int FindDataIndex(CString sName, VectorDataType*pType);
	
protected :
	vector<CVectorData_Bool>m_bData;
	vector<CVectorData_Int>m_nData;
	vector<CVectorData_Float>m_fData;
};
