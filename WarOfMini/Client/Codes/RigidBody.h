#ifndef RigidBody_h__
#define RigidBody_h__

#include "Component.h"
#include  "Include.h"

class CTransform;
class CCalculator;

class CRigidBody
	:public CComponent
{
private:
	explicit CRigidBody(void);
	virtual ~CRigidBody(void);

public:
	_bool	Get_Gravity(void);
	_float	Get_AccDist(void);

public:
	void Set_Gravity(_bool bGravity);
	void Set_RevisionGravity(const _float& fRevisionPower);
	void Set_Jump(const _float& fForce);
	void Set_AccMotion(const XMFLOAT3* pDir, const _float& fPower);

public:
	virtual _int Update(const _float& fTime);
	virtual void Release(void);

private:
	CTransform*		m_pTransform;
	CCalculator*	m_pCalculator;
private:
	XMFLOAT3		m_vVelocity; //���� ���� �ӵ�
	XMFLOAT3		m_vAccMotionVelocity; //��ӵ� � ����
	_float			m_fMass; //����
	_float			m_fGravityAcc; //�߷� ���ӵ�
	_float			m_fGravityDist; //1�����Ӵ� �̵��� �Ÿ�(�߷�)
	_float			m_fMaxGravityDist;//�߷� ���� �Ÿ�(���ѿ뵵)
	_float			m_fRevisionPower;//�߷� ���� ����
	_bool			m_bGravity;	//�߷��� �ް� �ִ��� �ƴ���?

private:
	HRESULT  Ready_RigidBody(CTransform* pTransform, CCalculator* pCalculator);
	void     SetUp_Gravity(const _float& fTimeDelta);
	void	 SetUp_AccMotionLine(const _float& fTimeDelta); //��ӵ� �

public:
	static CRigidBody* Create(CTransform* pTransform, CCalculator* pCalculator);
};


#endif