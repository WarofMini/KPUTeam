#include "stdafx.h"
#include "StaticObject.h"
#include "Transform.h"
#include "Management.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "MeshMgr.h"

CStaticObject::CStaticObject(ID3D11DeviceContext * pContext)
: CGameObject(pContext)
, m_uiObjNum(0)
, m_pTransform(NULL)
{
}

CStaticObject::~CStaticObject(void)
{
}

CStaticObject * CStaticObject::Create(ID3D11DeviceContext * pContext)
{
	CStaticObject* pObject = new CStaticObject(pContext);

	if (FAILED(pObject->Initialize()))
		Safe_Release(pObject);

	return pObject;
}

HRESULT CStaticObject::Initialize(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_uiObjNum = 1;

	m_pTransform->m_vPos = _vec3(0.f, 0.f, 0.f);
	m_pTransform->m_vScale = _vec3(100.f, 100.f, 100.f);
	m_pTransform->m_vAngle = _vec3(0.f, 0.f, 0.f);

	return S_OK;
}

_int CStaticObject::Update(const _float & fTimeDelta)
{
	CGameObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderInstGroup(CRenderer::RENDER_INST, m_uiObjNum, &m_pTransform->m_matWorld);

	return 0;
}

void CStaticObject::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Default"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADERCB tBaseShaderCB;

	_matrix  matView, matProj;
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matProj);

	tBaseShaderCB.matWorld = m_pTransform->m_matWorld;
	tBaseShaderCB.matView = (*CCameraMgr::GetInstance()->Get_CurCameraView());
	tBaseShaderCB.matProj = (*CCameraMgr::GetInstance()->Get_CurCameraProj());



	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Default"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Default"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	CMeshMgr::GetInstance()->Render_MeshMgr(m_uiObjNum, FALSE);
}

void CStaticObject::Release(void)
{
	CGameObject::Release();
	delete this;
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
