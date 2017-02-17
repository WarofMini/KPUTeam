#ifndef ToolCubeCol_h__
#define ToolCubeCol_h__

#include "VIBuffer.h"

class CCubeCol : public CVIBuffer 
{
public:
	CCubeCol();
	virtual ~CCubeCol();

	VTXCOL		m_pVtxCol[8];

private:
	virtual HRESULT CreateBuffer(void);
	virtual HRESULT CreateBuffer(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax, D3DXCOLOR& dwColor);

public:
	static CCubeCol* Create(void);

	static CCubeCol* Create(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax, D3DXCOLOR& dwColor);

public:
	virtual CResources* CloneResource(void);
	virtual void	CreateRasterizerState();
};

#endif