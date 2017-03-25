#ifndef Scene_h__
#define Scene_h__

#include "Layer.h"

class CScene
{
protected:
	explicit CScene(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CScene(void);

public:
	virtual HRESULT Ready_Scene(void)	PURE;

public:
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);

public:
	virtual void Release(void);

protected:
	ID3D11Device*			m_pGraphicDev;
	ID3D11DeviceContext*	m_pContext;

protected:
	typedef map<const _tchar*, CLayer*>   MAPLAYER;
	MAPLAYER							m_mapLayer;

public:
	CLayer*		FindLayer(const _tchar* LayerName);
	//MAPLAYER*	GetMapLayer(void) { return &m_mapLayer; }
};

#endif // Scene_h__