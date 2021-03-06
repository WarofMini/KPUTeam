#ifndef Effect_h__
#define Effect_h__

#include "GameObject.h"

class CTextures;
class CRcTex;

class CEffect 
	: public CGameObject
{
protected:
	explicit CEffect(ID3D11DeviceContext* pContext);
	virtual ~CEffect(void);

public:
	virtual HRESULT	Initialize(void);
	virtual INT		Update(const FLOAT& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);


protected:
	virtual HRESULT Ready_Component(void);

protected:
	CTextures*		m_pTexture;
	CRcTex*			m_pBuffer;

protected:
	_int			m_iFrame; //현재 텍스쳐 번호
	_int			m_iSizeX; //텍스쳐 가로 길이
	_int			m_iSizeY; //텍스쳐 세로 길이
	_float			m_fLifeTime;
	_float			m_fRealTime;
	_float			m_fDelayTime;
	_bool			m_bAllBillboardCheck;
	_bool			m_bYBillboardCheck;
	_bool			m_bLoop;

public:
	void			SetFrame(_int iNumber) { m_iFrame = iNumber; }
	void			SetSizeX(_int iSizeX) { m_iSizeX = iSizeX; }
	void			SetSizeY(_int iSizeY) { m_iSizeY = iSizeY; }
	void			SetLifeTime(_float Time) { m_fLifeTime = Time; }
	void			SetRealTime(_float Time) { m_fRealTime = Time; }
	void			SetDelayTime(_float Time) { m_fDelayTime = Time; }
	void			SetAllBillboard(_bool bCheck) { m_bAllBillboardCheck = bCheck; }
	void			SetYBillboard(_bool bCheck) { m_bYBillboardCheck = bCheck; }

	_int			GetFrame(void) { return m_iFrame; }
	_int			GetSizeX(void) { return m_iSizeX; }
	_int			GetSizeY(void) { return m_iSizeY; }
	_float			GetLifeTime(void) { return m_fLifeTime; }
	_float			GetRealTime(void) { return m_fRealTime; }
	_float			GetDelayTime(void) { return m_fDelayTime; }
	_bool			GetAllBillboard(void) { return m_bAllBillboardCheck; }
	_bool			GetYBillboard(void) { return m_bYBillboardCheck; }

public:
	void			ComputeBillboard(void);
	void			SetPosition(XMFLOAT3 vPos);
	void			SetScale(XMFLOAT3 vScale);
	void			SetRotate(XMFLOAT3 vRotate);
};

#endif