#include "stdafx.h"
#include "SoldierRoll.h"
#include "Input.h"
#include "Player.h"
#include "SoldierState.h"

CSoldierRoll::CSoldierRoll(CPlayer* pSoldier)
	: CSoldierState(pSoldier)
	, m_pSoldier(pSoldier)
{

}

CSoldierRoll::~CSoldierRoll()
{

}

int CSoldierRoll::InState()
{

	return 0;
}

int CSoldierRoll::OnState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);

	if (m_pSoldier->Check_AnimationFrame())
	{
		EndRoll();
		return 0;
	}

	return 1;
}

int CSoldierRoll::OutState()
{

	return 0;
}

void CSoldierRoll::EndRoll(void)
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
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
		break;
	case DIR_D:
	case DIR_DL:
	case DIR_DR:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_Runback);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
		break;
	case DIR_L:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunLeft);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
		break;
	case DIR_R:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunRightShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunRight);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
		break;
	default:
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_Shoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_idle);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		break;
	}
}

CSoldierRoll* CSoldierRoll::Create(CPlayer* pSoldier)
{
	return new CSoldierRoll(pSoldier);
}

void CSoldierRoll::Release(void)
{
	CSoldierState::Release();
}

