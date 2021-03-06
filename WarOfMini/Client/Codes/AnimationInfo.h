#ifndef AnimationInfo_h__
#define AnimationInfo_h__

#include "Component.h"
#include "Include.h"

class CAnimationInfo
	: public CComponent
{
public:
	typedef vector<FRAMEINFO>  VECFRAMEINFO;

private:
	explicit CAnimationInfo(MESHNUM eMeshNum);
	virtual ~CAnimationInfo(void);

public:
	static void Push_FrameInfo(VECFRAMEINFO& vecFrameInfo);

public:
	const _ushort& Get_CurKey(void);
	const _ushort& Get_NextKey(void);
	const _ushort& Get_StoreNextKey(void);
	const _ushort& Get_CurFrame(void);
	const _ushort& Get_NextFrame(void);
	const _ushort Get_LastFrame(void);
	const _float& Get_Ratio(void);
	_bool Get_Playing(void);

	void  Set_PlayAni(bool bPlay) { m_bPlay = bPlay; }
	bool  Get_PlayAni(void) { return m_bPlay; }
	void  Set_PlayReverse(bool bReverse) { m_bIsReverse = bReverse; }

public:
	void Set_Key(_ushort wKey);
	void Set_KeyImm(_ushort wKey);

public:
	static CAnimationInfo* Create(MESHNUM eMeshNum);
	virtual _int Update(const _float& fTime);
	virtual void Release(void);

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

	bool				m_bPlay;
	bool				m_bIsReverse;
};


#endif // AnimationInfo_h__