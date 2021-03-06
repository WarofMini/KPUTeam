#ifndef ToolScnen_h__
#define ToolScnen_h__


class CScene
{
public:
	CScene();
	virtual ~CScene();

public:
	virtual HRESULT Initialize(void) { return S_OK; }
	virtual int		Update(void) { return 0; }
	virtual void	Render(void)PURE; //{}
	virtual void	Release(void) {}
};

#endif
