#ifndef Loading_h__
#define Loading_h__

class CLoading
{
public:
	enum LOADINGID { LOADING_TITLE, LOADING_LOGO, LOADING_STAGE, LOADING_END };

public:
	explicit CLoading(LOADINGID eLoadID);
	~CLoading(void);

public:
	bool GetComplete(void);
	const TCHAR* GetLoadMessage(void);

public:
	HRESULT InitLoading(void);
	void	LogoLoading(void);
	void	StageLoading(void);


public:
	static CLoading* Create(LOADINGID eLoadID);
	static UINT WINAPI LoadingFunction(void* pArg);

public:
	void Release(void);

private:
	LOADINGID				m_eLoadID;
	CRITICAL_SECTION		m_CSKey;
	HANDLE					m_hThread;
	
	ID3D11Device*			pGraphicDev;
	ID3D11DeviceContext*	pContext;

private:
	TCHAR		m_szLoadMessage[128];
	bool		m_bComplete;
	bool		m_serverConnected{ false };

private:

	void		Ready_TextureFromFile(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

	void		Ready_DynamicMeshFromFile(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

	void		Ready_StaticMeshFromFile(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

};


#endif