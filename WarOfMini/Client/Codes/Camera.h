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
	const XMFLOAT4X4* Get_Proj(void);
	const XMFLOAT4X4* Get_View(void);

protected:
	void Set_Proj(_float fNear, _float fFar);
	void Set_View(XMFLOAT3 vEye, XMFLOAT3 vTarget);

public:
	virtual HRESULT	Initialize(_float fNear, _float fFar, XMFLOAT3 vEye, XMFLOAT3 vTarget);
	virtual _int Update(const _float& fTimeDelta)	PURE;
	virtual void Release(void);

protected:
	XMFLOAT4X4*				m_pProj;
	XMFLOAT4X4*				m_pView;

protected:
	XMFLOAT3*				m_pEye;
	XMFLOAT3*				m_pAt;
	XMFLOAT3*				m_pUp;

	_float					m_fNear;
	_float					m_fFar;
	_float					m_fCameraSpeed;
	_bool					m_bMouseFix;
	PxScene*				m_pPxScene;
public:
	void		MakeView(void);
	void		MakeProjection(void);


public:
	_bool*		GetMouseFixCheck(void);
	void		SetMouseFixCheck(_bool bCheck);

	void		ReduceCameraSpeed(_float fSpeed);
	void		AddCameraSpeed(_float fSpeed);

	_float		GetCameraSpeed(void);

	XMFLOAT3	GetCameraLookAt(void);

	void		SetCameraEye(XMFLOAT3 eye);
	XMFLOAT3	GetCameraEye(void);

};

#endif Camera_h__