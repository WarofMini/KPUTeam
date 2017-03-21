#ifndef Value_h__
#define Value_h__

#include "Typedef.h"

const _uint WINCX = 1600;
const _uint WINCY = 900;

const _uint	BONE_NUM = 8;
const _uint	BONE_MATRIX_NUM = 128;
const _ubyte MAX_NAME = 128;
const _uint VERTEXCOUNTX = 129;
const _uint VERTEXCOUNTZ = 129;
const _uint VERTEXINTERVAL = 1;

const _vec3 g_vLook(0.f, 0.f, 1.f);

const _ubyte NODELAYANICNT = 3;
const _ubyte MAX_BONE_MATRICES = 128;
const _ubyte INSTCNT = 32;

#define VS_SLOT_VIEW_MATRIX			0x00
#define VS_SLOT_PROJECTION_MATRIX	0x01
#define VS_SLOT_WORLD_MATRIX		0x02

#define VS_SLOT_BONE_MATRIX			0x03

#define PS_SLOT_TEXTURE				0x00
#define PS_SLOT_SAMPLER_STATE		0x00




#endif