#include "stdafx.h"
#include "Include.h"
#include "OtherPlayer.h"
#include "AnimationInfo.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "Gun.h"
#include "Management.h"
#include "StateMachine.h"
#include "OtherSoldierDefine.h"
#include "Layer.h"
#include "GunFlash.h"
#include "Management.h"
#include "Scene.h"

COtherPlayer::COtherPlayer(ID3D11DeviceContext* pContext)
: CDynamicObject(pContext)
, m_iEquipBone(0)
, m_dwAniIdx(PLAYER_idle)
, m_bIsSoldier(true)
, m_pPxCharacterController(NULL)

, m_dwState(SOLDIER_IDLE)
, m_pComStateMachine(NULL)
, m_eMoveDir(DIR_END)
, m_vMoveDir(0.f, 0.f, 0.f)
, m_fTimeDelta(0.f)
, m_fRollSpeed(0.f)
, m_fRateOfFire(0.f)
, m_bFire(false)
, m_bAbleReload(false)
, m_fFallAcceleration(9.8f)
, m_fFallvelocity(0.f)
, m_iHP(5)
, m_fBoost(0.0f)
, m_fMaxBoost(100.0f)
, m_bBoostDelay(false)
, m_fDelayCount(0.0f)
 , m_iGunFlashTexture(1)
{
	m_vLook = XMFLOAT3(0.f, 0.f, -1.f);

	XMStoreFloat4x4(&m_matEquipBone[0], XMMatrixIdentity());
	XMStoreFloat4x4(&m_matEquipBone[1], XMMatrixIdentity());

	ZeroMemory(m_pEquipment, sizeof(CEquipment*) * 1);

	m_pServer_PlayerData = new Ser_PLAYER_DATA;

	m_fSpeed = 40.f;
}

COtherPlayer::~COtherPlayer(void)
{
}

COtherPlayer* COtherPlayer::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	COtherPlayer* pObject = new COtherPlayer(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	return pObject;
}

COtherPlayer* COtherPlayer::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, XMFLOAT3 vPos, int iID)
{
	COtherPlayer* pObject = new COtherPlayer(pContext);
	pObject->m_iID = iID;

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	pObject->m_pTransform->m_vPos = vPos;
	return pObject;
}

HRESULT COtherPlayer::Initialize(ID3D11Device* pGraphicDev)
{
	m_uiObjNum = MESHNUM_PLAYER;
	m_uiObjNum_Iron = MESHNUM_PLAYER2;

	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	FAILED_CHECK(Prepare_StateMachine());

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_pTransform->m_vAngle.x = 90.f;
	m_pAnimInfo->Set_Key((_ushort)m_dwAniIdx);

	m_pComStateMachine->Enter_State(SOLDIER_IDLE);

	// Equipment
	m_pEquipment[0] = CGun::Create(pGraphicDev, m_pContext);

	if (m_iID % 2 == 0)
		m_iTextureNumber = 0;		//이걸 그냥 전역으로 만들어버려 ?
	else
		m_iTextureNumber = 1;

	//m_pPxCharacterController->setFootPosition(PxExtendedVec3(m_pTransform->m_vPos.x, m_pTransform->m_vPos.y, m_pTransform->m_vPos.z));

	return S_OK;
}

INT COtherPlayer::Update(const FLOAT& fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	UpdateBoost(fTimeDelta);

	CDynamicObject::Update(fTimeDelta);

	//PhysX 함수
	PhysXUpdate(fTimeDelta);

	Operate_StateMAchine(fTimeDelta);
	KeyState(fTimeDelta);

	// Temp	-------------------------------------------------------------------------------

	// Update
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	UpdateDir();

	Update_Equipment(fTimeDelta);

	return 0;
}
void COtherPlayer::Release(void)
{
	if (m_pPxCharacterController)
		m_pPxCharacterController->release();

	CDynamicObject::Release();
	Safe_Release(m_pEquipment[0]);
}

void COtherPlayer::UpdateDir(void)
{
	XMVECTOR vDir;

	vDir = XMLoadFloat3(&m_pTransform->m_vDir);

	vDir = XMVector3TransformNormal(XMLoadFloat3(&m_vLook), XMLoadFloat4x4(&m_pTransform->m_matWorld));

	vDir = XMVector3Normalize(vDir);

	XMStoreFloat3(&m_pTransform->m_vDir, vDir);
}

