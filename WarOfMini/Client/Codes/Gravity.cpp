#include "stdafx.h"
#include "Gravity.h"

CGravity::CGravity(const float& fFallAcc)
: CComponent()
, m_fFallAccelerate(fFallAcc)
, m_fFallVelocity(0.f)
, m_bOnGround(true)
, m_fGroundDist(0.0f)
, m_vBeforePos(0.0f, 0.0f, 0.0f)
{

}

CGravity::~CGravity()
{

}

//����
void CGravity::Move_Inertia(const float& fTimeDelta, XMFLOAT3* pPos)
{
	if (!m_bOnGround) //������ �ƴϸ�
	{
		m_fFallVelocity -= m_fFallAccelerate * fTimeDelta;
		(*pPos).y += m_fFallVelocity * fTimeDelta; //Y�� �϶�
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
	m_bOnGround = true; //����
	m_fFallVelocity = 0.f;
}

void CGravity::Set_LandOff(const float& fFallVel)
{
	m_bOnGround = false; //���� ����������
	m_fFallVelocity = fFallVel;
}

void CGravity::Set_GroundDist(float& fDist)
{
	m_fGroundDist = fDist;
}

void CGravity::Set_BeforePos(XMVECTOR pPos)
{
	XMStoreFloat3(&m_vBeforePos, pPos);
}

_float CGravity::Get_Velocity(void)
{
	return m_fFallVelocity;
}

XMVECTOR CGravity::Get_BeforePos(void)
{
	return XMLoadFloat3(&m_vBeforePos);
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

