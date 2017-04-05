#ifndef Calculator_h__
#define Calculator_h__


#include "Component.h"
#include  "Include.h"

class CTransform;

class CCalculator
	:public CComponent
{
private:
	explicit CCalculator(void);
	virtual ~CCalculator(void);

public:
	_bool			Get_IsCol(void);
	void			Set_IsCol(_bool bCol);
	void			Set_Jump(_bool bJump);

	const XMFLOAT3	GetCollPos(void);
	void			SetCollPos(const XMFLOAT3 vCollPos);

public:
	void			Set_MagicNum(const _float& fMagic);
	void			Set_Transform(CTransform* pTransform);

public:
	virtual _int	Update(const _float& fTime);
	virtual void	Release(void);

private:
	CTransform*		m_pTransform;
	XMFLOAT3		m_vPivotPos;
	XMFLOAT3		m_vRayDir;
	_uint			m_iMeshSize;
	_bool			m_bIsCol; //지형에 충돌이 되었는지 확인하는 변수
	_bool			m_bJump;
	XMFLOAT3		m_vCollPos;
	_float			m_fMagicNumber;


private:
	HRESULT   Ready_Caculator(void);

public:
	static   CCalculator* Create(void);

private:
	void Translation_ViewSpace(POINT ptMouse);
	void Translation_Local(const XMFLOAT4X4* pWorld);
};

#endif // Calculator_h__
