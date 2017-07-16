#ifndef GageUI_h__
#define GageUI_h__

#include "UI.h"

class CGageUI
	: public CUI
{
private:
	explicit CGageUI(ID3D11DeviceContext* pContext);
	virtual ~CGageUI(void);

public:
	static CGageUI* Create(ID3D11DeviceContext* pContext);


private:
	_float		m_fXGage;
	_bool		m_bGageStart;
	_bool		m_bGoalCheck;

public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	UpdateGage(void);
	void	SetGageStart(_bool bCheck) { m_bGageStart = bCheck; }
	_bool	GetGageStart(void) { return m_bGageStart; }

	void	SetGoalCheck(_bool bCheck) { m_bGoalCheck = bCheck; }
	_bool	GetGoalCheck(void) { return m_bGoalCheck; }
	void	SetXGage(_float fGage) { m_fXGage = fGage; }
};


#endif


