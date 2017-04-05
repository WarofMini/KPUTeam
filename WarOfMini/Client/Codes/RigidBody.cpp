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
	//�ӵ� �ʱ�ȭ
	m_vVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//�߷�
	SetUp_Gravity(fTime);

	//��ӵ� �
	//SetUp_AccMotionLine(fTime);

	//�ӵ��� ���� ������ ����
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
	if (!m_bGravity) //�߷�->False ���鿡 ��Ҵ�.
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


	//������
	//�Ÿ� = 1/2 * �߷°��ӵ� * �ð� * �ð�
	m_fGravityDist -= (m_fGravityAcc + m_fRevisionPower) * fTimeDelta * fTimeDelta;

	if (m_fGravityDist <= -2.f)
	{
		m_fGravityDist = -2.f;
	}

	//���� ���� �Ÿ�(����϶��� ���ϱ�)
	if (m_fGravityDist > 0.0f)
		m_fMaxGravityDist += m_fGravityDist;

	//��ġ�ߴ� Y���� 12�̻� Ŀ���� ���̻� ���������� �Ѵ�
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
