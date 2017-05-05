#include "stdafx.h"
#include "Include.h"
#include "Player.h"
#include "AnimationInfo.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "Gun.h"
#include "Management.h"
#include "Input.h"
#include "StateMachine.h"
#include "SoldierDefine.h"
#include "CameraMgr.h"
#include "Layer.h"
#include "DefaultObject.h"
#include "Management.h"
#include "Scene.h"
#include "SphereMesh.h"

XMFLOAT3		g_vPlayerPos;

CPlayer::CPlayer(ID3D11DeviceContext* pContext)
	: CDynamicObject(pContext)
	, m_dwState(SOLDIER_IDLE)
	, m_dwAniIdx(PLAYER_idle)
	, m_pComStateMachine(NULL)
	, m_eMoveDir(DIR_END)
	, m_vMoveDir(0.f, 0.f, 0.f)
	, m_bIsSoldier(true)
	, m_fTimeDelta(0.f)
	, m_iEquipBone(0)
	, m_fRollSpeed(0.f)
	, m_fRateOfFire(0.f)
	, m_bFire(false)
	, m_bAbleReload(false)
	, m_pPxActor(NULL)
	, m_pPxCharacterController(NULL)
	, m_fFallAcceleration(9.8f)
	, m_fFallvelocity(0.f)
{
	m_pInput = CInput::GetInstance();
	m_vLook = XMFLOAT3(0.f, 1.f, 0.f);

	XMStoreFloat4x4(&m_matEquipBone[0], XMMatrixIdentity());
	XMStoreFloat4x4(&m_matEquipBone[1], XMMatrixIdentity());

	ZeroMemory(m_pEquipment, sizeof(CEquipment*) * 2);
	ZeroMemory(&m_bKey, sizeof(bool) * KEY_END);

	m_pServer_PlayerData = new Ser_PLAYER_DATA;

	m_fSpeed = 50.f;
}

CPlayer::~CPlayer(void)
{
}

CPlayer* CPlayer::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CPlayer* pObject = new CPlayer(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	return pObject;
}

CPlayer* CPlayer::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, XMFLOAT3 vPos)
{
	CPlayer* pObject = new CPlayer(pContext);
	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	pObject->m_pTransform->m_vPos = vPos;

	return pObject;
}

HRESULT CPlayer::Initialize(ID3D11Device* pGraphicDev)
{
	m_uiObjNum = MESHNUM_PLAYER;
	m_uiObjNum_Iron = MESHNUM_PLAYER2;

	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	FAILED_CHECK(Prepare_StateMachine());

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_pTransform->m_vAngle.x = 90.f;

	//m_pTransform->m_vPos = g_vPos;

	m_pTransform->m_vPos = XMFLOAT3(10.0f, 0.0f, 10.0f);

	m_pTransform->m_vDir = XMFLOAT3(0.f, 0.f, -1.f);
	m_pAnimInfo->Set_Key((_ushort)m_dwAniIdx);

	m_pComStateMachine->Enter_State(SOLDIER_IDLE);

	//m_pServer_PlayerData->vPos = m_pTransform->m_vPos;

	//g_Client.sendPacket(sizeof(XMFLOAT3),INIT_CLIENT,)

	// Equipment
	m_pEquipment[0] = CGun::Create(pGraphicDev, m_pContext);

	return S_OK;
}

INT CPlayer::Update(const FLOAT& fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	//if (m_pInput->Get_DIKeyState(DIK_C)) //디버그용
	//{
	//	m_pPxCharacterController->setPosition(PxExtendedVec3(10.f, 50.f, 10.f));
	//}

	CDynamicObject::Update(fTimeDelta);
	


	//임시로 만든 Jump
	if (m_pInput->Get_DIKeyState(DIK_C))
	{
		m_pPxCharacterController->move(PxVec3(0.0f, 1.0f, 0.0f) * m_fSpeed * fTimeDelta * 5.f, 0, fTimeDelta, PxControllerFilters());
	}



	//PhysX 함수
	PhysXUpdate(fTimeDelta);

	//Dynamic카메라 체크 함수(Dynamic 카메라일시 Update 안돌린다.
	if (!DynamicCameraCheck())
	{
		Operate_StateMAchine(fTimeDelta);
		KeyState(fTimeDelta);
		SendPacketAlways();
	}

	// Temp	----------------------------------------------------------------------------
	


	// Update
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);


	UpdateDir();

	Update_Equipment(fTimeDelta);

	return 0;
}

void CPlayer::Release(void)
{
	CDynamicObject::Release();
	Safe_Release(m_pEquipment[0]);
}

