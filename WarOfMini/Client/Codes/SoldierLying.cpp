#include "stdafx.h"
#include "SoldierLying.h"
#include "Input.h"

CSoldierLying::CSoldierLying(CPlayer* pSoldier)
	: CSoldierState(pSoldier)
	, m_pSoldier(pSoldier)
{

}

CSoldierLying::~CSoldierLying()
{

}

int CSoldierLying::InState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);
	return 0;
}

int CSoldierLying::OnState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);
	m_bIsLying = m_pInput->Get_DIKeyState(DIK_LCONTROL);

	if (ISLying())
		return 0;

	ShootCheck();

	MoveKeyCheck();


	return 1;
}

int CSoldierLying::OutState()
{
	return 0;
}

bool CSoldierLying::ISLying(void)
{
	if (!m_bIsLying)
	{
		if (m_bShoot)
			m_pSoldier->PlayAnimation(PLAYER_Shoot);
		else
			m_pSoldier->PlayAnimation(PLAYER_idle);
		*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;

		return true;
	}
	return false;
}

void CSoldierLying::MoveKeyCheck(void)
{
	if (m_bShoot)
		return;

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
		if (*m_pAniIdx != PLAYER_Lying && *m_pAniIdx != PLAYER_LyingShoot)
		{
			if(m_bShoot)
				m_pSoldier->PlayAnimation(PLAYER_LyingShoot);
			else
				m_pSoldier->PlayAnimation(PLAYER_Lying);
		}
		return;
	}

	if (bInputKey[KEY_UP] || bInputKey[KEY_DOWN])
	{
		if (bInputKey[KEY_UP] && *m_pAniIdx != PLAYER_CrawlingFront)
		{
			m_pSoldier->PlayAnimation(PLAYER_CrawlingFront);
		}
		if (bInputKey[KEY_DOWN] && *m_pAniIdx != PLAYER_CrawlingBack)
		{
			m_pSoldier->PlayAnimation(PLAYER_CrawlingBack);
		}
	}
	else
	{
		if (bInputKey[KEY_LEFT] && *m_pAniIdx != PLAYER_CrawlingLeft)
		{
			m_pSoldier->PlayAnimation(PLAYER_CrawlingLeft);
		}
		else if (bInputKey[KEY_RIGHT] && *m_pAniIdx != PLAYER_CrawlingRight)
		{
			m_pSoldier->PlayAnimation(PLAYER_CrawlingRight);
		}
	}
}

void CSoldierLying::ShootCheck(void)
{
	if (m_bShoot && *m_pAniIdx != PLAYER_LyingShoot)
	{
		m_pSoldier->PlayAnimation(PLAYER_LyingShoot);
	}
	else if (!m_bShoot && *m_pAniIdx == PLAYER_LyingShoot)
	{
		m_pSoldier->PlayAnimation(PLAYER_Lying);
	}
}

CSoldierLying* CSoldierLying::Create(CPlayer* pSoldier)
{
	return new CSoldierLying(pSoldier);
}

void CSoldierLying::Release(void)
{
	CSoldierState::Release();
}
