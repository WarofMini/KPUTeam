#ifndef AnimationInfo_h__
#define AnimationInfo_h__

#include "Component.h"
#include "Include.h"

class CAnimationInfo
	: public CComponent
{
public:
	typedef struct tagFrameCnt
	{
		_ushort wFrameCnt;
		_ushort wFrameSpeed;
	}FRAMEINFO;

	typedef vector<FRAMEINFO>	VECFRAMEINFO;


private:
	explicit CAnimationInfo(MESHNUM eMeshNum);
	virtual ~CAnimationInfo(void);

public:
	static void Push_FrameInfo(VECFRAMEINFO& vecFrameInfo);

public:
	const _ushort& Get_CurKey(void);
	const _ushort& Get_NextKey(void);
	const _ushort& Get_CurFrame(void);
	const _ushort& Get_NextFrame(void);
	const _float& Get_Ratio(void);
	_bool Get_Playing(void);

public:
	void Set_Key(_ushort wKey);

public:
	static CAnimationInfo* Create(MESHNUM eMeshNum);
	virtual _int	Update(const _float& fTime);
	virtual void	Release(void);

private:
	static vector<VECFRAMEINFO> m_vecMeshFrameInfo;

private:
	MESHNUM				m_eMeshNum;
	_ushort				m_wCurKey;
	_ushort				m_wNextKey;
	_ushort				m_wStoreNextKey;
	_ushort				m_wCurFrame;
	_ushort				m_wNextFrame;
	_float				m_fRatio;
};

#endif // AnimationInfo_h__