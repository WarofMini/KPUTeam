#include "stdafx.h"
#include "Sound.h"
#include "Sound_Manager.h"
#include "Camera.h"
#include "CameraMgr.h"


CSound::CSound(void)
: m_pSoundMgr(CSound_Manager::GetInstance())
, m_pTransCom(NULL)
, m_pSystem(NULL)
{
}

CSound::~CSound(void)
{
}

_bool CSound::IsPlaying(const wstring & wstrKey)
{
	MAPCHANNEL::iterator iter = m_mapChannel.find(wstrKey);
	if (iter == m_mapChannel.end())
		return false;

	return true;
}

_bool CSound::MyPlaySound(const wstring & wstrKey, _bool bLoop, _bool bOverWrite)
{
	MAPSOUND::iterator iter = m_mapSound.find(wstrKey);

	if (iter == m_mapSound.end())
		return false;

	if (bLoop)
		FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);

	MAPCHANNEL::iterator iter_channel = m_mapChannel.find(wstrKey);

	//채널이 없으면
	if (iter_channel == m_mapChannel.end())
	{
		//채널을 맵에 집어넣은후
		CHANNEL tChannel;
		tChannel.pChannel = NULL;
		tChannel.bLoop = bLoop;											//루프 하는앤지 아닌지
		tChannel.dwTime = GetTickCount();								//시작한 시간
		FMOD_Sound_GetLength(iter->second, &tChannel.iLength, 1);		//길이를 받아와서
		m_mapChannel.insert(MAPCHANNEL::value_type(wstrKey, tChannel));	//맵에 넣어
		//이터찾아와서
		iter_channel = m_mapChannel.find(wstrKey);
		//채널에 실행
		FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0, &iter_channel->second.pChannel);
		return true;
	}
	else
	{
		if (bOverWrite)
		{

			iter_channel->second.dwTime = GetTickCount();
			FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0, &iter_channel->second.pChannel);
		}
	}
	return false;
}

void CSound::MyStopSound(const wstring & wstrKey)
{
	MAPCHANNEL::iterator iter = m_mapChannel.find(wstrKey);
	if (iter == m_mapChannel.end())
		return;

	FMOD_Channel_Stop(iter->second.pChannel);
	m_mapChannel.erase(iter);
}

void CSound::MyStopSoundAll(void)
{
	MAPCHANNEL::iterator iter = m_mapChannel.begin();
	MAPCHANNEL::iterator iter_end = m_mapChannel.end();

	for (iter; iter != iter_end; iter++)
	{
		FMOD_Channel_Stop(iter->second.pChannel);
	}

	m_mapChannel.clear();
}

void CSound::Set_Sound(const wstring & wstrKey, const wstring & wstrFileName)
{
	FMOD_SOUND* pSound = NULL;
	pSound = m_pSoundMgr->GetSound(wstrFileName);
	
	m_mapSound.insert(MAPSOUND::value_type(wstrKey, pSound));
}

void CSound::Update_Component(const _float & fTimeDelta)
{
	Check_SoundEnd();
	Check_Distance();
}

HRESULT CSound::Ready_Sound(CTransform * pTrans)
{
	m_pTransCom = pTrans;
	m_pSystem = m_pSoundMgr->GetSystem();

	return S_OK;
}

void CSound::Check_SoundEnd(void)
{
	MAPCHANNEL::iterator iter = m_mapChannel.begin();
	MAPCHANNEL::iterator iter_end = m_mapChannel.end();

	for (iter; iter != iter_end;)
	{
		if (iter->second.bLoop)
		{
			iter++;
			continue;
		}

		if (iter->second.dwTime + iter->second.iLength < GetTickCount())
		{
			FMOD_Channel_Stop(iter->second.pChannel);
			iter = m_mapChannel.erase(iter);
		}
		else
			iter++;
	}

}

void CSound::Check_Distance(void)
{
	MAPCHANNEL::iterator iter = m_mapChannel.begin();
	MAPCHANNEL::iterator iter_end = m_mapChannel.end();

	for (iter; iter != iter_end; iter++)
	{
		if (iter->second.bLoop)
		{
			FMOD_Channel_SetVolume(iter->second.pChannel, 1.f * g_fBGMValue);
			continue;
		}
		XMFLOAT3 vPos;
		if (m_pTransCom == NULL)
		{
			FMOD_Channel_SetVolume(iter->second.pChannel, 0.5f * g_fEffectSoundValue);
			continue;
		}

		vPos = m_pTransCom->m_vPos;

		XMFLOAT3 vTargetPos = CCameraMgr::GetInstance()->Get_CurCameraEye();

		XMFLOAT3 vLength;

		XMStoreFloat3(&vLength, XMLoadFloat3(&vPos) - XMLoadFloat3(&vTargetPos));

		XMStoreFloat3(&vLength, XMVector3Length(XMLoadFloat3(&vLength)));

		_float fDist = vLength.x;

		if (fDist < 40.f)
		{
			FMOD_Channel_SetVolume(iter->second.pChannel, 1.f * g_fEffectSoundValue);
		}
		else
		{
			_float fVolume = 1.f - min(((fDist - 40.f) / 100.f), 1.f);
			FMOD_Channel_SetVolume(iter->second.pChannel, fVolume * g_fEffectSoundValue);
		}
	}
}

CSound * CSound::Create(CTransform * pTrans)
{
	CSound* pInstance = new CSound;

	if (pInstance->Ready_Sound(pTrans))
	{
		MSG_BOX(L"CSound Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSound::Release(void)
{
	MAPCHANNEL::iterator iter = m_mapChannel.begin();
	MAPCHANNEL::iterator iter_end = m_mapChannel.end();

	for (iter; iter != iter_end; iter++)
	{
		FMOD_Channel_Stop(iter->second.pChannel);
	}
	m_mapChannel.clear();

	m_mapSound.clear();

	CComponent::Release();

	delete this;
}