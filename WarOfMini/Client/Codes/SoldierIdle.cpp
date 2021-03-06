#include "stdafx.h"
#include "SoldierIdle.h"
#include "Input.h"
#include "Player.h"
#include "SoldierState.h"
#include "Player.h"

CSoldierIdle::CSoldierIdle(CPlayer* pSoldier)
	: CSoldierState(pSoldier)
{

}

CSoldierIdle::~CSoldierIdle()
{

}

int CSoldierIdle::InState()
{
	CSoldierState::InState();
	if (m_pSoldier->IsAbleReload())
		m_bShoot = false;
	else
		m_bShoot = m_pkey[KEY_LCLICK];

	return 0;
}

int CSoldierIdle::OnState()
{
	CSoldierState::OnState();
	if (m_pSoldier->IsAbleReload())
		m_bShoot = false;
	else
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

	if (m_pkey[KEY_Q]) //����
		m_pSoldier->SoldierChange();

	return 1;
}

int CSoldierIdle::OutState()
{
	CSoldierState::OutState();
	return 0;
}

void CSoldierIdle::ShootCheck(void)
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
				m_pSoldier->Reload();
				m_pSoldier->PlayAnimation(PLAYER_idle);
			}
		}

		if (m_bShoot && *m_pAniIdx != PLAYER_Shoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Shoot);
			m_pSoldier->Set_Fire(true);
		}
		else if (!m_bShoot && *m_pAniIdx == PLAYER_Shoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_idle);
			m_pSoldier->Set_Fire(false);
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
				m_pSoldier->Reload();
				m_pSoldier->PlayAnimation(PLAYER_Iron_Idle);
			}
		}

		if (m_bShoot && *m_pAniIdx != PLAYER_Iron_Shoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_Shoot);
			m_pSoldier->Set_Fire(true);
		}
		else if (!m_bShoot && *m_pAniIdx == PLAYER_Iron_Shoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_Idle);
			m_pSoldier->Set_Fire(false);
		}
	}
}

bool CSoldierIdle::IsSoldierMove(void)
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

bool CSoldierIdle::IsSoldierLying(void)
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

bool CSoldierIdle::IsSoldierJump(void)
{
	if (m_pSoldier->IsOnGround() && m_pkey[KEY_SPACE])
	{
		m_pSoldier->PlayAnimation(PLAYER_JumpIn);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_JUMP;
		return true;
	}

	return false;
}

bool CSoldierIdle::IsSoldierIronMove(void)
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

bool CSoldierIdle::IsSoldierIronLying(void)
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

bool CSoldierIdle::IsSoldierIronJump(void)
{
	if (m_pSoldier->IsOnGround() && m_pkey[KEY_SPACE])
	{
		m_pSoldier->PlayAnimation(PLAYER_Iron_JumpandShootIn);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_JUMP;
		return true;
	}

	return false;
}

CSoldierIdle* CSoldierIdle::Create(CPlayer* pSoldier)
{
	return new CSoldierIdle(pSoldier);
}

void CSoldierIdle::Release(void)
{
	CSoldierState::Release();
}
