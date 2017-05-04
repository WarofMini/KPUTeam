#ifndef Player_h__
#define Player_h__

#include "DynamicObject.h"

class CEquipment;
class CStateMachine;
class CInput;
class CCalculator;
class CRigidBody;

class CGravity;
class CGun;

class CPlayer
	: public CDynamicObject
{
protected:
	explicit CPlayer(ID3D11DeviceContext* pContext);
	virtual ~CPlayer(void);

public:
	enum STATE_SOLDIER { SOLDIER_IDLE, SOLDIER_MOVE, SOLDIER_LYING, SOLDIER_ROLL, SOLDIER_JUMP, SOLDIER_END };

public:
	static CPlayer* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	static CPlayer* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, XMFLOAT3 vPos);

public:
	virtual HRESULT	Initialize(ID3D11Device* pGraphicDev);
	virtual INT Update(const FLOAT& fTimeDelta);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(ID3D11Device* pGraphicDev);
	virtual void Update_Equipment(const FLOAT& fTimeDelta);

private:
	HRESULT		Prepare_StateMachine(void);
	void		Operate_StateMAchine(const FLOAT& fTimeDelta);
	void		Collision_Field(const FLOAT& fTimeDelta);
	void		KeyCheck(void);
	void		KeyState(const FLOAT& fTimeDelta);
	void		Soldier_Move(const FLOAT& fTimeDelta);
	void		Soldier_Iron_Move(const FLOAT& fTimeDelta);
	void		Soldier_Fire(const FLOAT& fTimeDelta);

public:
	CInput*		GetInput(void) { return m_pInput; }
	void		PlayAnimation(DWORD dwAniIdx, bool bImmediate = true);
	DWORD*		Get_State(void) { return &m_dwState; }
	DWORD*		Get_AniIdx(void) { return &m_dwAniIdx; }
	_bool		Check_AnimationFrame(void);
	_bool		IsOnGround(void);
	MOVE_DIR*	GetMoveDir(void);
	void		SoldierChange(void);
	_bool		IsSoldier(void) { return m_bIsSoldier; }
	void		Soldier_Iron_AddVelocity(float fFallVel);
	void		Set_Fire(_bool bFire) { m_bFire = bFire; }
	_bool		IsAbleReload(void) { return m_bAbleReload; }
	void		Reload(void);

public:
	void		UpdateDir(void);

	//현재 카메라 상태가 Dynamic일 경우  Player Update 실행X
	_bool		DynamicCameraCheck(void); 

	//server
	void		SendPacketAlways(void);

private:
	CInput*			m_pInput;
	XMFLOAT3		m_vLook;

	XMFLOAT4X4		m_matEquipBone[2];
	CGun*			m_pEquipment[2];
	_int			m_iEquipBone;

	//Component
	CGravity*		m_pComGravity;
	FLOAT			m_fTimeDelta;

	//Player Animation
	DWORD			m_dwState;
	DWORD			m_dwAniIdx;
	CStateMachine*	m_pComStateMachine;
	_bool			m_bKey[KEY_END];
	MOVE_DIR		m_eMoveDir;
	XMFLOAT3		m_vMoveDir;
	//Player Mesh
	_bool			m_bIsSoldier;
	CAnimationInfo* m_pAnimInfo_Normal;
	CAnimationInfo* m_pAnimInfo_Iron;
	MATNODE*		m_pMatBoneNode_Normal;
	MATNODE*		m_pMatBoneNode_Iron;
	_uint			m_uiObjNum_Normal;
	_uint			m_uiObjNum_Iron;
	FLOAT			m_fRollSpeed;
	XMFLOAT3		m_fRollDir;
	FLOAT			m_fRateOfFire;
	_bool			m_bFire;
	_bool			m_bAbleReload;

	//Physx SDK Member Variables =========================
private:
	PxRigidDynamic*		m_pPxActor;
	PxController*		m_pPxCharacterController;

public:
	void	BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, PxControllerManager *pPxControllerManager);
	void	SetPosition(XMFLOAT3 vPosition);
	void	SetRotate(XMFLOAT3 vRot);
	void	PhysXUpdate(const FLOAT& fTimeDelta);

};

#endif //