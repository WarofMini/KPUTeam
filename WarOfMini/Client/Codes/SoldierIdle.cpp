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

	return 1;
}

int CSoldierIdle::OutState()
{
	return 0;
}

void CSoldierIdle::ShootCheck(void)
{
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
	bool bInputKey[KEY_END] = {false};

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
		return false;

	
	if (bInputKey[KEY_LEFT])
	{
		if(m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunLeft);
	}
	if (bInputKey[KEY_RIGHT])
	{
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunRightShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunRight);
	}

	if (bInputKey[KEY_UP])
	{
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunForwardShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunForward);
	}
	if (bInputKey[KEY_DOWN])
	{
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_Runback);
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

CSoldierIdle* CSoldierIdle::Create(CPlayer* pSoldier)
{
	return new CSoldierIdle(pSoldier);
}

void CSoldierIdle::Release(void)
{
	CSoldierState::Release();
}
