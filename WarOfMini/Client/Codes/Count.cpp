#include "stdafx.h"
#include "Count.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Transform.h"

CCountUI::CCountUI(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_fCount(-1.0f)
, m_bCheck(false)
{
}

CCountUI::~CCountUI(void)
{
}

CCountUI* CCountUI::Create(ID3D11DeviceContext * pContext)
{
	CCountUI* pCount = new CCountUI(pContext);

	if (FAILED(pCount->Initialize()))
		Safe_Release(pCount);

	return pCount;
}

HRESULT CCountUI::Initialize(void)
{
	m_strTextureName = L"Texture_CountNumber";

	if (FAILED(Ready_Component()))
		return E_FAIL;


	m_fMoveX = 0.f;
	m_fOriginMoveX = 0.f;

	m_fMoveY = 0.f;
	m_fOriginMoveY = 0.f;


	m_fX = (_float)(WINCX >> 1) + m_fMoveX;
	m_fY = (_float)(WINCY >> 2) + m_fMoveY;

	m_fSizeX = 100;
	m_fSizeY = 100;

	m_fOriginSizeX = 100;
	m_fOriginSizeY = 100;


	return S_OK;
}

_int CCountUI::Update(const _float & fTimeDelta)
{
	if ((CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC))
		return 0;

	if (m_bCheck == false)
		return 0;

	if (m_fCount == 3)
		m_iTextureNumber = 2;
	else if(m_fCount == 2)
		m_iTextureNumber = 1;
	else if (m_fCount == 1)
		m_iTextureNumber = 0;
	



	CGameObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_UI, this);

	XMStoreFloat4x4(&m_pProj, XMMatrixOrthographicLH(_float(WINCX), _float(WINCY), 0.0f, 1.0f));


	return 0;
}

void CCountUI::Render(void)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return;

	if (m_bCheck == false)
		return ;


	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Default"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	XMFLOAT4X4			m_matWorld;
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

	ComputeChangeWindowSize();

	m_fX = (WINCX >> 1) + m_fMoveX;
	m_fY = (WINCY >> 2) + m_fMoveY;

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

void CCountUI::Release(void)
{
	CUI::Release();
}

HRESULT CCountUI::Ready_Component(void)
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

void CCountUI::SetCount(_float fCount)
{
	m_fCount = fCount; 

	if (m_fCount == 3)
		m_bCheck = true;
}
