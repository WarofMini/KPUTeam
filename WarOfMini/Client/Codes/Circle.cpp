#include "stdafx.h"
#include "Circle.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Transform.h"


CCircle::CCircle(ID3D11DeviceContext * pContext)
:CEffect(pContext)
, m_bActiveCheck(false)
{
}

CCircle::~CCircle(void)
{
}

CCircle * CCircle::Create(ID3D11DeviceContext * pContext)
{
	CCircle* pEffect = new CCircle(pContext);

	if (FAILED(pEffect->Initialize()))
		Safe_Release(pEffect);

	return pEffect;
}

HRESULT CCircle::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

INT CCircle::Update(const FLOAT & fTimeDelta)
{
	if (m_bActiveCheck)
	{

	}
	else
	{
	}



	CEffect::Update(fTimeDelta);


	XMMATRIX matView = XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView());
	XMFLOAT4X4 matViewWorld;

	XMStoreFloat4x4(&matViewWorld, XMLoadFloat4x4(&m_pTransform->m_matWorld) * matView);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_EFFECT_ALPHA, this, matViewWorld._43);


	return 0;
}

void CCircle::Render(void)
{

	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Default"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
	tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);



	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Default"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Default"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	m_pTexture->Render(0, m_iTextureNumber);
	m_pBuffer->Render();

}

void CCircle::Release(void)
{
	CEffect::Release();
}

HRESULT CCircle::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CRcTex*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Texture_Circle");
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