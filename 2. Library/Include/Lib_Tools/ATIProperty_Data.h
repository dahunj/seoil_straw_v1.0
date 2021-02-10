#pragma once

class CATIProperty_Data
{
public:
	CATIProperty_Data(void *pData, int nDataType);
	~CATIProperty_Data(void);
	void SetData(void *pData, int nDataType);

	void *m_pData;
	int m_nDataType;
};
