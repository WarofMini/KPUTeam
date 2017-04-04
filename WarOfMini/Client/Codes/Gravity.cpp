#include "stdafx.h"
#include "Gravity.h"

CGravity::CGravity(const float& fFallAcc)
	: CComponent()
	, m_fFallAccelerate(fFallAcc)
	, m_fFallVelocity(0.f)
	, m_bOnGround(true)
{

}

CGravity::~CGravity()
{

}

void CGravity::Move_Inertia(const float& fTimeDelta, XMFLOAT3* pPos)
{
	if (!m_bOnGround)
	{
		m_fFallVelocity -= m_fFallAccelerate * fTimeDelta;
		(*pPos).y += m_fFallVelocity * fTimeDelta;
	}
}

void CGravity::Set_Velocity(const float& fFallVel)
{
	m_fFallVelocity = fFallVel;
}

void CGravity::Add_Velocity(const float& fFallVel)
{
	m_fFallVelocity += fFallVel;
}

void CGravity::Set_OnGround(bool bGround)
{
	m_bOnGround = bGround;
}

void CGravity::Set_LandOn()
{
	m_bOnGround = true;
	m_fFallVelocity = 0.f;
}

void CGravity::Set_LandOff(const float& fFallVel)
{
	m_bOnGround = false;
	m_fFallVelocity = fFallVel;
}

CGravity* CGravity::Create(const float& fFallAcc)
{
	return new CGravity(fFallAcc);
}

void CGravity::Release()
{
	CComponent::Release();

	delete this;
}

