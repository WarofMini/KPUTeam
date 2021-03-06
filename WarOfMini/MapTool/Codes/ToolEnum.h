#ifndef ToolEnum_h__
#define ToolEnum_h__


enum OBJECT_MODE {MODE_CREATE, MODE_STOP, MODE_MODIFY, MODE_SELECT, MODE_FIX, MODE_DEBUG};

enum ANGLE { ANGLE_X, ANGLE_Y, ANGLE_Z, ANGLE_END };

enum RENDERGROUP { TYPE_PRIORITY, TYPE_NONEALPHA, TYPE_ALPHA, TYPE_UI, TYPE_END };

enum RESOURCETYPE { RESOURCE_LOGO, RESOURCE_STAGE, RESOURCE_STATIC, RESOURCE_END };

enum BUFFERTYPE { BUFFER_RCCOL, BUFFER_RCTEX, BUFFER_TERRAIN, BUFFER_CUBETEX, BUFFER_CUBECOL };

enum TEXTURETYPE { TEXTURE_NORMAL, TEXTURE_CUBE };

enum ShaderType { SHADER_VS, SHADER_PS, SHADER_ANI, SHADER_VS_COLOR, SHADER_END };

enum ReleaseType { Release_Static, Release_Dynamic, Release_End };

enum Scene_Tyep { SCENE_TOOLSTAGE, SCENE_END };

enum DrawType { DRAW_INDEX, DRAW_VERTEX };

enum MESHTYPE { MESH_STATIC, MESH_DYNAMIC };

enum OBJSTATE { STATE_ALIVE, STATE_DESTROY, STATE_END};
#endif // !ToolEnum_h__
