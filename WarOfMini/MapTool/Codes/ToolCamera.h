#ifndef ToolCamera_h__
#define ToolCamera_h__


class CInfo;

class CToolCamera
{
public:
	CToolCamera();
	virtual ~CToolCamera();

public:
	DECLARE_SINGLETON(CToolCamera)

public:
	D3DXMATRIX	m_matView;
	D3DXMATRIX	m_matProj;
	D3DXVECTOR3 m_vEye;
	D3DXVECTOR3 m_vAt;
	D3DXVECTOR3 m_vUp;


private:
	float m_fFovY;
	float m_fAspect; 
	float m_fNear;
	float m_fFar;
	float m_fCameraDistance;
	float m_fCameraSpeed;
	bool  m_bMouseFix;	

	DWORD m_dwTime;
	DWORD m_dwKey;

public:
	HRESULT Initialize(void);
	int		Update(void);
	void	Release(void);

public:
	void MakeView(void);
	void MakeProjection(void);

private:
	void KeyState(void);
	void FixMouse(void);
	void MouseMove(void);

public:
	D3DXVECTOR3	 GetCamEye(void);

	D3DXMATRIX   GetProj(void);
};

#endif