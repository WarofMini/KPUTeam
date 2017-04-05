#include "stdafx.h"
#include "SoldierJump.h"
#include "Input.h"

CSoldierJump::CSoldierJump(CPlayer* pSoldier)
	: CSoldierState(pSoldier)
	, m_pSoldier(pSoldier)
{

}

CSoldierJump::~CSoldierJump()
{

}

int CSoldierJump::InState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);

	if (*m_pSoldier->Get_AniIdx() == PLAYER_JumpIn && m_pSoldier->Check_AnimationFrame())
	{
		m_pSoldier->PlayAnimation(PLAYER_JumpLoop);
		return 0;
	}

	return 1;
}

int CSoldierJump::OnState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);

	if (EndJump())
		return 0;

	return 1;
}

int CSoldierJump::OutState()
{
	if (m_pSoldier->Check_AnimationFrame())
	{
		m_pSoldier->PlayAnimation(PLAYER_idle);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		return 0;
	}

	return 1;
}

void CSoldierJump::ShootCheck(void)
{
	
}

bool CSoldierJump::MoveKeyCheck(void)
{
	return true;
}

bool CSoldierJump::EndJump(void)
{
	if (m_pSoldier->IsOnGround())
	{
		m_pSoldier->PlayAnimation(PLAYER_JumpOut);
		return true;
	}

	return false;
}

CSoldierJump* CSoldierJump::Create(CPlayer* pSoldier)
{
	return new CSoldierJump(pSoldier);
}

void CSoldierJump::Release(void)
{
	CSoldierState::Release();
}
