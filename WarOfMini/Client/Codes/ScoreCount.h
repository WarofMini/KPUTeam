#ifndef ScoreCount_h__
#define ScoreCount_h__

#include "UI.h"

class CScoreCount
	: public CUI
{
private:
	explicit CScoreCount(ID3D11DeviceContext* pContext);
	virtual ~CScoreCount(void);

private:
	_bool	m_bStart;

public:
	static CScoreCount* Create(ID3D11DeviceContext* pContext);


public:
	virtual HRESULT	Initialize(void);
	virtual _int Update(const _float& fTimeDelta);
	virtual void Render(void);
	virtual void Release(void);

protected:
	virtual HRESULT Ready_Component(void);

public:
	void	SetStart(_bool bCheck) { m_bStart = bCheck; }
	_bool	GetStart(void) { return m_bStart; }




};


#endif


