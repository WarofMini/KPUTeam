#include "stdafx.h"
#include "Include.h"
#include "PlayerMain.h"
#include "AnimationInfo.h"
#include "Transform.h"
#include "Gun.h"
#include "Management.h"

XMFLOAT3 g_vPlayerPos = XMFLOAT3(0.f, 0.f, 0.f);
INT g_iPlayerHP = 120;

CPlayerMain::CPlayerMain(ID3D11DeviceContext* pContext)
	: CPlayer(pContext)
{
	m_vLook = XMFLOAT3(0.f, 0.f, -1.f);
}

CPlayerMain::~CPlayerMain(void)
{
}

CPlayerMain* CPlayerMain::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CPlayerMain* pObject = new CPlayerMain(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	return pObject;
}

HRESULT CPlayerMain::Initialize(ID3D11Device* pGraphicDev)
{
	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;


	m_uiObjNum = MESHNUM_PLAYER;


	m_pTransform->m_vScale = XMFLOAT3(0.06f, 0.06f, 0.06f);
	m_pTransform->m_vAngle.y = D3DXToRadian(180.f);
	m_pTransform->m_vPos = XMFLOAT3(0.f, 0.f, 0.f);
	m_pTransform->m_vDir = XMFLOAT3(0.f, 0.f, -1.f);
	m_pAnimInfo->Set_Key(PLAYER_IDLE);

	return S_OK;
}

INT CPlayerMain::Update(const FLOAT& fTimeDelta)
{

	// Temp	-------------------------------------------------------------------------------

	g_vPlayerPos = m_pTransform->m_vPos;


	// Update
	CPlayer::Update(fTimeDelta);
	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	m_pTransform->m_vDir = XMFLOAT3(m_pTransform->m_matWorld._31, m_pTransform->m_matWorld._32, m_pTransform->m_matWorld._33);
	XMStoreFloat3(&m_pTransform->m_vDir, XMVector3Normalize(XMLoadFloat3(&m_pTransform->m_vDir)));

	m_pTransform->Update_MatrixNotXRot();

	Update_Equipment(fTimeDelta);

	return 0;
}
void CPlayerMain::Release(void)
{
	CPlayer::Release();
	delete this;
}

HRESULT CPlayerMain::Ready_Component(ID3D11Device* pGraphicDev)
{
	if (FAILED(CPlayer::Ready_Component(pGraphicDev)))
		return E_FAIL;

	return S_OK;
}
void CPlayerMain::InputKey(const FLOAT& fTimeDelta)
{

}
void CPlayerMain::Move(const FLOAT& fTimeDelta)
{

}

void CPlayerMain::Attack(const FLOAT& fTimeDelta)
{

}