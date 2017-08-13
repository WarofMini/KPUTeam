#include "stdafx.h"
#include "HitScreen.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Transform.h"
#include "Sound.h"


CHitScreen::CHitScreen(ID3D11DeviceContext * pContext)
:CEffect(pContext)
, m_fAlpha(1.f)
, m_fAlphaSpeed(0.f)
{
	m_bAllBillboardCheck = true;
}

CHitScreen::~CHitScreen(void)
{
}

CHitScreen* CHitScreen::Create(ID3D11DeviceContext * pContext)
{
	CHitScreen* pHitScreen = new CHitScreen(pContext);

	if (FAILED(pHitScreen->Initialize()))
		Safe_Release(pHitScreen);

	return pHitScreen;
}

HRESULT CHitScreen::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

INT CHitScreen::Update(const FLOAT & fTimeDelta)
{
	m_fAlpha = max((_float)(m_fAlpha - (fTimeDelta * m_fAlphaSpeed)), 0.0f);

	m_pTransform->m_vPos.y = m_pTransform->m_vPos.y + (fTimeDelta * 10.f);


	CEffect::Update(fTimeDelta);


	XMMATRIX matView = XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView());
	XMFLOAT4X4 matViewWorld;

	XMStoreFloat4x4(&matViewWorld, XMLoadFloat4x4(&m_pTransform->m_matWorld) * matView);

	ComputeBillboard();

	if (m_fAlpha <= 0.0f)
	{
		m_bDead = true;
	}

	if (!m_bDead)
		CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_EFFECT_ALPHA, this, matViewWorld._43);

	return m_bDead;
}

void CHitScreen::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_GunFlash"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11Buffer* pAlphaShaderCB = CGraphicDev::GetInstance()->GetAlphaShaderCB();

	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	ALPHA_CB tAlphaShaderCB;
	tAlphaShaderCB.fAlpha = m_fAlpha;
	m_pContext->UpdateSubresource(pAlphaShaderCB, 0, NULL, &tAlphaShaderCB, 0, 0);


	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_GunFlash"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_GunFlash"), NULL, 0);
	m_pContext->PSSetConstantBuffers(1, 1, &pAlphaShaderCB);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);


	//m_iTextureNumber 0 : 공격
	//m_iTextureNumber 1 : 피격
	m_pTexture->Render(0, m_iTextureNumber);
	m_pBuffer->Render();

}

void CHitScreen::Release(void)
{
	CEffect::Release();
}

HRESULT CHitScreen::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_Hit");
	m_pTexture = dynamic_cast<CTextures*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));


	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));

	return S_OK;
}