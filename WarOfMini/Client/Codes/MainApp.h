#ifndef MainApp_h__
#define MainApp_h__

#include "Include.h"


class CMainApp
{
private:
	explicit CMainApp(void);
	virtual ~CMainApp(void);


public:
	HRESULT Initialize(void);
	_int Update(const _float& fTimeDelta);
	void Render(void);

public:
	static CMainApp* Create(void);

private:
	_ulong							m_dwRenderCnt;
	_tchar							m_szFPS[128];
	_float							m_fTime;
	_bool							m_bDebugRender;

	//Physx SDK Member Variables =========================
	PxPhysics*						m_pPxPhysicsSDK;
	PxScene*						m_pPxScene;
	PxControllerManager*			m_pPxControllerManager;
	PxFoundation*					m_pPxFoundation;
	PxDefaultErrorCallback			m_PxDefaultErrorCallback;
	PxDefaultAllocator				m_PxDefaultAllocatorCallback;
	PxVisualDebuggerConnection*     m_pPVDConnection;
	PxCooking*						m_pCooking;
	//====================================================

private:	//인풋 장치 소실 했을때 다시 찾는 함수.
	void		Set_Focus(void);

public:
	void Release(void);

private:
	void Ready_TextureFromFile(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	void Ready_SpriteTextureFromFile(_tchar* strFilePath, ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	void Render_FPS(void);
	void Render_CurrentScene(void);

	void Stage_DebugInfo(void);

	void Debug_KeyCheck(void);

public:
	/////////////// Physx SDK Member Function ///////////////
	void InitializePhysxEngine();
	void ReleasePhysxEngine();
};



#endif // MainApp_h__
