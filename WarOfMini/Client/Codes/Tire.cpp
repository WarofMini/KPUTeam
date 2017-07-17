#include "stdafx.h"
#include "Tire.h"
#include "Management.h"
#include "Renderer.h"
#include "Transform.h"
#include "AnimationInfo.h"
#include "Input.h"

CTire::CTire(ID3D11DeviceContext* pContext)
	: CDynamicObject(pContext)
	, m_dwAniIdx(Tire_Moving_slow)
	, m_fvelocity(0.f)
	, m_bUseable(true)
	, m_iUseID(-1)
{

}

CTire::~CTire(void)
{

}

CTire* CTire::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CTire* pObject = new CTire(pContext);

	if (FAILED(pObject->Initialize(pGraphicDev)))
		Safe_Release(pObject);

	return pObject;
}

HRESULT CTire::Initialize(ID3D11Device* pGraphicDev)
{
	m_uiObjNum = MESHNUM_TIRE;

	if (FAILED(Ready_Component(pGraphicDev)))
		return E_FAIL;

	m_pTransform->m_vScale = XMFLOAT3(1.f, 1.f, 1.f);
	m_pAnimInfo->Set_Key(_ushort(m_dwAniIdx));
	m_pTransform->m_vPos = XMFLOAT3(100.f, 0.0f, 200.f);

	m_byMoveDir = MOVE_END;

	return S_OK;
}

INT CTire::Update(const FLOAT& fTimeDelta)
{
	CDynamicObject::Update(fTimeDelta);

	
	//tset
	if (m_iUseID != -1)
	{
		if (CInput::GetInstance()->Get_DIKeyState(DIK_W))
			m_fvelocity += 10.f * fTimeDelta;
		if (CInput::GetInstance()->Get_DIKeyState(DIK_S))
			m_fvelocity -= 10.f * fTimeDelta;
	}
	if (m_fvelocity > 0.f)
		m_byMoveDir = MOVE_FORWARD;
	if (m_fvelocity < 0.f)
		m_byMoveDir = MOVE_BACK;

	if (m_byMoveDir == MOVE_FORWARD)
	{
		m_fvelocity -= 5.f * fTimeDelta;
		if (m_fvelocity <= 0.f)
		{
			m_fvelocity = 0.f;
			m_byMoveDir = MOVE_END;
		}
	}
	else if (m_byMoveDir == MOVE_BACK)
	{
		m_fvelocity += 5.f * fTimeDelta;
		if (m_fvelocity >= 0.f)
		{
			m_fvelocity = 0.f;
			m_byMoveDir = MOVE_END;
		}
	}
	// 	if (abs(m_fvelocity) > 20.f)
	// 	{
	// 		if(m_dwAniIdx != Tire_Moving_moderate)
	// 		m_pAnimInfo->Set_KeyImm(m_dwAniIdx = Tire_Moving_moderate);
	// 	}
	// 	else if (abs(m_fvelocity) > 0.f && abs(m_fvelocity) <= 20.f)
	// 	{
	// 		if (m_dwAniIdx != Tire_Moving_slow)
	// 			m_pAnimInfo->Set_KeyImm(m_dwAniIdx = Tire_Moving_slow);
	// 	}
	if (m_byMoveDir == MOVE_FORWARD)
	{
		m_pAnimInfo->Set_PlayAni(true);
		m_pAnimInfo->Set_PlayReverse(false);
	}
	else if (m_byMoveDir == MOVE_BACK)
	{
		m_pAnimInfo->Set_PlayAni(true);
		m_pAnimInfo->Set_PlayReverse(true);
	}
	else if (m_byMoveDir == MOVE_END)
	{
		m_pAnimInfo->Set_PlayAni(false);
	}


	CManagement::GetInstance()->Add_RenderGroup(CRenderer::RENDER_ZSORT, this);

	return 0;
}

void CTire::Release(void)
{
	CDynamicObject::Release();
}

void CTire::SetPos(XMFLOAT3 vPos)
{
	m_pTransform->m_vPos = vPos;
}

void CTire::SetUse(bool bUse, int iID)
{
	m_iUseID = iID;
}

HRESULT CTire::Ready_Component(ID3D11Device* pGraphicDev)
{
	if (FAILED(CDynamicObject::Ready_Component()))
		return E_FAIL;

	return S_OK;
}

