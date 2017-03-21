#ifndef FrameMgr_h__
#define FrameMgr_h__

#include "Include.h"

class CFrame;

class CFrameMgr
{
	DECLARE_SINGLETON(CFrameMgr)

private:
	explicit CFrameMgr(void);
			~CFrameMgr(void);

public:
	_bool isPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);

public:
	HRESULT Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit);

private:
	map<const _tchar*, CFrame*>				m_mapFrames;
	typedef map<const _tchar*, CFrame*>		MAPFRAMES;

private:
	CFrame* Find_Frame(const _tchar* pFrameTag);

public:
	void	Release(void);

};


#endif