HRESULT CPlayer::Ready_Component(ID3D11Device* pGraphicDev)
{
	if (FAILED(CDynamicObject::Ready_Component()))
		return E_FAIL;

	CComponent* pComponent = NULL;

	m_pAnimInfo_Normal = m_pAnimInfo;
	m_pMatBoneNode_Normal = m_pMatBoneNode;
	m_uiObjNum_Normal = m_uiObjNum;

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

void CPlayer::Update_Equipment(const FLOAT& fTimeDelta)
{
	m_matEquipBone[0] = CMeshMgr::GetInstance()->Get_TransMeshBone(m_uiObjNum, 0, m_iEquipBone, m_pMatBoneNode);

	XMMATRIX matWorld = XMLoadFloat4x4(&m_pTransform->m_matWorld);
	XMStoreFloat4x4(&m_matEquipBone[0], XMLoadFloat4x4(&m_matEquipBone[0]) * matWorld);
	m_pEquipment[0]->SetParent(m_matEquipBone[0]);
	m_pEquipment[0]->Update(fTimeDelta);
}

HRESULT CPlayer::Prepare_StateMachine(void)
{
	NULL_CHECK_RETURN(m_pComStateMachine, E_FAIL);
	CState*	pState = NULL;

	pState = CSoldierIdle::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = CSoldierMove::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = CSoldierLying::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = CSoldierRoll::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	pState = CSoldierJump::Create(this);
	FAILED_CHECK(m_pComStateMachine->Add_State(pState));

	return S_OK;
}

void CPlayer::Operate_StateMAchine(const FLOAT& fTimeDelta)
{
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
			m_fFallvelocity = 200.f;
		}
		break;
	case SOLDIER_MOVE:
		if (m_dwState == SOLDIER_IDLE)
 			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
		if (m_dwState == SOLDIER_ROLL)
		{
			m_pComStateMachine->Enter_State(SOLDIER_ROLL);
			m_fFallvelocity = 70.f;
			m_fRollSpeed = m_fSpeed * 2.f;
			m_fRollDir = m_vMoveDir;
		}

		if (m_dwState == SOLDIER_JUMP)
		{
			m_pComStateMachine->Enter_State(SOLDIER_JUMP);
			m_fFallvelocity = 200.f;
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

void CPlayer::PlayAnimation(DWORD dwAniIdx, bool bImmediate)
{
	if (bImmediate)
		m_pAnimInfo->Set_KeyImm((_ushort)dwAniIdx);
	m_pAnimInfo->Set_Key((_ushort)dwAniIdx);
	m_dwAniIdx = dwAniIdx;

	Ser_ANIMATION_DATA m_AnimationData;
	m_AnimationData.size = sizeof(Ser_ANIMATION_DATA);
	m_AnimationData.type = CLIENT_ANIMATION;
	m_AnimationData.ID = g_myid;
	m_AnimationData.bImmediate = bImmediate;
	m_AnimationData.dwAniIdx = dwAniIdx;
	m_AnimationData.bIsSoldier = m_bIsSoldier;
	g_Client->sendPacket(sizeof(Ser_ANIMATION_DATA), CLIENT_ANIMATION, reinterpret_cast<BYTE*>(&m_AnimationData));

}

bool CPlayer::Check_AnimationFrame(void)
{
	if (m_pAnimInfo->Get_CurFrame() >= m_pAnimInfo->Get_LastFrame())
		return true;
	return false;
}

bool CPlayer::IsOnGround(void)
{
	PxControllerState   m_pPxState;

	m_pPxCharacterController->getState(m_pPxState);

	//피직스 객체의 상태값을 m_pPxState에 넣어준다.
	m_pPxCharacterController->getState(m_pPxState);

	/*return m_pComGravity->Get_OnGround();*/
	if (m_pPxState.collisionFlags == PxControllerCollisionFlag::eCOLLISION_DOWN)
		return true;
	return false;
}

MOVE_DIR* CPlayer::GetMoveDir(void)
{
	return &m_eMoveDir;
}

void CPlayer::SoldierChange(void)
{//처음에 좆같은거는 어떻게?
	if (m_bIsSoldier)
	{
		m_bIsSoldier = false;
		m_uiObjNum = m_uiObjNum_Iron;
		m_pAnimInfo = m_pAnimInfo_Iron;
		m_pMatBoneNode = m_pMatBoneNode_Iron;
		PlayAnimation((_ushort)PLAYER_Iron_Idle);
		((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_SPECIALGUN);
		m_iEquipBone = 2;
	}
	else
	{
		m_bIsSoldier = true;
		m_uiObjNum = m_uiObjNum_Normal;
		m_pAnimInfo = m_pAnimInfo_Normal;
		m_pMatBoneNode = m_pMatBoneNode_Normal;
		PlayAnimation((_ushort)PLAYER_idle);
		((CGun*)m_pEquipment[0])->ChangeWeapon(MESHNUM_GUN);
		m_iEquipBone = 0;
	}
	m_pAnimInfo->Update(1.f);
}

void CPlayer::KeyCheck(void)
{
	m_eMoveDir = DIR_END;
	m_vMoveDir = XMFLOAT3(0.f, 0.f, 0.f);
	ZeroMemory(&m_bKey, sizeof(bool) * KEY_END);

	if (m_pInput->Get_DIKeyState(DIK_W))
		m_bKey[KEY_UP] = true;
	if (m_pInput->Get_DIKeyState(DIK_S))
		m_bKey[KEY_DOWN] = true;
	if (m_pInput->Get_DIKeyState(DIK_A))
		m_bKey[KEY_LEFT] = true;
	if (m_pInput->Get_DIKeyState(DIK_D))
		m_bKey[KEY_RIGHT] = true;

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

void CPlayer::KeyState(const FLOAT& fTimeDelta)
{
	_long lMouseMove = 0;

	if (lMouseMove = m_pInput->Get_DIMouseMove(CInput::DIMS_X))
	{
		if (m_pTransform->m_vAngle.y >= 360.f)
			m_pTransform->m_vAngle.y = 0.f;
		if (m_pTransform->m_vAngle.y < 0.f)
			m_pTransform->m_vAngle.y = 360.f;
		m_pTransform->m_vAngle.y += lMouseMove * fTimeDelta * 5.f;
	}

	if(m_bIsSoldier)
		Soldier_Move(fTimeDelta);
	else
		Soldier_Iron_Move(fTimeDelta);

	Soldier_Fire(fTimeDelta);
}

void CPlayer::Soldier_Move(const FLOAT& fTimeDelta)
{
	m_fSpeed = 50.f;
	XMVECTOR vDir;// = XMLoadFloat3(&m_vMoveDir);
	XMVECTOR vPos = XMLoadFloat3(&m_pTransform->m_vPos);



	switch (m_dwState)
	{
	case SOLDIER_MOVE:
		vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx == PLAYER_sprint)
		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 1.5f, 0, fTimeDelta, PxControllerFilters());
		else
		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		//XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		//m_pTransform->Update(fTimeDelta);
		break;
	case SOLDIER_LYING:
		vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx != PLAYER_Lying && m_dwAniIdx != PLAYER_LyingShoot)
		{
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 0.5f, 0, fTimeDelta, PxControllerFilters());
			//vPos += vDir * m_fSpeed * fTimeDelta * 0.5f;
			//XMStoreFloat3(&m_pTransform->m_vPos, vPos);
			//m_pTransform->Update(fTimeDelta);
		}
		break;
	case SOLDIER_JUMP:
		vDir = XMLoadFloat3(&m_vMoveDir);
		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		//vPos += vDir * m_fSpeed * fTimeDelta;
		//XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		//m_pTransform->Update(fTimeDelta);
		break;
	case SOLDIER_ROLL:
		vDir = XMLoadFloat3(&m_fRollDir);

		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fRollSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());

		//vPos += vDir * m_fRollSpeed * fTimeDelta;
		m_fRollSpeed -= 20.f * fTimeDelta;
		//XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		//m_pTransform->Update(fTimeDelta);
		break;
	}
}

