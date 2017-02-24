#ifndef ToolToolStage_h__
#define ToolToolStage_h__

#include "Scene.h"

class CShader;
class CTexture;
class CObjCol;

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
	void		InitFloorMesh(void);
	void		InitDeskMesh(void);
	void		InitChairMesh(void);
	void		InitCabinetMesh(void);
	void		InitShelfMesh(void);
	void		InitFloor(void);
	void		InitToiletFloor(void);
	void		InitPhotoCopier(void);
	void		InitPlant(void);
	void		InitRubbishBin(void);
	void		InitWaterCooler(void);
	void		InitCouch(void);
	void		InitCoffeetable(void);
	void		InitBookBox(void);
	void		InitWall(void);
	void		InitPainting(void);
	void		InitDoor(void);
	void		InitToiletStall(void);
	void		InitEtc(void);

};

#endif