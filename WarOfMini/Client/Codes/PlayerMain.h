#ifndef PlayerMain_h__
#define PlayerMain_h__

#include "Player.h"

class CPlayerMain
	: public CPlayer
{
protected:
	explicit CPlayerMain(ID3D11DeviceContext* pContext);
	virtual ~CPlayerMain(void);

public:
	static CPlayerMain* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);

protected:
	void Move(const FLOAT& fTimeDelta);
	void InputKey(const FLOAT& fTimeDelta);
	void Attack(const FLOAT& fTimeDelta);
private:
	XMFLOAT3		m_vLook;

};

#endif //