#include "stdafx.h"
#include "StaticObject.h"
#include "Transform.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "MeshMgr.h"

CStaticObject::CStaticObject(ID3D11DeviceContext* pContext)
: CGameObject(pContext)
{
}

CStaticObject::~CStaticObject(void)
{
}

HRESULT CStaticObject::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

_int CStaticObject::Update(const _float& fTimeDelta)
{
	return CGameObject::Update(fTimeDelta);
}

void CStaticObject::Render(void)
{
	if (m_uiObjNum == MESHNUM_END + 3)
	{
		m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Object"));

		ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
		ID3D11Buffer* pDirShaderCB = CGraphicDev::GetInstance()->GetDirLightShaderCB();
		ID3D11Buffer* pMaterialCB = CGraphicDev::GetInstance()->GetMaterialShaderCB();

		
		ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

		BASESHADER_CB tBaseShaderCB;

		tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
		tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
		tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
		tBaseShaderCB.vLightPos = XMVectorSet(g_vLightPos.x - g_vPlayerPos.x, g_vLightPos.y - g_vPlayerPos.y
								, g_vLightPos.z - g_vPlayerPos.z, 1.f);

		m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

		DIRECTIONALIGHT_CB tDirCB;

		tDirCB.Ambient = XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f);
		tDirCB.Diffuse = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		tDirCB.Specular = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		tDirCB.Direction = XMVectorSet(0.57735f, -0.17735f, 0.57735f, 0.0f);

		m_pContext->UpdateSubresource(pDirShaderCB, 0, NULL, &tDirCB, 0, 0);

		MATERIAL_CB tMaterialCB;

		tMaterialCB.Ambient = XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);
		tMaterialCB.Diffuse = XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);
		tMaterialCB.Specular = XMVectorSet(0.8f, 0.8f, 0.8f, 10.0f);
		tMaterialCB.Eye = XMLoadFloat3(&CCameraMgr::GetInstance()->Get_CurCameraEye());

		m_pContext->UpdateSubresource(pMaterialCB, 0, NULL, &tMaterialCB, 0, 0);

		m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Object"), NULL, 0);
		m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
		m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Object"), NULL, 0);
		m_pContext->PSSetSamplers(0, 1, &pBaseSampler);
		m_pContext->PSSetConstantBuffers(1, 1, &pDirShaderCB);
		m_pContext->PSSetConstantBuffers(2, 1, &pMaterialCB);


		CMeshMgr::GetInstance()->Render_MeshMgr(m_uiObjNum, m_iTextureNumber, FALSE);
	}
	else
	{
		m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Normal"));

		ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
		ID3D11Buffer* pDirShaderCB = CGraphicDev::GetInstance()->GetDirLightShaderCB();
		ID3D11Buffer* pMaterialCB = CGraphicDev::GetInstance()->GetMaterialShaderCB();


		ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

		BASESHADER_CB tBaseShaderCB;

		tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
		tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
		tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
		tBaseShaderCB.vLightPos = XMVectorSet(g_vLightPos.x - g_vPlayerPos.x, g_vLightPos.y - g_vPlayerPos.y
			, g_vLightPos.z - g_vPlayerPos.z, 1.f);

		m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

		DIRECTIONALIGHT_CB tDirCB;

		tDirCB.Ambient = XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f);
		tDirCB.Diffuse = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		tDirCB.Specular = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		tDirCB.Direction = XMVectorSet(0.57735f, -0.17735f, 0.57735f, 0.0f);

		m_pContext->UpdateSubresource(pDirShaderCB, 0, NULL, &tDirCB, 0, 0);

		MATERIAL_CB tMaterialCB;

		tMaterialCB.Ambient = XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);
		tMaterialCB.Diffuse = XMVectorSet(0.8f, 0.8f, 0.8f, 1.0f);
		tMaterialCB.Specular = XMVectorSet(0.8f, 0.8f, 0.8f, 10.0f);
		tMaterialCB.Eye = XMLoadFloat3(&CCameraMgr::GetInstance()->Get_CurCameraEye());

		m_pContext->UpdateSubresource(pMaterialCB, 0, NULL, &tMaterialCB, 0, 0);

		m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Normal"), NULL, 0);
		m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
		m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Normal"), NULL, 0);
		m_pContext->PSSetConstantBuffers(1, 1, &pDirShaderCB);
		m_pContext->PSSetConstantBuffers(2, 1, &pMaterialCB);
		m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

		CMeshMgr::GetInstance()->Render_MeshMgr(m_uiObjNum, m_iTextureNumber, FALSE);
	}
}



void CStaticObject::Release(void)
{
	CGameObject::Release();
}

HRESULT CStaticObject::Ready_Component(void)
{
	CComponent* pComponent = NULL;

	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));

	return S_OK;
}