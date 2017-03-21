#include "stdafx.h"
#include "Animation.h"
#include "AnimationInfo.h"
#include "GraphicDev.h"

CAnimation::CAnimation(ID3D11DeviceContext* pContext)
: m_pContext(pContext)
{
}

CAnimation::~CAnimation(void)
{
}

void CAnimation::Push_AnimData(ANIMDATA& tAniData)
{
	m_vecAnimData.push_back(tAniData);
}

void CAnimation::UpdateSubresource(CAnimationInfo* pAnimInfo, _matrix* pBoneWorld)
{
	_matrix matBoneReturn;
	_matrix matBoneWorld[MAX_BONE_MATRICES];

	for (_int i = 0; i < MAX_BONE_MATRICES; ++i)
	{

		D3DXMatrixIdentity(&matBoneWorld[i]);

		if (pBoneWorld)
			D3DXMatrixIdentity(&pBoneWorld[i]);
	}

	if (pAnimInfo != NULL)
	{
		const _ushort& wCurKey = pAnimInfo->Get_CurKey();
		const _ushort& wNextKey = pAnimInfo->Get_NextKey();
		const _ushort& wCurFrame = pAnimInfo->Get_CurFrame();
		const _ushort& wNextFrame = pAnimInfo->Get_NextFrame();
		const _float& fRatio = pAnimInfo->Get_Ratio();

		size_t uiClusterSize = m_vecAnimData[wCurKey][wCurFrame].size();

		for (size_t iCluster = 0; iCluster < uiClusterSize; ++iCluster)
		{
			_matrix matRot, matRot2;
			_matrix matTrans, matTrans2;
			_matrix matScale, matScale2;
			
			D3DXMatrixRotationQuaternion(&matRot, &(D3DXQUATERNION)m_vecAnimData[wCurKey][wCurFrame][iCluster].R);
			D3DXMatrixRotationQuaternion(&matRot2, &(D3DXQUATERNION)m_vecAnimData[wCurKey][wNextFrame][iCluster].R);


			D3DXMatrixTranslation(&matTrans, m_vecAnimData[wCurKey][wCurFrame][iCluster].T.x, 
											 m_vecAnimData[wCurKey][wCurFrame][iCluster].T.y,
											 m_vecAnimData[wCurKey][wCurFrame][iCluster].T.z);

			D3DXMatrixTranslation(&matTrans2, m_vecAnimData[wCurKey][wNextFrame][iCluster].T.x,
											  m_vecAnimData[wCurKey][wNextFrame][iCluster].T.y,
											  m_vecAnimData[wCurKey][wNextFrame][iCluster].T.z);


			D3DXMatrixScaling(&matScale, m_vecAnimData[wCurKey][wCurFrame][iCluster].S.x,
										 m_vecAnimData[wCurKey][wCurFrame][iCluster].S.y,
										 m_vecAnimData[wCurKey][wCurFrame][iCluster].S.z);


			D3DXMatrixScaling(&matScale2, m_vecAnimData[wCurKey][wNextFrame][iCluster].S.x,
										  m_vecAnimData[wCurKey][wNextFrame][iCluster].S.y,
										  m_vecAnimData[wCurKey][wNextFrame][iCluster].S.z);


			for (int iRow = 0; iRow < 4; ++iRow)
			{
				for (int iColumn = 0; iColumn < 4; ++iColumn)
				{
					matTrans.m[iRow][iColumn] = (matTrans.m[iRow][iColumn] * (1.f - fRatio)) + (matTrans2.m[iRow][iColumn] * fRatio);
					matScale.m[iRow][iColumn] = (matScale.m[iRow][iColumn] * (1.f - fRatio)) + (matScale2.m[iRow][iColumn] * fRatio);

					matRot.m[iRow][iColumn] = (matRot.m[iRow][iColumn] * (1.f - fRatio)) + (matRot2.m[iRow][iColumn] * fRatio);
				}
			}

			matBoneWorld[iCluster] = matScale * matRot * matTrans;
			pBoneWorld[iCluster] = matScale * matRot * matTrans;
		}
	}

	ID3D11Buffer* pDynamicShaderCB = CGraphicDev::GetInstance()->GetDynamicShaderCB();
	m_pContext->UpdateSubresource(pDynamicShaderCB, 0, NULL, &matBoneWorld, 0, 0);
}

CAnimation* CAnimation::Create(ID3D11DeviceContext* pContext)
{
	CAnimation* pAnimation = new CAnimation(pContext);
	return pAnimation;
}

void CAnimation::Release(void)
{
	delete this;
}