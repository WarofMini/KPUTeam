#include "stdafx.h"
#include "FbxParser.h"
#include "DynamicMesh.h"
#include "AniBuffer.h"


void CFbxParser::ParsingVertex(FbxNode* _pNode, Animation* _pAnimation)
{
	std::map<unsigned int, std::vector<unsigned int>> mVertexByIndex;
	FbxNodeAttribute *pFbxNodeAttribute = _pNode->GetNodeAttribute();

	_pAnimation->pAniBuffer->m_strName = _pNode->GetName();

	if (pFbxNodeAttribute != nullptr && pFbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		//std::cout << "EXIST Attribute in " << _pNode->GetName() << std::endl;

		FbxMesh * pMesh = _pNode->GetMesh();

		//std::cout << "Load Vetex Count...";
		const unsigned int VertexCount = pMesh->GetControlPointsCount();
		const unsigned int IndexCount = pMesh->GetPolygonVertexCount();
		//std::cout << "DONE!" << std::endl;

		//std::cout << "Load Vertex Position...";
		{
			FbxVector4 *lVertexArray = new FbxVector4[VertexCount];
			for (unsigned int i = 0; i < VertexCount; i++)
				lVertexArray[i] = pMesh->GetControlPoints()[i];

			//====================================================
			int *IndexArray = new int[IndexCount];
			for (unsigned int i = 0; i < IndexCount; i++)
				IndexArray[i] = pMesh->GetPolygonVertices()[i];

			//인덱스 카운트만큼 크기할당
			_pAnimation->pAniBuffer->SetVertexSize(IndexCount);

			//==========================================================
			for (unsigned int i = 0; i < IndexCount; i++)
			{
				/* 버텍스의 정점 위치 정보를 가져온다. */
				_pAnimation->pAniBuffer->GetVertex(i)->SetPos(
					(float)lVertexArray[IndexArray[i]].mData[0],
					(float)lVertexArray[IndexArray[i]].mData[1],
					(float)lVertexArray[IndexArray[i]].mData[2]);

				mVertexByIndex[IndexArray[i]].push_back(i);
			}
		}
		//std::cout << "DONE!" << std::endl;

		//std::cout << "Load Vertex Normal...";
		FbxArray<FbxVector4> NormalArray;
		pMesh->GetPolygonVertexNormals(NormalArray);

		for (unsigned int i = 0; i < IndexCount; i++)
		{
			/* 버텍스의 노말 위치 정보를 가져온다. */
			_pAnimation->pAniBuffer->GetVertex(i)->SetNormal(
				(float)NormalArray[i].mData[0],
				(float)NormalArray[i].mData[1],
				(float)NormalArray[i].mData[2]);
		}
		//std::cout << "DONE!" << std::endl;


		{	// UV정보 저장
			FbxArray<FbxVector2> UVArray;		// UV의 배열
			FbxStringList uvNameList;	pMesh->GetUVSetNames(uvNameList);
			pMesh->GetPolygonVertexUVs(uvNameList.GetStringAt(0), UVArray);
			for (unsigned int i = 0; i < IndexCount; ++i)
			{
				// 위의 UV 배열에서 i번째 노멀을 가져다 해당 정보를 메쉬의 i번째 정점에 저장한다
				_pAnimation->pAniBuffer->GetVertex(i)->vTextureUV.x = (float)UVArray[i].mData[0];
				_pAnimation->pAniBuffer->GetVertex(i)->vTextureUV.y = 1.0f - (float)UVArray[i].mData[1];
			}
		}
		
		//================================================================


		FbxGeometry * pGeo = _pNode->GetGeometry();
		int SkinCount = pGeo->GetDeformerCount(FbxDeformer::eSkin);
		//std::cout << _pNode->GetName() << " 스킨의 갯수 : " << SkinCount << std::endl;
		if (SkinCount == 0)
		{
			string BoneName = _pNode->GetName();
			int INDEX = _pAnimation->mapIndexByName[BoneName];

			FbxAMatrix GeometryMtx = FbxAMatrix(_pNode->GetGeometricTranslation(FbxNode::eSourcePivot),
				_pNode->GetGeometricRotation(FbxNode::eSourcePivot),
				_pNode->GetGeometricScaling(FbxNode::eSourcePivot));

			//XMMATRIX ParsingData = XMMatrixIdentity();
			for (int m = 0; m < 4; ++m)
				for (int n = 0; n < 4; ++n)
					_pAnimation->pBaseBoneMatrix[INDEX].m[m][n] = (float)(GeometryMtx.Get(m, n));

			for (unsigned int i = 0; i < IndexCount; i++)
			{
				_pAnimation->pAniBuffer->GetVertex(i)->AddBone(INDEX, 1.f);

			}


			//_pAnimation->pBaseBoneMatrix[INDEX] *= ParsingData;

		}
		else if(SkinCount != 0)
		{
			for (int i = 0; i < SkinCount; i++) {
				FbxSkin * pSkin = (FbxSkin*)pGeo->GetDeformer(i, FbxDeformer::eSkin);
				int ClusterCount = pSkin->GetClusterCount();

				//std::cout << _pNode->GetName() << " 클러스터의 갯수 : " << ClusterCount << " " << std::endl;

				for (int j = 0; j < ClusterCount; j++)
				{
					FbxCluster *pCluster = pSkin->GetCluster(j);

					int ClusterIndexCount = pCluster->GetControlPointIndicesCount();
					int * ClusterIndices = pCluster->GetControlPointIndices();
					double * ClusterWeights = pCluster->GetControlPointWeights();

					//std::cout << _pNode->GetName() << " 클러스터 인덱스의 갯수 : " << ClusterIndexCount << " " << std::endl;
					for (int k = 0; k < ClusterIndexCount; k++)
					{
						std::string BoneName = std::string(pCluster->GetLink()->GetName());

						int INDEX = _pAnimation->mapIndexByName[BoneName];

						//std::cout<<m_mIndexByName.size()<<std::endl;
						FbxAMatrix LinkBoneMtx;
						FbxAMatrix TransBoneMtx;
						FbxAMatrix GeometryMtx;

						FbxAMatrix ResultMatrix;

						pCluster->GetTransformLinkMatrix(LinkBoneMtx);
						pCluster->GetTransformMatrix(TransBoneMtx);
						GeometryMtx = FbxAMatrix(_pNode->GetGeometricTranslation(FbxNode::eSourcePivot),
							_pNode->GetGeometricRotation(FbxNode::eSourcePivot),
							_pNode->GetGeometricScaling(FbxNode::eSourcePivot));

						ResultMatrix = LinkBoneMtx.Inverse() *  TransBoneMtx * GeometryMtx;

						for (int m = 0; m < 4; ++m)
							for (int n = 0; n < 4; ++n)
								_pAnimation->pBaseBoneMatrix[INDEX].m[m][n] = (float)(ResultMatrix.Get(m, n));


						float BoneWeight = (float)ClusterWeights[k];
						int BoneIndex = ClusterIndices[k];

						for (auto iter : mVertexByIndex[BoneIndex])
						{

							if (INDEX == 0 || INDEX == -1)
							{

							}
							else
							{
								_pAnimation->pAniBuffer->m_pVertex[iter].AddBone(INDEX, BoneWeight);
							}
						}

					}
				}
			}
		}

		_pAnimation->pAniBuffer->CreateBuffer();
	}
	else
	{
		//std::cout << "NOT EXIST Attribute in " << _pNode->GetName() << std::endl;
	}




	unsigned int nNodeChild = _pNode->GetChildCount();

	for (unsigned int i = 0; i < nNodeChild; i++)
	{
		FbxNode *pChildNode = _pNode->GetChild(i);

		CAniBuffer* pChild = new CAniBuffer;				//추가할 자식 할당
		_pAnimation->pAniBuffer->AddChild(pChild);							//자식 추가

		ParsingVertex(pChildNode, _pAnimation);
	}
}

