#include "stdafx.h"
#include "FrameMgr.h"
#include "Frame.h"

IMPLEMENT_SINGLETON(CFrameMgr)

CFrameMgr::CFrameMgr(void)
{
}

CFrameMgr::~CFrameMgr(void)
{
}

_bool CFrameMgr::isPermit_Call(const _tchar * pFrameTag, const _float & fTimeDelta)
{
	CFrame*	pFrame = Find_Frame(pFrameTag);

	if (pFrame == NULL)
		return false;

	return pFrame->isPermit_Call(fTimeDelta);
}

HRESULT CFrameMgr::Ready_Frame(const _tchar * pFrameTag, const _float & fCallLimit)
{
	CFrame*	pFrame = Find_Frame(pFrameTag);

	if (pFrame != NULL)
		return E_FAIL;

	pFrame = CFrame::Create(fCallLimit);

	if (pFrame == NULL)
		return E_FAIL;

	m_mapFrames.insert(MAPFRAMES::value_type(pFrameTag, pFrame));

	return S_OK;
}

CFrame * CFrameMgr::Find_Frame(const _tchar * pFrameTag)
{
	MAPFRAMES::iterator	iter = find_if(m_mapFrames.begin(), m_mapFrames.end(), CTag_Finder(pFrameTag));

	if (iter == m_mapFrames.end())
		return NULL;

	return iter->second;
}

void CFrameMgr::Release(void)
{
	for_each(m_mapFrames.begin(), m_mapFrames.end(), CRelease_Pair());
	m_mapFrames.clear();

	delete this;
}
