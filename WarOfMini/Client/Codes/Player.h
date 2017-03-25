#ifndef Player_h__
#define Player_h__

#include "DynamicObject.h"

class CEquipment;
class CStateMachine;
class CInput;
class CPlayer
	: public CDynamicObject
{
protected:
	explicit CPlayer(ID3D11DeviceContext* pContext);
	virtual ~CPlayer(void);

public:
	enum STATE_SOLDIER { SOLDIER_IDLE, SOLDIER_MOVE, SOLDIER_LYING, SOLDIER_ROLL, SOLDIER_END };

public:
	static CPlayer* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);
	virtual void Update_Equipment(const FLOAT& fTimeDelta);

private:
	HRESULT		Prepare_StateMachine(void);
	void		Operate_StateMAchine(const float& fTimeDelta);

public:
	CInput*		GetInput(void) { return m_pInput; }
	void		PlayAnimation(DWORD dwAniIdx, bool bImmediate = true);
	DWORD*		Get_State(void) { return &m_dwState; }
	DWORD*		Get_AniIdx(void) { return &m_dwAniIdx; }
	bool		Check_AnimationFrame(void);

	void		KeyState(const FLOAT& fTimeDelta);

public:
	void		UpdateDir(void);

	//현재 카메라 상태가 Dynamic일 경우  Player Update 실행X
	_bool		DynamicCameraCheck(void); 

private:
	CInput*			m_pInput;
	XMFLOAT3		m_vLook;

	XMFLOAT4X4		m_matEquipBone[2];
	CEquipment*		m_pEquipment[2];

	//Player Animation
	DWORD			m_dwState;
	DWORD			m_dwAniIdx;
	CStateMachine*	m_pComStateMachine;
};

#endif //