HRESULT COtherPlayer::Ready_Component(ID3D11Device* pGraphicDev)
{
	if (FAILED(CDynamicObject::Ready_Component()))
		return E_FAIL;

	m_pAnimInfo_Normal = m_pAnimInfo;
	m_pMatBoneNode_Normal = m_pMatBoneNode;
	m_uiObjNum_Normal = m_uiObjNum;

	CComponent* pComponent = NULL;

	// AnimationInfo	Iron
	pComponent = CAnimationInfo::Create(MESHNUM(m_uiObjNum_Iron));
	m_pAnimInfo_Iron = dynamic_cast<CAnimationInfo*>(pComponent);
	if (pComponent == NULL) return E_FAIL;
	m_mapComponent.insert(MAPCOMPONENT::value_type(L"Com_AnimInfo_Iron", pComponent));
	// Matrix Bone		Iron
	m_pMatBoneNode_Iron = new MATNODE;
	CMeshMgr::GetInstance()->CreateBoneNode(m_uiObjNum_Iron, m_pMatBoneNode_Iron);

	//StateMachine
	pComponent = m_pComStateMachine = CStateMachine::Create(SOLDIER_END);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"StateMachine", pComponent));

	return S_OK;
}

void COtherPlayer::Update_Equipment(const FLOAT& fTimeDelta)
{
	m_matEquipBone[0] = CMeshMgr::GetInstance()->Get_TransMeshBone(m_uiObjNum, 0, m_iEquipBone, m_pMatBoneNode);

	if (XMMatrixIsIdentity(XMLoadFloat4x4(&m_matEquipBone[0])))
		return;

	XMMATRIX matWorld = XMLoadFloat4x4(&m_pTransform->m_matWorld);
	XMStoreFloat4x4(&m_matEquipBone[0], XMLoadFloat4x4(&m_matEquipBone[0]) * matWorld);

	m_pEquipment[0]->SetParent(m_matEquipBone[0]);
	m_pEquipment[0]->Update(fTimeDelta);


}

HRESULT COtherPlayer::Prepare_StateMachine(void)
{
	NULL_CHECK_RETURN(m_pComStateMachine, E_FAIL);
	CState*	pState = NULL;

	pState = COtherSoldierIdle::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = COtherSoldierMove::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = COtherSoldierLying::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = COtherSoldierRoll::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = COtherSoldierJump::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	return S_OK;
}

void COtherPlayer::Operate_StateMAchine(const FLOAT& fTimeDelta)
{
	if (m_iHP <= 0)
	{
		if (m_dwState != SOLDIER_DEAD)
		{
			m_dwState = SOLDIER_DEAD;

			if (m_bIsSoldier)
				PlayAnimation(PLAYER_Death2);
			else
				PlayAnimation(PLAYER_Iron_Death2);
		}
		else
		{
			if (Check_AnimationFrame())
				m_pAnimInfo->Set_PlayAni(false);
		}
	}
	else
	{
		if (m_dwState == SOLDIER_DEAD && Check_AnimationFrame())
		{
			m_dwState = SOLDIER_IDLE;
			if (m_bIsSoldier)
				PlayAnimation(PLAYER_idle);
			else
				PlayAnimation(PLAYER_Iron_Idle);
			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
		}

		KeyCheck();

		DWORD dwState = m_pComStateMachine->Get_State();

		switch (dwState)
		{
		case SOLDIER_IDLE:
			if (m_dwState == SOLDIER_MOVE)
				m_pComStateMachine->Enter_State(SOLDIER_MOVE);
			if (m_dwState == SOLDIER_LYING)
				m_pComStateMachine->Enter_State(SOLDIER_LYING);
			if (m_dwState == SOLDIER_JUMP)
			{
				m_pComStateMachine->Enter_State(SOLDIER_JUMP);
				m_fFallvelocity = 120.f;
			}
			break;
		case SOLDIER_MOVE:
			if (m_dwState == SOLDIER_IDLE)
				m_pComStateMachine->Enter_State(SOLDIER_IDLE);
			if (m_dwState == SOLDIER_ROLL)
			{
				m_pComStateMachine->Enter_State(SOLDIER_ROLL);
				m_fFallvelocity = 60.f;
				m_fRollSpeed = m_fSpeed * 1.7f;
				m_fRollDir = m_vMoveDir;
			}

			if (m_dwState == SOLDIER_JUMP)
			{
				m_pComStateMachine->Enter_State(SOLDIER_JUMP);
				m_fFallvelocity = 120.f;
			}

			break;

		case SOLDIER_LYING:
			if (m_dwState == SOLDIER_IDLE)
				m_pComStateMachine->Enter_State(SOLDIER_IDLE);
			break;
		case SOLDIER_ROLL:
			if (m_dwState == SOLDIER_IDLE)
				m_pComStateMachine->Enter_State(SOLDIER_IDLE);
			if (m_dwState == SOLDIER_MOVE)
				m_pComStateMachine->Enter_State(SOLDIER_MOVE);
			break;
		case SOLDIER_JUMP:
			if (m_dwState == SOLDIER_IDLE)
				m_pComStateMachine->Enter_State(SOLDIER_IDLE);
			break;
		default:
			break;
		}
		m_pComStateMachine->Update_State(dwState);
	}
}

