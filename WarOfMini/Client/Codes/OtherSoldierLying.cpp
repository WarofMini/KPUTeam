#include "stdafx.h"
#include "OtherSoldierLying.h"
#include "Input.h"

COtherSoldierLying::COtherSoldierLying(COtherPlayer* pSoldier)
	: COtherSoldierState(pSoldier)
{

}

COtherSoldierLying::~COtherSoldierLying()
{

}

int COtherSoldierLying::InState()
{
	COtherSoldierState::InState();
	m_bShoot = m_pkey[KEY_LCLICK];
	return 0;
}

int COtherSoldierLying::OnState()
{
	COtherSoldierState::OnState();
	m_bShoot = m_pkey[KEY_LCLICK];
	m_bIsLying = m_pkey[KEY_CTRL];

	if (ISLying())
		return 0;

	ShootCheck();

	MoveKeyCheck();

	return 1;
}

int COtherSoldierLying::OutState()
{
	COtherSoldierState::OutState();
	return 0;
}

bool COtherSoldierLying::ISLying(void)
{
	if (!m_bIsLying)
	{
		if (m_pSoldier->IsSoldier())
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Shoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_idle);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		}
		else
		{
			if (m_bShoot)
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_Shoot);
			}
			else
			{
				m_pSoldier->PlayAnimation(PLAYER_Iron_Idle);
			}
			*(m_pSoldier->Get_State()) = CPlayer::SOLDIER_IDLE;
		}
		return true;
	}
	return false;
}

void COtherSoldierLying::MoveKeyCheck(void)
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

void COtherSoldierLying::ShootCheck(void)
{
	if (m_pSoldier->IsSoldier())
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
	else
	{
		if (m_bShoot && *m_pAniIdx != PLAYER_Iron_LyingShoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_LyingShoot);
		}
		else if (!m_bShoot && *m_pAniIdx == PLAYER_Iron_LyingShoot)
		{
			m_pSoldier->PlayAnimation(PLAYER_Iron_Lying);
		}
	}
}

COtherSoldierLying* COtherSoldierLying::Create(COtherPlayer* pSoldier)
{
	return new COtherSoldierLying(pSoldier);
}

void COtherSoldierLying::Release(void)
{
	COtherSoldierState::Release();
}
