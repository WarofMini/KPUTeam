#pragma once
#include "Include.h"
#include "Object.h"

class CShader;
class CVIBuffer;
class CTexture;
class CLogoBack :
	public CObject
{
public:
	CLogoBack();
	virtual ~CLogoBack();

private:
	CShader*	m_pVertexShader;
	CShader*	m_pPixelShader;
	CTexture*	m_pTexture;
	CVIBuffer*	m_pPolygon;
	CInfo*		m_pInfo;

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	virtual void Release(void);

public:
	static CLogoBack* Create(void);

private:
	HRESULT	AddBuffer(void);
};

