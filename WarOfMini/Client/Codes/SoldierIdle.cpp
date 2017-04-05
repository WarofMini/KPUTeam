#include "stdafx.h"
#include "SoldierIdle.h"
#include "Input.h"
#include "Player.h"
#include "SoldierState.h"
CSoldierIdle::CSoldierIdle(CPlayer* pSoldier)
	: CSoldierState(pSoldier)
	, m_pSoldier(pSoldier)
{

}

CSoldierIdle::~CSoldierIdle()
{

}

int CSoldierIdle::InState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);

	return 0;
}

int CSoldierIdle::OnState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);

	ShootCheck();

	if (IsSoldierMove())
		return 0;

	if (IsSoldierLying())
		return 0;

	if (IsSoldierJump())
		return 0;

	return 1;
}

int CSoldierIdle::OutState()
{
	return 0;
}

void CSoldierIdle::ShootCheck(void)
{
	if (*m_pAniIdx != PLAYER_Reload && m_pInput->GetDIKeyStateOnce(DIK_R))
	{
		m_pSoldier->PlayAnimation(PLAYER_Reload);
	}
	if (*m_pAniIdx == PLAYER_Reload)
	{
		if(m_pSoldier->Check_AnimationFrame())
			m_pSoldier->PlayAnimation(PLAYER_idle);
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
	if (m_pInput->Get_DIKeyState(DIK_LCONTROL))
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
	if (m_pSoldier->IsOnGround() && m_pInput->Get_DIKeyState(DIK_SPACE))
	{
		m_pSoldier->PlayAnimation(PLAYER_JumpIn);
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
