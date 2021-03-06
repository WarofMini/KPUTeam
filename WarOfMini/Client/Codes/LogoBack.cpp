#include "stdafx.h"
#include "LogoBack.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "RcTex.h"
#include "CameraMgr.h"
#include "Transform.h"
#include "Sound.h"

CLogoBack::CLogoBack(ID3D11DeviceContext * pContext)
: CUI(pContext)
{
}

CLogoBack::~CLogoBack(void)
{
}

CLogoBack * CLogoBack::Create(ID3D11DeviceContext * pContext)
{
	CLogoBack* pLogoBack = new CLogoBack(pContext);

	if (FAILED(pLogoBack->Initialize()))
		Safe_Release(pLogoBack);

	return pLogoBack;
}

HRESULT CLogoBack::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fMoveX = 0.f;
	m_fOriginMoveX = 0.f;

	m_fMoveY = 0.f;
	m_fOriginMoveY = 0.f;

	m_fX = (WINCX >> 1) + m_fMoveX;
	m_fY = (WINCY >> 1) + m_fMoveY;

	m_fSizeX = 1600;
	m_fSizeY = 900;

	m_fOriginSizeX = 1600;
	m_fOriginSizeY = 900;

	m_pSound->IsPlaying(L"Intro");
	m_pSound->MyStopSoundAll();
	m_pSound->MyPlaySound(L"Intro", true);
	m_pSound->Update_Component(0.f);

	return S_OK;
}

INT CLogoBack::Update(const FLOAT & fTimeDelta)
{
	CGameObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_UI, this);

	XMStoreFloat4x4(&m_pProj, XMMatrixOrthographicLH(_float(WINCX), _float(WINCY), 0.0f, 1.0f));

	return 0;
}

void CLogoBack::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Default"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	XMFLOAT4X4			m_matWorld;
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

	ComputeChangeWindowSize();

	m_fX = (WINCX >> 1) + m_fMoveX;
	m_fY = (WINCY >> 1) + m_fMoveY;

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fX - WINCX * 0.5f;
	m_matWorld._42 = -m_fY + WINCY * 0.5f;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(&m_pView));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(&m_pProj));

	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Default"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Default"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	m_pTexture->Render(0, m_iTextureNumber);
	m_pBuffer->Render();
}

void CLogoBack::Release(void)
{
	m_pSound->MyStopSound(L"Town");
	CGameObject::Release();
	delete this;
}

HRESULT CLogoBack::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_LogoBack");
	m_pTexture = dynamic_cast<CTextures*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));


	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));

	//SoundBGM
	pComponent = CSound::Create((CTransform*)pComponent);
	m_pSound = dynamic_cast<CSound*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Sound", pComponent));
	m_pSound->Set_Sound(L"Intro", L"GameIntro_bgm.mp3");

	return S_OK;
}
