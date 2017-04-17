#ifndef SceneSelector_h__
#define SceneSelector_h__

#include "Include.h"
#include "Logo.h"
#include "Stage.h"

class CSceneSelector
{

public:
	explicit CSceneSelector(Scene_Type eSceneID) { m_eSceneID = eSceneID; }
	~CSceneSelector(void) {}


public:
	HRESULT operator () (CScene** ppScene, ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, PxPhysics*	pPxPhysicsSDK, PxScene* pPxScene, PxControllerManager*	pPxControllerManager, PxCooking* pCooking)
	{
		switch (m_eSceneID)
		{
		case SCENE_LOGIN:
			//*ppScene = CLoginScene::Create(pGraphicDev, pContext);
			break;

		case SCENE_LOGO:
			*ppScene = CLogo::Create(pGraphicDev, pContext, pPxPhysicsSDK, pPxScene, pPxControllerManager, pCooking);
			break;

		case SCENE_STAGE:
			*ppScene = CStage::Create(pGraphicDev, pContext, pPxPhysicsSDK, pPxScene, pPxControllerManager, pCooking);
			break;
		}

		if (*ppScene == NULL)
			return E_FAIL;

		return S_OK;
	}

	//CLoading::LOADINGTYPE	m_eLoadingType;
};


#endif // SceneSelector_h__
