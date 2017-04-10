#ifndef Gabiscon_h__
#define Gabiscon_h__

#include "DynamicObject.h"

class CStateMachine;

class CGabiscon
	: public CDynamicObject
{
protected:
	explicit CGabiscon(ID3D11DeviceContext* pContext);
	virtual ~CGabiscon(void);

public:
	enum STATE_GABISCON { GABISCON_IDLE, GABISCON_GETOFF, GABISCONEND };

public:
	static CGabiscon* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);

private:
	HRESULT		Prepare_StateMachine(void);
	void		Operate_StateMAchine(const FLOAT& fTimeDelta);

	void		UpdateDir(void);


private:
	XMFLOAT3		m_vLook;

	//Player Animation
	DWORD			m_dwState;
	DWORD			m_dwAniIdx;
	CStateMachine*	m_pComStateMachine;
};

#endif // Gabiscon_h__
