#ifndef Title_h__
#define Title_h__

#include "Scene.h"


class CTitle : public CScene
{
private:
	explicit CTitle(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics* pPxPhysicsSDK, PxScene* pPxScene, PxControllerManager*	pPxControllerManager, PxCooking* pCooking);
	virtual ~CTitle(void);

public:
	static CTitle* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics* pPxPhysicsSDK, PxScene* pPxScene, PxControllerManager*	pPxControllerManager, PxCooking* pCooking);

public:
	virtual HRESULT Ready_Scene(void);
	virtual _int Update(const _float& fTimeDelta);


private:
	HRESULT Ready_GameLogic(void);
	HRESULT Ready_Environment(void);


public:
	virtual void Release(void);

private:
	_bool*		m_bSceneCheck;

};

#endif