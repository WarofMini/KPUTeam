#include "stdafx.h"
#include "OtherSoldierMove.h"
#include "Input.h"

COtherSoldierMove::COtherSoldierMove(COtherPlayer* pSoldier)
	: COtherSoldierState(pSoldier)
{

}

COtherSoldierMove::~COtherSoldierMove()
{

}

int COtherSoldierMove::InState()
{
	COtherSoldierState::InState();
	m_bShoot = m_pkey[KEY_LCLICK];
	return 0;
}

int COtherSoldierMove::OnState()
{
	COtherSoldierState::OnState();
	m_bShoot = m_pkey[KEY_LCLICK];

	ShootCheck();

	if (m_pSoldier->IsSoldier())
	{
		if (MoveKeyCheck())
			return 0;
	}
	else
	{
		if (MoveKeyCheck_Iron())
			return 0;
	}
		
	if (RollKeyCheck())
		return 0;

	if (LyingKeyCheck())
		return 0;

	if (IsSoldierJump())
		return 0;

	return 1;
}

int COtherSoldierMove::OutState()
{
	COtherSoldierState::OutState();
	return 0;
}

bool COtherSoldierMove::MoveKeyCheck(void)
{
	switch (*m_pMoveDir)
	{
	case DIR_U:
	case DIR_UL:
	case DIR_UR:
		if (m_pkey[KEY_SHIFT])
		{
			if (*m_pAniIdx != PLAYER_sprint)
			{
				m_pSoldier->PlayAnimation(PLAYER_sprint);
			}
		}
		else
		{
			if (*m_pAniIdx != PLAYER_RunForward && *m_pAniIdx != PLAYER_RunForwardShoot)
			{
				if (m_bShoot)
				{
					m_pSoldier->PlayAnimation(PLAYER_RunForwardShoot);
				}
				else
				{
					m_pSoldier->PlayAnimation(PLAYER_RunForward);
				}
			}
		}
		break;
	case DIR_D:
	case DIR_DL:
	case DIR_DR:
		if (*m_pAniIdx != PLAYER_Runback && *m_pAniIdx != PLAYER_RunBackShoot)
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Runback);
			}
		}
		break;
	case DIR_L:
		if (*m_pAniIdx != PLAYER_RunLeft && *m_pAniIdx != PLAYER_RunLeftShoot)
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_RunLeft);
			}
		}
		break;
	case DIR_R:
		if (*m_pAniIdx != PLAYER_RunRight && *m_pAniIdx != PLAYER_RunRightShoot)
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_RunRightShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_RunRight);
			}
		}
		break;
	default:
		if (m_bShoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Shoot);
		}
		else
		{
			m_pSoldier->PlayAnimation(PLAYER_idle);
		}
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		return true;
		break;
	}

	return false;
}

bool COtherSoldierMove::MoveKeyCheck_Iron(void)
{
	switch (*m_pMoveDir)
	{
	case DIR_U:
	case DIR_UL:
	case DIR_UR:
		if (m_pkey[KEY_SHIFT])
		{
			if (*m_pAniIdx != PLAYER_Iron_Sprint)
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_Sprint);
			}
		}
		else
		{
			if (*m_pAniIdx != PLAYER_Iron_RunForward && *m_pAniIdx != PLAYER_Iron_RunForwardShoot)
			{
				if (m_bShoot)
				{
					m_pSoldier->PlayAnimation(PLAYER_Iron_RunForwardShoot);
				}
				else
				{
					m_pSoldier->PlayAnimation(PLAYER_Iron_RunForward);
				}
			}
		}
		break;
	case DIR_D:
	case DIR_DL:
	case DIR_DR:
		if (*m_pAniIdx != PLAYER_Iron_RunBack && *m_pAniIdx != PLAYER_Iron_RunBackShoot)
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunBackShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunBack);
			}
		}
		break;
	case DIR_L:
		if (*m_pAniIdx != PLAYER_Iron_RunLeft && *m_pAniIdx != PLAYER_Iron_RunLeftShoot)
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunLeftShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunLeft);
			}
		}
		break;
	case DIR_R:
		if (*m_pAniIdx != PLAYER_Iron_RunRight && *m_pAniIdx != PLAYER_Iron_RunRightShoot)
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunRightShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunRight);
			}
		}
		break;
	default:
		if (m_bShoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_Shoot);
		}
		else
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_Idle);
		}
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		return true;
		break;
	}

	return false;
}

