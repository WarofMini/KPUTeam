#include "stdafx.h"
#include "LightParticle.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "CameraMgr.h"
#include "Particle.h"


CLightParticle::CLightParticle(ID3D11DeviceContext * pContext)
:CEffect(pContext)
, m_iParticleCount(100)
, m_bStart(false)
, m_bEnd(false)
{
	ZeroMemory(m_pParticle, sizeof(CParticle*) * m_iParticleCount);
}


CLightParticle::~CLightParticle(void)
{
}

CLightParticle* CLightParticle::Create(ID3D11DeviceContext * pContext)
{
	CLightParticle* pLightParticle = new CLightParticle(pContext);

	if (FAILED(pLightParticle->Initialize()))
		Safe_Release(pLightParticle);

	return pLightParticle;
}

HRESULT CLightParticle::Initialize(void)
{
	for (int i = 0; i < m_iParticleCount; ++i)
	{
		m_pParticle[i] = CParticle::Create(m_pContext);
		m_pParticle[i]->SetCrnP(XMFLOAT3(600.f, 300.f, 600.f));
		m_pParticle[i]->SetTransformScale(XMFLOAT3(10.f, 10.f, 10.f));
	}

	return S_OK;
}

_int CLightParticle::Update(const _float & fTimeDelta)
{
	if (!g_bBlackOut) //정전이 아니면
	{
		m_bStart = false;
		m_bEnd = false;
		return 0;
	}
	else //정전이 된경우
	{
		if (!m_bStart)
		{
			ResetParticle(); //파티클 설정 초기화
			m_bStart = true;
		}
		if (m_pParticle[0]->GetAlpha() <= 0.0f)
		{
			m_bEnd = true;
			return 0;
		}
	}




	for (int i = 0; i < m_iParticleCount; ++i)
	{
		m_pParticle[i]->Update(fTimeDelta);
	}

	return 0;
}

void CLightParticle::Render(void)
{
	if ( (!g_bBlackOut) || (m_bEnd == true))
	{
		return;
	}
	

	for (int i = 0; i < m_iParticleCount; ++i)
	{
		m_pParticle[i]->Render();
	}
}

void CLightParticle::Release(void)
{
	for (int i = 0; i < m_iParticleCount; ++i)
	{
		m_pParticle[i]->Release();
	}

	CEffect::Release();
}

HRESULT CLightParticle::Ready_Component(void)
{
	return S_OK;
}

void CLightParticle::ResetParticle(void)
{
	for (int i = 0; i < m_iParticleCount; ++i)
	{
		m_pParticle[i]->InitParticle();
		m_pParticle[i]->SetCrnP(XMFLOAT3(600.f, 300.f, 600.f));
		m_pParticle[i]->SetTransformScale(XMFLOAT3(10.f, 10.f, 10.f));
		m_pParticle[i]->SetAlpha(1.0f);
	}
}
