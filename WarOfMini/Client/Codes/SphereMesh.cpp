#include "stdafx.h"
#include "SphereMesh.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "Management.h"
#include "GraphicDev.h"
#include "CameraMgr.h"

CSphereMesh::CSphereMesh(ID3D11DeviceContext* pContext, _float fRadius, XMFLOAT3* pPos)
	: CGameObject(pContext)
	, m_fRadius(fRadius)
	, m_pTransform(NULL)
	, m_vPos(pPos)
	, m_matWorld(NULL)
{
	m_uiObjNum = MESHNUM_SPHERE;
}

CSphereMesh::CSphereMesh(ID3D11DeviceContext * pContext, _float fRadius)
: CGameObject(pContext)
, m_fRadius(fRadius)
, m_pTransform(NULL)
, m_matWorld(NULL)
, m_vPos(NULL)
{
	m_uiObjNum = MESHNUM_SPHERE;
}

CSphereMesh::~CSphereMesh(void)
{
}

CSphereMesh* CSphereMesh::Create(ID3D11DeviceContext* pContext, _float fRadius, XMFLOAT3* pPos)
{
	CSphereMesh* pObject = new CSphereMesh(pContext, fRadius, pPos);

	if (FAILED(pObject->Initialize()))
		Safe_Release(pObject);

	return pObject;
}

CSphereMesh * CSphereMesh::Create(ID3D11DeviceContext * pContext, _float fRadius)
{
	CSphereMesh* pObject = new CSphereMesh(pContext, fRadius);

	if (FAILED(pObject->Initialize()))
		Safe_Release(pObject);

	return pObject;
}

HRESULT CSphereMesh::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransform->m_vScale = XMFLOAT3(m_fRadius, m_fRadius, m_fRadius);


	return S_OK;
}

_int CSphereMesh::Update(const _float& fTimeDelta)
{
	if(m_vPos != NULL)
		m_pTransform->m_vPos = (*m_vPos);


	CGameObject::Update(fTimeDelta);

	if (m_matWorld != NULL)
	{
		XMStoreFloat4x4(&m_pTransform->m_matWorld, XMLoadFloat4x4(m_matWorld));

		XMMATRIX matScale = XMMatrixScaling(m_fRadius, m_fRadius, m_fRadius);

		if(m_fRadius > 0.0f)
			XMStoreFloat4x4(&m_pTransform->m_matWorld, matScale * XMLoadFloat4x4(&m_pTransform->m_matWorld));

	}
		
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	return 0;
}

void CSphereMesh::Render(void)
{

	CGraphicDev::GetInstance()->SetWireFrame(TRUE);

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


	CMeshMgr::GetInstance()->Render_MeshMgr(m_uiObjNum, FALSE);

	CGraphicDev::GetInstance()->SetWireFrame(FALSE);

}

void CSphereMesh::Release(void)
{
	CGameObject::Release();
	delete this;
}

HRESULT CSphereMesh::Ready_Component()
{
	CComponent* pComponent = NULL;

	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));

	return S_OK;
}

void CSphereMesh::SetObjNum(_uint uNum)
{
	m_uiObjNum = uNum;
}

void CSphereMesh::SetmatWorld(XMFLOAT4X4 * matWorld)
{
	m_matWorld = matWorld;
}

void CSphereMesh::SetPosition(XMFLOAT3 vPos)
{
	m_pTransform->m_vPos = vPos;
}