bool COtherSoldierMove::RollKeyCheck(void)
{
	if (m_pkey[KEY_RCLICK])
	{
		if (m_pSoldier->IsSoldier())
		{
			switch (*m_pAniIdx)
			{
			case PLAYER_RunForward:
			case PLAYER_RunForwardShoot:
				m_pSoldier->PlayAnimation(PLAYER_RollFront);
				break;
			case PLAYER_Runback:
			case PLAYER_RunBackShoot:
				m_pSoldier->PlayAnimation(PLAYER_RollBack);
				break;
			case PLAYER_RunRight:
			case PLAYER_RunRightShoot:
				m_pSoldier->PlayAnimation(PLAYER_RollRight);
				break;
			case PLAYER_RunLeft:
			case PLAYER_RunLeftShoot:
				m_pSoldier->PlayAnimation(PLAYER_RollLeft);
				break;
			default:
				return false;
				break;
			}
		}
		else
		{
			switch (*m_pAniIdx)
			{
			case PLAYER_Iron_RunForward:
			case PLAYER_Iron_RunForwardShoot:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RollFront);
				break;
			case PLAYER_Iron_RunBack:
			case PLAYER_Iron_RunBackShoot:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RollBack);
				break;
			case PLAYER_Iron_RunRight:
			case PLAYER_Iron_RunRightShoot:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RollRight);
				break;
			case PLAYER_Iron_RunLeft:
			case PLAYER_Iron_RunLeftShoot:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RollLeft);
				break;
			default:
				return false;
				break;
			}
		}
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_ROLL;
		return true;
	}

	return false;
}

bool COtherSoldierMove::LyingKeyCheck(void)
{
	return false;
}

bool COtherSoldierMove::IsSoldierJump(void)
{
	if (m_pSoldier->IsOnGround() && m_pkey[KEY_SPACE])
	{
		if (m_pSoldier->IsSoldier())
			m_pSoldier->PlayAnimation(PLAYER_JumpIn);
		else
			m_pSoldier->PlayAnimation(PLAYER_Iron_JumpandShootIn);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_JUMP;
		return true;
	}

	return false;
}

void COtherSoldierMove::ShootCheck(void)
{
	if (m_pSoldier->IsSoldier())
	{
		if (*m_pAniIdx == PLAYER_sprint)
			return;
		if (m_bShoot)
		{
			switch (*m_pAniIdx)
			{
			case PLAYER_RunForward:
				m_pSoldier->PlayAnimation(PLAYER_RunForwardShoot);
				break;
			case PLAYER_Runback:
				m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
				break;
			case PLAYER_RunLeft:
				m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
				break;
			case PLAYER_RunRight:
				m_pSoldier->PlayAnimation(PLAYER_RunRightShoot);
				break;
			}
		}
		else
		{
			switch (*m_pAniIdx)
			{
			case PLAYER_RunForwardShoot:
				m_pSoldier->PlayAnimation(PLAYER_RunForward);
				break;
			case PLAYER_RunBackShoot:
				m_pSoldier->PlayAnimation(PLAYER_Runback);
				break;
			case PLAYER_RunLeftShoot:
				m_pSoldier->PlayAnimation(PLAYER_RunLeft);
				break;
			case PLAYER_RunRightShoot:
				m_pSoldier->PlayAnimation(PLAYER_RunRight);
				break;
			}
		}
	}
	else
	{
		if (*m_pAniIdx == PLAYER_Iron_Sprint)
			return;
		if (m_bShoot)
		{
			switch (*m_pAniIdx)
			{
			case PLAYER_Iron_RunForward:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunForwardShoot);
				break;
			case PLAYER_Iron_RunBack:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunBackShoot);
				break;
			case PLAYER_Iron_RunLeft:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunLeftShoot);
				break;
			case PLAYER_Iron_RunRight:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunRightShoot);
				break;
			}
		}
		else
		{
			switch (*m_pAniIdx)
			{
			case PLAYER_Iron_RunForwardShoot:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunForward);
				break;
			case PLAYER_Iron_RunBackShoot:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunBack);
				break;
			case PLAYER_Iron_RunLeftShoot:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunLeft);
				break;
			case PLAYER_Iron_RunRightShoot:
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunRight);
				break;
			}
		}
	}
	
}

COtherSoldierMove* COtherSoldierMove::Create(COtherPlayer* pSoldier)
{
	return new COtherSoldierMove(pSoldier);
}

void COtherSoldierMove::Release(void)
{
	COtherSoldierState::Release();
}
