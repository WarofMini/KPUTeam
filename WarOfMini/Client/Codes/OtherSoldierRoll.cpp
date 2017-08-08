#include "stdafx.h"
#include "OtherSoldierRoll.h"
#include "Input.h"
#include "OtherPlayer.h"
#include "OtherSoldierState.h"

COtherSoldierRoll::COtherSoldierRoll(COtherPlayer* pSoldier)
	: COtherSoldierState(pSoldier)
{

}

COtherSoldierRoll::~COtherSoldierRoll()
{

}

int COtherSoldierRoll::InState()
{
	COtherSoldierState::InState();
	return 0;
}

int COtherSoldierRoll::OnState()
{
	COtherSoldierState::OnState();
	m_bShoot = m_pkey[KEY_LCLICK];

	if (m_pSoldier->Check_AnimationFrame())
	{
		EndRoll();
		return 0;
	}

	return 1;
}

int COtherSoldierRoll::OutState()
{
	COtherSoldierState::OutState();
	return 0;
}

void COtherSoldierRoll::EndRoll(void)
{
	if (m_pSoldier->IsSoldier())
	{
		switch (*m_pMoveDir)
		{
		case DIR_U:
		case DIR_UL:
		case DIR_UR:
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_RunForwardShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_RunForward);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
			break;
		case DIR_D:
		case DIR_DL:
		case DIR_DR:
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Runback);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
			break;
		case DIR_L:
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_RunLeft);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
			break;
		case DIR_R:
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_RunRightShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_RunRight);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
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
			break;
		}
	}
	else
	{
		switch (*m_pMoveDir)
		{
		case DIR_U:
		case DIR_UL:
		case DIR_UR:
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunForwardShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunForward);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
			break;
		case DIR_D:
		case DIR_DL:
		case DIR_DR:
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunBackShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunBack);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
			break;
		case DIR_L:
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunLeftShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunLeft);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
			break;
		case DIR_R:
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunRightShoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_RunRight);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
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
			break;
		}
	}
	
}

COtherSoldierRoll* COtherSoldierRoll::Create(COtherPlayer* pSoldier)
{
	return new COtherSoldierRoll(pSoldier);
}

void COtherSoldierRoll::Release(void)
{
	COtherSoldierState::Release();
}

