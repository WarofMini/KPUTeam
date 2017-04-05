#ifndef Enum_h__
#define Enum_h__


enum RENDERGROUP { TYPE_PRIORITY, TYPE_NONEALPHA, TYPE_ALPHA, TYPE_UI, TYPE_END };

enum RESOURCETYPE { RESOURCE_STATIC, RESOURCE_LOGIN, RESOURCE_STAGE, RESOURCE_END };


enum Scene_Type {SCENE_LOGIN, SCENE_LOGO, SCENE_STAGE, SCENE_END};


enum MESHNUM { MESHNUM_PLAYER, MESHNUM_GUN, MESHNUM_FLOOR1, MESHNUM_FLOOR2, MESHNUM_FLOOR3, MESHNUM_BOOK1, MESHNUM_BOOK2, MESHNUM_BOOK3,
			   MESHNUM_BOOKBOX1, MESHNUM_BOOKBOX2, MESHNUM_CABINET1, MESHNUM_CABINET2,
			   MESHNUM_CHAIR1, MESHNUM_CHAIR2, MESHNUM_CHAIR3, MESHNUM_CHAIR4, 
			   MESHNUM_COFFEETABLE1, MESHNUM_COFFEETABLE2, MESHNUM_COUCH1, MESHNUM_COUCH2, MESHNUM_COUCH3, 
			   MESHNUM_COUCH4, MESHNUM_DESK1, MESHNUM_DESK2, MESHNUM_DESK3, MESHNUM_DESK4, MESHNUM_DESK5,
			   MESHNUM_DESK6, MESHNUM_DESK7, MESHNUM_DESK8, MESHNUM_DESK9, MESHNUM_DESK10, MESHNUM_DESK11,
			   MESHNUM_DESK12, MESHNUM_DOOR1, MESHNUM_DOOR2, MESHNUM_PAINTING1, MESHNUM_PAINTING2,
			   MESHNUM_PLANT1, MESHNUM_PLANT2, MESHNUM_PLANT3, MESHNUM_PLANT4, MESHNUM_PLANT5,
			   MESHNUM_RUBBISHBIN1, MESHNUM_RUBBISHBIN2, MESHNUM_SHELF1, MESHNUM_SHELF2, MESHNUM_SHELF3,
			   MESHNUM_TOILETSTALL1, MESHNUM_TOILETSTALL2, MESHNUM_WALL1, MESHNUM_WALL2, MESHNUM_WALL3,
			   MESHNUM_WALL4, MESHNUM_WALL5, MESHNUM_WALL6, MESHNUM_WALL7, MESHNUM_WALL8, MESHNUM_WALL9, MESHNUM_WALL10,
			   MESHNUM_CLOCK, MESHNUM_HANDDRYER, MESHNUM_MIRROR, MESHNUM_PAPERTOWELDISPENSER,
			   MESHNUM_PHOTOCOPIER, MESHNUM_PINBOARD, MESHNUM_POWEROUTLET, MESHNUM_SINKS, MESHNUM_SOAPDISPENSER,
			   MESHNUM_URINAL, MESHNUM_WATERCOOLER, MESHNUM_SPHERE, MESHNUM_END };

enum ANIPLAYER {
	PLAYER_idle
	, PLAYER_idle2
	, PLAYER_sprint
	, PLAYER_Runback
	, PLAYER_RunForward
	, PLAYER_RunLeft
	, PLAYER_RunRight
	, PLAYER_Jump				//이거 안쓰고	in loop out
	, PLAYER_JumpIn
	, PLAYER_JumpLoop
	, PLAYER_JumpOut
	, PLAYER_RollBack
	, PLAYER_RollFront
	, PLAYER_RollLeft
	, PLAYER_RollRight
	, PLAYER_Damage
	, PLAYER_Reload
	, PLAYER_Shoot
	, PLAYER_GetCover			//이거 안쓰고	in loop out
	, PLAYER_GetCoverIn//
	, PLAYER_GetCoverLoop//
	, PLAYER_GetCoverOut//
	, PLAYER_RunForwardShoot
	, PLAYER_RunBackShoot
	, PLAYER_RunLeftShoot
	, PLAYER_RunRightShoot
	, PLAYER_JumpAndShoot
	, PLAYER_JumpAndShootIn
	, PLAYER_JumpAndShootLoop
	, PLAYER_JumpAndShootOut
	, PLAYER_ShootFromCover
	, PLAYER_Death
	, PLAYER_Death2
	, PLAYER_Lying
	, PLAYER_LyingShoot
	, PLAYER_CrawlingFront
	, PLAYER_CrawlingBack
	, PLAYER_CrawlingLeft
	, PLAYER_CrawlingRight
	, PLAYER_END
};

#endif