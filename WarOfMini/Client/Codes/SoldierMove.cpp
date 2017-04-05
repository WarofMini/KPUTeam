#include "stdafx.h"
#include "SoldierMove.h"
#include "Input.h"

CSoldierMove::CSoldierMove(CPlayer* pSoldier)
	: CSoldierState(pSoldier)
	, m_pSoldier(pSoldier)
{

}

CSoldierMove::~CSoldierMove()
{

}

int CSoldierMove::InState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);
	return 0;
}

int CSoldierMove::OnState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);

	ShootCheck();

	if (MoveKeyCheck())
		return 0;
	
	if (RollKeyCheck())
		return 0;

	if (LyingKeyCheck())
		return 0;

	if (IsSoldierJump())
		return 0;

	return 1;
}

int CSoldierMove::OutState()
{
	return 0;
}

bool CSoldierMove::MoveKeyCheck(void)
{
	switch (*m_pMoveDir)
	{
	case DIR_U:
	case DIR_UL:
	case DIR_UR:
		if (*m_pAniIdx != PLAYER_RunForward && *m_pAniIdx != PLAYER_RunForwardShoot)
		{
			if (m_bShoot)
				m_pSoldier->PlayAnimation(PLAYER_RunForwardShoot);
			else
				m_pSoldier->PlayAnimation(PLAYER_RunForward);
		}
		break;
	case DIR_D:
	case DIR_DL:
	case DIR_DR:
		if (*m_pAniIdx != PLAYER_Runback && *m_pAniIdx != PLAYER_RunBackShoot)
		{
			if (m_bShoot)
				m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
			else
				m_pSoldier->PlayAnimation(PLAYER_Runback);
		}
		break;
	case DIR_L:
		if (*m_pAniIdx != PLAYER_RunLeft && *m_pAniIdx != PLAYER_RunLeftShoot)
		{
			if (m_bShoot)
				m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
			else
				m_pSoldier->PlayAnimation(PLAYER_RunLeft);
		}
		break;
	case DIR_R:
		if (*m_pAniIdx != PLAYER_RunRight && *m_pAniIdx != PLAYER_RunRightShoot)
		{
			if (m_bShoot)
				m_pSoldier->PlayAnimation(PLAYER_RunRightShoot);
			else
				m_pSoldier->PlayAnimation(PLAYER_RunRight);
		}
		break;
	default:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_Shoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_idle);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		return true;
		break;
	}

	return false;
}

bool CSoldierMove::RollKeyCheck(void)
{
	if (m_pInput->Get_DIMouseState(CInput::DIM_RB))
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
		}
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_ROLL;
		return true;
	}

	return false;
}

bool CSoldierMove::LyingKeyCheck(void)
{
	return false;
}

bool CSoldierMove::IsSoldierJump(void)
{
	if (m_pSoldier->IsOnGround() && m_pInput->Get_DIKeyState(DIK_SPACE))
	{
		m_pSoldier->PlayAnimation(PLAYER_JumpIn);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_JUMP;
		return true;
	}

	return false;
}

void CSoldierMove::ShootCheck(void)
{
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

CSoldierMove* CSoldierMove::Create(CPlayer* pSoldier)
{
	return new CSoldierMove(pSoldier);
}

void CSoldierMove::Release(void)
{
	CSoldierState::Release();
}
