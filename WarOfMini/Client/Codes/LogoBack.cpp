#include "stdafx.h"
#include "LogoBack.h"
#include "Transform.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"

CLogoBack::CLogoBack(ID3D11DeviceContext * pContext)
: CGameObject(pContext)
, m_pTransform(NULL)
, m_pBuffer(NULL)
, m_pTexture(NULL)
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

HRESULT CLogoBack::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));


	// Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	// Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_LogoBack");
	m_pTexture = dynamic_cast<CTextures*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));

	return S_OK;
}

HRESULT CLogoBack::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->m_vScale = _vec3(2.0f, 2.0f, 2.f);

	return S_OK;
}

_int CLogoBack::Update(const _float & fTimeDelta)
{
	CGameObject::Update(fTimeDelta);

	
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	return 0;
}

void CLogoBack::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Default"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	
	BASESHADERCB tBaseShaderCB;

	_matrix  matView, matProj;
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matProj);
	
	tBaseShaderCB.matWorld = m_pTransform->m_matWorld;
	tBaseShaderCB.matView = matView;
	tBaseShaderCB.matProj = matProj;

	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Default"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Default"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);


	m_pTexture->Render(0, 0);
	m_pBuffer->Render();
}

void CLogoBack::Release(void)
{
	CGameObject::Release();
	delete this;
}
