#ifndef RespawnUI_h__
#define RespawnUI_h__

#include "UI.h"

class CPanel;

class CRespawnUI
	: public CUI
{
private:
	explicit CRespawnUI(ID3D11DeviceContext* pContext);
	virtual ~CRespawnUI(void);

private:
	CPanel*		m_pPanel;
	CPanel*		m_pMiniMap;


public:
	static CRespawnUI* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

};


#endif


