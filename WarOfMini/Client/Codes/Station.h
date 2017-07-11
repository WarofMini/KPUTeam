#ifndef Station_h__
#define Station_h__

#include "GameObject.h"
#include "Transform.h"

class CCloth;

class CStation
	: public CGameObject
{
private:
	explicit CStation(ID3D11DeviceContext* pContext);
	virtual ~CStation(void);

public:
	static CStation* Create(ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

private:
	virtual HRESULT Ready_Component(void);

private:
	_uint			m_uiObjNum;
	CTransform*		m_pTransform;
	CCloth*			m_pFlag;

public:
	void			SetObjNum(_uint uNum);
	_uint			GetObjNum(void);
	void			SetPosition(XMFLOAT3 vPos) { m_pTransform->m_vPos = vPos; }
	void			SetRotate(XMFLOAT3 vRotate) { m_pTransform->m_vAngle = vRotate; }
	void			SetScale(XMFLOAT3 vScale) { m_pTransform->m_vScale = vScale; }
	void			SetFlag(CCloth* pFlag) { m_pFlag = pFlag; }
};

#endif // Station_h__
