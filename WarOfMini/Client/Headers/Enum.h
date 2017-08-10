#ifndef Enum_h__
#define Enum_h__


enum RENDERGROUP { TYPE_PRIORITY, TYPE_NONEALPHA, TYPE_ALPHA, TYPE_UI, TYPE_END };

enum RESOURCETYPE { RESOURCE_STATIC, RESOURCE_LOGIN, RESOURCE_STAGE, RESOURCE_END };


enum Scene_Type {SCENE_TITLE, SCENE_LOGO, SCENE_STAGE, SCENE_END};


enum MESHNUM { MESHNUM_PLAYER, MESHNUM_PLAYER2, MESHNUM_GUN, MESHNUM_SPECIALGUN, MESHNUM_FLOOR1, MESHNUM_FLOOR2, MESHNUM_FLOOR3, MESHNUM_BOOK1, MESHNUM_BOOK2, MESHNUM_BOOK3,
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
			   MESHNUM_URINAL, MESHNUM_WATERCOOLER, MESHNUM_CEILING, MESHNUM_SPHERE, MESHNUM_TOWER, MESHNUM_END };

enum ANIPLAYER {
	PLAYER_idle
	, PLAYER_idle2
	, PLAYER_sprint
	, PLAYER_Runback
	, PLAYER_RunForward
	, PLAYER_RunRight
	, PLAYER_RunLeft
	, PLAYER_Jump				//이거 안쓰고	in loop out
	, PLAYER_JumpIn
	, PLAYER_JumpLoop
	, PLAYER_JumpOut
	, PLAYER_RollBack
	, PLAYER_RollFront
	, PLAYER_RollRight
	, PLAYER_RollLeft
	, PLAYER_Damage
	, PLAYER_Reload
	, PLAYER_Shoot
	, PLAYER_GetCover			//이거 안쓰고	in loop out
	, PLAYER_GetCoverIn//
	, PLAYER_GetCoverLoop//
	, PLAYER_GetCoverOut//
	, PLAYER_RunForwardShoot
	, PLAYER_RunBackShoot
	, PLAYER_RunRightShoot
	, PLAYER_RunLeftShoot
	, PLAYER_JumpAndShoot		//이거 안쓰고	in loop out
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
	, PLAYER_CrawlingRight
	, PLAYER_CrawlingLeft
	, PLAYER_END
};

enum ANIPLAYER_Iron {
	PLAYER_Iron_RunForward
	, PLAYER_Iron_RunLeft
	, PLAYER_Iron_RunRight
	, PLAYER_Iron_RunBack
	, PLAYER_Iron_Sprint
	, PLAYER_Iron_RunForwardShoot
	, PLAYER_Iron_RunLeftShoot
	, PLAYER_Iron_RunRightShoot
	, PLAYER_Iron_RunBackShoot
	, PLAYER_Iron_Idle
	, PLAYER_Iron_Idle2
	, PLAYER_Iron_Shoot
	, PLAYER_Iron_JumpandShoot
	, PLAYER_Iron_JumpandShootIn
	, PLAYER_Iron_JumpandShootLoop
	, PLAYER_Iron_JumpandShootOut
	, PLAYER_Iron_GetCover
	, PLAYER_Iron_ShootFromCover
	, PLAYER_Iron_RollFront
	, PLAYER_Iron_RollBack
	, PLAYER_Iron_RollRight
	, PLAYER_Iron_RollLeft
	, PLAYER_Iron_DownLying
	, PLAYER_Iron_Lying
	, PLAYER_Iron_LyingShoot
	, PLAYER_Iron_CrawlingReady
	, PLAYER_Iron_CrawlingFront
	, PLAYER_Iron_CrawlingRight
	, PLAYER_Iron_CrawlingLeft
	, PLAYER_Iron_CrawlingBack
	, PLAYER_Iron_StandUp
	, PLAYER_Iron_ReLoad
	, PLAYER_Iron_Damage
	, PLAYER_Iron_Death
	, PLAYER_Iron_Death2
};

enum ANIGABISCON {
	GABISCON_Idle_Rifle_Hip_Break1
	, GABISCON_Walk_Fwd_Rifle_Ironsights
	, GABISCON_Walk_Bwd_Rifle_Ironsights
	, GABISCON_Walk_Lt_Rifle_Ironsights
	, GABISCON_Walk_Rt_Rifle_Ironsights
	, GABISCON_Sprint_Fwd_Rifle
	, GABISCON_Fire_Shotgun_Hip
	, GABISCON_Fire_Shotgun_Ironsights
	, GABISCON_anim_spear_ready
	, GABISCON_anim_spear_throwing
	, GABISCON_Reload_Rifle_Ironsights
	, GABISCON_Reload_Shotgun_Ironsights
};

enum ANITIRE
{
	Tire_aimspace
	, Tire_engine_idle
	, Tire_engine_idle2
	, Tire_Moving_fast
	, Tire_Moving_moderate
	, Tire_Moving_slow
};

enum eKey {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_Q, KEY_R,
	KEY_SHIFT, KEY_CTRL, KEY_SPACE, KEY_LCLICK, KEY_RCLICK, KEY_END
};
enum MOVE_DIR { DIR_U, DIR_UR, DIR_UL, DIR_R, DIR_L, DIR_D, DIR_DR, DIR_DL, DIR_END};

#endif

enum ANITRUCK
{
	Truck_aimspace
	, Truck_door_close
	, Truck_door_open
	, Truck_engine_idle
	, Truck_engine_idle2
	, Truck_engine_off
	, Truck_Moving_fast
	, Truck_Moving_moderate
	, Truck_Moving_slow
	, Truck_shoot2
	, Truck_turret_aimspace
};
/*
Anim_BT_aimspace	0 ~ 24
Anim_BT_door_close 25 _ 84
Anim_BT_door_open 85 ~ 120
Anim_BT_engine_idle 121 ~181
Anim_BT_engine_idle2 182 ~ 242
Anim_bt_engine_off 243 ~ 244
Anim_BT_Moving_fast 245 ~ 305
Anim_BT_Moving_moderate 306 ~ 366
Anim_BT_Moving_slow 367 ~ 427
Anim_BT_shoot2 428 ~ 448
Anim_BT_turret_aimspace 449 _544
*/