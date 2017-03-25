#ifndef OtherPlayer_h__
#define OtherPlayer_h__

#include "DynamicObject.h"

class CEquipment;

class COtherPlayer
	: public CDynamicObject
{
protected:
	explicit COtherPlayer(ID3D11DeviceContext* pContext);
	virtual ~COtherPlayer(void);

public:
	static COtherPlayer* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);
	virtual void Update_Equipment(const FLOAT& fTimeDelta);

protected:
	void Move(const FLOAT& fTimeDelta);
	void InputKey(const FLOAT& fTimeDelta);

private:
	XMFLOAT3		m_vLook;

	XMFLOAT4X4		m_matEquipBone[2];
	CEquipment*		m_pEquipment[2];
};

#endif //