void COtherPlayer::KeyCheck(void)
{
	m_eMoveDir = DIR_END;
	m_vMoveDir = XMFLOAT3(0.f, 0.f, 0.f);

	if (m_bKey[KEY_UP] == true && m_bKey[KEY_DOWN] == true)
	{
		m_bKey[KEY_UP] = false;
		m_bKey[KEY_DOWN] = false;
	}
	if (m_bKey[KEY_LEFT] == true && m_bKey[KEY_RIGHT] == true)
	{
		m_bKey[KEY_LEFT] = false;
		m_bKey[KEY_RIGHT] = false;
	}

	if (!m_bKey[KEY_UP] && !m_bKey[KEY_DOWN] && !m_bKey[KEY_LEFT] && !m_bKey[KEY_RIGHT])
		return;

	XMVECTOR vDir = XMVectorSet(m_pTransform->m_vDir.x, 0.f, m_pTransform->m_vDir.z, 0.0f);
	XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR vRight = XMVector3Cross(vUp, vDir);
	XMVECTOR vMoveDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	vDir = XMVector3Normalize(vDir);
	vRight = XMVector3Normalize(vRight);

	if (m_bKey[KEY_UP])
	{
		vMoveDir += vDir;

		if (m_bKey[KEY_LEFT])
		{
			vMoveDir -= vRight;
			m_eMoveDir = DIR_UL;
		}
		else if (m_bKey[KEY_RIGHT])
		{
			vMoveDir += vRight;
			m_eMoveDir = DIR_UR;
		}
		else
			m_eMoveDir = DIR_U;
	}
	else if (m_bKey[KEY_DOWN])
	{
		vMoveDir -= vDir;
		if (m_bKey[KEY_LEFT])
		{
			vMoveDir -= vRight;
			m_eMoveDir = DIR_DL;
		}
		else if (m_bKey[KEY_RIGHT])
		{
			vMoveDir += vRight;
			m_eMoveDir = DIR_DR;
		}
		else
			m_eMoveDir = DIR_D;
	}
	else if (m_bKey[KEY_LEFT])
	{
		vMoveDir -= vRight;
		m_eMoveDir = DIR_L;
	}
	else if (m_bKey[KEY_RIGHT])
	{
		vMoveDir += vRight;
		m_eMoveDir = DIR_R;
	}
	vMoveDir = XMVector3Normalize(vMoveDir);
	XMStoreFloat3(&m_vMoveDir, vMoveDir);
}

void COtherPlayer::KeyState(const FLOAT& fTimeDelta)
{
	if (m_bIsSoldier)
		Soldier_Move(fTimeDelta);
	else
		Soldier_Iron_Move(fTimeDelta);

	Soldier_Fire(fTimeDelta);
}

void COtherPlayer::Soldier_Move(const FLOAT& fTimeDelta)
{
	switch (m_dwState)
	{
	case SOLDIER_MOVE:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx == PLAYER_sprint)
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 1.4f, 0, fTimeDelta, PxControllerFilters());
		else
		{
			if (m_eMoveDir == DIR_D || m_eMoveDir == DIR_DL || m_eMoveDir == DIR_DR)
				m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 0.7f, 0, fTimeDelta, PxControllerFilters());
			else
				m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		}
		break;
	case SOLDIER_LYING:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx != PLAYER_Lying && m_dwAniIdx != PLAYER_LyingShoot)
		{
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 0.25f, 0, fTimeDelta, PxControllerFilters());
		}
		break;
	case SOLDIER_JUMP:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		break;
	case SOLDIER_ROLL:
		//vDir = XMLoadFloat3(&m_fRollDir);
		m_pPxCharacterController->move(PxVec3(m_fRollDir.x, m_fRollDir.y, m_fRollDir.z) * m_fRollSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());

		m_fRollSpeed -= 20.f * fTimeDelta;
		break;
	}
}

