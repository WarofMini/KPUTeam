#include "stdafx.h"
#include "GageUI.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Transform.h"
#include "Loading.h"

CLoadingBarUI::CLoadingBarUI(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_fXGage(0.0f)
{
}

CLoadingBarUI::~CLoadingBarUI(void)
{
}

CLoadingBarUI* CLoadingBarUI::Create(ID3D11DeviceContext * pContext)
{
	CLoadingBarUI* pGage = new CLoadingBarUI(pContext);

	if (FAILED(pGage->Initialize()))
		Safe_Release(pGage);

	return pGage;
}

HRESULT CLoadingBarUI::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fMoveX = 0.f;
	m_fOriginMoveX = 0.f;

	m_fMoveY = 400.f;
	m_fOriginMoveY = 400.f;


	m_fX = (_float)(WINCX >> 1) + m_fMoveX;
	m_fY = (_float)(WINCY >> 1) + m_fMoveY;

	m_fSizeX = 1200;
	m_fSizeY = 40;

	m_fOriginSizeX = 1200;
	m_fOriginSizeY = 40;


	return S_OK;
}

_int CLoadingBarUI::Update(const _float & fTimeDelta)
{

	_float m_fCurCount = (_float)g_iCurrentCount;

	m_fXGage = m_fCurCount / (_float)g_TotalLoadingCount;


	CGameObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_UI, this);

	XMStoreFloat4x4(&m_pProj, XMMatrixOrthographicLH(_float(WINCX), _float(WINCY), 0.0f, 1.0f));


	return 0;
}

void CLoadingBarUI::Render(void)
{

	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Gage"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11Buffer* pUIShaderCB = CGraphicDev::GetInstance()->GetUIShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();


	BASESHADER_CB tBaseShaderCB;

	XMFLOAT4X4			m_matWorld;
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

	ComputeChangeWindowSize();

	m_fX = (_float)(WINCX >> 1) + m_fMoveX;
	m_fY = (_float)(WINCY >> 1) + m_fMoveY;


	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fX - WINCX * 0.5f;
	m_matWorld._42 = -m_fY + WINCY * 0.5f;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(&m_pView));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(&m_pProj));

	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	UI_CB tUIShaderCB;

	tUIShaderCB.fXGage = m_fXGage;

	m_pContext->UpdateSubresource(pUIShaderCB, 0, NULL, &tUIShaderCB, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Gage"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->VSSetConstantBuffers(1, 1, &pUIShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Gage"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	m_pTexture->Render(0, m_iTextureNumber);
	m_pBuffer->Render();
}

void CLoadingBarUI::Release(void)
{
	CUI::Release();
}

HRESULT CLoadingBarUI::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_Gage");
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