void CFbxParser::ParsingAnimation(FbxNode* _pNode,
	Animation* _pAnimation,
	FbxAnimStack* _pAniStatck)
{
	//std::cout<<pNode->GetName()<<"\'s Child Count :"<<pNode->GetChildCount()<<std::endl;

	if (_pNode != nullptr)
	{
		unsigned int BoneIndex = _pAnimation->mapIndexByName[_pNode->GetName()];	//본의 인덱스를 가져온다.

																					//std::cout << _pNode->GetName() << "(INDEX:" << g_mIndexByName[_pNode->GetName()] << ") Load..." << g_mIndexByName.size() << std::endl;
		{
			FbxTime maxTime = _pAniStatck->GetLocalTimeSpan().GetDuration();
			for (long long i = 0; i < maxTime.GetMilliSeconds() / 10; i++)
			{
				FbxTime n_time;
				n_time.SetMilliSeconds(i * 10);

				for (int m = 0; m < 4; ++m)
					for (int n = 0; n < 4; ++n)
					{
						float fAni = (float)((_pNode->EvaluateGlobalTransform(n_time)).Get(m, n));

						_pAnimation->ppAniMatrix[i][BoneIndex].m[m][n] = fAni;
					}

			}
		}
	}

	for (int i = 0; i < _pNode->GetChildCount(); ++i)
		ParsingAnimation(_pNode->GetChild(i), _pAnimation, _pAniStatck);
}

