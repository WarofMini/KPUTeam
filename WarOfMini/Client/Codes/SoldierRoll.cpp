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
	{//End To Idle
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_Shoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_idle);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		return;
	}

	if (bInputKey[KEY_UP])
	{
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunForwardShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunForward);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
		return;
	}

	if (bInputKey[KEY_DOWN])
	{
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunBackShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_Runback);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
		return;
	}

	if (bInputKey[KEY_LEFT])
	{
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunLeftShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunLeft);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
		return;
	}

	if (bInputKey[KEY_RIGHT])
	{
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_RunRightShoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_RunRight);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_MOVE;
		return;
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

