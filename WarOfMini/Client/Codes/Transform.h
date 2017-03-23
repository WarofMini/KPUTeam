#ifndef Transform_h__
#define Transform_h__

#include "Component.h"


class CTransform
	: public CComponent
{
private:
	explicit CTransform(void);
	virtual ~CTransform(void);

public:
	virtual _int  Update(const _float& fTime);
	void Update_MatrixNotXRot(void);

public:
	static CTransform* Create(void);

public:
	virtual void Release(void);

public:
	XMFLOAT3			m_vScale;
	XMFLOAT3			m_vAngle;
	XMFLOAT3			m_vPos;
	XMFLOAT3			m_vDir;

public:
	XMFLOAT4X4			m_matWorld;
	XMFLOAT4X4			m_matNRotWorld;
	XMFLOAT4X4			m_matNScaleWorld;


public:
	void				Move_Position(const XMVECTOR* pDirection);

	void				Move_TargetPos(const XMVECTOR* pTargetPos, const _float& fSpeed, const _float& fTimeDelta);

	void				Move_Forward(const _float& fSpeed, const _float& fTimeDelta);

	void				Move_Right(const XMVECTOR* pDirection, const _float& fSpeed, const _float& fTimeDelta);

	void				Move_Left(const XMVECTOR* pDirection, const _float& fSpeed, const _float& fTimeDelta);
};

#endif