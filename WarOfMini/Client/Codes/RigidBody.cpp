#include "stdafx.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Calculator.h"


CRigidBody::CRigidBody(void)
	: m_pTransform(NULL)
	, m_vVelocity(0.0f, 0.0f, 0.0f)
	, m_vAccMotionVelocity(.0f, 0.0f, 0.0f)
	, m_fMass(1.f)
	, m_fGravityAcc(0.f)
	, m_fGravityDist(0.f)
	, m_fMaxGravityDist(0.f)
	, m_fRevisionPower(0.f)
	, m_bGravity(false)
{

}

CRigidBody::~CRigidBody(void)
{

}

_bool CRigidBody::Get_Gravity(void)
{
	return m_bGravity;
}

_float CRigidBody::Get_AccDist(void)
{
	return m_fMaxGravityDist;
}

void CRigidBody::Set_Gravity(_bool bGravity)
{
	m_bGravity = bGravity;
}

void CRigidBody::Set_RevisionGravity(const _float& fRevisionPower)
{
	m_fRevisionPower = fRevisionPower;
}

void CRigidBody::Set_Jump(const _float& fForce)
{
	m_fGravityDist = fForce;
}

void CRigidBody::Set_AccMotion(const XMFLOAT3* pDir, const _float& fPower)
{
	m_vAccMotionVelocity = *pDir;
	m_vAccMotionVelocity.y = 0.f;

	XMVECTOR vVelocity;
	vVelocity = XMLoadFloat3(&m_vAccMotionVelocity);


	vVelocity = XMVector3Normalize(vVelocity);

	vVelocity = vVelocity * fPower;


	XMStoreFloat3(&m_vAccMotionVelocity, vVelocity);

}

_int CRigidBody::Update(const _float& fTime)
{
	//속도 초기화
	m_vVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//중력
	SetUp_Gravity(fTime);

	//등속도 운동
	//SetUp_AccMotionLine(fTime);

	//속도에 따른 포지션 변경
	m_pTransform->Set_AccMotion(&XMLoadFloat3(&m_vVelocity));

	return 0;
}

void CRigidBody::Release(void)
{
	CComponent::Release();

	delete this;
}

HRESULT CRigidBody::Ready_RigidBody(CTransform* pTransform, CCalculator* pCalculator)
{
	if (pTransform == NULL)
		return E_FAIL;

	if (pCalculator == NULL)
		return E_FAIL;


	m_pTransform = pTransform;
	m_pCalculator = pCalculator;

	m_fGravityAcc = 20.f;

	return S_OK;
}

void CRigidBody::SetUp_Gravity(const _float& fTimeDelta)
{
	if (!m_bGravity) //중력->False 지면에 닿았다.
	{
		m_fMaxGravityDist = 0.0f;
		m_fRevisionPower = 0.0f;

		if (m_fGravityDist < 0.0f)
			m_fGravityDist = 0.0f;

		return ;
	}


	_float fRimitY = -20.0f;

	if (m_pTransform->m_vPos.y < fRimitY)
	{
		m_pTransform->m_vPos.y = 50.f;
		m_fMaxGravityDist = 0.f;
		m_fRevisionPower = 0.f;
		m_fGravityDist = 0.f;
	}


	//점프중
	//거리 = 1/2 * 중력가속도 * 시간 * 시간
	m_fGravityDist -= (m_fGravityAcc + m_fRevisionPower) * fTimeDelta * fTimeDelta;

	if (m_fGravityDist <= -2.f)
	{
		m_fGravityDist = -2.f;
	}

	//점프 누적 거리(양수일때만 더하기)
	if (m_fGravityDist > 0.0f)
		m_fMaxGravityDist += m_fGravityDist;

	//위치했던 Y값이 12이상 커지면 더이상 못떠오르게 한다
	if (m_fMaxGravityDist > 12.f)
	{
		m_fGravityDist = 0.f;
		m_fMaxGravityDist = 0.f;
	}
	m_vVelocity.y = m_fGravityDist;

}

void CRigidBody::SetUp_AccMotionLine(const _float& fTimeDelta)
{

}

CRigidBody* CRigidBody::Create(CTransform* pTransform, CCalculator* pCalculator)
{
	CRigidBody* pInstance = new CRigidBody;

	if (FAILED(pInstance->Ready_RigidBody(pTransform, pCalculator)))
	{
		MSG_BOX(L"RigidBody Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}
