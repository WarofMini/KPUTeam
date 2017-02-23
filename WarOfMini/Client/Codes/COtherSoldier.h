#pragma once

#include "Object.h"

class CShader;
class CVIBuffer;
class CTexture;
class CDynamicMesh;
class CStateMachine;
class CInput;

class COtherSoldier :
	public CObject
{
public:
	COtherSoldier();
	virtual ~COtherSoldier();

public:
	//enum STATE_SOLDIER { SOLDIER_IDLE, SOLDIER_MOVE, SOLDIER_ATTACK, SOLDIER_END };

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	VTXTEX*			m_pVerTex;

	CInput*			m_pInputDev;
	CStateMachine*	m_pComStateMachine;



private:
	DWORD			m_dwAniIdx;

public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	void			KeyInput(void);

private:
	HRESULT		Prepare_StateMachine(void);
	void		Operate_StateMAchine(const float& fTimeDelta);

public:
	static COtherSoldier* Create(void);
	void	Release(void);

private:
	HRESULT	AddComponent(void);
};