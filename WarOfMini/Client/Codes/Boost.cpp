#include "stdafx.h"
#include "Boost.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Transform.h"
#include "Scene.h"
#include "Player.h"
#include "Layer.h"
#include "FontMgr.h"

CBoostUI::CBoostUI(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_fXGage(0.0f)
, m_pPlayer(NULL)
{
}

CBoostUI::~CBoostUI(void)
{
}

CBoostUI* CBoostUI::Create(ID3D11DeviceContext * pContext)
{
	CBoostUI* pGage = new CBoostUI(pContext);

	if (FAILED(pGage->Initialize()))
		Safe_Release(pGage);

	return pGage;
}

HRESULT CBoostUI::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fMoveX = -250.f;
	m_fOriginMoveX = -250.f;

	m_fMoveY = 400.f;
	m_fOriginMoveY = 400.f;


	m_fX = (_float)(WINCX >> 2) + m_fMoveX;
	m_fY = (_float)(WINCY >> 1) + m_fMoveY;

	m_fSizeX = 250;
	m_fSizeY = 40;

	m_fOriginSizeX = 250;
	m_fOriginSizeY = 40;


	return S_OK;
}

_int CBoostUI::Update(const _float & fTimeDelta)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return 0;

	if (m_pPlayer == NULL)
		CheckPlayer();

	UpdateGage();


	CGameObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_UI, this);

	XMStoreFloat4x4(&m_pProj, XMMatrixOrthographicLH(_float(WINCX), _float(WINCY), 0.0f, 1.0f));


	return 0;
}

void CBoostUI::Render(void)
{
	if(CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return;


	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Gage"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11Buffer* pUIShaderCB = CGraphicDev::GetInstance()->GetUIShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();


	BASESHADER_CB tBaseShaderCB;

	XMFLOAT4X4			m_matWorld;
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

	ComputeChangeWindowSize();

	m_fX = (_float)(WINCX >> 2) + m_fMoveX;
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

	CFontMgr::GetInstance()->Render_Font(L"koverwatch", L"BOOST", 30.f, 120.f, 834.f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f));
}

void CBoostUI::Release(void)
{
	CUI::Release();
}

HRESULT CBoostUI::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_BoostBar");
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

void CBoostUI::CheckPlayer(void)
{

	CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");
	list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"Player");

	if (pObjList != NULL)
		m_pPlayer = (CPlayer*)(*pObjList->begin());
}

void CBoostUI::UpdateGage(void)
{
	if (m_pPlayer == NULL)
		return;

	_float m_fMaxBoost = m_pPlayer->GetMaxBoost();

	_float m_fRatio = (_float)(m_fOriginSizeX / m_fMaxBoost); //ºñÀ²

	_float m_fBoost = m_pPlayer->GetBoost();

	m_fBoost *= m_fRatio;


	m_fXGage = (_float)(m_fBoost / m_fOriginSizeX);

}
