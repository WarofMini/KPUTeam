#ifndef GameObject_h__
#define GameObject_h__

#include "Include.h"
#include "Transform.h"

class CComponent;


class CGameObject
{
protected:
	explicit CGameObject(ID3D11DeviceContext* pContext);
	virtual ~CGameObject(void);

protected:
	typedef map<const _tchar*, CComponent*>		MAPCOMPONENT;
	MAPCOMPONENT								m_mapComponent;
	Ser_PLAYER_DATA*							m_pServer_PlayerData;
	
	_int										m_iTextureNumber;//텍스쳐 번호
	wstring										m_strTextureName;
	CTransform*									m_pTransform;
	_uint										m_uiObjNum; //메시 번호
	_float										m_fWeight; //레이와 객체가 부딛혔을때
	_float										m_fMass; //객체와 객체가 부딛혔을때

protected:
	ID3D11DeviceContext* m_pContext;

private:
	virtual HRESULT Ready_Component(void);

public:
	const CComponent* Get_Component(const _tchar* pComponentTag);

public:
	void SetPacketData(Ser_PLAYER_DATA* SetPacketData)
	{
		//m_pServer_PlayerData = SetPacketData;
		memcpy(m_pServer_PlayerData, SetPacketData, sizeof(Ser_PLAYER_DATA));
	}
	Ser_PLAYER_DATA* GetPacketData(void) { return m_pServer_PlayerData; }

public:
	virtual HRESULT	Initialize(void);
	virtual _int  Update(const _float& fTimeDelta);
	virtual void Render(void);

public:
	virtual void Release(void);

public:
	//객체의 텍스쳐를 바꿔주는 함수(요걸로 각각의 객체의 텍스쳐를 바꿔줄수 있다.)
	void	Set_TextureNumber(_uint uiTextureNum);
	_uint	Get_TextureNumber(void);

public:
	void		SetName(wstring strName) { m_strTextureName = strName; }

	void		SetTransformPosition(XMFLOAT3 vPos) {m_pTransform->m_vPos = vPos;}
	void		SetTransformScale(XMFLOAT3 vScale) { m_pTransform->m_vScale = vScale; }
	void		SetTransformRotate(XMFLOAT3 vAngle) { m_pTransform->m_vAngle = vAngle; }


	XMFLOAT3	GetTransformPosition(void) { return m_pTransform->m_vPos; }
	XMFLOAT3	GetTransformScale(void) { return m_pTransform->m_vScale; }
	XMFLOAT3	GetTransformRotate(void) { return m_pTransform->m_vAngle; }

	void			SetObjNum(_uint uNum) { m_uiObjNum = uNum; }
	_uint			GetObjNum(void) { return m_uiObjNum; }

	void			SetWeight(_float fWeight) { m_fWeight = fWeight; }
	_float			GetWeight(void) { return m_fWeight; }

	void			SetMass(_float fMass) { m_fMass = fMass; }
	_float			GetMass(void) { return m_fMass; }
};

#endif