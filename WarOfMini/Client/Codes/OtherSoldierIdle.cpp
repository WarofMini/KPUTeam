#include "stdafx.h"
#include "OtherSoldierIdle.h"
#include "Input.h"
#include "OtherPlayer.h"
#include "OtherSoldierState.h"

COtherSoldierIdle::COtherSoldierIdle(COtherPlayer* pSoldier)
	: COtherSoldierState(pSoldier)
{

}

COtherSoldierIdle::~COtherSoldierIdle()
{

}

int COtherSoldierIdle::InState()
{
	COtherSoldierState::InState();
	
	m_bShoot = m_pkey[KEY_LCLICK];

	return 0;
}

int COtherSoldierIdle::OnState()
{
	COtherSoldierState::OnState();

	m_bShoot = m_pkey[KEY_LCLICK];

	ShootCheck();

	if (m_pSoldier->IsSoldier())
	{
		if (IsSoldierMove())
			return 0;

		if (IsSoldierLying())
			return 0;

		if (IsSoldierJump())
			return 0;
	}
	else
	{
		if (IsSoldierIronMove())
			return 0;

		if (IsSoldierIronLying())
			return 0;

		if (IsSoldierIronJump())
			return 0;
	}

	if (m_pkey[KEY_Q]) //º¯½Å
		m_pSoldier->SoldierChange();

	return 1;
}

int COtherSoldierIdle::OutState()
{
	COtherSoldierState::OutState();
	return 0;
}

void COtherSoldierIdle::ShootCheck(void)
{
	if (m_pSoldier->IsSoldier())
	{
		if (*m_pAniIdx != PLAYER_Reload && m_pkey[KEY_R])
		{
			m_pSoldier->PlayAnimation(PLAYER_Reload);
		}
		if (*m_pAniIdx == PLAYER_Reload)
		{
			if (m_pSoldier->Check_AnimationFrame())
			{
				m_pSoldier->PlayAnimation(PLAYER_idle);
			}
		}

		if (m_bShoot && *m_pAniIdx != PLAYER_Shoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Shoot);
		}
		else if (!m_bShoot && *m_pAniIdx == PLAYER_Shoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_idle);
		}
	}
	else
	{
		if (*m_pAniIdx != PLAYER_Iron_ReLoad && m_pkey[KEY_R])
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_ReLoad);
		}
		if (*m_pAniIdx == PLAYER_Iron_ReLoad)
		{
			if (m_pSoldier->Check_AnimationFrame())
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_Idle);
			}
		}

		if (m_bShoot && *m_pAniIdx != PLAYER_Iron_Shoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_Shoot);
		}
		else if (!m_bShoot && *m_pAniIdx == PLAYER_Iron_Shoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_Idle);
		}
	}
}

bool COtherSoldierIdle::IsSoldierMove(void)
{
	switch (*m_pMoveDir)
	{
	case DIR_U:
	case DIR_UL:
	case DIR_UR:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunForwardShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunForward);
		break;
	case DIR_D:
	case DIR_DL:
	case DIR_DR:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_Runback);
		break;
	case DIR_L:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunLeft);
		break;
	case DIR_R:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunRightShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunRight);
		break;
	default:
		return false;
		break;
	}

	*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
	return true;
}

bool COtherSoldierIdle::IsSoldierLying(void)
{
	if (m_pkey[KEY_CTRL])
	{
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_LyingShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_Lying);

		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_LYING;
		return true;
	}
	return false;
}

bool COtherSoldierIdle::IsSoldierJump(void)
{
	if (m_pSoldier->IsOnGround() && m_pkey[KEY_SPACE])
	{
		m_pSoldier->PlayAnimation(PLAYER_JumpIn);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_JUMP;
		return true;
	}

	return false;
}

bool COtherSoldierIdle::IsSoldierIronMove(void)
{
	switch (*m_pMoveDir)
	{
	case DIR_U:
	case DIR_UL:
	case DIR_UR:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_Iron_RunForwardShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_Iron_RunForward);
		break;
	case DIR_D:
	case DIR_DL:
	case DIR_DR:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_Iron_RunBackShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_Iron_RunBack);
		break;
	case DIR_L:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_Iron_RunLeftShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_Iron_RunLeft);
		break;
	case DIR_R:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_Iron_RunRightShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_Iron_RunRight);
		break;
	default:
		return false;
		break;
	}

	*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
	return true;
}

bool COtherSoldierIdle::IsSoldierIronLying(void)
{
	if (m_pkey[KEY_CTRL])
	{
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_Iron_LyingShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_Iron_Lying);

		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_LYING;
		return true;
	}
	return false;
}

bool COtherSoldierIdle::IsSoldierIronJump(void)
{
	if (m_pSoldier->IsOnGround() && m_pkey[KEY_SPACE])
	{
		m_pSoldier->PlayAnimation(PLAYER_Iron_JumpandShootIn);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_JUMP;
		return true;
	}

	return false;
}

COtherSoldierIdle* COtherSoldierIdle::Create(COtherPlayer* pSoldier)
{
	return new COtherSoldierIdle(pSoldier);
}

void COtherSoldierIdle::Release(void)
{
	COtherSoldierState::Release();
}
