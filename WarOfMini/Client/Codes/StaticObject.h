#ifndef StaticObject_h__
#define StaticObject_h__

#include "Object.h"

class CShader;
class CVIBuffer;
class CTexture;

class CStaticObject :
	public CObject
{
public:
	explicit CStaticObject();
	virtual  ~CStaticObject();

private:
	CVIBuffer*		m_pBuffer;
	CShader*		m_pVertexShader;
	CShader*		m_pPixelShader;
	CTexture*		m_pTexture;
	OBJ_INFO		m_tInfo;
	wstring			m_strName;

	CVIBuffer*		m_pBoundingBox;



public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);

public:
	static			CStaticObject* Create(wstring strName);
	void			Release(void);

private:
	HRESULT			AddComponent(void);

public:
	void			SetStrName(wstring _strName);
	wstring			GetStrName(void);

	CVIBuffer*		 GetBuffer(void);


	OBJ_INFO*		 GetObjInfo(void);

	void			 SetObjInfo(OBJ_INFO* pInfo);

	void			 ObjInfoSetting(void);


	void			 InfoSetting(void);

};




#endif
