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
	XMFLOAT3		m_vVelocity; //실제 연산 속도
	XMFLOAT3		m_vAccMotionVelocity; //등속도 운동 변수
	_float			m_fMass; //질량
	_float			m_fGravityAcc; //중력 가속도
	_float			m_fGravityDist; //1프레임당 이동할 거리(중력)
	_float			m_fMaxGravityDist;//중력 누적 거리(제한용도)
	_float			m_fRevisionPower;//중력 보정 변수
	_bool			m_bGravity;	//중력을 받고 있는지 아닌지?

private:
	HRESULT  Ready_RigidBody(CTransform* pTransform, CCalculator* pCalculator);
	void     SetUp_Gravity(const _float& fTimeDelta);
	void	 SetUp_AccMotionLine(const _float& fTimeDelta); //등속도 운동

public:
	static CRigidBody* Create(CTransform* pTransform, CCalculator* pCalculator);
};


#endif