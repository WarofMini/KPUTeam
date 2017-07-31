#include "stdafx.h"
#include "TitleBack.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "RcTex.h"
#include "CameraMgr.h"
#include "Transform.h"

CTitleBack::CTitleBack(ID3D11DeviceContext * pContext)
: CGameObject(pContext)
, m_pBuffer(NULL)
, m_pTexture(NULL)
, m_fAlpha(0.f)
, m_fAlphaSpeed(0.3f)
, m_bAlphaCheck(false)
{
}

CTitleBack::~CTitleBack(void)
{
}

CTitleBack * CTitleBack::Create(ID3D11DeviceContext * pContext)
{
	CTitleBack* pTitleBack = new CTitleBack(pContext);

	if (FAILED(pTitleBack->Initialize()))
		Safe_Release(pTitleBack);

	return pTitleBack;
}

HRESULT CTitleBack::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->m_vScale = XMFLOAT3(2.f, 2.f, 2.f);

	return S_OK;
}

INT CTitleBack::Update(const FLOAT & fTimeDelta)
{
	m_fAlpha = min(m_fAlpha  + (fTimeDelta * m_fAlphaSpeed), 1.f);

	if (m_fAlpha >= 1.f)
		m_bAlphaCheck = true;

	CGameObject::Update(fTimeDelta);
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CTitleBack::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_TitleBack"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11Buffer* pAlphaShaderCB = CGraphicDev::GetInstance()->GetSpriteShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMMatrixIdentity());
	tBaseShaderCB.matProj = XMMatrixTranspose(XMMatrixIdentity());
	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);


	TEREFFSHADER_CB tAlphaShaderCB;

	tAlphaShaderCB.fAlpha = m_fAlpha;

	m_pContext->UpdateSubresource(pAlphaShaderCB, 0, NULL, &tAlphaShaderCB, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_TitleBack"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->VSSetConstantBuffers(1, 1, &pAlphaShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_TitleBack"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	m_pTexture->Render(0, 0);
	m_pBuffer->Render();
}

void CTitleBack::Release(void)
{
	CGameObject::Release();
	delete this;
}

HRESULT CTitleBack::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_TitleBack");
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
