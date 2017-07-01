#include "stdafx.h"
#include "UI.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "RcTex.h"
#include "CameraMgr.h"
#include "Transform.h"

CUI::CUI(ID3D11DeviceContext * pContext)
: CGameObject(pContext)
, m_pBuffer(NULL)
, m_pTexture(NULL)
, m_pTransform(NULL)
{
	XMStoreFloat4x4(&m_pProj, XMMatrixIdentity());
	XMStoreFloat4x4(&m_pView, XMMatrixIdentity());
}

CUI::~CUI(void)
{
}

CUI * CUI::Create(ID3D11DeviceContext * pContext)
{
	CUI* pUI = new CUI(pContext);

	if (FAILED(pUI->Initialize()))
		Safe_Release(pUI);

	return pUI;
}

HRESULT CUI::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fX = (WINCX >> 1);
	m_fY = (WINCY >> 1);

	m_fSizeX = 200;
	m_fSizeY = 200;

	//m_pTransform->m_vScale = XMFLOAT3(2.f, 1.f, 1.f);
	//m_pTransform->m_vPos = XMFLOAT3(0.0f, 0.f, 0.0f);

	return S_OK;
}

INT CUI::Update(const FLOAT & fTimeDelta)
{
	CGameObject::Update(fTimeDelta);
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_UI, this);


	XMStoreFloat4x4(&m_pProj,   XMMatrixOrthographicLH(_float(WINCX), _float(WINCY), 0.0f, 1.0f));

	return 0;
}

void CUI::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Default"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	XMFLOAT4X4			m_matWorld;
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

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

	m_pTexture->Render(0, 0);
	m_pBuffer->Render();
}

void CUI::Release(void)
{
	CGameObject::Release();
	delete this;
}

HRESULT CUI::Ready_Component(void)
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

	return S_OK;
}
