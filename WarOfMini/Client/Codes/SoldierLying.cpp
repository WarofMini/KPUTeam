#include "stdafx.h"
#include "SoldierLying.h"
#include "Input.h"

CSoldierLying::CSoldierLying(CPlayer* pSoldier)
	: CSoldierState(pSoldier)
{

}

CSoldierLying::~CSoldierLying()
{

}

int CSoldierLying::InState()
{
	CSoldierState::InState();
	if (m_pSoldier->IsAbleReload())
		m_bShoot = false;
	else
		m_bShoot = m_pkey[KEY_LCLICK];
	return 0;
}

int CSoldierLying::OnState()
{
	CSoldierState::OnState();
	if (m_pSoldier->IsAbleReload())
		m_bShoot = false;
	else
		m_bShoot = m_pkey[KEY_LCLICK];
	m_bIsLying = m_pkey[KEY_CTRL];

	if (ISLying())
		return 0;

	ShootCheck();

	MoveKeyCheck();

	return 1;
}

int CSoldierLying::OutState()
{
	CSoldierState::OutState();
	return 0;
}

bool CSoldierLying::ISLying(void)
{
	if (!m_bIsLying)
	{
		if (m_pSoldier->IsSoldier())
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Shoot);
				m_pSoldier->Set_Fire(true);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_idle);
				m_pSoldier->Set_Fire(false);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		}
		else
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_Shoot);
				m_pSoldier->Set_Fire(true);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_Idle);
				m_pSoldier->Set_Fire(false);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		}
		return true;
	}
	return false;
}

void CSoldierLying::MoveKeyCheck(void)
{
	if (m_bShoot)
		return;

	if (m_pSoldier->IsSoldier())
	{
		switch (*m_pMoveDir)
		{
		case DIR_U:
		case DIR_UL:
		case DIR_UR:
			if (*m_pAniIdx != PLAYER_CrawlingFront)
				m_pSoldier->PlayAnimation(PLAYER_CrawlingFront);
			break;
		case DIR_D:
		case DIR_DL:
		case DIR_DR:
			if (*m_pAniIdx != PLAYER_CrawlingBack)
				m_pSoldier->PlayAnimation(PLAYER_CrawlingBack);
			break;
		case DIR_L:
			if (*m_pAniIdx != PLAYER_CrawlingLeft)
				m_pSoldier->PlayAnimation(PLAYER_CrawlingLeft);
			break;
		case DIR_R:
			if (*m_pAniIdx != PLAYER_CrawlingRight)
				m_pSoldier->PlayAnimation(PLAYER_CrawlingRight);
			break;
		default:
			if (*m_pAniIdx != PLAYER_Lying && *m_pAniIdx != PLAYER_LyingShoot)
			{
				if (m_bShoot)
					m_pSoldier->PlayAnimation(PLAYER_LyingShoot);
				else
					m_pSoldier->PlayAnimation(PLAYER_Lying);
			}
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
			if (*m_pAniIdx != PLAYER_Iron_CrawlingFront)
				m_pSoldier->PlayAnimation(PLAYER_Iron_CrawlingFront);
			break;
		case DIR_D:
		case DIR_DL:
		case DIR_DR:
			if (*m_pAniIdx != PLAYER_Iron_CrawlingBack)
				m_pSoldier->PlayAnimation(PLAYER_Iron_CrawlingBack);
			break;
		case DIR_L:
			if (*m_pAniIdx != PLAYER_Iron_CrawlingLeft)
				m_pSoldier->PlayAnimation(PLAYER_Iron_CrawlingLeft);
			break;
		case DIR_R:
			if (*m_pAniIdx != PLAYER_Iron_CrawlingRight)
				m_pSoldier->PlayAnimation(PLAYER_Iron_CrawlingRight);
			break;
		default:
			if (*m_pAniIdx != PLAYER_Iron_Lying && *m_pAniIdx != PLAYER_Iron_LyingShoot)
			{
				if (m_bShoot)
					m_pSoldier->PlayAnimation(PLAYER_Iron_LyingShoot);
				else
					m_pSoldier->PlayAnimation(PLAYER_Iron_Lying);
			}
			break;
		}
	}
}

void CSoldierLying::ShootCheck(void)
{
	if (m_pSoldier->IsSoldier())
	{
		if (m_bShoot && *m_pAniIdx != PLAYER_LyingShoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_LyingShoot);
			m_pSoldier->Set_Fire(true);
		}
		else if (!m_bShoot && *m_pAniIdx == PLAYER_LyingShoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Lying);
			m_pSoldier->Set_Fire(false);
		}
	}
	else
	{
		if (m_bShoot && *m_pAniIdx != PLAYER_Iron_LyingShoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_LyingShoot);
			m_pSoldier->Set_Fire(true);
		}
		else if (!m_bShoot && *m_pAniIdx == PLAYER_Iron_LyingShoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_Lying);
			m_pSoldier->Set_Fire(false);
		}
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
