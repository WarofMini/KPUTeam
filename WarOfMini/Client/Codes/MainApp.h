#ifndef MainApp_h__
#define MainApp_h__

class CDevice;
class CScene;

class CMainApp
{
public:
	explicit CMainApp();
	virtual ~CMainApp();

private:
 	CDevice*	m_pGrapicDevcie;

private:
	HRESULT Add_ShaderFile(void);
public:
	HRESULT Initialize(void);
	int		Update(void);
	void	Render(void);
	void	Release(void);

public:
	static CMainApp* Create(void);
};

#endif // MainApp_h__