void COtherPlayer::Soldier_Iron_Move(const FLOAT& fTimeDelta)
{
	switch (m_dwState)
	{
	case SOLDIER_MOVE:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx == PLAYER_Iron_Sprint)
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 1.4f, 0, fTimeDelta, PxControllerFilters());
		else
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		break;
	case SOLDIER_LYING:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx != PLAYER_Iron_Lying && m_dwAniIdx != PLAYER_Iron_LyingShoot)
		{
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 0.25f, 0, fTimeDelta, PxControllerFilters());
		}
		break;
	case SOLDIER_JUMP:
		//vDir = XMLoadFloat3(&m_vMoveDir);
		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		break;
	case SOLDIER_ROLL:
		//vDir = XMLoadFloat3(&m_fRollDir);
		m_pPxCharacterController->move(PxVec3(m_fRollDir.x, m_fRollDir.y, m_fRollDir.z) * m_fRollSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		m_fRollSpeed -= 20.f * fTimeDelta;
		break;
	}
}

void COtherPlayer::Soldier_Fire(const FLOAT& fTimeDelta)
{
	if (!m_bKey[KEY_LCLICK])
	{
		m_fRateOfFire = 0.f;
		return;
	}

	m_fRateOfFire += fTimeDelta;
	if (m_fRateOfFire >= 0.2f)
	{
		m_fRateOfFire = 0.1f;

		//Player의 월드 총구 포지션 설정
		XMFLOAT4X4 vmatWorld = ((CTransform*)m_pEquipment[0]->Get_Component(L"Com_Transform"))->m_matWorld;
		XMFLOAT3 m_vWorldGunPos;

		if (m_bIsSoldier)
		{
			//x 가 z축의 역할을하네   z+가 x-
			m_vWorldGunPos = XMFLOAT3(50.f, -27.f, 40.f);
		}
		else
		{
			//x+가 z+축 z축 -x
			m_vWorldGunPos = XMFLOAT3(45.f, 5.f, 0.f);
		}

		XMStoreFloat3(&m_vWorldGunPos, XMVector3TransformCoord(XMLoadFloat3(&m_vWorldGunPos), XMLoadFloat4x4(&vmatWorld)));
		CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");

		//GunFlash
		CGameObject* pGunFlash = CGunFlash::Create(m_pContext);
		pGunFlash->SetTransformPosition(XMFLOAT3(m_vWorldGunPos.x, m_vWorldGunPos.y, m_vWorldGunPos.z));

		if (m_iGunFlashTexture == 1)
		{
			m_iGunFlashTexture = 0;
			pGunFlash->SetTransformScale(XMFLOAT3(70.f, 50.f, 50.f));
			((CGunFlash*)pGunFlash)->SetAlphaSpeed(15.f);
		}
		else
		{
			m_iGunFlashTexture = 1;
			pGunFlash->SetTransformScale(XMFLOAT3(30.f, 30.f, 30.f));
			((CGunFlash*)pGunFlash)->SetAlphaSpeed(15.f);
		}

		pGunFlash->Set_TextureNumber(m_iGunFlashTexture);
		pLayer->Ready_Object(L"Effect", pGunFlash);
	}
}

void COtherPlayer::SetPlayerData(XMFLOAT3 vPos, float fAngle)
{
	m_pPxCharacterController->setFootPosition(PxExtendedVec3(vPos.x, vPos.y, vPos.z));

	_float m_fRevice = 0.5f; //Player의 Y보정값(발이 지면에 안박히게 보정)

	if (!m_bIsSoldier)
		m_fRevice = 0.0f;

	m_pTransform->m_vPos = XMFLOAT3((_float)m_pPxCharacterController->getFootPosition().x, 
									(_float)m_pPxCharacterController->getFootPosition().y + m_fRevice, 
									(_float)m_pPxCharacterController->getFootPosition().z);

	m_pTransform->m_vPos.y += m_fRevice;
	m_pTransform->m_vAngle.y = fAngle;

}

void COtherPlayer::PlayAnimation(DWORD dwAniIdx, bool bImmediate)
{
	if (bImmediate)
		m_pAnimInfo->Set_KeyImm((_ushort)dwAniIdx);
	m_pAnimInfo->Set_Key((_ushort)dwAniIdx);
	m_dwAniIdx = dwAniIdx;
}

