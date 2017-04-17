#ifndef MainApp_h__
#define MainApp_h__

#include "Include.h"

class CMainApp
{
private:
	explicit CMainApp(void);
	virtual ~CMainApp(void);


public:
	HRESULT Initialize(void);
	_int Update(const _float& fTimeDelta);
	void Render(void);

public:
	static CMainApp* Create(void);

private:
	_ulong							m_dwRenderCnt;
	_tchar							m_szFPS[128];
	_float							m_fTime;

private:	//��ǲ ��ġ �ҽ� ������ �ٽ� ã�� �Լ�.
	void		Set_Focus(void);

public:
	void Release(void);

private:
	void Ready_TextureFromFile(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	void Render_FPS(void);
	void Render_CurrentScene(void);

	void Stage_DebugInfo(void);

	void Debug_KeyCheck(void);
};

#endif // MainApp_h__