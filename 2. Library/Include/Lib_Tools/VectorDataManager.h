#pragma once

#include "VectorData_Bool.h"
#include "VectorData_Int.h"
#include "VectorData_Float.h"

// 저장 공간이 부족하면 push하여 늘려줌.
// 종료시 해지 필요없음.
// 삽입 속도가 3~4배, 읽기가 2배 느려짐 (10,000,000개 삽입 70ms, 읽기 40ms)
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
