#ifndef StageScene_h__
#define StageScene_h__

#include "Scene.h"

class CStage
	: public CScene
{
private:
	explicit CStage(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics* pPxPhysicsSDK, PxScene* pPxScene, PxControllerManager*	pPxControllerManager, PxCooking* pCooking);
	virtual ~CStage(void);

public:
	static CStage* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics* pPxPhysicsSDK, PxScene* pPxScene, PxControllerManager*	pPxControllerManager, PxCooking* pCooking);

public:
	virtual HRESULT Ready_Scene(void);
	virtual _int Update(const _float& fTimeDelta);

private:
	HRESULT Ready_GameLogic(void);
	HRESULT Ready_Environment(void);

public:
	virtual void Release(void);

public:
	HRESULT	 InitFloor(void);

	HRESULT  InitToiletFloor(void);

	HRESULT  LoadStageMap(void);

	MESHNUM	 MatchingObject(wstring strMeshName);

private:
	bool	m_bTestInit;
};


#endif // StageScene_h__
