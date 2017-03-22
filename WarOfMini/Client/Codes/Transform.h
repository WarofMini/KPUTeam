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
	XMFLOAT4X4			m_matWorld;
};

#endif