#ifndef ToolObj_h__
#define ToolObj_h__

class CDevice;
class CComponent;
class CInfo;

class CObj
{
public:
	explicit CObj();
	virtual ~CObj();

protected:
	map<wstring, CComponent*>	m_mapComponent;
	CInfo*						m_pInfo;
	float						m_fViewZ;
	bool						m_bZSort;
	CDevice*					m_pGrapicDevice;
	ReleaseType					m_eReleaseType;
	vector<string>				vecAniName;

	OBJSTATE					m_eState;

	RENDERGROUP					m_eRenderGroup;

public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void) {};
	virtual void Release(void);

public:
	static CObj* Create(ReleaseType _eType);
	

public:
	const float&	GetViewZ(void) const;
	CInfo*			GetInfo(void) { return m_pInfo; }
	ReleaseType&	GetReleaseType(void) { return m_eReleaseType; }

	void			 SetObjState(OBJSTATE eState);

	OBJSTATE		 GetObjState(void);

	RENDERGROUP		 GetRenderGroup(void);

	void			 SetRenderGroup(RENDERGROUP eRender);
	

public:
	void SetPos(D3DXVECTOR3 vPos);

protected:
	void Compute_ViewZ(const D3DXVECTOR3* pPos);
};

#endif