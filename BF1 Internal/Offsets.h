#pragma once

#define OFFSET_GAMERENDERER			0x143631010  //0x14360ed98//0x14368E2A0
#define OFFSET_DXRENDERER			0x1436313b8  //0x14360f298//0x14368E798
#define BORDER_INPUT_NODE			0x1430ddd68 //0x1430BBD58//0x143112D58 not used; if dont have this offset BORDER_INPUT_NODE in DX11.h disable: pfh = new CVMTHookManager64((QWORD**)BorderInputNode::Get()->input_node);
#define OFFSET_GAMECONTEXT_2		0x143441c68 //0x14341f2d0//0x14349E810  //tova e CLIENT_GAME_CONTEXT

/*
20.07.2017:
//Core Functions
+OFFSET_CLIENTGAMECONTEXT : 0x1434272d0
+OFFSET_GAMERENDERER : 0x143616d98
OFFSET_MAIN : 0x143133dc8
OFFSET_FIRSTTYPEINFO : 0x143397ad0
OFFSET_DISPATCHMESSAGE : 0x143f66f30
OFFSET_ENTITYLIST : 0x1447bf020

OFFSET_DXRENDERER : 0x143617298
OFFSET_GAMERENDERER : 0x143616d98
OFFSET_WORLDRENDER_SETTINGS : 0x14343af80
OFFSET_CLIENTGAMECONTEXT : 0x1434272d0
OFFSET_MAIN : 0x143133dc8
OFFSET_FFSCREENSHOT : 0xffffffff00000000
OFFSET_FIRSTTYPEINFO : 0x143397ad0
OFFSET_DX12RENDERER : 0x1436182e0
OFFSET_TEAMID : 0x1c34

#define OFFSET_GAMERENDERER 0x143616D98
#define CLIENT_GAME_CONTEXT 0x1434272D0
#define BORDER_INPUT_NODE 0x1430C3D58
#define CLIENT_GAME_WORLD 0x1434272E8
#define OFFSET_MAIN 0x143133DC8
#define OFFSET_FIRST_TYPE_INFO 0x143397AD0
#define GET_ENTITY_LIST 0x1447BF020
#define DX11_RENDERER 0x143617298
#define m_dwResourceManager 0x144000AF5
#define m_dwServerSettings 0x143F5D4B1
#define m_dwDispatchMessage 0x143F66F30

#define VeniceClientBulletEntity 0x143CCC3C0
#define ClientSupplySphereEntity 0x143C98B10
#define ClientExplosionPackEntity 0x143C98ED0
#define ClientGrenadeEntity 0x143C98D90
#define ClientPickupEntity 0x143C97070

old:
https://www.unknowncheats.me/forum/battlefield-1-a/186728-battlefield-1-reversing-struct-offsets-thread-56.html
#define OFFSET_GAMERENDERER 0x14360ED98
#define CLIENT_GAME_CONTEXT 0x14341F2D0
#define BORDER_INPUT_NODE 0x1430BBD58
#define CLIENT_GAME_WORLD 0x14341F2E8
#define OFFSET_MAIN 0x14312BDC8
#define OFFSET_FIRST_TYPE_INFO 0x14338FAD0
#define GET_ENTITY_LIST 0x1447BF390
#define DX_RENDERER 0x14360F298
#define m_dwResourceManager 0x143FFCEEE
#define m_dwServerSettings 0x143F58FAF
#define m_dwDispatchMessage 0x143F62E90
#define VeniceClientBulletEntity 0x143CC4300
#define ClientSupplySphereEntity 0x143C90AC0
#define ClientExplosionPackEntity 0x143C90E80
#define ClientGrenadeEntity 0x143C90D40
#define ClientPickupEntity 0x143C8F020
*/