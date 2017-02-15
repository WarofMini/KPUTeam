#ifndef ToolToolStage_h__
#define ToolToolStage_h__

#include "Scene.h"

class CShader;
class CTexture;

class CToolStage :
	public CScene
{
public:
	CToolStage();
	virtual ~CToolStage();

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	virtual void Release(void);

public:
	static CToolStage* Create(void);
	HRESULT Add_Environment_Layer(void);
	HRESULT Add_GameLogic_Layer(void);
	HRESULT Add_Dynamic_Buffer(void);

public:
	void		InitMesh(void);

	void		InitBookMesh(void);

};

#endif