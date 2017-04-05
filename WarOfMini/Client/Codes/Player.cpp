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
#include "Gravity.h"
#include "Layer.h"
#include "DefaultObject.h"
#include "Management.h"
#include "Scene.h"

XMFLOAT3		g_vPlayerPos;

CPlayer::CPlayer(ID3D11DeviceContext* pContext)
	: CDynamicObject(pContext)
	, m_dwState(SOLDIER_IDLE)
	, m_dwAniIdx(PLAYER_idle)
	, m_pComStateMachine(NULL)
	, m_pCalculator(NULL)
	, m_pRigidBody(NULL)
	, m_pComGravity(NULL)
	, m_eMoveDir(DIR_END)
	, m_vMoveDir(0.f, 0.f, 0.f)
{
	m_pInput = CInput::GetInstance();
	m_vLook = XMFLOAT3(0.f, 1.f, 0.f);

	XMStoreFloat4x4(&m_matEquipBone[0], XMMatrixIdentity());
	XMStoreFloat4x4(&m_matEquipBone[1], XMMatrixIdentity());

	ZeroMemory(m_pEquipment, sizeof(CEquipment*) * 1);
	ZeroMemory(&m_bKey, sizeof(bool) * KEY_END);

	m_pServer_PlayerData = new Ser_PLAYER_DATA;

	m_fSpeed = 200.f;
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

HRESULT CPlayer::Initialize(ID3D11Device* pGraphicDev)
{
	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	FAILED_CHECK(Prepare_StateMachine());

	m_uiObjNum = MESHNUM_PLAYER;

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_pTransform->m_vAngle.x = 90.f;

	m_pTransform->m_vPos = XMFLOAT3(20.f, 0.f, 20.f);
	m_pTransform->m_vDir = XMFLOAT3(0.f, 0.f, -1.f);
	m_pAnimInfo->Set_Key((_ushort)m_dwAniIdx);

	m_pComStateMachine->Enter_State(SOLDIER_IDLE);

//	m_pServer_PlayerData->vPos = m_pTransform->m_vPos;

//	g_Client.sendPacket()

	return S_OK;
}

INT CPlayer::Update(const FLOAT& fTimeDelta)
{
	if (m_pInput->Get_DIKeyState(DIK_1))
	{
		m_pComGravity->Set_LandOff(50.f);
	}
	if (m_pInput->Get_DIKeyState(DIK_2))
	{
		m_pComGravity->Add_Velocity(10.f);
		m_pComGravity->Set_OnGround(false);
	}

	Collision_Field(fTimeDelta);
	
	CDynamicObject::Update(fTimeDelta);

	//Dynamic카메라 체크 함수(Dynamic 카메라일시 Update 안돌린다.
	if (!DynamicCameraCheck())
	{
		Operate_StateMAchine(fTimeDelta);
		KeyState(fTimeDelta);
	}

	// Temp	-------------------------------------------------------------------------------
	
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
	// Equipment
	m_pEquipment[0] = CGun::Create(pGraphicDev, m_pContext);

	CComponent* pComponent = NULL;
	//StateMachine
	pComponent = m_pComStateMachine = CStateMachine::Create(SOLDIER_END);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"StateMachine", pComponent));

	//Gravity
	pComponent = m_pComGravity = CGravity::Create(100.f);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Gravity", pComponent));

	return S_OK;
}

void CPlayer::Update_Equipment(const FLOAT& fTimeDelta)
{
	m_matEquipBone[0] = CMeshMgr::GetInstance()->Get_TransMeshBone(m_uiObjNum, 0, 0, m_pMatBoneNode);

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
 		{
 			m_pComStateMachine->Enter_State(SOLDIER_MOVE);
 		}
		if (m_dwState == SOLDIER_LYING)
		{
			m_pComStateMachine->Enter_State(SOLDIER_LYING);
		}
		if (m_dwState == SOLDIER_JUMP)
		{
			m_pComStateMachine->Enter_State(SOLDIER_JUMP);
			m_pComGravity->Set_LandOff(50.f);				// 일단 이렇게
		}
		break;
	case SOLDIER_MOVE:
		if (m_dwState == SOLDIER_IDLE)
		{
 			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
 		}
		if (m_dwState == SOLDIER_ROLL)
		{
			m_pComStateMachine->Enter_State(SOLDIER_ROLL);
		}
		break;
	case SOLDIER_LYING:
		if (m_dwState == SOLDIER_IDLE)
		{
			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
		}
		break;
	case SOLDIER_ROLL:
		if (m_dwState == SOLDIER_IDLE)
		{
			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
		}
		if (m_dwState == SOLDIER_MOVE)
		{
			m_pComStateMachine->Enter_State(SOLDIER_MOVE);
		}
		break;
	case SOLDIER_JUMP:
		if (m_dwState == SOLDIER_IDLE)
		{
			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
		}
		break;
	default:
		break;
	}
	m_pComStateMachine->Update_State(dwState);
}

