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
	
	if (RollLeyCheck())
		return 0;

	return 1;
}

int CSoldierMove::OutState()
{
	return 0;
}

bool CSoldierMove::MoveKeyCheck(void)
{
	bool bInputKey[KEY_END] = { false };

	if (m_pInput->Get_DIKeyState(DIK_W))
		bInputKey[KEY_UP] = true;
	if (m_pInput->Get_DIKeyState(DIK_S))
		bInputKey[KEY_DOWN] = true;
	if (m_pInput->Get_DIKeyState(DIK_A))
		bInputKey[KEY_LEFT] = true;
	if (m_pInput->Get_DIKeyState(DIK_D))
		bInputKey[KEY_RIGHT] = true;

	if (bInputKey[KEY_UP] == true && bInputKey[KEY_DOWN] == true)
	{
		bInputKey[KEY_UP] = false;
		bInputKey[KEY_DOWN] = false;
	}
	if (bInputKey[KEY_LEFT] == true && bInputKey[KEY_RIGHT] == true)
	{
		bInputKey[KEY_LEFT] = false;
		bInputKey[KEY_RIGHT] = false;
	}

	if (bInputKey[KEY_UP] == false && bInputKey[KEY_DOWN] == false &&
		bInputKey[KEY_LEFT] == false && bInputKey[KEY_RIGHT] == false)
	{
		if(m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_Shoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_idle);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		return true;
	}

	switch (*m_pAniIdx)
	{
	case PLAYER_RunForward:
	case PLAYER_RunForwardShoot:
	{
		if (!bInputKey[KEY_UP])
		{
			if (m_bShoot)
			{
				if (bInputKey[KEY_DOWN])
				{
					m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
					break;
				}
				if (bInputKey[KEY_LEFT])
					m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
				if (bInputKey[KEY_RIGHT])
					m_pSoldier->PlayAnimation(PLAYER_RunRightShoot);
			}
			else
			{
				if (bInputKey[KEY_DOWN])
				{
					m_pSoldier->PlayAnimation(PLAYER_Runback);
					break;
				}
				if (bInputKey[KEY_LEFT])
					m_pSoldier->PlayAnimation(PLAYER_RunLeft);
				if (bInputKey[KEY_RIGHT])
					m_pSoldier->PlayAnimation(PLAYER_RunRight);
			}
			
		}
	}
	break;
	case PLAYER_Runback:
	case PLAYER_RunBackShoot:
	{
		if (!bInputKey[KEY_DOWN])
		{
			if (m_bShoot)
			{
				if (bInputKey[KEY_UP])
				{
					m_pSoldier->PlayAnimation(PLAYER_RunForwardShoot);
					break;
				}
				if (bInputKey[KEY_LEFT])
					m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
				if (bInputKey[KEY_RIGHT])
					m_pSoldier->PlayAnimation(PLAYER_RunRightShoot);
			}
			else
			{
				if (bInputKey[KEY_UP])
				{
					m_pSoldier->PlayAnimation(PLAYER_RunForward);
					break;
				}
				if (bInputKey[KEY_LEFT])
					m_pSoldier->PlayAnimation(PLAYER_RunLeft);
				if (bInputKey[KEY_RIGHT])
					m_pSoldier->PlayAnimation(PLAYER_RunRight);
			}
		}
	}
	break;
	case PLAYER_RunRight:
	case PLAYER_RunRightShoot:
	{
		if (!bInputKey[KEY_RIGHT])
		{
			if (m_bShoot)
			{
				if (bInputKey[KEY_UP])
				{
					m_pSoldier->PlayAnimation(PLAYER_RunForwardShoot);
					break;
				}
				if (bInputKey[KEY_DOWN])
				{
					m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
					break;
				}
				if (bInputKey[KEY_LEFT])
					m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
			}
			else
			{
				if (bInputKey[KEY_UP])
				{
					m_pSoldier->PlayAnimation(PLAYER_RunForward);
					break;
				}
				if (bInputKey[KEY_DOWN])
				{
					m_pSoldier->PlayAnimation(PLAYER_Runback);
					break;
				}
				if (bInputKey[KEY_LEFT])
					m_pSoldier->PlayAnimation(PLAYER_RunLeft);
			}
		}
	}
	break;
	case PLAYER_RunLeft:
	case PLAYER_RunLeftShoot:
	{
		if (!bInputKey[KEY_LEFT])
		{
			if (m_bShoot)
			{
				if (bInputKey[KEY_UP])
				{
					m_pSoldier->PlayAnimation(PLAYER_RunForwardShoot);
					break;
				}
				if (bInputKey[KEY_DOWN])
				{
					m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
					break;
				}
				if (bInputKey[KEY_RIGHT])
					m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
			}
			else
			{
				if (bInputKey[KEY_UP])
				{
					m_pSoldier->PlayAnimation(PLAYER_RunForward);
					break;
				}
				if (bInputKey[KEY_DOWN])
				{
					m_pSoldier->PlayAnimation(PLAYER_Runback);
					break;
				}
				if (bInputKey[KEY_RIGHT])
					m_pSoldier->PlayAnimation(PLAYER_RunLeft);
			}
		}
	}
	break;
	}

	return false;
}

bool CSoldierMove::RollLeyCheck(void)
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
