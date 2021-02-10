#pragma once
#include <vector>
using namespace std;

class AFX_EXT_CLASS CVectorData_Float
{
public:
	CVectorData_Float(void);
	~CVectorData_Float(void);

	BOOL Save(CFile *fp);
	BOOL Read(CFile *fp);

	void SetData( CString sName );
	void ResetData();
	int GetDataNum();
	void AddData( float fData );
	float GetData( int nIndex );
	CString GetDataName();

protected :
	char m_pBuffer[256];
	CString m_sName;
	int m_nElementNum;
	vector<float>m_fData;
};
