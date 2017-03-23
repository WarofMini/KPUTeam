
#ifndef CameraMgr_h__
#define CameraMgr_h__

#include "Include.h"

class CTransform;
class CCamera;

class CCameraMgr
{
	DECLARE_SINGLETON(CCameraMgr)

public:
	enum CAMERALIST { CAMERA_DYNAMIC, CAMERA_WALK, CAMERA_RIDE, CAMERA_END };

private:
	CCameraMgr(void);
	~CCameraMgr(void);

public:
	void Ready_DynamicCamera(ID3D11DeviceContext* pContext, CAMERALIST eCameraName, _float fNear, _float fFar, XMFLOAT3& vPos, XMFLOAT3& vTarget);
	void Ready_StaticCamera(ID3D11DeviceContext* pContext, CAMERALIST eCameraName, const CTransform* pObjTrans, _float fDist, _float fHeightPivot
		, _float fNear, _float fFar, XMFLOAT3& vPos, XMFLOAT3& vTarget);
	void Update_CurCamera(const FLOAT& fTimeDelta);
	void Release(void);

public:
	const XMFLOAT4X4* Get_CurCameraProj(void);
	const XMFLOAT4X4* Get_CurCameraView(void);
	const _bool*	Get_CurCameraFixMouseCheck(void);

public:
	void Set_CurCamera(CAMERALIST eCameraName);

private:
	vector<CCamera*>	m_vecCamera;
	CAMERALIST			m_eCurCamera;
};

#endif // CameraMgr_h__

