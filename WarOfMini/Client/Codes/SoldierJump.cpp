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
	if (m_pSoldier->IsAbleReload())
		m_bShoot = false;
	else
		m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);

	if (m_pSoldier->IsSoldier())
	{
		if (*m_pSoldier->Get_AniIdx() == PLAYER_JumpIn && m_pSoldier->Check_AnimationFrame())
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_JumpAndShootLoop);
				m_pSoldier->Set_Fire(true);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_JumpLoop);
				m_pSoldier->Set_Fire(false);
			}
			return 0;
		}
	}
	else
	{
		if (*m_pSoldier->Get_AniIdx() == PLAYER_Iron_JumpandShootIn && m_pSoldier->Check_AnimationFrame())
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_JumpandShootLoop);
			if (m_bShoot)	m_pSoldier->Set_Fire(true);
			else			m_pSoldier->Set_Fire(false);
			return 0;
		}
	}

	return 1;
}

int CSoldierJump::OnState()
{
	if (m_pSoldier->IsAbleReload())
		m_bShoot = false;
	else
		m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);

	if(m_pSoldier->IsSoldier())
		LoopJump(m_bShoot);
	else
	{
		if (m_pInput->Get_DIKeyState(DIK_SPACE) && !m_pSoldier->IsOnGround())
			m_pSoldier->Soldier_Iron_AddVelocity(200.f * m_pSoldier->Get_Time());
		if (m_bShoot)	m_pSoldier->Set_Fire(true);
		else			m_pSoldier->Set_Fire(false);
	}

	if (EndJump())
		return 0;

	return 1;
}

int CSoldierJump::OutState()
{
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

void CSoldierJump::ShootCheck(void)
{
	
}

void CSoldierJump::LoopJump(bool bShoot)
{
	if (bShoot)
	{
		if (*m_pAniIdx == PLAYER_JumpLoop)
		{
			m_pSoldier->PlayAnimation(PLAYER_JumpAndShootLoop);
			m_pSoldier->Set_Fire(true);
		}
	}
	else
	{
		if (*m_pAniIdx == PLAYER_JumpAndShootLoop)
		{
			m_pSoldier->PlayAnimation(PLAYER_JumpLoop);
			m_pSoldier->Set_Fire(false);
		}
	}
}

bool CSoldierJump::EndJump(void)
{
	if (m_pSoldier->IsOnGround())
	{
		m_pSoldier->Set_Fire(false);
		if(m_pSoldier->IsSoldier())
			m_pSoldier->PlayAnimation(PLAYER_JumpOut);
		else
			m_pSoldier->PlayAnimation(PLAYER_Iron_JumpandShootOut);
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
