#include "stdafx.h"
#include "BulletNumbering.h"
#include "Management.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"
#include "CameraMgr.h"
#include "Numbering.h"
#include "Gun.h"


CBulletNumbering::CBulletNumbering(ID3D11DeviceContext * pContext)
: CUI(pContext)
, m_pGun(NULL)
{
	m_eState = BULLETNUMBER_END;
}

CBulletNumbering::~CBulletNumbering(void)
{
}

CBulletNumbering * CBulletNumbering::Create(ID3D11DeviceContext * pContext)
{
	CBulletNumbering* pNumbering = new CBulletNumbering(pContext);

	if (FAILED(pNumbering->Initialize()))
		Safe_Release(pNumbering);

	return pNumbering;
}

HRESULT CBulletNumbering::Initialize(void)
{
	for (int i = 0; i < 2; ++i)
	{
		m_pNumbering[i] = CNumbering::Create(m_pContext);
	}

	return S_OK;
}

_int CBulletNumbering::Update(const _float & fTimeDelta)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return 0;


	ChangingNumbering();


	for (int i = 0; i < 2; ++i)
	{
		m_pNumbering[i]->Update(fTimeDelta);
	}

	return 0;
}

void CBulletNumbering::Render(void)
{
	if (CCameraMgr::GetInstance()->Get_CurCamera() == CCameraMgr::CAMERA_DYNAMIC)
		return;

	for (int i = 0; i < 2; ++i)
	{
		m_pNumbering[i]->Render();
	}
}

void CBulletNumbering::Release(void)
{
	for (int i = 0; i < 2; ++i)
	{
		m_pNumbering[i]->Release();
	}

	CUI::Release();
}

HRESULT CBulletNumbering::Ready_Component(void)
{
	return S_OK;
}

void CBulletNumbering::SetNumberSize(_float fSizeX, _float fSizeY)
{
	for (int i = 0; i < 2; ++i)
	{
		m_pNumbering[i]->SetSizeX(fSizeX);
		m_pNumbering[i]->SetSizeY(fSizeY);
	}
}

void CBulletNumbering::SetNumberPosition(_float fMoveX, _float fMoveY)
{
	m_pNumbering[0]->SetMoveX(fMoveX);
	m_pNumbering[0]->SetMoveY(fMoveY);

	m_pNumbering[1]->SetMoveX(fMoveX + (m_pNumbering[0]->GetSizeX()));
	m_pNumbering[1]->SetMoveY(fMoveY);
	
}

void CBulletNumbering::ChangingNumbering(void)
{

	if (m_pGun != NULL)
	{
		_uint* m_pAmmo = NULL;

		if (m_eState == ORIGIN_BULLETNUMBER)
		{
			m_pAmmo = m_pGun->GetOriginAmmo();
		}
		else if (m_eState == CHANGE_BULLETNUMBER)
		{
			m_pAmmo = m_pGun->GetAmmo();
		}

		int m_iQuotient = 0;
		int m_iRemainder = 0;

		m_iQuotient = (*m_pAmmo) / 10;
		m_iRemainder = (*m_pAmmo) % 10;

		m_pNumbering[0]->Set_TextureNumber(m_iQuotient);
		m_pNumbering[1]->Set_TextureNumber(m_iRemainder);
	}

}
