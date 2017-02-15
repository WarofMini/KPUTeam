#ifndef ToolApp_h__
#define ToolApp_h__

class CDevice;
class CInfo;
class CVIBuffer;
class CShader;
class CTexture;
class CScene;

class CToolApp
{
public:
	CToolApp();
	virtual ~CToolApp();

private:
	CDevice*	m_pGrapicDevcie;
	CShader*	m_pVertexShader;
	CShader*	m_pPixelShader;

public:
	HRESULT Initialize(void);
	int		Update(void);
	void	Render(void);
	void	Release(void);

public:
	static CToolApp* Create(void);
	
};

#endif