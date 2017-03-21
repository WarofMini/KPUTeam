#ifndef CubeTex_h__
#define CubeTex_h__

#include "Resources.h"

class CCubeTex
	: public CResource
{
private:
	explicit CCubeTex(ID3D11Device* pGraphicDev, ID3D11DeviceContext*	pContext);
	virtual ~CCubeTex(void);

public:
	static CCubeTex* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

private:
	CCubeTex* Clone_Resource(void);
	HRESULT Create_Buffer(void);

public:
	void Render(void);
	void Release(void);

private:
	ID3D11Buffer*	m_pVB;
	ID3D11Buffer*	m_pIB;
};

#endif
