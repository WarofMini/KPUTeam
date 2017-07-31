#include "stdafx.h"
#include "Include.h"
#include "OtherPlayer.h"
#include "AnimationInfo.h"
#include "Transform.h"
#include "MeshMgr.h"
#include "Gun.h"
#include "Management.h"

COtherPlayer::COtherPlayer(ID3D11DeviceContext* pContext)
: CDynamicObject(pContext)
, m_iEquipBone(0)
, m_dwAniIdx(PLAYER_idle)
, m_bIsSoldier(true)
, m_pPxCharacterController(NULL)
{
	m_vLook = XMFLOAT3(0.f, 0.f, -1.f);

	XMStoreFloat4x4(&m_matEquipBone[0], XMMatrixIdentity());
	XMStoreFloat4x4(&m_matEquipBone[1], XMMatrixIdentity());

	ZeroMemory(m_pEquipment, sizeof(CEquipment*) * 1);

	m_pServer_PlayerData = new Ser_PLAYER_DATA;
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

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_pTransform->m_vAngle.x = 90.f;
	m_pAnimInfo->Set_Key((_ushort)m_dwAniIdx);

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
	CDynamicObject::Update(fTimeDelta);

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

void COtherPlayer::SetPlayerData(XMFLOAT3 vPos, XMFLOAT3 vDir)
{
	m_pPxCharacterController->setFootPosition(PxExtendedVec3(vPos.x, vPos.y, vPos.z));

	_float m_fRevice = 0.5f; //Player의 Y보정값(발이 지면에 안박히게 보정)

	if (!m_bIsSoldier)
		m_fRevice = 0.0f;

	m_pTransform->m_vPos = XMFLOAT3((_float)m_pPxCharacterController->getFootPosition().x, 
									(_float)m_pPxCharacterController->getFootPosition().y + m_fRevice, 
									(_float)m_pPxCharacterController->getFootPosition().z);

	m_pTransform->m_vPos.y += m_fRevice;
	m_pTransform->m_vAngle = vDir;

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

	m_pTransform->m_vPos = XMFLOAT3(20.f, 0.f, 10.f);
	m_pPxCharacterController->setFootPosition(PxExtendedVec3(m_pTransform->m_vPos.x, m_pTransform->m_vPos.y, m_pTransform->m_vPos.z));
	
	m_strName = "OtherPlayer_";

	m_strName += to_string(m_iID);
	m_pPxCharacterController->getActor()->setName(m_strName.c_str());
}
