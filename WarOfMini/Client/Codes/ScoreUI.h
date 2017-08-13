#ifndef ScoreUI_h__
#define ScoreUI_h__

#include "UI.h"

class CPanel;
class CFlagUI;
class CScoreCount;

class CScoreUI
	: public CUI
{
private:
	explicit CScoreUI(ID3D11DeviceContext* pContext);
	virtual ~CScoreUI(void);

private:
	CPanel*			m_pPanel;
	CFlagUI*		m_pFlag[2];
	CScoreCount*	m_pCount[2];

public:
	static CScoreUI* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int	Update(const _float& fTimeDelta);
	virtual void	Render(void);
	virtual void	Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	FlagUpdate(void);

};


#endif


