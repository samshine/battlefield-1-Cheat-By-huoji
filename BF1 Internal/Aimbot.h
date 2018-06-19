#pragma once

#include "stdafx.h"

void AimbotThread();

class Aimbot
{
public:
	
	 static int Aimkey1;
	 static  int Aimkey2;
	 static float SmoothFactor;
	 static  float FOV;
	 static float Distance;
	 static  int RetargetTimer;
	 static  int Bone;
	 static 	 bool RandomBone;
	 static  bool PrioritizeDistance;
	 static  bool AimPrediction;
	 static  float AimPredictXYZ;
	 static  float AimOffsetY;
	 static  bool AimVehicles;
	 static  bool AimHorses;
	 static  int VehicleBone;
	 static  int HorseBone;
	 static  bool AimSlots2;
	 static  bool AimSlots5;
	 static  bool AimSlots6;
	 static  bool AimSlots7;
	 static  int Weapon;
	 struct
	{
		 bool Enable;
		 int Aimkey1;
		 int Aimkey2;
		 float SmoothFactor = 0.070f;
		 float FOV = 90.f;
		 float Distance = 300.f;
		 int RetargetTimer = 200.f;
		 bool RandomBone;
		 bool PrioritizeDistance;
		 bool AimPrediction;
		 float AimPredictXYZ = 1;
		 float AimOffsetY = 1;
		 bool AimVehicles;
		 bool AimHorses;
		 int VehicleBone;
		 int HorseBone;
		 int Bone;
		 int BoneSelected;
	} A;
	 struct
	{
		 bool Enable;
		 int Aimkey1;
		 int Aimkey2;
		 float SmoothFactor = 0.070f;
		 float FOV = 90.f;
		 float Distance = 300.f;
		 int RetargetTimer = 200.f;
		 bool RandomBone;
		 bool PrioritizeDistance;
		 bool AimPrediction;
		 float AimPredictXYZ = 1;
		 float AimOffsetY = 1;
		 bool AimVehicles;
		 bool AimHorses;
		 int VehicleBone;
		 int HorseBone;
		 int Bone;
		 int BoneSelected;
	} B;
	 struct
	{
		 bool Enable;
		 int Aimkey1;
		 int Aimkey2;
		 float SmoothFactor = 0.070f;
		 float FOV = 90.f;
		 float Distance = 300.f;
		 int RetargetTimer = 200.f;
		 bool RandomBone;
		 bool PrioritizeDistance;
		 bool AimPrediction;
		 float AimPredictXYZ = 1;
		 float AimOffsetY = 1;
		 bool AimVehicles;
		 bool AimHorses;
		 int VehicleBone;
		 int HorseBone;
		 int Bone;
		 int BoneSelected;
	} C;
	 struct
	{
		 bool Enable;
		 int Aimkey1;
		 int Aimkey2;
		 float SmoothFactor = 0.070f;
		 float FOV = 90.f;
		 float Distance = 300.f;
		 int RetargetTimer = 200.f;
		 bool RandomBone;
		 bool PrioritizeDistance;
		 bool AimPrediction;
		 float AimPredictXYZ = 1;
		 float AimOffsetY = 1;
		 bool AimVehicles;
		 bool AimHorses;
		 int VehicleBone;
		 int HorseBone;
		 int Bone;
		 int BoneSelected;
	} D;
     struct
	{
		 bool Enable;
		 int Aimkey1;
		 int Aimkey2;
		 float SmoothFactor = 0.070f;
		 float FOV = 90.f;
		 float Distance = 300.f;
		 int RetargetTimer = 200.f;
		 bool RandomBone;
		 bool PrioritizeDistance;
		 bool AimPrediction;
		 float AimPredictXYZ = 1;
		 float AimOffsetY = 1;
		 bool AimVehicles;
		 bool AimHorses;
		 int VehicleBone;
		 int HorseBone;
		 int Bone;
		 int BoneSelected;
	} E;
	static Vector3 CalculateAngle(Vector3 _Target, Vector3 _LocalPos, Vector3 _Angles);
};
extern Aimbot Vars;
extern int key_StartHack;
extern int key_ShowMenu;
extern int key_Quit;
extern int key_Save;
extern int key_ShowName;
extern int key_Aimbot;
extern int key_Radar;
extern int key_RadarWide;
extern int key_NoSway;
extern int key_giveCleanSS;
extern int key_MedicBugfix;
extern int key_ESP;
extern int key_HEAD;
extern int key_NECK;
extern int key_HIP;
extern int key_TestSSBitBlt;
extern int key_TestSSCopyResource;
static const char* Weapon[] = 
{
	u8"第一武器",
	u8"第二武器",
	u8"第五武器",	
	u8"第六武器",
	u8"第七武器"
};
static const char* keyNames[] =
{
	"",
	"Mouse 1",
	"Mouse 2",
	"Cancel",
	"Middle Mouse",
	"Mouse 4",
	"Mouse 5",
	"",
	"Backspace",
	"Tab",
	"",
	"",
	"Clear",
	"Enter",
	"",
	"",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"Escape",
	"",
	"",
	"",
	"",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"",
	"",
	"",
	"Print",
	"Insert",
	"Delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"",
	"",
	"",
	"",
	"",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",

};