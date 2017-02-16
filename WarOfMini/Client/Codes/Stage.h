#pragma once
#include "Scene.h"

class CStage :
	public CScene
{
public:
	CStage();
	virtual ~CStage();

private:
	HRESULT CreateSoldier(void);
public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	virtual void Release(void);

public:
	static CStage* Create(void);
};
