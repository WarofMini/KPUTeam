#ifndef ToolRcTerrain_h__
#define ToolRcTerrain_h__

#include "VIBuffer.h"

class CRcTerrain :
	public CVIBuffer
{
private:
	CRcTerrain();
public:
	virtual ~CRcTerrain();

public:
	virtual HRESULT CreateBuffer(UINT iCountX, UINT iCountZ, UINT iInterval);
	static CRcTerrain* Create(UINT iCountX, UINT iCountZ, UINT iInterval);
	virtual CResources * CloneResource(void) override;
	DWORD*	LoadImage(void);

private:
	DWORD Release(void);

};

#endif