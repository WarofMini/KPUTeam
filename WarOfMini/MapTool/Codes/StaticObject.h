#ifndef ToolStaticObject_h__
#define ToolStaticObject_h__

#include "Obj.h"
#include "ToolInclude.h"

class CShader;
class CVIBuffer;
class CTexture;

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

};


#endif //StaticObject_h__