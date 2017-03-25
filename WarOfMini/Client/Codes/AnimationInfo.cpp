#include "stdafx.h"
#include "AnimationInfo.h"


vector<CAnimationInfo::VECFRAMEINFO> CAnimationInfo::m_vecMeshFrameInfo;

CAnimationInfo::CAnimationInfo(MESHNUM eMeshNum)
: CComponent()
, m_eMeshNum(eMeshNum)
, m_wCurKey(0)
, m_wNextKey(0)
, m_wStoreNextKey(0)
, m_wCurFrame(0)
, m_wNextFrame(0)
, m_fRatio(0.f)
{
}

CAnimationInfo::~CAnimationInfo(void)
{
}

CAnimationInfo* CAnimationInfo::Create(MESHNUM eMeshNum)
{
	CAnimationInfo* pAniminfo = new CAnimationInfo(eMeshNum);

	return pAniminfo;
}

_int CAnimationInfo::Update(const _float& fTime)
{
	m_fRatio += fTime * m_vecMeshFrameInfo[m_eMeshNum][m_wCurKey].wFrameSpeed;

	if (m_fRatio > 1.f)
	{
		m_fRatio -= 1.f;

		if (m_fRatio > 1.f)
			m_fRatio = 0.f;

		m_wCurKey = m_wNextKey;
		m_wCurFrame = m_wNextFrame;

		if (m_wStoreNextKey != m_wNextKey
			&& (m_wCurKey < NODELAYANICNT || m_wCurFrame == m_vecMeshFrameInfo[m_eMeshNum][m_wCurKey].wFrameCnt - 1))
		{
			m_wNextKey = m_wStoreNextKey;
			//m_wNextFrame = 0;
		}

		else
		{
			++m_wNextFrame;

			if (m_wNextFrame > m_vecMeshFrameInfo[m_eMeshNum][m_wCurKey].wFrameCnt - 1)
				m_wNextFrame = 0;
		}
	}

	return 0;
}

void CAnimationInfo::Release(void)
{
	CComponent::Release();

	delete this;
}

void CAnimationInfo::Push_FrameInfo(VECFRAMEINFO& vecFrameInfo)
{
	if (m_vecMeshFrameInfo.size() == 0)
		m_vecMeshFrameInfo.reserve(MESHNUM_END);

	m_vecMeshFrameInfo.push_back(vecFrameInfo);
}

const _ushort& CAnimationInfo::Get_CurKey(void)
{
	return m_wCurKey;
}

const _ushort& CAnimationInfo::Get_NextKey(void)
{
	return m_wNextKey;
}

const _ushort& CAnimationInfo::Get_StoreNextKey(void)
{
	return m_wStoreNextKey;
}

const _ushort& CAnimationInfo::Get_CurFrame(void)
{
	return m_wCurFrame;
}

const _ushort& CAnimationInfo::Get_NextFrame(void)
{
	return m_wNextFrame;
}

const _float& CAnimationInfo::Get_Ratio(void)
{
	return m_fRatio;
}

_bool CAnimationInfo::Get_Playing(void)
{
	if ((m_wCurFrame == m_vecMeshFrameInfo[m_eMeshNum][m_wCurKey].wFrameCnt - 2 || m_wNextKey < NODELAYANICNT)
		&& m_wNextKey == m_wStoreNextKey)
		return FALSE;

	return TRUE;
}

void CAnimationInfo::Set_Key(_ushort wKey)
{
	if (wKey < m_vecMeshFrameInfo[m_eMeshNum].size())
		m_wStoreNextKey = wKey;
}

void CAnimationInfo::Set_KeyImm(_ushort wKey)
{
	if (wKey < m_vecMeshFrameInfo[m_eMeshNum].size())
	{
		m_wStoreNextKey = wKey;

		if (m_wNextKey != m_wStoreNextKey)
			m_wNextFrame = 0;

		m_wNextKey = m_wStoreNextKey;
	}
}

const WORD CAnimationInfo::Get_LastFrame(void)
{
	return m_vecMeshFrameInfo[m_eMeshNum][m_wCurKey].wFrameCnt - 2;
}