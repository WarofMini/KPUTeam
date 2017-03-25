
#ifndef CameraMgr_h__
#define CameraMgr_h__

#include "Include.h"

class CTransform;
class CCamera;

class CCameraMgr
{
	DECLARE_SINGLETON(CCameraMgr)

public:
	enum CAMERALIST { CAMERA_DYNAMIC, CAMERA_STATIC, CAMERA_END };

private:
	CCameraMgr(void);
	~CCameraMgr(void);

public:
	void Ready_DynamicCamera(ID3D11DeviceContext* pContext, CAMERALIST eCameraName, _float fNear, _float fFar, XMFLOAT3& vPos, XMFLOAT3& vTarget);

	void Ready_StaticCamera(ID3D11DeviceContext* pContext, CAMERALIST eCameraName, const CTransform* pTargetTransform, _float fGap,
		_float fNear, _float fFar, XMFLOAT3& vEye, XMFLOAT3& vAt);

	void Update_CurCamera(const FLOAT& fTimeDelta);
	void Release(void);
public:
	const XMFLOAT4X4* Get_CurCameraProj(void);
	const XMFLOAT4X4* Get_CurCameraView(void);
	const _bool*	Get_CurCameraFixMouseCheck(void);

	_float Get_CurrentCameraSpeed(void);
public:
	void Set_CurCamera(CAMERALIST eCameraName);


	CAMERALIST	Get_CurCamera(void);

private:
	vector<CCamera*>	m_vecCamera;
	CAMERALIST			m_eCurCamera;
};

#endif // CameraMgr_h__

