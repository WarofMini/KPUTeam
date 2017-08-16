#include "stdafx.h"
#include "BloodyScreen.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Transform.h"
#include "Scene.h"
#include "Player.h"
#include "Layer.h"

CBloodyScreen::CBloodyScreen(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_pPlayer(NULL)
, m_fAlpha(0.0f)
, m_bCheck(false)
{
}

CBloodyScreen::~CBloodyScreen(void)
{
}

CBloodyScreen* CBloodyScreen::Create(ID3D11DeviceContext * pContext)
{
	CBloodyScreen* pBloodyScreen = new CBloodyScreen(pContext);

	if (FAILED(pBloodyScreen->Initialize()))
		Safe_Release(pBloodyScreen);

	return pBloodyScreen;
}

HRESULT CBloodyScreen::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fMoveX = 0.f;
	m_fOriginMoveX = 0.f;

	m_fMoveY = 0.f;
	m_fOriginMoveY = 0.f;


	m_fX = (_float)(WINCX >> 1) + m_fMoveX;
	m_fY = (_float)(WINCY >> 1) + m_fMoveY;

	m_fSizeX = 1800.f;
	m_fSizeY = 1100.f;

	m_fOriginSizeX = 1800.f;
	m_fOriginSizeY = 1100.f;


	return S_OK;
}

_int CBloodyScreen::Update(const _float & fTimeDelta)
{
	if (m_pPlayer == NULL)
		CheckPlayer();

	if (m_pPlayer->GetHP() <= 2.f)
	{
		if (m_bCheck == false)
		{
			m_fAlpha = min(m_fAlpha + (fTimeDelta * 1.f), 1.0f);

			if (m_fAlpha >= 1.0f)
			{
				m_bCheck = true;
			}
		}
		else
		{
			m_fAlpha = max(m_fAlpha - (fTimeDelta * 1.f), 0.0f);

			if (m_fAlpha <= 0.0f)
			{
				m_bCheck = false;
			}
		}

	}
	else
	{
		if (m_fAlpha > 0.0f)
		{
			m_fAlpha = max(m_fAlpha - (fTimeDelta * 1.f), 0.0f);
		}
	}

	CGameObject::Update(fTimeDelta);

	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return 0;
	

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_UI, this);

	XMStoreFloat4x4(&m_pProj, XMMatrixOrthographicLH(_float(WINCX), _float(WINCY), 0.0f, 1.0f));


	return 0;
}

void CBloodyScreen::Render(void)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return;


	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Alpha"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11Buffer* pAlphaShaderCB = CGraphicDev::GetInstance()->GetAlphaShaderCB();
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

	ALPHA_CB tAlphaShaderCB;

	tAlphaShaderCB.fAlpha = m_fAlpha;

	m_pContext->UpdateSubresource(pAlphaShaderCB, 0, NULL, &tAlphaShaderCB, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Alpha"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);

	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Alpha"), NULL, 0);
	m_pContext->PSSetConstantBuffers(1, 1, &pAlphaShaderCB);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	m_pTexture->Render(0, m_iTextureNumber);
	m_pBuffer->Render();
}

void CBloodyScreen::Release(void)
{
	CUI::Release();
}

HRESULT CBloodyScreen::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_BloodyScreen");
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

void CBloodyScreen::CheckPlayer(void)
{
	CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");
	list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"Player");

	if (pObjList != NULL)
		m_pPlayer = (CPlayer*)(*pObjList->begin());
}