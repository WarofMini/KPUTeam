#ifndef TrailTex_h__
#define TrailTex_h__

#include "Resources.h"

class CTrailTex
	: public CResource
{
private:
	explicit CTrailTex(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CTrailTex(void);

public:
	static CTrailTex* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
							, const WORD& wCntX, const WORD& wCntZ, const WORD& wItv);


private:
	CTrailTex* Clone_Resource(void);
	HRESULT Create_Buffer(const WORD& wCntX, const WORD& wCntZ, const WORD& wItv);

public:
	void Render(void);
	void Release(void);

private:
	ID3D11Buffer*			m_pVB;
	ID3D11Buffer*			m_pIB;
	UINT					m_uiVtxCnt;
	UINT					m_uiIdxCnt;
	_float					m_fSizeX;
	_float					m_fSizeZ;

private:
	VTXTEX*					m_pVertex;

public:
	void					GetVtxInfo(void* pVtxInfo);
	void					SetVtxInfo(void* pVtxInfo);
	_float					GetSizeX(void) { return m_fSizeX; }
	_float					GetSizeZ(void) { return m_fSizeZ; }
};

#endif