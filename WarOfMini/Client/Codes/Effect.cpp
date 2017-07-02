#include "stdafx.h"
#include "Effect.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "RcTex.h"
#include "CameraMgr.h"
#include "Transform.h"


CEffect::CEffect(ID3D11DeviceContext * pContext)
:CGameObject(pContext)
, m_pBuffer(NULL)
, m_pTexture(NULL)
, m_pTransform(NULL)
, m_iFrame(0)
, m_iSizeX(0)
, m_iSizeY(0)
, m_fLifeTime(1.0f)
, m_fRealTime(0.0f)
, m_fDelayTime(0.0f)
{

}

CEffect::~CEffect(void)
{
}

HRESULT CEffect::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

INT CEffect::Update(const FLOAT & fTimeDelta)
{
	return 0;
}

void CEffect::Render(void)
{	
}

void CEffect::Release(void)
{
	CGameObject::Release();
	delete this;

}

HRESULT CEffect::Ready_Component(void)
{
	return S_OK;
}