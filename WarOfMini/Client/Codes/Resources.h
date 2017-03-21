#ifndef ClientResourcess_h__
#define ClientResourcess_h__

#include "Component.h"


class CResource :
	public CComponent
{
protected:
	explicit CResource(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CResource(void);

public:
	virtual CResource* Clone_Resource(void)  PURE;

	virtual void Release(void);



protected:
	_ulong*					m_dwRefCount;
	ID3D11Device*			m_pGraphicDev;
	ID3D11DeviceContext*	m_pContext;

};

#endif