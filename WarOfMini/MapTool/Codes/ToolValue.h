#ifndef ToolValue_h__
#define ToolValue_h__


const UINT WINCX = 1024;
const UINT WINCY = 768;

const UINT	BONE_NUM = 8;
const UINT	BONE_MATRIX_NUM = 128;

const UINT VERTEXCOUNTX = 129;
const UINT VERTEXCOUNTZ = 129;
const UINT VERTEXINTERVAL = 1;

const D3DXVECTOR3 g_vLook(0.f, 0.f, 1.f);

#define VS_SLOT_VIEW_MATRIX			0x00
#define VS_SLOT_PROJECTION_MATRIX	0x01
#define VS_SLOT_WORLD_MATRIX		0x02

#define VS_SLOT_BONE_MATRIX			0x03

#define PS_SLOT_TEXTURE				0x00
#define PS_SLOT_SAMPLER_STATE		0x00



#endif