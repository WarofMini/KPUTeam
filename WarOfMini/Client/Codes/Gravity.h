#ifndef Gravity_h__
#define Gravity_h__

#include "Component.h"

class CGravity : public CComponent
{
private:
	explicit	CGravity(const float& fFallAcc);
	virtual		~CGravity();

public:
	void		Move_Inertia(const float& fTimeDelta, XMFLOAT3* pPos);
	void		Set_Velocity(const float& fFallVel);
	void		Add_Velocity(const float& fFallVel);
	void		Set_OnGround(bool bGround);
	void		Set_LandOn();
	void		Set_LandOff(const float& fFallVel);

	void		Set_GroundDist(float& fDist);
	void		Set_BeforePos(XMVECTOR pPos);

private:
	float		m_fFallAccelerate; //����ȭ�ϴ�
	float		m_fFallVelocity; //�������� �ӵ�
	bool		m_bOnGround;


	_float		m_fGroundDist; //�޽ÿ��� �Ÿ�
	XMFLOAT3	m_vBeforePos; //��ü�� �� ��ġ

public:	//Getter
	const bool&		Get_OnGround() { return m_bOnGround; }
	_float			Get_GroundDist() { return m_fGroundDist; }
	XMVECTOR		Get_BeforePos(void);
	_float			Get_Velocity(void);

public:
	static	CGravity*		Create(const float& fFallAcc);

private:
	virtual	void	Release();
};

#endif // Gravity_h__
