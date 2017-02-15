#ifndef ToolFbxParser_h__
#define ToolFbxParser_h__

#include "DynamicMesh.h"

class CFbxParser
{
public:
	static void	ParsingVertex(FbxNode* _pNode, Animation* _pAnimation);
	static void	ParsingAnimation(FbxNode* _pNode, Animation* _pAnimation, FbxAnimStack* _pAniStatck);
};

#endif