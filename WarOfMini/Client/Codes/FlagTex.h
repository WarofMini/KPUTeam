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
		, const WORD& wCntX, const WORD& wCntZ, const WORD& wItv);

private:
	CFlagTex* Clone_Resource(void);
	HRESULT Create_Buffer(const WORD& wCntX, const WORD& wCntZ, const WORD& wItv);

public:
	void Render(void);
	void Release(void);

private:
	ID3D11Buffer*			m_pVB;
	ID3D11Buffer*			m_pIB;
	UINT					m_uiVtxCnt;
	UINT					m_uiIdxCnt;

private:
	VTXTEX*					m_pVertex;
};


#endif