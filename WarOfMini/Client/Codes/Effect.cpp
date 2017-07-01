#include "stdafx.h"
#include "Effect.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "RcTex.h"
#include "CameraMgr.h"
#include "Transform.h"


CEffect::CEffect(ID3D11DeviceContext * pContext)
:CGameObject(pContext)
, m_pBuffer(NULL)
, m_pTexture(NULL)
, m_pTransform(NULL)
{
	m_iCurrentTextureNumber = 0;
	m_iLastTextureNumber = 89;
	m_fFrameSpeed = 100.f;
}

CEffect::~CEffect(void)
{
}

CEffect * CEffect::Create(ID3D11DeviceContext * pContext)
{
	CEffect* pEffect = new CEffect(pContext);

	if (FAILED(pEffect->Initialize()))
		Safe_Release(pEffect);

	return pEffect;
}

HRESULT CEffect::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->m_vScale = XMFLOAT3(100.f, 100.f, 0.f);

	m_pTransform->m_vPos = XMFLOAT3(50.f, 80.f, 0.f);

	return S_OK;
}

INT CEffect::Update(const FLOAT & fTimeDelta)
{
	m_iCurrentTextureNumber += (fTimeDelta * m_fFrameSpeed);
	m_iCurrentTextureNumber %= m_iLastTextureNumber;


	CGameObject::Update(fTimeDelta);
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ALPHA, this);


	return 0;
}

void CEffect::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Effect"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Effect"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Effect"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	m_pTexture->Render(0, m_iCurrentTextureNumber);
	m_pBuffer->Render();

}

void CEffect::Release(void)
{
	CGameObject::Release();
	delete this;

}

HRESULT CEffect::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_Explosion");
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
