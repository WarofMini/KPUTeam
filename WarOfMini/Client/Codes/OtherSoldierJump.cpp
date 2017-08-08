#include "stdafx.h"
#include "OtherSoldierJump.h"
#include "Input.h"
#include "OtherPlayer.h"
#include "OtherSoldierState.h"

COtherSoldierJump::COtherSoldierJump(COtherPlayer* pSoldier)
	: COtherSoldierState(pSoldier)
{

}

COtherSoldierJump::~COtherSoldierJump()
{

}

int COtherSoldierJump::InState()
{
	COtherSoldierState::InState();
	
	m_bShoot = m_pkey[KEY_LCLICK];

	if (m_pSoldier->IsSoldier())
	{
		if (*m_pSoldier->Get_AniIdx() == PLAYER_JumpIn && m_pSoldier->Check_AnimationFrame())
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_JumpAndShootLoop);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_JumpLoop);
			}
			return 0;
		}
	}
	else
	{
		if (*m_pSoldier->Get_AniIdx() == PLAYER_Iron_JumpandShootIn && m_pSoldier->Check_AnimationFrame())
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_JumpandShootLoop);
			return 0;
		}
	}

	return 1;
}

int COtherSoldierJump::OnState()
{
	COtherSoldierState::OnState();
	m_bShoot = m_pkey[KEY_LCLICK];

	if(m_pSoldier->IsSoldier())
		LoopJump(m_bShoot);
	else
	{
		if (m_pkey[KEY_SPACE] && !m_pSoldier->IsOnGround())
			m_pSoldier->Soldier_Iron_AddVelocity(200.f * m_pSoldier->Get_Time());
	}

	if (EndJump())
		return 0;

	return 1;
}

int COtherSoldierJump::OutState()
{
	COtherSoldierState::OutState();
	if (m_pSoldier->Check_AnimationFrame())
	{
		if (m_pSoldier->IsSoldier())
			m_pSoldier->PlayAnimation(PLAYER_idle);
		else
			m_pSoldier->PlayAnimation(PLAYER_Iron_Idle);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		return 0;
	}

	return 1;
}

void COtherSoldierJump::ShootCheck(void)
{
	
}

void COtherSoldierJump::LoopJump(bool bShoot)
{
	if (bShoot)
	{
		if (*m_pAniIdx == PLAYER_JumpLoop)
		{
			m_pSoldier->PlayAnimation(PLAYER_JumpAndShootLoop);
		}
	}
	else
	{
		if (*m_pAniIdx == PLAYER_JumpAndShootLoop)
		{
			m_pSoldier->PlayAnimation(PLAYER_JumpLoop);
		}
	}
}

bool COtherSoldierJump::EndJump(void)
{
	if (m_pSoldier->IsOnGround())
	{
		if(m_pSoldier->IsSoldier())
			m_pSoldier->PlayAnimation(PLAYER_JumpOut);
		else
			m_pSoldier->PlayAnimation(PLAYER_Iron_JumpandShootOut);
		return true;
	}

	return false;
}

COtherSoldierJump* COtherSoldierJump::Create(COtherPlayer* pSoldier)
{
	return new COtherSoldierJump(pSoldier);
}

void COtherSoldierJump::Release(void)
{
	COtherSoldierState::Release();
}