void CPlayer::Soldier_Iron_Move(const FLOAT& fTimeDelta)
{
	m_fSpeed = 70.f;
	XMVECTOR vDir;// = XMLoadFloat3(&m_vMoveDir);
	XMVECTOR vPos = XMLoadFloat3(&m_pTransform->m_vPos);

	switch (m_dwState)
	{
	case SOLDIER_MOVE:
		vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx == PLAYER_Iron_Sprint)
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 1.5f, 0, fTimeDelta, PxControllerFilters());
		else
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		break;
	case SOLDIER_LYING:
		vDir = XMLoadFloat3(&m_vMoveDir);
		if (m_dwAniIdx != PLAYER_Iron_Lying && m_dwAniIdx != PLAYER_Iron_LyingShoot)
		{
			//vPos += vDir * m_fSpeed * fTimeDelta * 0.5f;
			//XMStoreFloat3(&m_pTransform->m_vPos, vPos);
			//m_pTransform->Update(fTimeDelta);
			m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta * 0.5f, 0, fTimeDelta, PxControllerFilters());
		}
		break;
	case SOLDIER_JUMP:
		vDir = XMLoadFloat3(&m_vMoveDir);
		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		break;
	case SOLDIER_ROLL:
		vDir = XMLoadFloat3(&m_fRollDir);
		//vPos += vDir * m_fRollSpeed * fTimeDelta;
		//m_fRollSpeed -= 20.f * fTimeDelta;
		//XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		//m_pTransform->Update(fTimeDelta);
		m_pPxCharacterController->move(PxVec3(m_vMoveDir.x, m_vMoveDir.y, m_vMoveDir.z) * m_fRollSpeed * fTimeDelta, 0, fTimeDelta, PxControllerFilters());
		m_fRollSpeed -= 20.f * fTimeDelta;
		break;
	}
}

