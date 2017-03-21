#ifndef DynamicCamera_h__
#define DynamicCamera_h__

#include "Camera.h"

class CDynamicCamera
	: public CCamera
{
private:
	explicit CDynamicCamera(ID3D11DeviceContext* pContext);
	virtual ~CDynamicCamera(void);

public:
	static CDynamicCamera* Create(ID3D11DeviceContext* pContext, _float fNear, _float fFar, _vec3& vEye, _vec3& vAt);

public:
	virtual _int Update(const _float& fTimeDelta);
	virtual void Release(void);

private:
	_float	m_fSpeed;


private:
	void	KeyCheck(const _float& fTimeDelta);
	void	MouseMove(const _float& fTimeDelta);
	void	FixMouse(void);
};

#endif // DynamicCamera_h__