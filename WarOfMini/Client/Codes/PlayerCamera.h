#ifndef PlayerCamera_h__
#define PlayerCamera_h__

#include "Camera.h"


class CTransform;

class CPlayerCamera
	: public CCamera
{
private:
	explicit CPlayerCamera(ID3D11DeviceContext* pContext, PxScene* pPxScene, const CTransform* pTargetTransform, _float fGap);
	virtual ~CPlayerCamera(void);

public:
	static CPlayerCamera* Create(ID3D11DeviceContext* pContext, PxScene* pPxScene, const CTransform* pTargetTransform, _float fGap,
		_float fNear, _float fFar, XMFLOAT3& vEye, XMFLOAT3& vAt);

public:
	virtual _int Update(const _float& fTimeDelta);
	virtual void Release(void);

private:
	const CTransform*	m_pTargetTransform;
	bool				m_bCreate;
	_float				m_fEpsilonY;
	_float				m_fGap;
	_float				m_fAngleY;

	CGameObject*		m_pPlayer;

public:
	void				PlayerState(void);
	void				FixMouse(void);
	void				KeyState(const _float& fTimeDelta);
	_float				GetGap(void) { return m_fGap; }
	void				CheckCollision(void);
};



#endif