void CPlayer::Soldier_Fire(const FLOAT& fTimeDelta)
{
	if (!m_bFire)
	{
		m_fRateOfFire = 0.f;
		return;
	}
	m_fRateOfFire += fTimeDelta;
	if (m_fRateOfFire >= 0.2f)
	{
		m_fRateOfFire = 0.1f;
		if (((CGun*)m_pEquipment[0])->Fire())
		{

			XMFLOAT3 vDir = CCameraMgr::GetInstance()->Get_CurCameraLookAt();
			XMFLOAT3 vPos = CCameraMgr::GetInstance()->Get_CurCameraEye();

			PxVec3 OriginPos = PxVec3(vPos.x, vPos.y, vPos.z);
			PxVec3 Dir		 = PxVec3(vDir.x, vDir.y, vDir.z);


			PxReal maxDistance = 1000.0f;
			PxRaycastBuffer hit;
			PxQueryFilterData fd;
			fd.flags |= PxQueryFlag::eANY_HIT;

			bool status = false;
			status = m_pScene->raycast(OriginPos, Dir, maxDistance, hit, PxHitFlags(PxHitFlag::eDEFAULT), fd);

			if (status == true)
			{

				testpos = XMFLOAT3(hit.block.position.x, hit.block.position.y, hit.block.position.z);

				CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");

				CGameObject* pGameObject = CSphereMesh::Create(m_pContext, 2.f, &testpos);


				pLayer->Ready_Object(L"test", pGameObject);

			}
		}
		else
		{
			m_bAbleReload = true;
		}
	}
}

void CPlayer::Soldier_Iron_AddVelocity(float fFallVel)
{
	m_fFallvelocity += 1000.f * m_fTimeDelta;
}

void CPlayer::Reload(void)
{
	((CGun*)m_pEquipment[0])->Reload();
	m_bAbleReload = false;
}

void CPlayer::UpdateDir(void)
{
	XMVECTOR vDir;

	vDir = XMLoadFloat3(&m_pTransform->m_vDir);

	vDir = XMVector3TransformNormal(XMLoadFloat3(&m_vLook), XMLoadFloat4x4(&m_pTransform->m_matWorld));

	vDir = XMVector3Normalize(vDir);

	XMStoreFloat3(&m_pTransform->m_vDir,vDir);
}

_bool CPlayer::DynamicCameraCheck(void)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
	{
		return true;
	}

	return false;
}

void CPlayer::BuildObject(PxPhysics* pPxPhysics, PxScene* pPxScene, PxMaterial *pPxMaterial, PxControllerManager *pPxControllerManager)
{
	//XMFLOAT3 vMin = *(CMeshMgr::GetInstance()->Get_MeshMin(m_uiObjNum));
	//XMFLOAT3 vMax = *(CMeshMgr::GetInstance()->Get_MeshMax(m_uiObjNum));

	//XMFLOAT3 _d3dxvExtents =
	//	XMFLOAT3((abs(vMin.x) + abs(vMax.x)) / 2, (abs(vMin.y) + abs(vMax.y)) / 2, (abs(vMin.z) + abs(vMax.z)) / 2);

	//Player의 바운딩 박스 생성
	//PxBoxControllerDesc PxBoxdesc;
	//PxBoxdesc.position = PxExtendedVec3(0, 0, 0);
	//PxBoxdesc.halfForwardExtent = _d3dxvExtents.y / 2;
	//PxBoxdesc.halfSideExtent = _d3dxvExtents.z / 2;
	//PxBoxdesc.halfHeight = _d3dxvExtents.x / 2;
	//PxBoxdesc.slopeLimit = 10;
	//PxBoxdesc.contactOffset = 0.00001;
	//PxBoxdesc.upDirection = PxVec3(0, 1, 0);
	//PxBoxdesc.material = pPxMaterial;
	
	m_pScene = pPxScene;
		
	PxCapsuleControllerDesc	PxCapsuledesc;
	PxCapsuledesc.position = PxExtendedVec3(0, 0, 0);
	PxCapsuledesc.radius = 5.0f;
	PxCapsuledesc.height = 5.0f;

	//캐릭터가 올라갈 수있는 장애물의 최대 높이를 정의합니다. 
	PxCapsuledesc.stepOffset = 4.f;

	//등반모드
	PxCapsuledesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	PxCapsuledesc.nonWalkableMode = PxControllerNonWalkableMode::eFORCE_SLIDING;
	
	//캐시 된 볼륨 증가. 
	//성능을 향상시키기 위해 캐싱하는 컨트롤러 주변의 공간입니다.  이것은 1.0f보다 커야하지만 너무 크지 않아야하며, 2.0f보다 낮아야합니다.
	PxCapsuledesc.volumeGrowth = 1.9f;


	//캐릭터가 걸어 갈 수있는 최대 경사. 
	PxCapsuledesc.slopeLimit = 5.f;
	
	PxCapsuledesc.upDirection = PxVec3(0, 1, 0);
	PxCapsuledesc.contactOffset = 0.00001;
	PxCapsuledesc.contactOffset = 0.05f; //접촉 오프셋
	PxCapsuledesc.material = pPxMaterial;


	m_pPxCharacterController = pPxControllerManager->createController(PxCapsuledesc);

}

