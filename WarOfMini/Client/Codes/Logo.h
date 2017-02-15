#pragma once
#include "Scene.h"

class CShader;
class CTexture;
class CVIBuffer;
class CLoading;
class CLogo :
	public CScene
{
public:
	CLogo();
	virtual ~CLogo();

private:
	CLoading*	m_pLoading;
	bool		m_bDynamicLoading;

private:
	HRESULT Add_GameLogic_Layer(void);
	//
	HRESULT Add_Dynamic_Buffer(void);

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	virtual void Release(void);

public:
	static CLogo* Create(void);
};