void CPlayer::Collision_Field(const FLOAT& fTimeDelta)
{
	m_pComGravity->Move_Inertia(fTimeDelta, &m_pTransform->m_vPos);//로봇 날아다닐때 쓰면 좋을듯. Add_Velocity
	if (m_pTransform->m_vPos.y <= 0.f)
	{
		m_pTransform->m_vPos.y = 0.f;
		m_pComGravity->Set_LandOn();
	}

	//_float tmin = 1000.0f;
	//XMVECTOR vRayDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	//XMVECTOR vOrigin = XMLoadFloat3(&m_pTransform->m_vPos);



	////반직선의 원점, 방향, 삼각형 정점0, 1, 2, 교점의 매개변수

	//CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");

	//if (pLayer == NULL)
	//	return ;

	//list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"StaticObject");

	//if (pObjList == NULL)
	//	return ;

	//list<CGameObject*>::iterator iter = pObjList->begin();
	//list<CGameObject*>::iterator iter_end = pObjList->end();


	//XMMATRIX	matWorld;

	//XMVECTOR    vecTest;
	//matWorld = XMMatrixInverse(&vecTest, XMLoadFloat4x4(&((CTransform*)((CDefaultObj*)(*iter))->Get_Component(L"Com_Transform"))->m_matWorld));

	//vOrigin = XMVector3TransformCoord(vOrigin, matWorld);
	//vRayDir = XMVector3TransformNormal(vRayDir, matWorld);
	//vRayDir = XMVector3Normalize(vRayDir);


	//for (iter; iter != iter_end; ++iter) //배치된 오브젝트를 순휘
	//{

	//	int nOffset = 3;

	//	int nPrimitives = CMeshMgr::GetInstance()->Get_MeshVtxCnt(((CDefaultObj*)(*iter))->GetObjNum()) / 3;

	//	VTXTEX*	m_pTex = CMeshMgr::GetInstance()->Get_MeshVtxTex(((CDefaultObj*)(*iter))->GetObjNum());


	//	if (m_pTex == NULL)
	//		continue;

	//	XMVECTOR v0, v1, v2;

	//	for (int i = 0; i < nPrimitives; ++i)
	//	{

	//		v0 = XMLoadFloat3(&m_pTex[i * nOffset + 0].vPos);
	//		v1 = XMLoadFloat3(&m_pTex[i * nOffset + 1].vPos);
	//		v2 = XMLoadFloat3(&m_pTex[i * nOffset + 2].vPos);

	//		XMMATRIX  matWorld;

	//		matWorld = XMLoadFloat4x4(&((CTransform*)((CDefaultObj*)(*iter))->Get_Component(L"Com_Transform"))->m_matWorld);

	//		v0 = XMVector3TransformCoord(v0, matWorld);
	//		v1 = XMVector3TransformCoord(v1, matWorld);
	//		v2 = XMVector3TransformCoord(v2, matWorld);

	//		float fDist = 0.0f;

	//		if (XNA::IntersectRayTriangle(vOrigin, vRayDir, v0, v1, v2, &fDist))
	//		{
	//			if (fDist < tmin)
	//			{
	//				tmin = fDist;
	//			}
	//		}
	//	}

	//}

	//
	////이부분을 메시충돌로 하자
	//if (tmin <= 0.1f)
	//{
	//	tmin = 0.0f;
	//	m_pComGravity->Set_LandOn(); //착지 한걸로 하자
	//	XMVECTOR vPos = vOrigin + (vRayDir * tmin);
	//	XMStoreFloat3(&m_pTransform->m_vPos, vPos);
	//}
	//else
	//{
	//	m_pComGravity->Set_OnGround(false); //아직 착지 안함
	//}
}

void CPlayer::PlayAnimation(DWORD dwAniIdx, bool bImmediate)
{
	if (bImmediate)
		m_pAnimInfo->Set_KeyImm((_ushort)dwAniIdx);
	m_pAnimInfo->Set_Key((_ushort)dwAniIdx);
	m_dwAniIdx = dwAniIdx;
}

bool CPlayer::Check_AnimationFrame(void)
{
	if (m_pAnimInfo->Get_CurFrame() >= m_pAnimInfo->Get_LastFrame())
		return true;
	return false;
}

bool CPlayer::IsOnGround(void)
{
	return m_pComGravity->Get_OnGround();
}

MOVE_DIR* CPlayer::GetMoveDir(void)
{
	return &m_eMoveDir;
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

	Soldier_Move(fTimeDelta);
}

void CPlayer::Soldier_Move(const FLOAT& fTimeDelta)
{
	XMVECTOR vDir = XMLoadFloat3(&m_vMoveDir);
	XMVECTOR vPos = XMLoadFloat3(&m_pTransform->m_vPos);

	switch (m_dwState)
	{
	case SOLDIER_MOVE:
		vPos += vDir * m_fSpeed * fTimeDelta;
		XMStoreFloat3(&m_pTransform->m_vPos, vPos);
		m_pTransform->Update(fTimeDelta);
		break;
	case SOLDIER_LYING:
		if (m_dwAniIdx != PLAYER_Lying && m_dwAniIdx != PLAYER_LyingShoot)
		{
			vPos += vDir * m_fSpeed * fTimeDelta * 0.5f;
			XMStoreFloat3(&m_pTransform->m_vPos, vPos);
			m_pTransform->Update(fTimeDelta);
		}
		break;
	}
}

void CPlayer::UpdateDir(void)
{
	XMVECTOR vDir;

	vDir = XMLoadFloat3(&m_pTransform->m_vDir);

	XMVECTOR vLook = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	vDir = XMVector3TransformNormal(vLook, XMLoadFloat4x4(&m_pTransform->m_matWorld));

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
