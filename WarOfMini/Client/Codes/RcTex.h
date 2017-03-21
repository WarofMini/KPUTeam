#ifndef RcTex_h__
#define RcTex_h__

#include "Resources.h"

class CRcTex
	: public CResource
{
private:
	explicit CRcTex(ID3D11Device* pGraphicDev, ID3D11DeviceContext*	pContext);
	virtual ~CRcTex(void);

public:
	static CRcTex* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

private:
	CRcTex* Clone_Resource(void);
	HRESULT Create_Buffer(void);

public:
	void Render(void);
	void Release(void);

private:
	ID3D11Buffer*	m_pVB;
	ID3D11Buffer*	m_pIB;
};

#endif // RcTex_h__