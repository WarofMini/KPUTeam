#ifndef ToolCubeCol_h__
#define ToolCubeCol_h__


#include "VIBuffer.h"
class CCubeCol : public CVIBuffer 
{
public:
	CCubeCol();
	virtual ~CCubeCol();
private:
	virtual HRESULT CreateBuffer(void);

public:
	static CCubeCol* Create(void);

public:
	virtual CResources* CloneResource(void);
};

#endif