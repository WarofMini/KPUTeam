#ifndef RespawnUI_h__
#define RespawnUI_h__

#include "UI.h"

class CPanel;
class CStationUI;
class CFlagUI;
class CStation;
class CPlayer;
class CInput;

class CRespawnUI
	: public CUI
{
private:
	explicit CRespawnUI(ID3D11DeviceContext* pContext);
	virtual ~CRespawnUI(void);

private:
	CPanel*		m_pPanel;
	CPanel*		m_pMiniMap; //맵 텍스쳐
	CStationUI* m_pStationUI[5]; //타워
	CFlagUI*	m_pFlagUI[5]; //깃발
	CStation*   m_pStation[5]; //타워 포인터
	CPlayer*	m_pPlayer;
	_bool		m_bRespawnSelect;
	XMFLOAT3	m_vRespawnPos;

private:
	CInput*			m_pInput;

public:
	static CRespawnUI* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void  StationUIUpdate(void);
	void  StationFlagUIUpdate(void);

	void  StationCollision(void);

	_float PositionRand(_float a, _float b);

	void  RespawnSelectUpdate(void);

};


#endif


