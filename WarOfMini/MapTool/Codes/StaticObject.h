#ifndef ToolStaticObject_h__
#define ToolStaticObject_h__

#include "Obj.h"
#include "ToolInclude.h"

class CShader;
class CVIBuffer;
class CTexture;
class CMouseCol;


class CToolStaticObject :
		public CObj
{
public:
	explicit CToolStaticObject();
	virtual  ~CToolStaticObject();

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	OBJ_INFO		m_tInfo;
	wstring			m_strName;
	CMouseCol*		m_pMouseCol;

	CVIBuffer*		m_pBoundingBox;
	OBJECT_MODE		m_eMode;

	D3DXVECTOR3     m_vPickMeshPos;



public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);

public:
	static			CToolStaticObject* Create(wstring strName);
	void			Release(void);

private:
	HRESULT			AddComponent(void);

public:
	void			SetStrName(wstring _strName);
	wstring			GetStrName(void);

	void			BoundingBoxRender(void);

	void			SettingWork(void);


	void			SetMode(OBJECT_MODE eMode);

	OBJECT_MODE		GetMode(void);


	void			 ScaleAndRotateChange(void);

	CVIBuffer*		 GetBuffer(void);

	D3DXVECTOR3*     GetPickMeshPos(void);

	OBJ_INFO*		 GetObjInfo(void);

	void			 SetObjInfo(OBJ_INFO* pInfo);

	void			 ObjInfoSetting(void);


	void			 InfoSetting(void);

};


#endif //StaticObject_h__