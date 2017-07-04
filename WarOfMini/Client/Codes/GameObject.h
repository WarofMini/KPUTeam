#ifndef GameObject_h__
#define GameObject_h__

#include "Include.h"


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
	//텍스쳐 번호
	_int										m_iTextureNumber;
	wstring										m_strTextureName;

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
};

#endif