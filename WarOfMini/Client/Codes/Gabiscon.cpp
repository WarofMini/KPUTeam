#include "stdafx.h"
#include "Gabiscon.h"
#include "Management.h"
#include "Renderer.h"
#include "Transform.h"
#include "StateMachine.h"

CGabiscon::CGabiscon(ID3D11DeviceContext* pContext)
	: CDynamicObject(pContext)
	, m_dwState(GABISCON_IDLE)
	, m_dwAniIdx(GABISCON_Idle_Rifle_Hip_Break1)
	, m_pComStateMachine(NULL)
{
	m_vLook = XMFLOAT3(0.f, 1.f, 0.f);
}

CGabiscon::~CGabiscon(void)
{

}

CGabiscon* CGabiscon::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CGabiscon* pObject = new CGabiscon(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	return pObject;
}

HRESULT CGabiscon::Initialize(ID3D11Device* pGraphicDev)
{
	m_uiObjNum = MESHNUM_GABISCON;

	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	FAILED_CHECK(Prepare_StateMachine());

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_pTransform->m_vAngle.x = 90.f;

	return S_OK;
}

INT CGabiscon::Update(const FLOAT& fTimeDelta)
{
	CDynamicObject::Update(fTimeDelta);

	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	return 0;
}

void CGabiscon::Release(void)
{
	CDynamicObject::Release();
}

HRESULT CGabiscon::Prepare_StateMachine(void)
{
	NULL_CHECK_RETURN(m_pComStateMachine, E_FAIL);
	CState*	pState = NULL;

	return S_OK;
}

void CGabiscon::Operate_StateMAchine(const FLOAT& fTimeDelta)
{

}

void CGabiscon::UpdateDir(void)
{
	XMVECTOR vDir;

	vDir = XMLoadFloat3(&m_pTransform->m_vDir);

	vDir = XMVector3TransformNormal(XMLoadFloat3(&m_vLook), XMLoadFloat4x4(&m_pTransform->m_matWorld));

	vDir = XMVector3Normalize(vDir);

	XMStoreFloat3(&m_pTransform->m_vDir, vDir);
}

HRESULT CGabiscon::Ready_Component(ID3D11Device* pGraphicDev)
{
	if (FAILED(CDynamicObject::Ready_Component()))
		return E_FAIL;

	CComponent* pComponent = NULL;

	//StateMachine
	pComponent = m_pComStateMachine = CStateMachine::Create(GABISCONEND);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"StateMachine", pComponent));

	return S_OK;
}

