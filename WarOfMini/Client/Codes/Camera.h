#ifndef Camera_h__
#define Camera_h__

#include "GameObject.h"

class CCamera
	: public CGameObject
{
protected:
	explicit CCamera(ID3D11DeviceContext* pContext);
	virtual ~CCamera(void);

public:
	const _matrix* Get_Proj(void);
	const _matrix* Get_View(void);

protected:
	void Set_Proj(_float fNear, _float fFar);
	void Set_View(_vec3 vEye, _vec3 vAt);

public:
	virtual HRESULT	Initialize(_float fNear, _float fFar, _vec3 vEye, _vec3 vAt);
	virtual _int Update(const _float& fTimeDelta)	PURE;
	virtual void Release(void);

public:
	void	MakeView(void);
	void	MakeProjection(void);

protected:
	_matrix				m_matProj;
	_matrix				m_matView;
	float				m_fNear;
	float				m_fFar;

protected:
	_vec3				m_vEye;
	_vec3				m_vUp;
	_vec3				m_vAt;
	bool				m_bMouseFix;
};

#endif Camera_h__