void CPlayer::SetPosition(XMFLOAT3 vPosition)
{
	m_pPxCharacterController->setPosition(PxExtendedVec3(vPosition.x, vPosition.y, vPosition.z));
}

void CPlayer::SetRotate(XMFLOAT3 vRot)
{

}

//PhysX 함수
void CPlayer::PhysXUpdate(const FLOAT& fTimeDelta)
{
	//PhysX에 값을 전달해준다. 중력
	m_fFallvelocity -= m_fFallAcceleration * fTimeDelta * 50.f;
	if (m_fFallvelocity < -1000.f)
		m_fFallvelocity = -1000.f;
	m_pPxCharacterController->move(PxVec3(0, 1.f, 0) * fTimeDelta * m_fFallvelocity, 0, fTimeDelta, PxControllerFilters());

	PxControllerState   m_pPxState;

	m_pPxCharacterController->getState(m_pPxState);

	//피직스 객체의 상태값을 m_pPxState에 넣어준다.
	m_pPxCharacterController->getState(m_pPxState);
	if (m_pPxState.collisionFlags == PxControllerCollisionFlag::eCOLLISION_DOWN)
		m_fFallvelocity = 0.f;


	//현재 PhysX의 값으로 객체의 월드행렬을 만들어준다.
	m_pTransform->m_vPos = XMFLOAT3(m_pPxCharacterController->getFootPosition().x, m_pPxCharacterController->getFootPosition().y, m_pPxCharacterController->getFootPosition().z);

	_float m_fRevice = 0.5f; //Player의 Y보정값(발이 지면에 안박히게 보정)
	
	XMMATRIX matTrans = XMMatrixTranslation(m_pPxCharacterController->getFootPosition().x, m_pPxCharacterController->getFootPosition().y + m_fRevice, m_pPxCharacterController->getFootPosition().z);
	XMMATRIX matScale = XMMatrixScaling(m_pTransform->m_vScale.x, m_pTransform->m_vScale.y, m_pTransform->m_vScale.z);

	XMMATRIX matRotX = XMMatrixRotationX((_float)D3DXToRadian(m_pTransform->m_vAngle.x));
	XMMATRIX matRotY = XMMatrixRotationY((_float)D3DXToRadian(m_pTransform->m_vAngle.y));
	XMMATRIX matRotZ = XMMatrixRotationZ((_float)D3DXToRadian(m_pTransform->m_vAngle.z));

	XMStoreFloat4x4(&m_pTransform->m_matWorld, matScale * matRotX * matRotY * matRotZ * matTrans);

}

void CPlayer::SendPacketAlways(void)
{
	Ser_PLAYER_DATA m_pPlayerData;
	m_pPlayerData.size = sizeof(Ser_PLAYER_DATA);
	m_pPlayerData.type = CLIENT_POSITION;
	m_pPlayerData.ID = g_myid;
	m_pPlayerData.vPos = m_pTransform->m_vPos;
	m_pPlayerData.vDir = m_pTransform->m_vAngle;
	g_Client->sendPacket(sizeof(Ser_PLAYER_DATA), CLIENT_POSITION, reinterpret_cast<BYTE*>(&m_pPlayerData));
}
