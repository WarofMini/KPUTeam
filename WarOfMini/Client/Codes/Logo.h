#ifndef Logo_h__
#define Logo_h__

#include "Scene.h"

class CLoading;

class CLogo : public CScene
{
private:
	explicit CLogo(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CLogo(void);

public:
	static CLogo* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Ready_Scene(void);
	virtual _int Update(const _float& fTimeDelta);


private:
	HRESULT Ready_GameLogic(void);
	HRESULT Ready_Environment(void);

private:
	CLoading*	m_pLoading;


public:
	virtual void Release(void);

};

#endif