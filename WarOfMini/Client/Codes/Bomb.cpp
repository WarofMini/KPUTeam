#include "stdafx.h"
#include "Bomb.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Transform.h"


CBomb::CBomb(ID3D11DeviceContext * pContext)
:CEffect(pContext)
{
	m_iFrame = 0;
	m_iSizeX = 4;
	m_iSizeY = 4;

	m_fLifeTime = 1.f;
	m_fRealTime = 0.0f;
	m_fDelayTime = 0.0f;
	m_bAllBillboardCheck = true;
}

CBomb::~CBomb(void)
{
}

CBomb * CBomb::Create(ID3D11DeviceContext * pContext)
{
	CBomb* pEffect = new CBomb(pContext);

	if (FAILED(pEffect->Initialize()))
		Safe_Release(pEffect);

	return pEffect;
}

HRESULT CBomb::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->m_vScale = XMFLOAT3(100.f, 100.f, 0.f);

	m_pTransform->m_vPos = XMFLOAT3(50.f, 80.f, 0.f);

	return S_OK;
}

INT CBomb::Update(const FLOAT & fTimeDelta)
{

	m_fRealTime += fTimeDelta;

	m_iFrame = int((m_fRealTime / m_fLifeTime) * (m_iSizeX * m_iSizeY));

	CGameObject::Update(fTimeDelta);
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ALPHA, this);

	ComputeBillboard();

	return 0;
}

void CBomb::Render(void)
{

	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Effect"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11Buffer* pSpriteShaderCB = CGraphicDev::GetInstance()->GetSpriteShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);


	TEREFFSHADER_CB tSpriteShaderCB;

	tSpriteShaderCB.iSizeX = m_iSizeX;
	tSpriteShaderCB.iSizeY = m_iSizeY;
	tSpriteShaderCB.iFrame = m_iFrame;
	m_pContext->UpdateSubresource(pSpriteShaderCB, 0, NULL, &tSpriteShaderCB, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Effect"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->VSSetConstantBuffers(1, 1, &pSpriteShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Effect"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	m_pTexture->Render(0, 0);
	m_pBuffer->Render();

}

void CBomb::Release(void)
{
	CEffect::Release();
}

HRESULT CBomb::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_Bomb");
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