#include "stdafx.h"
#include "Panel.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Transform.h"


CPanel::CPanel(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_fAlpha(0.0f)
, m_bStart(false)
, m_bEnd(false)
{
}

CPanel::~CPanel(void)
{
}

CPanel* CPanel::Create(ID3D11DeviceContext * pContext, wstring strName)
{
	CPanel* pUI = new CPanel(pContext);

	pUI->SetName(strName);

	if (FAILED(pUI->Initialize()))
		Safe_Release(pUI);

	return pUI;
}

HRESULT CPanel::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fX = (WINCX >> 1) + m_fMoveX;
	m_fY = (WINCY >> 1) + m_fMoveY;

	return S_OK;
}

_int CPanel::Update(const _float & fTimeDelta)
{
	if ((!m_bStart))
		return 0;

	PanelUpdate(fTimeDelta);

	


	CGameObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_UI, this);

	XMStoreFloat4x4(&m_pProj, XMMatrixOrthographicLH(_float(WINCX), _float(WINCY), 0.0f, 1.0f));

	return 0;
}

void CPanel::Render(void)
{
	if ((!m_bStart))
		return;

	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Panel"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11Buffer* pAlphaShaderCB = CGraphicDev::GetInstance()->GetAlphaShaderCB();

	
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

	ALPHA_CB tAlphaShaderCB;
	tAlphaShaderCB.fAlpha = m_fAlpha;

	m_pContext->UpdateSubresource(pAlphaShaderCB, 0, NULL, &tAlphaShaderCB, 0, 0);


	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Panel"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Panel"), NULL, 0);
	m_pContext->PSSetConstantBuffers(1, 1, &pAlphaShaderCB);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	m_pTexture->Render(0, m_iTextureNumber);
	m_pBuffer->Render();
}

void CPanel::Release(void)
{
	CUI::Release();
}

HRESULT CPanel::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, m_strTextureName.c_str());
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

void CPanel::PanelUpdate(const _float & fTimeDelta)
{
	if (m_bStart && (!m_bEnd))
	{
		m_fAlpha = min(m_fAlpha + fTimeDelta, 0.5f);
	}
	if (m_bEnd)
	{
		m_fAlpha = max(m_fAlpha - fTimeDelta, 0.0f);

		if (m_fAlpha <= 0.0f)
		{
			m_bStart = false;
			m_bEnd = false;
		}
	}
}

_bool CPanel::GetResetCheck(void)
{
	if ((m_bStart == false) && (m_bEnd == false))
		return true;
	else
		return false;
}
