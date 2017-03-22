#include "stdafx.h"
#include "StaticObject.h"
#include "Transform.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "MeshMgr.h"

CStaticObject::CStaticObject(ID3D11DeviceContext* pContext)
: CGameObject(pContext)
, m_pTransform(nullptr)
, m_uiObjNum(0)
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
		m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Default"));

		ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
		ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

		BASESHADER_CB tBaseShaderCB;

		tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
		tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
		tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
		tBaseShaderCB.vLightPos = XMVectorSet(g_vLightPos.x - g_vPlayerPos.x, g_vLightPos.y - g_vPlayerPos.y
			, g_vLightPos.z - g_vPlayerPos.z, 1.f);

		m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

		m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Default"), NULL, 0);
		m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
		m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Default"), NULL, 0);
		m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

		CMeshMgr::GetInstance()->Render_MeshMgr(m_uiObjNum, FALSE);
	}

	else
	{
		m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Normal"));

		ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
		ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

		BASESHADER_CB tBaseShaderCB;

		tBaseShaderCB.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
		tBaseShaderCB.matView = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView()));
		tBaseShaderCB.matProj = XMMatrixTranspose(XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj()));
		tBaseShaderCB.vLightPos = XMVectorSet(g_vLightPos.x - g_vPlayerPos.x, g_vLightPos.y - g_vPlayerPos.y
			, g_vLightPos.z - g_vPlayerPos.z, 1.f);

		m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

		m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Normal"), NULL, 0);
		m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
		m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Normal"), NULL, 0);
		m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

		CMeshMgr::GetInstance()->Render_MeshMgr(m_uiObjNum, FALSE);
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