void COtherPlayer::SoldierChange(void)
{
	if (m_bIsSoldier)
	{
		m_bIsSoldier = false;
		m_uiObjNum = m_uiObjNum_Iron;
		m_pAnimInfo = m_pAnimInfo_Iron;
		m_pMatBoneNode = m_pMatBoneNode_Iron;
		PlayAnimation((_ushort)PLAYER_Iron_Idle, true);
		((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_SPECIALGUN);
		m_iEquipBone = 2;
	}
	else
	{
		m_bIsSoldier = true;
		m_uiObjNum = m_uiObjNum_Normal;
		m_pAnimInfo = m_pAnimInfo_Normal;
		m_pMatBoneNode = m_pMatBoneNode_Normal;
		PlayAnimation((_ushort)PLAYER_idle, true);
		((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_GUN);
		m_iEquipBone = 0;
	}
	m_pAnimInfo->Update(1.f);
}

MOVE_DIR* COtherPlayer::GetMoveDir(void)
{
	return &m_eMoveDir;
}

_bool COtherPlayer::Check_AnimationFrame(void)
{
	if (m_pAnimInfo->Get_CurFrame() >= m_pAnimInfo->Get_LastFrame())
		return true;
	return false;
}

_bool COtherPlayer::IsOnGround(void)
{
	PxControllerState   m_pPxState;

	//피직스 객체의 상태값을 m_pPxState에 넣어준다.
	m_pPxCharacterController->getState(m_pPxState);

	/*return m_pComGravity->Get_OnGround();*/
	if (m_pPxState.collisionFlags == PxControllerCollisionFlag::eCOLLISION_DOWN)
		return true;
	return false;
}

void COtherPlayer::Soldier_Iron_AddVelocity(float fFallVel)
{
	m_fFallvelocity += 1000.f * m_fTimeDelta;
}

void COtherPlayer::Set_KeyState(_short sBitKeyState, float fAngle, short sHP)
{
	m_bKey[KEY_UP] = sBitKeyState & 1;
	m_bKey[KEY_DOWN] = sBitKeyState & 2;
	m_bKey[KEY_LEFT] = sBitKeyState & 4;
	m_bKey[KEY_RIGHT] = sBitKeyState & 8;
	m_bKey[KEY_Q] = sBitKeyState & 16;
	m_bKey[KEY_R] = sBitKeyState & 32;
	m_bKey[KEY_SHIFT] = sBitKeyState & 64;
	m_bKey[KEY_CTRL] = sBitKeyState & 128;
	m_bKey[KEY_SPACE] = sBitKeyState & 256;
	m_bKey[KEY_LCLICK] = sBitKeyState & 512;
	m_bKey[KEY_RCLICK] = sBitKeyState & 1024;

	m_pTransform->m_vAngle.y = fAngle;
	m_iHP = sHP;
}

void COtherPlayer::PhysXUpdate(const FLOAT& fTimeDelta)
{
	//PhysX에 값을 전달해준다. 중력
	m_fFallvelocity -= m_fFallAcceleration * fTimeDelta * 50.f;

	if (m_fFallvelocity < -1000.f)
		m_fFallvelocity = -1000.f;

	m_pPxCharacterController->move(PxVec3(0, 1.f, 0) * fTimeDelta * m_fFallvelocity, 0, fTimeDelta, PxControllerFilters());

	PxControllerState   m_pPxState;

	//피직스 객체의 상태값을 m_pPxState에 넣어준다.
	m_pPxCharacterController->getState(m_pPxState);

	//윗쪽 충돌하거나 아랫쪽 충돌하면 m_fFallvelocity = 0.0f
	if (m_pPxState.collisionFlags == PxControllerCollisionFlag::eCOLLISION_DOWN ||
		m_pPxState.collisionFlags == PxControllerCollisionFlag::eCOLLISION_UP)
		m_fFallvelocity = 0.f;

	//현재 PhysX의 값으로 객체의 월드행렬을 만들어준다.
	m_pTransform->m_vPos = XMFLOAT3((_float)m_pPxCharacterController->getFootPosition().x, (_float)m_pPxCharacterController->getFootPosition().y, (_float)m_pPxCharacterController->getFootPosition().z);


	_float m_fRevice = 0.5f; //Player의 Y보정값(발이 지면에 안박히게 보정)

	if (!m_bIsSoldier)
		m_fRevice = 0.0f;

	XMMATRIX matTrans = XMMatrixTranslation(m_pTransform->m_vPos.x, m_pTransform->m_vPos.y + m_fRevice, m_pTransform->m_vPos.z);
	XMMATRIX matScale = XMMatrixScaling(m_pTransform->m_vScale.x, m_pTransform->m_vScale.y, m_pTransform->m_vScale.z);

	XMMATRIX matRotX = XMMatrixRotationX((_float)D3DXToRadian(m_pTransform->m_vAngle.x));
	XMMATRIX matRotY = XMMatrixRotationY((_float)D3DXToRadian(m_pTransform->m_vAngle.y));
	XMMATRIX matRotZ = XMMatrixRotationZ((_float)D3DXToRadian(m_pTransform->m_vAngle.z));

	XMStoreFloat4x4(&m_pTransform->m_matWorld, matScale * matRotX * matRotY * matRotZ * matTrans);
}

void COtherPlayer::UpdateBoost(const FLOAT& fTimeDelta)
{
	_float m_fSpeed = 0.0f;

	if (m_bIsSoldier) //일반상태
	{
		if (m_bBoostDelay)
		{
			m_fDelayCount += fTimeDelta;

			if (m_fDelayCount >= 5.0f)
			{
				m_fDelayCount = 0.0f;
				m_bBoostDelay = false;
			}
		}
		else
		{
			m_fSpeed = 10.f;

			m_fBoost += (fTimeDelta * m_fSpeed);

			if (m_fBoost >= m_fMaxBoost)
				m_fBoost = m_fMaxBoost;
		}
	}
	else //변신상태
	{
		m_fSpeed = 1.f;

		m_fBoost -= (fTimeDelta * m_fSpeed);

		if (m_fBoost <= 0.0f) //변신상태 -> 일반상태로 변환
		{
			//변신상태->일반상태로 변환될경우 Delay를 준다.
			m_bBoostDelay = true;

			m_fBoost = 0.0f;
			m_bIsSoldier = true;
			m_uiObjNum = m_uiObjNum_Normal;
			m_pAnimInfo = m_pAnimInfo_Normal;
			m_pMatBoneNode = m_pMatBoneNode_Normal;
			PlayAnimation((_ushort)PLAYER_idle);
			((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_GUN);
			m_iEquipBone = 0;
			m_pAnimInfo->Update(1.f);
		}
	}
}

void COtherPlayer::BuildObject(PxPhysics * pPxPhysics, PxScene * pPxScene, PxMaterial * pPxMaterial, PxControllerManager * pPxControllerManager)
{

	PxCapsuleControllerDesc	PxCapsuledesc;
	PxCapsuledesc.position = PxExtendedVec3(50, 0, 20);
	PxCapsuledesc.radius = 5.0f;
	PxCapsuledesc.height = 10.0f;
	//캐릭터가 올라갈 수있는 장애물의 최대 높이를 정의합니다. 
	PxCapsuledesc.stepOffset = 2.f;
	
	//캐시 된 볼륨 증가. 
	//성능을 향상시키기 위해 캐싱하는 컨트롤러 주변의 공간입니다.  이것은 1.0f보다 커야하지만 너무 크지 않아야하며, 2.0f보다 낮아야합니다.
	PxCapsuledesc.volumeGrowth = 1.9f;
	//캐릭터가 걸어 갈 수있는 최대 경사. 
	PxCapsuledesc.slopeLimit = cosf(XMConvertToRadians(15.f));
	PxCapsuledesc.upDirection = PxVec3(0, 1, 0);
	PxCapsuledesc.contactOffset = 0.001f; //접촉 오프셋
	PxCapsuledesc.material = pPxMaterial;
	//PxCapsuledesc.nonWalkableMode = PxControllerNonWalkableMode::eFORCE_SLIDING;
	m_pPxCharacterController = pPxControllerManager->createController(PxCapsuledesc);

	m_pTransform->m_vPos = XMFLOAT3(50.f, 0.f, 20.f);
	m_pPxCharacterController->setFootPosition(PxExtendedVec3(m_pTransform->m_vPos.x, m_pTransform->m_vPos.y, m_pTransform->m_vPos.z));
	
	m_strName = "OtherPlayer_";

	m_strName += to_string(m_iID);
	m_pPxCharacterController->getActor()->setName(m_strName.c_str());
}
