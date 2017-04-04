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

private:
	float		m_fFallAccelerate;
	float		m_fFallVelocity;
	bool		m_bOnGround;

public:	//Getter
	const bool&		Get_OnGround() { return m_bOnGround; }

public:
	static	CGravity*		Create(const float& fFallAcc);

private:
	virtual	void	Release();
};

#endif // Gravity_h__
