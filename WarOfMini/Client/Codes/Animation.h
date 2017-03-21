#ifndef Animation_h__
#define Animation_h__

#include "Include.h"

class CAnimationInfo;

class CAnimation
{
public:

	typedef vector<CLUSDATA> FRAME;

	typedef vector<FRAME> ANIMDATA;


private:
	explicit CAnimation(ID3D11DeviceContext* pContext);
	~CAnimation(void);

public:
	void Push_AnimData(ANIMDATA& tAniData);
	void UpdateSubresource(CAnimationInfo* pAnimInfo, _matrix* pBoneWorld = NULL);

public:
	static CAnimation* Create(ID3D11DeviceContext* pContext);
	void Release(void);

private:
	ID3D11DeviceContext*	m_pContext;

private:
	vector<ANIMDATA>	m_vecAnimData;
};

#endif // Animation_h__