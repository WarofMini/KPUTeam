#include "stdafx.h"
#include "DefaultObject.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "ShaderMgr.h"
#include "Management.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "SphereMesh.h"

CDefaultObj::CDefaultObj(ID3D11DeviceContext* pContext)
: CGameObject(pContext)
, m_uiObjNum(0)
, m_fRadius(0.f)
, m_pTransform(NULL)
, m_pSphereMesh(NULL)
{
}

CDefaultObj::~CDefaultObj(void)
{
}

CDefaultObj* CDefaultObj::Create(ID3D11DeviceContext* pContext)
{
	CDefaultObj* pObject = new CDefaultObj(pContext);

	if (FAILED(pObject->Initialize()))
		Safe_Release(pObject);

	return pObject;
}

HRESULT CDefaultObj::Initialize()
{
	if (FAILED(Ready_Component()))
		return E_FAIL;


	return S_OK;
}

_int CDefaultObj::Update(const _float& fTimeDelta)
{

	CGameObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);


	//if (m_pSphereMesh != NULL && g_bCollisionDraw)
	//	m_pSphereMesh->Update(fTimeDelta);
	
	
	return 0;
}

void CDefaultObj::Render(void)
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


	CMeshMgr::GetInstance()->Render_MeshMgr(m_uiObjNum, TRUE);


	//if (m_pSphereMesh != NULL && g_bCollisionDraw)
	//{
	//	m_pSphereMesh->Render();
	//}
}

void CDefaultObj::Release(void)
{
	Safe_Release(m_pSphereMesh);

	CGameObject::Release();
	delete this;
}

HRESULT CDefaultObj::Ready_Component()
{
	CComponent* pComponent = NULL;

	// Transform
	pComponent = CTransform::Create();
	m_pTransform = dynamic_cast<CTransform*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));

	return S_OK;
}

void CDefaultObj::SetObjNum(_uint uNum)
{
	m_uiObjNum = uNum;
}

_uint CDefaultObj::GetObjNum(void)
{
	return m_uiObjNum;
}

void CDefaultObj::ComputeCollider(void)
{
	XMFLOAT3 vMax = *CMeshMgr::GetInstance()->Get_MeshMax(m_uiObjNum);

	vMax = XMFLOAT3(vMax.x * m_pTransform->m_vScale.x,
	vMax.y * m_pTransform->m_vScale.y,
	vMax.z * m_pTransform->m_vScale.z);



	m_fRadius = (vMax.x > vMax.y) ? vMax.x : vMax.y;
	m_fRadius = (vMax.z > m_fRadius) ? vMax.z : m_fRadius;


	m_pSphereMesh = CSphereMesh::Create(m_pContext, m_fRadius , &m_pTransform->m_vPos);
}
