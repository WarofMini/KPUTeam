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

XMFLOAT3		g_vPlayerPos;

CPlayer::CPlayer(ID3D11DeviceContext* pContext)
	: CDynamicObject(pContext)
	, m_dwState(SOLDIER_IDLE)
	, m_dwAniIdx(PLAYER_idle)
	, m_pComStateMachine(NULL)
{
	m_vLook = XMFLOAT3(0.f, 0.f, -1.f);

	XMStoreFloat4x4(&m_matEquipBone[0], XMMatrixIdentity());
	XMStoreFloat4x4(&m_matEquipBone[1], XMMatrixIdentity());

	ZeroMemory(m_pEquipment, sizeof(CEquipment*) * 1);

	m_pServer_PlayerData = new Ser_PLAYER_DATA;
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
	m_pTransform->m_vPos = XMFLOAT3(20.f, 10.f, 20.f);
	m_pTransform->m_vDir = XMFLOAT3(0.f, 0.f, -1.f);
	m_pAnimInfo->Set_Key(m_dwAniIdx);

	m_pComStateMachine->Enter_State(SOLDIER_IDLE);

//	m_pServer_PlayerData->vPos = m_pTransform->m_vPos;

//	g_Client.sendPacket()

	return S_OK;
}

INT CPlayer::Update(const FLOAT& fTimeDelta)
{
	CDynamicObject::Update(fTimeDelta);

	Operate_StateMAchine(fTimeDelta);

	/*if (CInput::GetInstance()->GetDIKeyStateOnce(DIK_UP))
		cout << "ÇÑ¹ø ´­¸²" << endl;
	if (CInput::GetInstance()->GetDIKeyStateLeave(DIK_UP))
		cout << "ÇÑ¹ø ¶³¾îÁü" << endl;*/
	if (CInput::GetInstance()->Get_DIKeyState(DIK_UP))
		m_pTransform->m_vPos.z += 10.f * fTimeDelta;
	if (CInput::GetInstance()->Get_DIKeyState(DIK_DOWN))
		m_pTransform->m_vPos.z -= 10.f * fTimeDelta;
	if (CInput::GetInstance()->Get_DIKeyState(DIK_LEFT))
		m_pTransform->m_vPos.x -= 10.f * fTimeDelta;
	if (CInput::GetInstance()->Get_DIKeyState(DIK_RIGHT))
		m_pTransform->m_vPos.x += 10.f * fTimeDelta;

	// Temp	-------------------------------------------------------------------------------
	
	g_vPlayerPos = m_pTransform->m_vPos;


	// Update
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	m_pTransform->m_vDir = XMFLOAT3(m_pTransform->m_matWorld._31, m_pTransform->m_matWorld._32, m_pTransform->m_matWorld._33);
	XMStoreFloat3(&m_pTransform->m_vDir, XMVector3Normalize(XMLoadFloat3(&m_pTransform->m_vDir)));

	//m_pTransform->Update_MatrixNotXRot();

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

	return S_OK;
}

void CPlayer::Operate_StateMAchine(const float& fTimeDelta)
{
	DWORD dwState = m_pComStateMachine->Get_State();

	switch (dwState)
	{
	case SOLDIER_IDLE:
		if (m_dwAniIdx != PLAYER_idle)
		{
			m_pComStateMachine->Enter_State(SOLDIER_MOVE);
		}
		break;
	case SOLDIER_MOVE:
		if (m_dwAniIdx != PLAYER_RunForward)
		{
			m_pComStateMachine->Enter_State(SOLDIER_IDLE);
		}
		break;
	default:
		break;
	}
	m_pComStateMachine->Update_State(dwState);
}

void CPlayer::PlayAnimation(DWORD dwAniIdx, bool bImmediate)
{
	if (bImmediate)
		m_pAnimInfo->Set_KeyImm(dwAniIdx);
	m_pAnimInfo->Set_Key(dwAniIdx);
	m_dwAniIdx = dwAniIdx;
}
