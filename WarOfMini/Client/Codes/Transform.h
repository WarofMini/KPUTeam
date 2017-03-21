#ifndef Transform_h__
#define Transform_h__

#include "Component.h"


class CTransform :
	public CComponent
{

private:
	explicit CTransform(void);
	virtual ~CTransform(void);

public:
	_vec3					m_vAngle;
	_vec3					m_vScale;
	_vec3					m_vPos;
	_vec3					m_vDir;
	_matrix					m_matWorld;
	Ser_PLAYER_DATA			m_ServerInfo;


public:
	virtual _int Update(const _float& fTime);
	static CTransform* Create(void);

public:
	virtual  void		Release(void);
};

#endif