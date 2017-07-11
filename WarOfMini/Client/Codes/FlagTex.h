#ifndef FlagTex_h__
#define FlagTex_h__

#include "Resources.h"

class CFlagTex
	: public CResource
{
private:
	explicit CFlagTex(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CFlagTex(void);

public:
	static CFlagTex* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
		, const WORD& wCntX, const WORD& wCntZ, const WORD& wItvX, const WORD& wItvZ);

private:
	CFlagTex* Clone_Resource(void);
	HRESULT Create_Buffer(const WORD& wCntX, const WORD& wCntZ, const WORD& wItvX, const WORD& wItvZ);

public:
	void Render(void);
	void Release(void);

private:
	ID3D11Buffer*			m_pVB;
	ID3D11Buffer*			m_pIB;
	_uint					m_uiVtxCnt;
	_uint					m_uiIdxCnt;
	_float					m_fResX;
	_float					m_fResZ;
	_float					m_fSizeX;
	_float					m_fSizeZ;
	
private:
	VTXTEX*					m_pVertex;

public:
	VTXTEX*					GetVertex(void) { return m_pVertex; }
	void					GetVtxInfo(void* pVtxInfo);
	void					SetVtxInfo(void* pVtxInfo);
	_float					GetResX(void) { return m_fResX; }
	_float					GetResZ(void) { return m_fResZ; }
	_float					GetSizeX(void) { return m_fSizeX; }
	_float					GetSizeZ(void) { return m_fSizeZ; }
};


#endif