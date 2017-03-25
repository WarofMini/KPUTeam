#ifndef StaticCamera_h__
#define StaticCamera_h__

#include "Camera.h"


class CStaticCamera
	: public CCamera
{
private:
	explicit CStaticCamera(ID3D11DeviceContext* pContext);
	virtual ~CStaticCamera(void);

public:
	static CStaticCamera* Create(ID3D11DeviceContext* pContext, _float fNear, _float fFar, XMFLOAT3& vEye, XMFLOAT3& vAt);

public:
	virtual _int Update(const _float& fTimeDelta);
	virtual void Release(void);


};



#endif