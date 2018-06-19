#include "stdafx.h"
#include "DX11.h"
#include "Game.h"
#include "imgui.h"
ImColor mainColor = ImColor(54, 54, 54, 255);
float DX11Renderer::ScreenX = 0, DX11Renderer::ScreenY = 0;
float DX11Renderer::ScreenSX = 0, DX11Renderer::ScreenSY = 0;

Vector2 GUI2::LastMousePosition = Vector2 (0, 0);
Vector2 GUI2::MousePosition = Vector2(0, 0);

using D3D11PresentHook = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
D3D11PresentHook DX11HookPresent = NULL;
PLH::Detour* PresentHook;

using PerFrameHook_t = int(__stdcall*)(uintptr_t, float);
PerFrameHook_t PerFrameHook = NULL;
PLH::VEHHook* DetourPerFrameHook;
CVMTHookManager64* pfh;

static INT LastTick;
static int LastLogSize=0;
bool ShowLogAlert = false;
bool giveCleanSS = false;
std::mutex DX11_mutex;
Aimbot Vars;
extern  CEntity* LocalPlayers;
int __stdcall HookedPerFrame(uintptr_t _this, float a1)
{	

	typedef CEntity* (__fastcall* tGetLocalPlayerByIndex)(ClientPlayerManager*, int index);
	tGetLocalPlayerByIndex GetLocalPlayerByIndex = reinterpret_cast<tGetLocalPlayerByIndex>(0x14486BE80);
	ClientPlayerManager* temp = Game::GetClientPlayerManager();
	if(temp)
		LocalPlayers = GetLocalPlayerByIndex(temp, 0);
	return PerFrameHook(_this, a1);
}
void DX11Renderer::AimbotTab()
{
	/*
	
	const char* Bones[] = { u8"头", u8"脖子", u8"胸", u8"髋关节" };
//	static int BoneSelected = -1;
	ImGui::Columns(3, "Aimbot Shit", true);
	{
		ImGui::Checkbox(u8"激活自瞄", &Features::Aimbot);
	}

	ImGui::NextColumn();
	{
		ImGui::TextColored(ImColor(0, 255, 0), u8"目前的设置: %i", Vars.Bone);
	}
	ImGui::NextColumn();
	{
		ImGui::Combo(u8"武器配置", &Vars.Weapon, Weapon, ARRAYSIZE(Weapon));
	}

	ImGui::Columns(1);

	ImGui::Separator();
	if (Vars.Weapon == 0)
	{
		ImGui::Columns(2, NULL, true);
		{
			ImGui::BeginChild("Shit1", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(u8"激活自瞄", &Features::Aimbot);
				ImGui::Combo(u8"自瞄第一按键", &Vars.A.Aimkey1, keyNames, ARRAYSIZE(keyNames));
				ImGui::Combo(u8"自瞄第二按键", &Vars.A.Aimkey2, keyNames, ARRAYSIZE(keyNames));
				ImGui::Checkbox(u8"优先考虑距离", &Vars.A.PrioritizeDistance);
				ImGui::Checkbox(u8"随即部位", &Vars.A.RandomBone);
				if (ImGui::Combo(u8"部位", &Vars.A.BoneSelected, Bones, 4))
				{
					if (Vars.A.BoneSelected == 0)
						Vars.A.Bone = eBones::HEAD;
					else if (Vars.A.BoneSelected == 1)
						Vars.A.Bone = eBones::NECK;
					else if (Vars.A.BoneSelected == 2)
						Vars.A.Bone = eBones::CHEST;
					else if (Vars.A.BoneSelected == 3)
						Vars.A.Bone = eBones::HIP;
				}
				ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.A.AimVehicles);
				if (Vars.A.AimVehicles) {
					ImGui::RadioButton(u8"顶部", &Vars.A.VehicleBone, TOP); ImGui::SameLine();
					ImGui::RadioButton(u8"偏上", &Vars.A.VehicleBone, UP); ImGui::SameLine();
					ImGui::RadioButton(u8"中间", &Vars.A.VehicleBone, CENTER); ImGui::SameLine();
					ImGui::RadioButton(u8"偏下", &Vars.A.VehicleBone, DOWN); ImGui::SameLine();
					ImGui::RadioButton(u8"底下", &Vars.A.VehicleBone, BOTTOM);
				}
				ImGui::EndChild();
			}
			


		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("Shit2", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(u8"预测", &Vars.A.AimPrediction);
				if (Vars.A.AimPrediction) {
					ImGui::SliderFloat(u8"预测的 XYZ", &Vars.A.AimPredictXYZ, 0.1f, 3.f, "%.1f");
					ImGui::SliderFloat(u8"Y轴预测", &Vars.A.AimOffsetY, 0.1f, 3.f, "%.1f");
				}
				ImGui::SliderFloat(u8"角度", &Vars.A.FOV, 1.f, 360.f, "%.0f");
				ImGui::SliderFloat(u8"最大瞄准距离", &Vars.A.Distance, 1.f, 750.f, "%.0f");
				ImGui::SliderFloat(u8"速度", &Vars.A.SmoothFactor, 0.001f, 1.f, "%.3f");
				ImGui::SliderInt(u8"重新定位时间", &Vars.A.RetargetTimer, 1, 1000);
				ImGui::Checkbox(u8"瞄准马", &Vars.A.AimHorses);
				if (Vars.A.AimHorses) {
					ImGui::RadioButton(u8"顶部", &Vars.A.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
					ImGui::RadioButton(u8"偏上", &Vars.A.HorseBone, UP); ImGui::SameLine();
					ImGui::RadioButton(u8"中间", &Vars.A.HorseBone, CENTER); ImGui::SameLine();
					ImGui::RadioButton(u8"偏下", &Vars.A.HorseBone, DOWN); ImGui::SameLine();
					ImGui::RadioButton(u8"底下", &Vars.A.HorseBone, BOTTOM);
				}
				ImGui::EndChild();
			}

		}
	}
	if (Vars.Weapon == 1)
	{
		ImGui::Columns(2, NULL, true);
		{
			ImGui::BeginChild("Shit3", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(u8"启用第二武器自瞄", &Vars.AimSlots2);
				ImGui::Combo(u8"自瞄第一按键", &Vars.B.Aimkey1, keyNames, ARRAYSIZE(keyNames));
				ImGui::Combo(u8"自瞄第二按键", &Vars.B.Aimkey2, keyNames, ARRAYSIZE(keyNames));
				ImGui::Checkbox(u8"优先考虑距离", &Vars.B.PrioritizeDistance);
				ImGui::Checkbox(u8"随即部位", &Vars.B.RandomBone);
				if (ImGui::Combo(u8"部位", &Vars.B.BoneSelected, Bones, 4))
				{
					if (Vars.B.BoneSelected == 0)
						Vars.B.Bone = eBones::HEAD;
					else if (Vars.B.BoneSelected == 1)
						Vars.B.Bone = eBones::NECK;
					else if (Vars.B.BoneSelected == 2)
						Vars.B.Bone = eBones::CHEST;
					else if (Vars.B.BoneSelected == 3)
						Vars.B.Bone = eBones::HIP;
				}
				ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.B.AimVehicles);
				if (Vars.B.AimVehicles) {
					ImGui::RadioButton(u8"顶部", &Vars.B.VehicleBone, TOP); ImGui::SameLine();
					ImGui::RadioButton(u8"偏上", &Vars.B.VehicleBone, UP); ImGui::SameLine();
					ImGui::RadioButton(u8"中间", &Vars.B.VehicleBone, CENTER); ImGui::SameLine();
					ImGui::RadioButton(u8"偏下", &Vars.B.VehicleBone, DOWN); ImGui::SameLine();
					ImGui::RadioButton(u8"底下", &Vars.B.VehicleBone, BOTTOM);
				}

				ImGui::EndChild();
			}


		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("Shit4", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(u8"预测", &Vars.B.AimPrediction);
				if (Vars.B.AimPrediction) {
					ImGui::SliderFloat(u8"预测的 XYZ", &Vars.B.AimPredictXYZ, 0.1f, 3.f, "%.1f");
					ImGui::SliderFloat(u8"Y轴预测", &Vars.B.AimOffsetY, 0.1f, 3.f, "%.1f");
				}
				ImGui::SliderFloat(u8"角度", &Vars.B.FOV, 1.f, 360.f, "%.0f");
				ImGui::SliderFloat(u8"最大瞄准距离", &Vars.B.Distance, 1.f, 750.f, "%.0f");
				ImGui::SliderFloat(u8"速度", &Vars.B.SmoothFactor, 0.001f, 1.f, "%.3f");
				ImGui::SliderInt(u8"重新定位时间", &Vars.B.RetargetTimer, 1, 1000);
				ImGui::Checkbox(u8"瞄准马", &Vars.B.AimHorses);
				if (Vars.B.AimHorses) {
					ImGui::RadioButton(u8"顶部", &Vars.B.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
					ImGui::RadioButton(u8"偏上", &Vars.B.HorseBone, UP); ImGui::SameLine();
					ImGui::RadioButton(u8"中间", &Vars.B.HorseBone, CENTER); ImGui::SameLine();
					ImGui::RadioButton(u8"偏下", &Vars.B.HorseBone, DOWN); ImGui::SameLine();
					ImGui::RadioButton(u8"底下", &Vars.B.HorseBone, BOTTOM);
				}
				ImGui::EndChild();
			}

		}
	}
	if (Vars.Weapon == 2)
	{
		ImGui::Columns(2, NULL, true);
		{
			ImGui::BeginChild("Shit5", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(u8"启用第五武器自瞄", &Vars.AimSlots5);
				ImGui::Combo(u8"自瞄第一按键", &Vars.C.Aimkey1, keyNames, ARRAYSIZE(keyNames));
				ImGui::Combo(u8"自瞄第二按键", &Vars.C.Aimkey2, keyNames, ARRAYSIZE(keyNames));
				ImGui::Checkbox(u8"优先考虑距离", &Vars.C.PrioritizeDistance);
				ImGui::Checkbox(u8"随即部位", &Vars.C.RandomBone);
				if (ImGui::Combo(u8"部位", &Vars.C.BoneSelected, Bones, 4))
				{
					if (Vars.C.BoneSelected == 0)
						Vars.C.Bone = eBones::HEAD;
					else if (Vars.C.BoneSelected == 1)
						Vars.C.Bone = eBones::NECK;
					else if (Vars.C.BoneSelected == 2)
						Vars.C.Bone = eBones::CHEST;
					else if (Vars.C.BoneSelected == 3)
						Vars.C.Bone = eBones::HIP;
				}
				ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.C.AimVehicles);
				if (Vars.C.AimVehicles) {
					ImGui::RadioButton(u8"顶部", &Vars.C.VehicleBone, TOP); ImGui::SameLine();
					ImGui::RadioButton(u8"偏上", &Vars.C.VehicleBone, UP); ImGui::SameLine();
					ImGui::RadioButton(u8"中间", &Vars.C.VehicleBone, CENTER); ImGui::SameLine();
					ImGui::RadioButton(u8"偏下", &Vars.C.VehicleBone, DOWN); ImGui::SameLine();
					ImGui::RadioButton(u8"底下", &Vars.C.VehicleBone, BOTTOM);
				}
				ImGui::EndChild();
			}



		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("Shit6", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(u8"预测", &Vars.C.AimPrediction);
				if (Vars.C.AimPrediction) {
					ImGui::SliderFloat(u8"预测的 XYZ", &Vars.C.AimPredictXYZ, 0.1f, 3.f, "%.1f");
					ImGui::SliderFloat(u8"Y轴预测", &Vars.C.AimOffsetY, 0.1f, 3.f, "%.1f");
				}
				ImGui::SliderFloat(u8"角度", &Vars.C.FOV, 1.f, 360.f, "%.0f");
				ImGui::SliderFloat(u8"最大瞄准距离", &Vars.C.Distance, 1.f, 750.f, "%.0f");
				ImGui::SliderFloat(u8"速度", &Vars.C.SmoothFactor, 0.001f, 1.f, "%.3f");
				ImGui::SliderInt(u8"重新定位时间", &Vars.C.RetargetTimer, 1, 1000);
				ImGui::Checkbox(u8"瞄准马", &Vars.C.AimHorses);
				if (Vars.C.AimHorses) {
					ImGui::RadioButton(u8"顶部", &Vars.C.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
					ImGui::RadioButton(u8"偏上", &Vars.C.HorseBone, UP); ImGui::SameLine();
					ImGui::RadioButton(u8"中间", &Vars.C.HorseBone, CENTER); ImGui::SameLine();
					ImGui::RadioButton(u8"偏下", &Vars.C.HorseBone, DOWN); ImGui::SameLine();
					ImGui::RadioButton(u8"底下", &Vars.C.HorseBone, BOTTOM);
				}
				ImGui::EndChild();
			}

		}
	}
	if (Vars.Weapon == 3)
	{
		ImGui::Columns(2, NULL, true);
		{
			ImGui::BeginChild("Shit7", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(u8"启用第六武器自瞄", &Vars.AimSlots6);
				ImGui::Combo(u8"自瞄第一按键", &Vars.D.Aimkey1, keyNames, ARRAYSIZE(keyNames));
				ImGui::Combo(u8"自瞄第二按键", &Vars.D.Aimkey2, keyNames, ARRAYSIZE(keyNames));
				ImGui::Checkbox(u8"优先考虑距离", &Vars.D.PrioritizeDistance);
				ImGui::Checkbox(u8"随即部位", &Vars.D.RandomBone);
				if (ImGui::Combo(u8"部位", &Vars.D.BoneSelected, Bones, 4))
				{
					if (Vars.D.BoneSelected == 0)
						Vars.D.Bone = eBones::HEAD;
					else if (Vars.D.BoneSelected == 1)
						Vars.D.Bone = eBones::NECK;
					else if (Vars.D.BoneSelected == 2)
						Vars.D.Bone = eBones::CHEST;
					else if (Vars.D.BoneSelected == 3)
						Vars.D.Bone = eBones::HIP;
				}
				ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.D.AimVehicles);
				if (Vars.D.AimVehicles) {
					ImGui::RadioButton(u8"顶部", &Vars.D.VehicleBone, TOP); ImGui::SameLine();
					ImGui::RadioButton(u8"偏上", &Vars.D.VehicleBone, UP); ImGui::SameLine();
					ImGui::RadioButton(u8"中间", &Vars.D.VehicleBone, CENTER); ImGui::SameLine();
					ImGui::RadioButton(u8"偏下", &Vars.D.VehicleBone, DOWN); ImGui::SameLine();
					ImGui::RadioButton(u8"底下", &Vars.D.VehicleBone, BOTTOM);
				}
				ImGui::EndChild();
			}



		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("Shit8", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(u8"预测", &Vars.D.AimPrediction);
				if (Vars.D.AimPrediction) {
					ImGui::SliderFloat(u8"预测的 XYZ", &Vars.D.AimPredictXYZ, 0.1f, 3.f, "%.1f");
					ImGui::SliderFloat(u8"Y轴预测", &Vars.D.AimOffsetY, 0.1f, 3.f, "%.1f");
				}
				ImGui::SliderFloat(u8"角度", &Vars.D.FOV, 1.f, 360.f, "%.0f");
				ImGui::SliderFloat(u8"最大瞄准距离", &Vars.D.Distance, 1.f, 750.f, "%.0f");
				ImGui::SliderFloat(u8"速度", &Vars.D.SmoothFactor, 0.001f, 1.f, "%.3f");
				ImGui::SliderInt(u8"重新定位时间", &Vars.D.RetargetTimer, 1, 1000);
				ImGui::Checkbox(u8"瞄准马", &Vars.D.AimHorses);
				if (Vars.D.AimHorses) {
					ImGui::RadioButton(u8"顶部", &Vars.D.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
					ImGui::RadioButton(u8"偏上", &Vars.D.HorseBone, UP); ImGui::SameLine();
					ImGui::RadioButton(u8"中间", &Vars.D.HorseBone, CENTER); ImGui::SameLine();
					ImGui::RadioButton(u8"偏下", &Vars.D.HorseBone, DOWN); ImGui::SameLine();
					ImGui::RadioButton(u8"底下", &Vars.D.HorseBone, BOTTOM);
				}
				ImGui::EndChild();
			}

		}
	}
	if (Vars.Weapon == 4)
	{
		ImGui::Columns(2, NULL, true);
		{
			ImGui::BeginChild("Shit9", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(u8"启用第七武器自瞄", &Vars.AimSlots7);
				ImGui::Combo(u8"自瞄第一按键", &Vars.E.Aimkey1, keyNames, ARRAYSIZE(keyNames));
				ImGui::Combo(u8"自瞄第二按键", &Vars.E.Aimkey2, keyNames, ARRAYSIZE(keyNames));
				ImGui::Checkbox(u8"优先考虑距离", &Vars.E.PrioritizeDistance);
				ImGui::Checkbox(u8"随即部位", &Vars.E.RandomBone);
				if (ImGui::Combo(u8"部位", &Vars.E.BoneSelected, Bones, 4))
				{
					if (Vars.E.BoneSelected == 0)
						Vars.E.Bone = eBones::HEAD;
					else if (Vars.E.BoneSelected == 1)
						Vars.E.Bone = eBones::NECK;
					else if (Vars.E.BoneSelected == 2)
						Vars.E.Bone = eBones::CHEST;
					else if (Vars.E.BoneSelected == 3)
						Vars.E.Bone = eBones::HIP;
				}
				ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.E.AimVehicles);
				if (Vars.E.AimVehicles) {
					ImGui::RadioButton(u8"顶部", &Vars.E.VehicleBone, TOP); ImGui::SameLine();
					ImGui::RadioButton(u8"偏上", &Vars.E.VehicleBone, UP); ImGui::SameLine();
					ImGui::RadioButton(u8"中间", &Vars.E.VehicleBone, CENTER); ImGui::SameLine();
					ImGui::RadioButton(u8"偏下", &Vars.E.VehicleBone, DOWN); ImGui::SameLine();
					ImGui::RadioButton(u8"底下", &Vars.E.VehicleBone, BOTTOM);
				}
				ImGui::EndChild();
			}


		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild("Shit10", ImVec2(0, 0), true);
			{
				ImGui::Checkbox(u8"预测", &Vars.E.AimPrediction);
				if (Vars.E.AimPrediction) {
					ImGui::SliderFloat(u8"预测的 XYZ", &Vars.E.AimPredictXYZ, 0.1f, 3.f, "%.1f");
					ImGui::SliderFloat(u8"Y轴预测", &Vars.E.AimOffsetY, 0.1f, 3.f, "%.1f");
				}
				ImGui::SliderFloat(u8"角度", &Vars.E.FOV, 1.f, 360.f, "%.0f");
				ImGui::SliderFloat(u8"最大瞄准距离", &Vars.E.Distance, 1.f, 750.f, "%.0f");
				ImGui::SliderFloat(u8"速度", &Vars.E.SmoothFactor, 0.001f, 1.f, "%.3f");
				ImGui::SliderInt(u8"重新定位时间", &Vars.E.RetargetTimer, 1, 1000);
				ImGui::Checkbox(u8"瞄准马", &Vars.E.AimHorses);
				if (Vars.E.AimHorses) {
					ImGui::RadioButton(u8"顶部", &Vars.E.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
					ImGui::RadioButton(u8"偏上", &Vars.E.HorseBone, UP); ImGui::SameLine();
					ImGui::RadioButton(u8"中间", &Vars.E.HorseBone, CENTER); ImGui::SameLine();
					ImGui::RadioButton(u8"偏下", &Vars.E.HorseBone, DOWN); ImGui::SameLine();
					ImGui::RadioButton(u8"底下", &Vars.E.HorseBone, BOTTOM);
				}
				ImGui::EndChild();
			}


		}
	}
	ImGui::Separator();
	/*
	ImGui::Columns(3, NULL, true);
	{
		ImGui::BeginChild("Legit1", ImVec2(0, 0), true);
		{
			ImGui::Text(u8"第一武器");
			ImGui::Checkbox(u8"激活自瞄", &Vars.A.Enable);
			ImGui::Combo(u8"自瞄第一按键", &Vars.A.Aimkey1, keyNames, ARRAYSIZE(keyNames));
			ImGui::Combo(u8"自瞄第二按键", &Vars.A.Aimkey2, keyNames, ARRAYSIZE(keyNames));
			ImGui::Checkbox(u8"预测", &Vars.A.AimPrediction);
			if (Vars.A.AimPrediction) {
				ImGui::SliderFloat(u8"预测的 XYZ", &Vars.A.AimPredictXYZ, 0.1f, 3.f, "%.1f");
				ImGui::SliderFloat(u8"Y轴预测", &Vars.A.AimOffsetY, 0.1f, 3.f, "%.1f");
			}
			ImGui::SliderFloat(u8"角度", &Vars.A.FOV, 1.f, 360.f, "%.0f");
			ImGui::SliderFloat(u8"最大瞄准距离", &Vars.A.Distance, 1.f, 750.f, "%.0f");
			ImGui::SliderFloat(u8"速度", &Vars.A.SmoothFactor, 0.001f, 1.f, "%.3f");
			ImGui::SliderInt(u8"重新定位时间", &Vars.A.RetargetTimer, 1, 1000);

			ImGui::Checkbox(u8"随即部位", &Vars.A.RandomBone);
			if (ImGui::Combo(u8"部位", &Vars.A.BoneSelected, Bones, 4))
			{
				if (Vars.A.BoneSelected == 0)
					Vars.A.Bone = eBones::HEAD;
				else if (Vars.A.BoneSelected == 1)
					Vars.A.Bone = eBones::NECK;
				else if (Vars.A.BoneSelected == 2)
					Vars.A.Bone = eBones::CHEST;
				else if (Vars.A.BoneSelected == 3)
					Vars.A.Bone = eBones::HIP;
			}
			ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.A.AimVehicles);
			if (Vars.A.AimVehicles) {
				ImGui::RadioButton(u8"顶部", &Vars.A.VehicleBone, TOP); ImGui::SameLine();
				ImGui::RadioButton(u8"偏上", &Vars.A.VehicleBone, UP); ImGui::SameLine();
				ImGui::RadioButton(u8"中间", &Vars.A.VehicleBone, CENTER); ImGui::SameLine();
				ImGui::RadioButton(u8"偏下", &Vars.A.VehicleBone, DOWN); ImGui::SameLine();
				ImGui::RadioButton(u8"底下", &Vars.A.VehicleBone, BOTTOM);
			}
			ImGui::Checkbox(u8"瞄准马", &Vars.A.AimHorses);
			if (Vars.A.AimHorses) {
				ImGui::RadioButton(u8"顶部", &Vars.A.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
				ImGui::RadioButton(u8"偏上", &Vars.A.HorseBone, UP); ImGui::SameLine();
				ImGui::RadioButton(u8"中间", &Vars.A.HorseBone, CENTER); ImGui::SameLine();
				ImGui::RadioButton(u8"偏下", &Vars.A.HorseBone, DOWN); ImGui::SameLine();
				ImGui::RadioButton(u8"底下", &Vars.A.HorseBone, BOTTOM);
			}

			ImGui::Separator();
			ImGui::Text(u8"第五武器");

			ImGui::Checkbox(u8"激活自瞄", &Vars.AimSlots5);
			ImGui::Combo(u8"自瞄第一按键", &Vars.C.Aimkey1, keyNames, ARRAYSIZE(keyNames));
			ImGui::Combo(u8"自瞄第二按键", &Vars.C.Aimkey2, keyNames, ARRAYSIZE(keyNames));
			ImGui::Checkbox(u8"预测", &Vars.C.AimPrediction);
			if (Vars.C.AimPrediction) {
				ImGui::SliderFloat(u8"预测的 XYZ", &Vars.C.AimPredictXYZ, 0.1f, 3.f, "%.1f");
				ImGui::SliderFloat(u8"Y轴预测", &Vars.C.AimOffsetY, 0.1f, 3.f, "%.1f");
			}
			ImGui::SliderFloat(u8"角度", &Vars.C.FOV, 1.f, 360.f, "%.0f");
			ImGui::SliderFloat(u8"最大瞄准距离", &Vars.C.Distance, 1.f, 750.f, "%.0f");
			ImGui::SliderFloat(u8"速度", &Vars.C.SmoothFactor, 0.001f, 1.f, "%.3f");
			ImGui::SliderInt(u8"重新定位时间", &Vars.C.RetargetTimer, 1, 1000);

			ImGui::Checkbox(u8"随即部位", &Vars.C.RandomBone);
			if (ImGui::Combo(u8"部位", &Vars.C.BoneSelected, Bones, 4))
			{
				if (Vars.C.BoneSelected == 0)
					Vars.C.Bone = eBones::HEAD;
				else if (Vars.C.BoneSelected == 1)
					Vars.C.Bone = eBones::NECK;
				else if (Vars.C.BoneSelected == 2)
					Vars.C.Bone = eBones::CHEST;
				else if (Vars.C.BoneSelected == 3)
					Vars.C.Bone = eBones::HIP;
			}
			ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.C.AimVehicles);
			if (Vars.C.AimVehicles) {
				ImGui::RadioButton(u8"顶部", &Vars.C.VehicleBone, TOP); ImGui::SameLine();
				ImGui::RadioButton(u8"偏上", &Vars.C.VehicleBone, UP); ImGui::SameLine();
				ImGui::RadioButton(u8"中间", &Vars.C.VehicleBone, CENTER); ImGui::SameLine();
				ImGui::RadioButton(u8"偏下", &Vars.C.VehicleBone, DOWN); ImGui::SameLine();
				ImGui::RadioButton(u8"底下", &Vars.C.VehicleBone, BOTTOM);
			}
			ImGui::Checkbox(u8"瞄准马", &Vars.C.AimHorses);
			if (Vars.C.AimHorses) {
				ImGui::RadioButton(u8"顶部", &Vars.C.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
				ImGui::RadioButton(u8"偏上", &Vars.C.HorseBone, UP); ImGui::SameLine();
				ImGui::RadioButton(u8"中间", &Vars.C.HorseBone, CENTER); ImGui::SameLine();
				ImGui::RadioButton(u8"偏下", &Vars.C.HorseBone, DOWN); ImGui::SameLine();
				ImGui::RadioButton(u8"底下", &Vars.C.HorseBone, BOTTOM);
			}
			ImGui::EndChild();
		}

	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild("Legit2", ImVec2(0, 0), true);
		{
			ImGui::Text(u8"第二武器");
			ImGui::Checkbox(u8"激活自瞄", &Vars.AimSlots2);
			ImGui::Combo(u8"自瞄第一按键", &Vars.B.Aimkey1, keyNames, ARRAYSIZE(keyNames));
			ImGui::Combo(u8"自瞄第二按键", &Vars.B.Aimkey2, keyNames, ARRAYSIZE(keyNames));
			ImGui::Checkbox(u8"预测", &Vars.B.AimPrediction);
			if (Vars.B.AimPrediction) {
				ImGui::SliderFloat(u8"预测的 XYZ", &Vars.B.AimPredictXYZ, 0.1f, 3.f, "%.1f");
				ImGui::SliderFloat(u8"Y轴预测", &Vars.B.AimOffsetY, 0.1f, 3.f, "%.1f");
			}
			ImGui::SliderFloat(u8"角度", &Vars.B.FOV, 1.f, 360.f, "%.0f");
			ImGui::SliderFloat(u8"最大瞄准距离", &Vars.B.Distance, 1.f, 750.f, "%.0f");
			ImGui::SliderFloat(u8"速度", &Vars.B.SmoothFactor, 0.001f, 1.f, "%.3f");
			ImGui::SliderInt(u8"重新定位时间", &Vars.B.RetargetTimer, 1, 1000);

			ImGui::Checkbox(u8"随即部位", &Vars.B.RandomBone);
			if (ImGui::Combo(u8"部位", &Vars.B.BoneSelected, Bones, 4))
			{
				if (Vars.B.BoneSelected == 0)
					Vars.B.Bone = eBones::HEAD;
				else if (Vars.B.BoneSelected == 1)
					Vars.B.Bone = eBones::NECK;
				else if (Vars.B.BoneSelected == 2)
					Vars.B.Bone = eBones::CHEST;
				else if (Vars.B.BoneSelected == 3)
					Vars.B.Bone = eBones::HIP;
			}
			ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.B.AimVehicles);
			if (Vars.B.AimVehicles) {
				ImGui::RadioButton(u8"顶部", &Vars.B.VehicleBone, TOP); ImGui::SameLine();
				ImGui::RadioButton(u8"偏上", &Vars.B.VehicleBone, UP); ImGui::SameLine();
				ImGui::RadioButton(u8"中间", &Vars.B.VehicleBone, CENTER); ImGui::SameLine();
				ImGui::RadioButton(u8"偏下", &Vars.B.VehicleBone, DOWN); ImGui::SameLine();
				ImGui::RadioButton(u8"底下", &Vars.B.VehicleBone, BOTTOM);
			}
			ImGui::Checkbox(u8"瞄准马", &Vars.B.AimHorses);
			if (Vars.B.AimHorses) {
				ImGui::RadioButton(u8"顶部", &Vars.B.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
				ImGui::RadioButton(u8"偏上", &Vars.B.HorseBone, UP); ImGui::SameLine();
				ImGui::RadioButton(u8"中间", &Vars.B.HorseBone, CENTER); ImGui::SameLine();
				ImGui::RadioButton(u8"偏下", &Vars.B.HorseBone, DOWN); ImGui::SameLine();
				ImGui::RadioButton(u8"底下", &Vars.B.HorseBone, BOTTOM);
			}
			ImGui::Separator();
			ImGui::Text(u8"第六武器");

			ImGui::Checkbox(u8"激活自瞄", &Vars.AimSlots6);
			ImGui::Combo(u8"自瞄第一按键", &Vars.D.Aimkey1, keyNames, ARRAYSIZE(keyNames));
			ImGui::Combo(u8"自瞄第二按键", &Vars.D.Aimkey2, keyNames, ARRAYSIZE(keyNames));
			ImGui::Checkbox(u8"预测", &Vars.D.AimPrediction);
			if (Vars.D.AimPrediction) {
				ImGui::SliderFloat(u8"预测的 XYZ", &Vars.D.AimPredictXYZ, 0.1f, 3.f, "%.1f");
				ImGui::SliderFloat(u8"Y轴预测", &Vars.D.AimOffsetY, 0.1f, 3.f, "%.1f");
			}
			ImGui::SliderFloat(u8"角度", &Vars.D.FOV, 1.f, 360.f, "%.0f");
			ImGui::SliderFloat(u8"最大瞄准距离", &Vars.D.Distance, 1.f, 750.f, "%.0f");
			ImGui::SliderFloat(u8"速度", &Vars.D.SmoothFactor, 0.001f, 1.f, "%.3f");
			ImGui::SliderInt(u8"重新定位时间", &Vars.D.RetargetTimer, 1, 1000);

			ImGui::Checkbox(u8"随即部位", &Vars.D.RandomBone);
			if (ImGui::Combo(u8"部位", &Vars.D.BoneSelected, Bones, 4))
			{
				if (Vars.D.BoneSelected == 0)
					Vars.D.Bone = eBones::HEAD;
				else if (Vars.D.BoneSelected == 1)
					Vars.D.Bone = eBones::NECK;
				else if (Vars.D.BoneSelected == 2)
					Vars.D.Bone = eBones::CHEST;
				else if (Vars.D.BoneSelected == 3)
					Vars.D.Bone = eBones::HIP;
			}
			ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.D.AimVehicles);
			if (Vars.D.AimVehicles) {
				ImGui::RadioButton(u8"顶部", &Vars.D.VehicleBone, TOP); ImGui::SameLine();
				ImGui::RadioButton(u8"偏上", &Vars.D.VehicleBone, UP); ImGui::SameLine();
				ImGui::RadioButton(u8"中间", &Vars.D.VehicleBone, CENTER); ImGui::SameLine();
				ImGui::RadioButton(u8"偏下", &Vars.D.VehicleBone, DOWN); ImGui::SameLine();
				ImGui::RadioButton(u8"底下", &Vars.D.VehicleBone, BOTTOM);
			}
			ImGui::Checkbox(u8"瞄准马", &Vars.D.AimHorses);
			if (Vars.D.AimHorses) {
				ImGui::RadioButton(u8"顶部", &Vars.D.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
				ImGui::RadioButton(u8"偏上", &Vars.D.HorseBone, UP); ImGui::SameLine();
				ImGui::RadioButton(u8"中间", &Vars.D.HorseBone, CENTER); ImGui::SameLine();
				ImGui::RadioButton(u8"偏下", &Vars.D.HorseBone, DOWN); ImGui::SameLine();
				ImGui::RadioButton(u8"底下", &Vars.D.HorseBone, BOTTOM);
			}
			ImGui::EndChild();
			
		}
		
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild("Legit3", ImVec2(0, 0), true);
		{
			ImGui::Text(u8"第七武器");

			ImGui::Checkbox(u8"激活自瞄", &Vars.AimSlots7);
			ImGui::Combo(u8"自瞄第一按键", &Vars.E.Aimkey1, keyNames, ARRAYSIZE(keyNames));
			ImGui::Combo(u8"自瞄第二按键", &Vars.E.Aimkey2, keyNames, ARRAYSIZE(keyNames));
			ImGui::Checkbox(u8"预测", &Vars.E.AimPrediction);
			if (Vars.E.AimPrediction) {
				ImGui::SliderFloat(u8"预测的 XYZ", &Vars.E.AimPredictXYZ, 0.1f, 3.f, "%.1f");
				ImGui::SliderFloat(u8"Y轴预测", &Vars.E.AimOffsetY, 0.1f, 3.f, "%.1f");
			}
			ImGui::SliderFloat(u8"角度", &Vars.E.FOV, 1.f, 360.f, "%.0f");
			ImGui::SliderFloat(u8"最大瞄准距离", &Vars.E.Distance, 1.f, 750.f, "%.0f");
			ImGui::SliderFloat(u8"速度", &Vars.E.SmoothFactor, 0.001f, 1.f, "%.3f");
			ImGui::SliderInt(u8"重新定位时间", &Vars.E.RetargetTimer, 1, 1000);

			ImGui::Checkbox(u8"随即部位", &Vars.E.RandomBone);
			if (ImGui::Combo(u8"部位", &Vars.E.BoneSelected, Bones, 4))
			{
				if (Vars.E.BoneSelected == 0)
					Vars.E.Bone = eBones::HEAD;
				else if (Vars.E.BoneSelected == 1)
					Vars.E.Bone = eBones::NECK;
				else if (Vars.E.BoneSelected == 2)
					Vars.E.Bone = eBones::CHEST;
				else if (Vars.E.BoneSelected == 3)
					Vars.E.Bone = eBones::HIP;
			}
			ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.E.AimVehicles);
			if (Vars.E.AimVehicles) {
				ImGui::RadioButton(u8"顶部", &Vars.E.VehicleBone, TOP); ImGui::SameLine();
				ImGui::RadioButton(u8"偏上", &Vars.E.VehicleBone, UP); ImGui::SameLine();
				ImGui::RadioButton(u8"中间", &Vars.E.VehicleBone, CENTER); ImGui::SameLine();
				ImGui::RadioButton(u8"偏下", &Vars.E.VehicleBone, DOWN); ImGui::SameLine();
				ImGui::RadioButton(u8"底下", &Vars.E.VehicleBone, BOTTOM);
			}
			ImGui::Checkbox(u8"瞄准马", &Vars.E.AimHorses);
			if (Vars.E.AimHorses) {
				ImGui::RadioButton(u8"顶部", &Vars.E.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
				ImGui::RadioButton(u8"偏上", &Vars.E.HorseBone, UP); ImGui::SameLine();
				ImGui::RadioButton(u8"中间", &Vars.E.HorseBone, CENTER); ImGui::SameLine();
				ImGui::RadioButton(u8"偏下", &Vars.E.HorseBone, DOWN); ImGui::SameLine();
				ImGui::RadioButton(u8"底下", &Vars.E.HorseBone, BOTTOM);
			}
			ImGui::EndChild();
		}

	}*/
ImGui::Columns(2, "Aimbot Shit", true);
{
	ImGui::Checkbox(u8"激活自瞄", &Features::Aimbot);
}

ImGui::NextColumn();
{
	ImGui::TextColored(ImColor(0, 255, 0), u8"目前的设置: %i", Vars.Bone);
}
/*
ImGui::NextColumn();
{
	ImGui::Combo(u8"武器配置", &Vars.Weapon, Weapon, ARRAYSIZE(Weapon));
}*/

ImGui::Columns(1);

ImGui::Separator();
ImGui::Columns(2, NULL, true);
{
	ImGui::Combo(u8"自瞄第一按键", &Vars.Aimkey1, keyNames, ARRAYSIZE(keyNames));

	ImGui::Checkbox(u8"拿起第二武器时自瞄", &Vars.AimSlots2);
	ImGui::Checkbox(u8"拿起第五武器时自瞄", &Vars.AimSlots5);
	ImGui::Checkbox(u8"拿起第六武器时自瞄", &Vars.AimSlots6);
	ImGui::Checkbox(u8"拿起第七武器时自瞄", &Vars.AimSlots7);
	ImGui::Checkbox(u8"优先考虑距离", &Vars.PrioritizeDistance);
	const char* Bones[] = { u8"头", u8"脖子", u8"胸", u8"髋关节" };
	static int BoneSelected = -1;

	ImGui::Checkbox(u8"随即部位", &Vars.RandomBone);
	if (ImGui::Combo(u8"部位", &BoneSelected, Bones, 4))
	{
		if (BoneSelected == 0)
			Vars.Bone = eBones::HEAD;
		else if (BoneSelected == 1)
			Vars.Bone = eBones::NECK;
		else if (BoneSelected == 2)
			Vars.Bone = eBones::CHEST;
		else if (BoneSelected == 3)
			Vars.Bone = eBones::HIP;
	}
	ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.AimVehicles);
	if (Vars.AimVehicles) {
		ImGui::RadioButton(u8"顶部", &Vars.VehicleBone, TOP); ImGui::SameLine();
		ImGui::RadioButton(u8"偏上", &Vars.VehicleBone, UP); ImGui::SameLine();
		ImGui::RadioButton(u8"中间", &Vars.VehicleBone, CENTER); ImGui::SameLine();
		ImGui::RadioButton(u8"偏下", &Vars.VehicleBone, DOWN); ImGui::SameLine();
		ImGui::RadioButton(u8"底下", &Vars.VehicleBone, BOTTOM);
	}

}
ImGui::NextColumn();
{
	ImGui::Combo(u8"自瞄第二按键", &Vars.Aimkey2, keyNames, ARRAYSIZE(keyNames));
	ImGui::Checkbox(u8"预测", &Vars.AimPrediction);
	if (Vars.AimPrediction) {
		ImGui::SliderFloat(u8"预测的 XYZ", &Vars.AimPredictXYZ, 0.1f, 3.f, "%.1f");
		ImGui::SliderFloat(u8"Y轴预测", &Vars.AimOffsetY, 0.1f, 3.f, "%.1f");
	}
	ImGui::SliderFloat(u8"角度", &Vars.FOV, 1.f, 360.f, "%.0f");
	ImGui::SliderFloat(u8"最大瞄准距离", &Vars.Distance, 1.f, 750.f, "%.0f");
	ImGui::SliderFloat(u8"速度", &Vars.SmoothFactor, 0.001f, 1.f, "%.3f");
	ImGui::SliderInt(u8"重新定位时间", &Vars.RetargetTimer, 1, 1000);
	ImGui::Checkbox(u8"瞄准马", &Vars.AimHorses);
	if (Vars.AimHorses) {
		ImGui::RadioButton(u8"顶部", &Vars.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
		ImGui::RadioButton(u8"偏上", &Vars.HorseBone, UP); ImGui::SameLine();
		ImGui::RadioButton(u8"中间", &Vars.HorseBone, CENTER); ImGui::SameLine();
		ImGui::RadioButton(u8"偏下", &Vars.HorseBone, DOWN); ImGui::SameLine();
		ImGui::RadioButton(u8"底下", &Vars.HorseBone, BOTTOM);
	}
}

	ImGui::Separator();
	/*
	ImGui::Columns(3, "Aimbot Shit", true);
	{
		ImGui::Checkbox(u8"激活自瞄", &Features::Aimbot);
	}

	ImGui::NextColumn();
	{
		ImGui::TextColored(ImColor(0, 255, 0),u8"目前的设置: %i", Vars.Bone);
	}
	ImGui::NextColumn();
	{
		ImGui::Combo(u8"武器配置", &Vars.Weapon, Weapon, ARRAYSIZE(Weapon));
	}

	ImGui::Columns(1);
	
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Combo(u8"自瞄第一按键", &Vars.Aimkey1, keyNames, ARRAYSIZE(keyNames));

		ImGui::Checkbox(u8"拿起第二武器时自瞄", &Vars.AimSlots2);
		ImGui::Checkbox(u8"拿起第五武器时自瞄", &Vars.AimSlots5);
		ImGui::Checkbox(u8"拿起第六武器时自瞄", &Vars.AimSlots6);
		ImGui::Checkbox(u8"拿起第七武器时自瞄", &Vars.AimSlots7);
		ImGui::Checkbox(u8"优先考虑距离", &Vars.PrioritizeDistance);
		const char* Bones[] = { u8"头", u8"脖子", u8"胸", u8"髋关节" };
		static int BoneSelected = -1;
		
		ImGui::Checkbox(u8"随即部位", &Vars.RandomBone);
		if (ImGui::Combo(u8"部位", &BoneSelected, Bones, 4))
		{
			if (BoneSelected == 0)
				Vars.Bone = eBones::HEAD;
			else if (BoneSelected == 1)
				Vars.Bone = eBones::NECK;
			else if (BoneSelected == 2)
				Vars.Bone = eBones::CHEST;
			else if (BoneSelected == 3)
				Vars.Bone = eBones::HIP;
		}
		ImGui::Checkbox(u8"瞄准坐骑(车/飞机等)", &Vars.AimVehicles);
		if (Vars.AimVehicles) {
			ImGui::RadioButton(u8"顶部", &Vars.VehicleBone, TOP); ImGui::SameLine();
			ImGui::RadioButton(u8"偏上", &Vars.VehicleBone, UP); ImGui::SameLine();
			ImGui::RadioButton(u8"中间", &Vars.VehicleBone, CENTER); ImGui::SameLine();
			ImGui::RadioButton(u8"偏下", &Vars.VehicleBone, DOWN); ImGui::SameLine();
			ImGui::RadioButton(u8"底下", &Vars.VehicleBone, BOTTOM);
		}

	}
	ImGui::NextColumn();
	{
		ImGui::Combo(u8"自瞄第二按键", &Vars.Aimkey2, keyNames, ARRAYSIZE(keyNames));
		ImGui::Checkbox(u8"预测", &Vars.AimPrediction);
		if (Vars.AimPrediction) {
			ImGui::SliderFloat(u8"预测的 XYZ", &Vars.AimPredictXYZ, 0.1f, 3.f, "%.1f");
			ImGui::SliderFloat(u8"Y轴预测", &Vars.AimOffsetY, 0.1f, 3.f, "%.1f");
		}
		ImGui::SliderFloat(u8"角度", &Vars.FOV, 1.f, 360.f, "%.0f");
		ImGui::SliderFloat(u8"最大瞄准距离", &Vars.Distance, 1.f, 750.f, "%.0f");
		ImGui::SliderFloat(u8"速度", &Vars.SmoothFactor, 0.001f, 1.f, "%.3f");
		ImGui::SliderInt(u8"重新定位时间", &Vars.RetargetTimer, 1, 1000);
		ImGui::Checkbox(u8"瞄准马", &Vars.AimHorses);
		if (Vars.AimHorses) {
			ImGui::RadioButton(u8"顶部", &Vars.HorseBone, TOP); ImGui::SameLine(); //must be different string name or will bug
			ImGui::RadioButton(u8"偏上", &Vars.HorseBone, UP); ImGui::SameLine();
			ImGui::RadioButton(u8"中间", &Vars.HorseBone, CENTER); ImGui::SameLine();
			ImGui::RadioButton(u8"偏下", &Vars.HorseBone, DOWN); ImGui::SameLine();
			ImGui::RadioButton(u8"底下", &Vars.HorseBone, BOTTOM);
		}
	}
	ImGui::Separator();
	*/
}
void DX11Renderer::VisualsTab()
{
	ImGui::Checkbox(u8"激活视觉功能", &Features::ESP);
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(u8"显示载具", &Features::ShowVehicles);
		ImGui::Checkbox(u8"显示队友", &Features::ESPShowFriends);
		ImGui::Checkbox(u8"显示骨架", &Features::ShowBones);
		ImGui::SliderFloat(u8"距离", &Features::ESPDistance, 5.f, 2500.f, "%.0f");

	}
	ImGui::NextColumn();
	{
		ImGui::Checkbox(u8"显示FOV", &Features::ShowFOV);
		ImGui::Checkbox(u8"显示名字", &Features::ShowName);
		ImGui::Checkbox(u8"显示距离", &Features::ShowDistance);
		ImGui::Checkbox(u8"显示生命值", &Features::ShowHealth);
		ImGui::Checkbox(u8"显示方框", &Features::ShowESPBoxes);

	}
	ImGui::Separator();
}
void DX11Renderer::MiscHackTab()
{
	ImGui::Text("Ez Game Ez Life");
	ImGui::Separator();
	ImGui::Columns(2, NULL, true);
	{
		ImGui::Checkbox(u8"准星", &Features::Crosshair);
		ImGui::RadioButton(u8"雷达关闭", &Features::Radar, 0); ImGui::SameLine();
		ImGui::RadioButton(u8"雷达打开[样式1]", &Features::Radar, 1); ImGui::SameLine();
		ImGui::RadioButton(u8"雷达打开[样式2]", &Features::Radar, 2); ImGui::SameLine();
		ImGui::RadioButton(u8"雷达窗口设置", &Features::Radar, 3);
		if (Features::Radar) {
			ImGui::SliderInt(u8"雷达大小", &Features::RadarSize, 50.f, ScreenSY);
			if (Features::Radar != 3) {
				ImGui::SliderInt(u8"雷达X轴位置", &Features::RadarPosX, 1.f, ScreenSX);
				ImGui::SliderInt(u8"雷达Y轴位置", &Features::RadarPosY, 1.f, ScreenSY);
			}
		}
		ImGui::Checkbox(u8"移除后坐力[危险]", &Features::NoSway);
		if (Features::NoSway) {
			ImGui::SliderInt(u8"随机后坐力", &Features::NoSwayRandomize, 1, 10);
		}
	}
	ImGui::NextColumn();
	{
		ImGui::Checkbox(u8"瞬间攻击模式[危险]", &Features::InstantHit);
		ImGui::Checkbox(u8"观察者预警", &Features::SpectWarn);
		ImGui::Checkbox(u8"显示医生位置(如果有需要)", &Features::MedicBugfix);
		ImGui::Checkbox(u8"[反FF]手动提交截图", &Features::giveAutoCleanSS);
		if (Features::giveAutoCleanSS)
			ImGui::SliderFloat(u8"提交截图时间", &Features::AutoCleanSSTimer, 0.1f, 30.f, "%.1f");
		else {
			ImGui::Checkbox(u8"[反FF]提供干净的截图", &Features::giveCleanSS);
			ImGui::Checkbox(u8"[反FF]提供干净的截图当自瞄时候", &Features::giveCleanSSwithRMB);
		}
		ImGui::Columns(4, "##config-settings", false);
		if (ImGui::Button(u8"保存配置", ImVec2(93.f, 20.f)))MarkSettingsDirty();
		ImGui::NextColumn();
		if (ImGui::Button(u8"读取配置", ImVec2(93.f, 20.f)))LoadSettings();
		ImGui::NextColumn();
		if (ImGui::Button(u8"测试截图", ImVec2(93.f, 20.f)))
		{
			DX11_mutex.lock();
			ID3D11Texture2D* ScreenShotProvided = nullptr;
			ID3D11Texture2D* ScreenShotBuffer = nullptr;
			D3D11_TEXTURE2D_DESC ScreenShot;
			HRESULT hr = DX11->DX11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&ScreenShotBuffer));
			memset(&ScreenShot, 0, sizeof(D3D11_TEXTURE2D_DESC));
			ScreenShotBuffer->GetDesc(&ScreenShot);
			DX11->DX11Device->CreateTexture2D(&ScreenShot, NULL, &ScreenShotProvided);
			DX11->DX11DeviceContext->CopyResource(ScreenShotProvided, ScreenShotBuffer);

			ScreenShotBuffer->Release();
			ScreenShotBuffer = nullptr;

			HDC dxDC = GetDC(DX11->WTarget);
			WriteBMP(extractBitmap(ScreenShotProvided), dxDC, L"..\\AA3Cheat\\hkCopyResource_provided.bmp");  //extractBitmap will call the CopyResource

			ScreenShotProvided->Release();
			ReleaseDC(DX11->WTarget, dxDC);
			DX11_mutex.unlock();
			Sleep(200);  //IMPORTANT! Sleep! Otherwise button will repeat 10 times
		}
		ImGui::NextColumn();
		if (ImGui::Button(u8"测试BitBit", ImVec2(93.f, 20.f))) {
			wchar_t wnd_title[256];
			HWND win = GetForegroundWindow(); //FindWindowEx(NULL,NULL,L"Battlefield?1",NULL); //GetDesktopWindow();
			GetWindowText(win, wnd_title, sizeof(wnd_title) / sizeof(wchar_t));
			if (wcscmp(wnd_title, L"Battlefield™ 1")) //do not make screenshot when game minimized!!!					
				return;
			HDC dc = GetDC(NULL);
			HDCToFile("..\\AA3Cheat\\hkBitBlt_provided.bmp", dc, { 0, 0, static_cast<int> (DX11Renderer::ScreenSX), static_cast<int> (DX11Renderer::ScreenSY) });
			ReleaseDC(win, dc);
			Sleep(200);   //IMPORTANT! Sleep! Otherwise button will repeat 10 times
		}
		ImGui::Columns(1);
		ImGui::Separator();
	}
	ImGui::Separator();

}
void DX11Renderer::DX11RenderScene()
{
	DX11Begin();

	if (!Closing)
	{
		if (SSCleaner->BitBltState && !giveCleanSS)  //if giveCleanSS On, then hide all from screen and prepare for clean screenshot and wait FF to call BitBlt
		{		
			if (Features::giveCleanSS)
				Features::giveAutoCleanSS = false;  //when giveCleanSS On firmly disable the other feature giveAutoCleanSS. Dont want both features to interfere

			if (Features::giveAutoCleanSS) {
				Features::giveCleanSS = false;  //when giveAutoCleanSS On firmly disable the other feature giveCleanSS. Dont want both features to interfere
				SSCleaner->TakeSS();
			}
			else   //if Features::giveAutoCleanSS off => delete stored SS texture; BitBlt will be blocked
				SSCleaner->NeedSS = false;			

			if (!SSCleaner->NeedSS) 
			{
				if (Features::ShowMenu)
				{
					static int page = 0;
					ImGui::SetNextWindowSize(ImVec2(960, 645), ImGuiSetCond_FirstUseEver);

					if (ImGui::Begin(u8"RooKie's 的 AA3Cheats 框架 2017年度版 For Battlefield 1", &Features::ShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders))
					{
						const char* tabs[] = {
							u8"自瞄设置",
							u8"视觉设置",
							u8"其他设置"
						};
						int tabs_size = sizeof(tabs) / sizeof(tabs[0]);

						for (int i = 0; i < tabs_size; i++)
						{
							ImVec2 windowSize = ImGui::GetWindowSize();
							int width = windowSize.x / tabs_size - 9;

							int distance;
							if (i == page)
								distance = 0;
							else
								distance = i > page ? i - page : page - i;

							ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(
								mainColor.Value.x - (distance * 0.035f),
								mainColor.Value.y - (distance * 0.035f),
								mainColor.Value.z - (distance * 0.035f),
								mainColor.Value.w
							);
							ImGui::GetStyle().Colors[ImGuiCol_Button] = mainColor;
							


							if (ImGui::Button(tabs[i], ImVec2(width, 0)))
								page = i;



							if (i < tabs_size - 1)
								ImGui::SameLine();
						}

						switch (page)
						{
						case 0:
							AimbotTab();
							break;
						case 1:
							VisualsTab();
							break;
						case 2:
							MiscHackTab();
							break;
						}
						ImGui::End();
					}
				}												
					if (Features::Crosshair) {					
						DrawLine(ScreenSX / 2.f - ScreenSX*0.005, ScreenSY / 2.f, ScreenSX / 2.f + ScreenSX*0.005, ScreenSY / 2.f, Color(0, 255, 9, 200));
						DrawLine(ScreenSX / 2.f, ScreenSY / 2.f - ScreenSX*0.005, ScreenSX / 2.f, ScreenSY / 2.f + ScreenSX*0.005, Color(0, 255, 9, 200));
					}
					if (!Mem::IsValid(Game::GetLocalPlayer()->GetCurrentVehicle())) { //sorry no radar when in a vehicle. Radar cant get coordinates of local soldier LPSoldier; GetPosition() or GetBonePosition() return incorect value
						/////////////////////////////////////////////////RADAR https://www.unknowncheats.me/forum/battlefield-1-a/210381-bf1-internal-hack-source-2.html
						if (Features::Radar == 1)  //draw radar box
						{
							rad_orginX = ((float)Features::RadarPosX) - Features::RadarSize / 2;
							rad_orginY = ((float)Features::RadarPosY) + Features::RadarSize / 2;
							FillRGB(rad_orginX - Features::RadarSize / 2, rad_orginY - Features::RadarSize, Features::RadarSize/**2*/, Features::RadarSize, Color(0, 0, 200, 50));
							DrawLine(rad_orginX - Features::RadarSize / 2, rad_orginY, rad_orginX + Features::RadarSize / 2, rad_orginY, Color(0, 0, 200, 180));   //horizontal bottom line
							DrawLine(rad_orginX, rad_orginY, rad_orginX, rad_orginY - Features::RadarSize, Color(0, 0, 200, 50));    //vertival central line					
						}
						if (Features::Radar == 2)  //draw radar box
						{
							rad_orginX = ((float)Features::RadarPosX) - Features::RadarSize;
							rad_orginY = ((float)Features::RadarPosY) + Features::RadarSize;
							FillRGB(rad_orginX - Features::RadarSize, rad_orginY - Features::RadarSize, Features::RadarSize * 2, Features::RadarSize, Color(0, 0, 200, 50));
							DrawLine(rad_orginX - Features::RadarSize, rad_orginY, rad_orginX + Features::RadarSize, rad_orginY, Color(0, 0, 200, 180));   //horizontal bottom line
							DrawLine(rad_orginX, rad_orginY, rad_orginX, rad_orginY - Features::RadarSize, Color(0, 0, 200, 50));  //vertival central line	
						}
						////////////////////End Radar
					}
					if (Features::ESP)
					{
						CEntity* LocalPlayer = Game::GetLocalPlayer();
						if (Mem::IsValid(LocalPlayer) && Mem::IsValid(LocalPlayer->GetSoldier()))
						{
							CSoldier* LPSoldier = LocalPlayer->GetSoldier();
							if ((Mem::IsValid(LPSoldier) && Mem::IsValid(LPSoldier->prediction)) || (Mem::IsValid(LocalPlayer->GetCurrentVehicle())))
							{
								if (Features::ShowFOV)
									DrawCircle(ScreenSX / 2.f, ScreenSY / 2.f, Color(0, 255, 255, 255), Vars.FOV, 25);

								//PLAYERS
								CEntityList* EntityList = Game::GetEntityList();
								ClientPlayerManager* PlayerManager = Game::GetClientPlayerManager();
								/////////////Spectators warning//////////////
								if (Features::SpectWarn && Mem::IsValid(PlayerManager))
								{
									CSpectatorList* SpectatorList = PlayerManager->GetSpectators();
									if (Mem::IsValid(SpectatorList))
									{
										int ValidSpecs = 0;
										bool FirstText = true;
										for (int i = 0; i < 16; i++)
										{
											CSpectator* Spec = SpectatorList->GetSpectator(i);
											if (Mem::IsValid(Spec) && LocalPlayer->GetPlayerView() == Spec->GetView())
											{
												if (FirstText)
												{
													DrawDXText(ScreenSX / 2.f, ScreenSY * 0.91f/*- 200.f*/, 1.f, true, 0.f, Color(255, 0, 70, 200), true, "*Spectator Warning*");
													FirstText = false;
												}
												//DrawDXText(ScreenSX / 2.f, (ScreenSY - 200.f) + 25.f + (ValidSpecs * 20.f), 1.f, true, 0.f, Color(255, 255, 0, 255), true, "-%i. %s %I64X", ValidSpecs + 1, Spec->Name, Spec);
												DrawDXText(ScreenSX / 2.f, (ScreenSY * 0.91f) + 25.f + (ValidSpecs * 20.f), 1.f, true, 0.f, Color(255, 0, 70, 200), true, "%s", Spec->Name);
												ValidSpecs++;
											}
										}
									}
								}
								/*//TEST AIM IN POINT CODE
								Vector3 testPos = Vector3(3, 3, 3);  
								Vector3 testSP;
								Vector3 testShootSpace;
								LPSoldier->GetBonePosition(testShootSpace, 98);
								Vector3 ShootSpaceSP;
								if (Game::W2S(testPos, testSP))
								{
									DrawCircle(testSP.x, testSP.y, Color(255, 0, 0, 255), 2, 25);
									DrawLine(testSP.x, testSP.y, ScreenSX / 2.f, ScreenSY / 2.f + ScreenSX*0.2, Color(255, 0, 0, 255));
								}*/
								/////////////////////////////////
								if (Mem::IsValid(EntityList))
								{
									/*std::ofstream outputFile;		//log						
									 outputFile.open(L"C:\\battlefield1_ESP.log");
									 outputFile << "start" << std::endl;*/
									for (int i = 0; i < 64; i++)
									{
										//outputFile << i << " ===== " << std::endl;  //log

										CEntity* Ent = EntityList->GetEntity(i);
										if (Mem::IsValid(Ent))
										{
											CSoldier* EntSoldier = Ent->GetSoldier();
											/*outputFile << "CEntity->getSoldier() -- ok" << std::endl;  //log
											outputFile << "Is CSoldier valid: " << Mem::IsValid(EntSoldier) << std::endl;
											if (Mem::IsValid(EntSoldier)) {
												outputFile << "Soldier Name: " << Ent->Name << std::endl;
												outputFile << "Is HealthComponent valid: " << Mem::IsValid(EntSoldier->HealthComponent) << std::endl;
												if (Mem::IsValid(EntSoldier->HealthComponent)) 
													outputFile << "HealthComponent HP: " <<  EntSoldier->HealthComponent->HP << std::endl;											
											}*/

											if (Mem::IsValid(EntSoldier) && Mem::IsValid(EntSoldier->HealthComponent) && EntSoldier->HealthComponent->HP >= 0.1f && EntSoldier->HealthComponent->HP <= 100.f)
											{
												Vector3 HeadPos;

												if (Features::MedicBugfix && Ent->GetTeam() == LocalPlayer->GetTeam()) { //if MedicBugfix do not show own team mates because GetBonePosition makes then unable to revive
													Features::ESPShowFriends = false;
													continue;
												}

												if (!EntSoldier->GetBonePosition(HeadPos, 53))
													continue;
												HeadPos = EntSoldier->GetPosition() + Vector3(0.f, 2.f, 0.f);
												//outputFile << "HEAD: " << HeadPos.x << " " << HeadPos.y << " " << HeadPos.z << std::endl;  //log

												Vector3 FeetPos = EntSoldier->GetPosition();
												Vector3 HeadSP;
												Vector3 FeetSP;
												float dist = 0.f;

												if (Mem::IsValid(LPSoldier) && Mem::IsValid(LPSoldier->prediction))  //local player on feet; GetPosition() works OK
													dist = Vector3::Distance(FeetPos, LPSoldier->GetPosition());  //distance between other soldier and local player
												else if (Mem::IsValid(LocalPlayer->GetCurrentVehicle())) {  //local player in vehicle; GetPosition() crash! => Take position from HEAD, not from GetPosition() prediction
													static Vector3 MyHeadPos;
													LPSoldier->GetBonePositionLP(MyHeadPos, HEAD);
													dist = Vector3::Distance(FeetPos, MyHeadPos); //Log->AddLog("pos x=%2.2f y=%2.2f z=%2.2f | %2.2f %2.2f %2.2f | %.2f %.2f %.2f\n", MyHeadPos.x, MyHeadPos.y, MyHeadPos.z,((ClientVehicleEntity*)LocalPlayer->GetCurrentVehicle())->m_Velocity.x, ((ClientVehicleEntity*)LocalPlayer->GetCurrentVehicle())->m_Velocity.y, ((ClientVehicleEntity*)LocalPlayer->GetCurrentVehicle())->m_Velocity.z, ((PhysicsBody*)LocalPlayer->GetCurrentVehicle())->m_RigidBodyData->m_CenterOfMass.x, ((PhysicsBody*)LocalPlayer->GetCurrentVehicle())->m_RigidBodyData->m_CenterOfMass.y, ((PhysicsBody*)LocalPlayer->GetCurrentVehicle())->m_RigidBodyData->m_CenterOfMass.z);
												}
												if(!Mem::IsValid(LocalPlayer->GetCurrentVehicle())) { //sorry no radar when in a vehicle. Radar cant get coordinates of local soldier LPSoldier; GetPosition() or GetBonePosition() return incorect value
													/////////////////RADAR https://www.unknowncheats.me/forum/battlefield-1-a/210381-bf1-internal-hack-source-2.html
													if (Features::Radar == 1 && dist > .01f && dist < 300 && Ent->GetTeam() != LocalPlayer->GetTeam()) //add player if not in same team
													{
														float zs = LPSoldier->GetPosition().z - FeetPos.z;
														float xs = LPSoldier->GetPosition().x - FeetPos.x;
														Vector3 Yaw = -LPSoldier->GetAngles(); // okay
														float xpos = xs * (float)cos(Yaw.x) - zs * (float)sin(Yaw.x);
														float ypos = xs * (float)sin(Yaw.x) + zs * (float)cos(Yaw.x);

														xpos = xpos * 2 + (rad_orginX);
														ypos = ypos * 2 + (rad_orginY);
														if (xpos < (float)(rad_orginX - Features::RadarSize / 2)) xpos = (float)(rad_orginX - Features::RadarSize / 2);
														if (ypos < (float)(rad_orginY - Features::RadarSize)) ypos = (float)(rad_orginY - Features::RadarSize);
														if (xpos > (float)((rad_orginX)+Features::RadarSize / 2 - Features::RadarSize / 40)) xpos = (float)((rad_orginX)+Features::RadarSize / 2 - Features::RadarSize / 40);
														if (ypos > (float)((rad_orginY)+Features::RadarSize - Features::RadarSize / 40)) ypos = (float)((rad_orginY)+Features::RadarSize - Features::RadarSize / 40);
														//radar_size/40 is the size of the dot on the radar; radar_size/30 = bigger dots
														FillRGB(xpos, ypos, Features::RadarSize / 40, Features::RadarSize / 40, Color(255, 0, 0, 200));
													}
													if (Features::Radar == 2 && dist > .01f && dist < 300 && Ent->GetTeam() != LocalPlayer->GetTeam()) //add player if not in same team
													{
														float zs = LPSoldier->GetPosition().z - FeetPos.z;
														float xs = LPSoldier->GetPosition().x - FeetPos.x;
														Vector3 Yaw = -LPSoldier->GetAngles(); // okay
														float xpos = xs * (float)cos(Yaw.x) - zs * (float)sin(Yaw.x);
														float ypos = xs * (float)sin(Yaw.x) + zs * (float)cos(Yaw.x);
														xpos = xpos * 2 + (rad_orginX);
														ypos = ypos * 2 + (rad_orginY);

														if (xpos < (float)(rad_orginX - Features::RadarSize)) xpos = (float)(rad_orginX - Features::RadarSize);
														if (ypos < (float)(rad_orginY - Features::RadarSize)) ypos = (float)(rad_orginY - Features::RadarSize);
														if (xpos > (float)((rad_orginX)+Features::RadarSize - Features::RadarSize / 40)) xpos = (float)((rad_orginX)+Features::RadarSize - Features::RadarSize / 40);
														if (ypos > (float)((rad_orginY)+Features::RadarSize - Features::RadarSize / 40)) ypos = (float)((rad_orginY)+Features::RadarSize - Features::RadarSize / 40);
														//radar_size/40 is the size of the dot on the radar; radar_size/30 = bigger dots
														FillRGB(xpos, ypos, Features::RadarSize / 40, Features::RadarSize / 40, Color(255, 0, 0, 200));
													}
													////////////////////End Radar
													////////////////////radar window
													if (Features::Radar == 3) {
														ImVec2 position = ImVec2(ScreenSX-Features::RadarSize-20, 0);

														ImVec2 radarSize = ImVec2(Features::RadarSize, Features::RadarSize);
														ImColor radarLinesColor = ImColor(100, 100, 100, 255);

														CEntity* LocalPlayer = Game::GetLocalPlayer();
														CEntityList* EntityList = Game::GetEntityList();

														ImGuiWindowFlags wFlags = /*ImGuiWindowFlags_NoResize |*/ ImGuiWindowFlags_NoCollapse;
														if (!Features::ShowMenu)
														{
															wFlags |= ImGuiWindowFlags_NoInputs;
															wFlags |= ImGuiWindowFlags_NoMove;
															wFlags |= ImGuiWindowFlags_NoTitleBar;
														}

														ImGui::SetNextWindowPos(position, ImGuiSetCond_FirstUseEver);														
														if (ImGui::Begin("Radar", (bool*)Features::Radar, radarSize, 0.4f, wFlags))
														{															
															ImVec2 pos1 = ImGui::GetWindowPos();
															ImVec2 center = ImVec2(pos1.x + radarSize.x / 2, pos1.y + radarSize.y / 2);
															ImGui::GetWindowDrawList()->AddLine(ImVec2(pos1.x, pos1.y), center, radarLinesColor);
															ImGui::GetWindowDrawList()->AddLine(ImVec2(pos1.x + radarSize.x, pos1.y), center, radarLinesColor);
															ImGui::GetWindowDrawList()->AddLine(ImVec2(pos1.x, pos1.y + radarSize.y / 2), ImVec2(pos1.x + radarSize.x, pos1.y + radarSize.y / 2), radarLinesColor);
															ImGui::GetWindowDrawList()->AddLine(center, ImVec2(pos1.x + radarSize.x / 2, pos1.y + radarSize.y), radarLinesColor);

															float b = 15.f;
															float d = (b / (2.f * sin(70.f / 2.f))) * cos(70.f / 2.f);
															ImGui::GetWindowDrawList()->AddTriangleFilled(center - ImVec2(b / 2, -d / 2), center - ImVec2(0, d / 2), center + ImVec2(b / 2, d / 2), ImColor(0, 255, 0));

															if (Mem::IsValid(EntityList) && Mem::IsValid(LocalPlayer))
															{																
																CSoldier* LPSoldier = LocalPlayer->GetSoldier();
																if (Mem::IsValid(LocalPlayer))
																{
																	Vector3 Yaw = LPSoldier->GetAngles();
																	Vector3 LPos = LPSoldier->GetShootSpace() - LPSoldier->GetSpawnOffset(); //=LPSoldier->getPosition();
																	for (int i = 0; i < 64; i++)
																	{																		
																		CEntity* Ent = EntityList->GetEntity(i); //ClientEntity* Ent = EntityList->getEntity(i);
																		
																		if (Mem::IsValid(Ent))
																		{
																			if (Ent == LocalPlayer)
																				continue;

																			CSoldier* EntSoldier = Ent->GetSoldier(); //ClientSoldierEntity* EntSoldier = Ent->getSoldier();
																			if (Mem::IsValid(EntSoldier))
																			{
																				//if (!(EntSoldier->HealthComponent->HP >= 0.1f && EntSoldier->HealthComponent->maxHealth >= 100.f)) continue;
																				Vector3 CPos; // = EntSoldier->getPosition();
																				EntSoldier->GetBonePosition(CPos, Vars.Bone); 

																				float dist = Vector3::Distance(CPos, LPos);
																				if (dist >= 2.f && dist <= Features::ESPDistance && EntSoldier->HealthComponent->HP >= 0.1f && EntSoldier->HealthComponent->HP <= 100.f)
																				{
																					ImColor pColor = ImColor(255, 0, 0);
																					if (Ent->GetTeam() == LocalPlayer->GetTeam()) {
																						if (!Features::ESPShowFriends) continue;
																						ImColor pColor = ImColor(0, 255, 0);
																					}

																					float r_1 = LPos.z - CPos.z;
																					float r_2 = LPos.x - CPos.x;

																					float x_1 = r_2 * (float)cos((long double)-Yaw.x) - r_1 * sin((long double)-Yaw.x);
																					float y_1 = r_2 * (float)sin((long double)-Yaw.x) + r_1 * cos((long double)-Yaw.x);

																					ImGui::GetWindowDrawList()->AddCircleFilled(center + ImVec2(x_1, y_1), 2, ImColor(255, 0, 0));
																				}
																			}

																		}
																	}
																}
															}
															ImGui::End();
														}
													}
													//////////////////////////end radar window
												}
												if (Game::W2S(HeadPos, HeadSP) && Game::W2S(FeetPos, FeetSP) && (Mem::IsValid(LocalPlayer->GetCurrentVehicle()) ? true : (dist >= .01f && dist < Features::ESPDistance))/*dist > 0.01f && dist < Features::ESPDistance*/) //if in vehicle show all, no matter distance (bugfix)
												{
													if (Ent->GetTeam() == LocalPlayer->GetTeam() && !Features::ESPShowFriends)
														continue;

													Color BoxColor = Color(0, 235, 235, 255);

													bool Enemy = false, Visible = false;
													if (Ent->GetTeam() != LocalPlayer->GetTeam())
													{
														Enemy = true;

														if(!EntSoldier->m_Occluded) // czy go widac?
														{
															Visible = true;
															BoxColor = Color(0, 255, 0, 255);
														}
														else
															BoxColor = Color(255, 0, 0, 255);
													}

													float BoxSY = FeetSP.y - HeadSP.y;
													float BoxSX = BoxSY / 2.f;
													float BoxX = HeadSP.x - BoxSX / 2.f;
													float BoxY = HeadSP.y;
													float HP = EntSoldier->HealthComponent->HP;

													if (Features::ShowESPBoxes)
													{
														DrawEmptyBox(BoxX - 1, BoxY - 1, BoxSX, BoxSY + 2, 3, Color(0, 0, 0, 175));
														DrawEmptyBox(BoxX, HeadSP.y, BoxSX, BoxSY, 1, BoxColor);
													}

													if (!Mem::IsValid(LocalPlayer->GetCurrentVehicle())) {  //sorry, no ESP distance in vehicle. bugfix
														std::string Distance;
														if (Features::ShowName) Distance = "\n[" + std::to_string(static_cast<int>(dist)) + " m]";
														else  Distance = "[" + std::to_string(static_cast<int>(dist)) + " m]";
														DrawDXText(BoxX - 10.f, Features::ShowHealth ? BoxY + BoxSY + 4.f : BoxY + BoxSY, 0.6f, false, 0.f, Color(255, 255, 255, 255), true, "%s%s", Features::ShowName ? Ent->Name : "", Features::ShowDistance ? Distance.c_str() : "");
													}
													else
														DrawDXText(BoxX - 10.f, Features::ShowHealth ? BoxY + BoxSY + 4.f : BoxY + BoxSY, 0.6f, false, 0.f, Color(255, 255, 255, 255), true, "%s", Features::ShowName ? Ent->Name : "");

													if (Features::ShowHealth)
													{
														DrawBox(BoxX - 1, FeetSP.y + 2, (HP * BoxSX) / 100 + 3, 5, Color(0, 0, 0, 255));
														DrawBox(BoxX, FeetSP.y + 3, (HP * BoxSX) / 100 + 1, 3, Color(((100 - HP) * 255) / 100, (HP * 255) / 100, 0, 255));
													}

													/*for (int i = 0; i < 200; i++)
													{
														Vector3 fucking, f2;
														EntSoldier->GetBonePosition(fucking, i);
														Game::W2S(fucking, f2);
														DrawDXText(f2.x, f2.y, 0.5f, true, 0.f, Color(255, 255, 255, 255), true, "%i", i);
													}*/ //BONES TEST

													if (Features::ShowBones)
													{
														Vector3 NeckSP, ChestSP, StomachSP, HipSP, LShoulderSP, LBicepsSP, LElbowSP, LUlnaSP, LHandSP, LFemurSP, LKneeSP, LShinboneSP, LMalleolusSP, LTipToeSP,
															RShoulderSP, RBicepsSP, RElbowSP, RUlnaSP, RHandSP, RFemurSP, RKneeSP, RShinboneSP, RMalleolusSP, RTipToeSP, NeckPos, ChestPos, StomachPos, HipPos,
															LShoulderPos, LBicepsPos, LElbowPos, LUlnaPos, LHandPos, LFemurPos, LKneePos, LShinbonePos, LMalleolusPos, LTipToePos, RShoulderPos, RBicepsPos,
															RElbowPos, RUlnaPos, RHandPos, RFemurPos, RKneePos, RShinbonePos, RMalleolusPos, RTipToePos;

														EntSoldier->GetBonePosition(HeadPos, HEAD/*53*/);
														EntSoldier->GetBonePosition(NeckPos, NECK);
														/*Vector3 fucking, f2;
														EntSoldier->GetBonePosition(fucking, HEAD);
														Game::W2S(fucking, f2);
														DrawCircle(f2.x, f2.y, Color(255, 0, 0, 255), 4.f, 10);*/
														EntSoldier->GetBonePosition(ChestPos, CHEST);
														EntSoldier->GetBonePosition(StomachPos, STOMACH);
														EntSoldier->GetBonePosition(HipPos, HIP);

														EntSoldier->GetBonePosition(LShoulderPos, LEFT_SHOULDER);
														EntSoldier->GetBonePosition(LBicepsPos, LEFT_BICEPS);
														EntSoldier->GetBonePosition(LElbowPos, LEFT_ELBOW);
														EntSoldier->GetBonePosition(LUlnaPos, LEFT_ULNA);
														EntSoldier->GetBonePosition(LHandPos, LEFT_HAND);
														EntSoldier->GetBonePosition(LFemurPos, LEFT_FEMUR);
														EntSoldier->GetBonePosition(LKneePos, LEFT_KNEE);
														EntSoldier->GetBonePosition(LShinbonePos, LEFT_SHINBONE);
														EntSoldier->GetBonePosition(LMalleolusPos, LEFT_MALLEOLUS);
														EntSoldier->GetBonePosition(LTipToePos, LEFT_TIPTOE);

														EntSoldier->GetBonePosition(RShoulderPos, RIGHT_SHOULDER);
														EntSoldier->GetBonePosition(RBicepsPos, RIGHT_BICEPS);
														EntSoldier->GetBonePosition(RElbowPos, RIGHT_ELBOW);
														EntSoldier->GetBonePosition(RUlnaPos, RIGHT_ULNA);
														EntSoldier->GetBonePosition(RHandPos, RIGHT_HAND);
														EntSoldier->GetBonePosition(RFemurPos, RIGHT_FEMUR);
														EntSoldier->GetBonePosition(RKneePos, RIGHT_KNEE);
														EntSoldier->GetBonePosition(RShinbonePos, RIGHT_SHINBONE);
														EntSoldier->GetBonePosition(RMalleolusPos, RIGHT_MALLEOLUS);
														EntSoldier->GetBonePosition(RTipToePos, RIGHT_TIPTOE);

														if (Game::W2S(HeadPos, HeadSP) &&
															Game::W2S(NeckPos, NeckSP) &&
															//Game::W2S(ChestPos, ChestSP) &&
															//Game::W2S(StomachPos, StomachSP) &&
															Game::W2S(HipPos, HipSP) &&
															Game::W2S(LShoulderPos, LShoulderSP) &&
															Game::W2S(LBicepsPos, LBicepsSP) &&
															Game::W2S(LElbowPos, LElbowSP) &&
															Game::W2S(LUlnaPos, LUlnaSP) &&
															Game::W2S(LHandPos, LHandSP) &&
															Game::W2S(LFemurPos, LFemurSP) &&
															Game::W2S(LKneePos, LKneeSP) &&
															Game::W2S(LShinbonePos, LShinboneSP) &&
															Game::W2S(LMalleolusPos, LMalleolusSP) &&
															Game::W2S(LTipToePos, LTipToeSP) &&
															Game::W2S(RShoulderPos, RShoulderSP) &&
															Game::W2S(RBicepsPos, RBicepsSP) &&
															Game::W2S(RElbowPos, RElbowSP) &&
															Game::W2S(RUlnaPos, RUlnaSP) &&
															Game::W2S(RHandPos, RHandSP) &&
															Game::W2S(RFemurPos, RFemurSP) &&
															Game::W2S(RKneePos, RKneeSP) &&
															Game::W2S(RShinbonePos, RShinboneSP) &&
															Game::W2S(RMalleolusPos, RMalleolusSP) &&
															Game::W2S(RTipToePos, RTipToeSP))
														{
															Color BonesColor = Color(0, 255, 255, 255);
															if (Enemy)
															{
																if (Visible)
																	BonesColor = Color(0, 255, 0, 255);
																else
																	BonesColor = Color(255, 0, 0, 255);
															}											
														
															float HeadSize = sqrt(pow(HeadSP.x- NeckSP.x,2) + pow(HeadSP.y - NeckSP.y, 2));
															DrawCircle(HeadSP.x, HeadSP.y, BonesColor, HeadSize/2.7, 10);														
															//DrawCircle(HeadSP.x, HeadSP.y, BonesColor, 70.f / dist, 10);

															DrawLine(HeadSP.x, HeadSP.y, NeckSP.x, NeckSP.y, BonesColor);
															DrawLine(NeckSP.x, NeckSP.y, HipSP.x, HipSP.y, BonesColor);

															DrawLine(NeckSP.x, NeckSP.y, LShoulderSP.x, LShoulderSP.y, BonesColor);
															DrawLine(LShoulderSP.x, LShoulderSP.y, LBicepsSP.x, LBicepsSP.y, BonesColor);
															DrawLine(LBicepsSP.x, LBicepsSP.y, LElbowSP.x, LElbowSP.y, BonesColor);
															DrawLine(LElbowSP.x, LElbowSP.y, LHandSP.x, LHandSP.y, BonesColor);

															DrawLine(NeckSP.x, NeckSP.y, RShoulderSP.x, RShoulderSP.y, BonesColor);
															DrawLine(RShoulderSP.x, RShoulderSP.y, RBicepsSP.x, RBicepsSP.y, BonesColor);
															DrawLine(RBicepsSP.x, RBicepsSP.y, RElbowSP.x, RElbowSP.y, BonesColor);
															DrawLine(RElbowSP.x, RElbowSP.y, RHandSP.x, RHandSP.y, BonesColor);

															DrawLine(HipSP.x, HipSP.y, LFemurSP.x, LFemurSP.y, BonesColor);
															DrawLine(LFemurSP.x, LFemurSP.y, LKneeSP.x, LKneeSP.y, BonesColor);
															DrawLine(LKneeSP.x, LKneeSP.y, LShinboneSP.x, LShinboneSP.y, BonesColor);
															DrawLine(LShinboneSP.x, LShinboneSP.y, LMalleolusSP.x, LMalleolusSP.y, BonesColor);
															DrawLine(LMalleolusSP.x, LMalleolusSP.y, LTipToeSP.x, LTipToeSP.y, BonesColor);

															DrawLine(HipSP.x, HipSP.y, RFemurSP.x, RFemurSP.y, BonesColor);
															DrawLine(RFemurSP.x, RFemurSP.y, RKneeSP.x, RKneeSP.y, BonesColor);
															DrawLine(RKneeSP.x, RKneeSP.y, RShinboneSP.x, RShinboneSP.y, BonesColor);
															DrawLine(RShinboneSP.x, RShinboneSP.y, RMalleolusSP.x, RMalleolusSP.y, BonesColor);
															DrawLine(RMalleolusSP.x, RMalleolusSP.y, RTipToeSP.x, RTipToeSP.y, BonesColor);
														}
													}
												}
											}
										}
										if (Features::ShowVehicles && Mem::IsValid(Ent->GetCurrentVehicle()))
										{
											TransformAABBStruct _AABB;
											Vector3 minSP, maxSP, cor2SP, cor3SP, cor4SP, cor5SP, cor6SP, cor7SP, NameSP;
											CVehicleEntity* Vehicle = Ent->GetCurrentVehicle();
											Matrix _Transform;
											Vehicle->GetTransform(&_Transform);
											Vector3 Position = Vector3(_Transform.m[3][0], _Transform.m[3][1], _Transform.m[3][2]);
											glm::vec3 glmPos = glm::vec3(Position.x, Position.y, Position.z);

											Color BoxColor = Color(0, 0, 150, 255); 

											//to fix the vehicle friend/enemy color issue (all of them having the same color), change the following line
											//https://www.unknowncheats.me/forum/battlefield-1-a/210381-bf1-internal-hack-source-3.html
											//if (Vehicle->GetTeam() != LocalPlayer->GetTeam())
											if (Ent->GetTeam() != LocalPlayer->GetTeam())
												BoxColor = Color(255, 220, 0, 255);
											else
												if (!Features::ESPShowFriends)
													continue;

											Vehicle->GetAABB(&_AABB);

											glm::vec3 min = glm::vec3(_AABB.AABB.m_Min.x, _AABB.AABB.m_Min.y, _AABB.AABB.m_Min.z);
											glm::vec3 max = glm::vec3(_AABB.AABB.m_Max.x, _AABB.AABB.m_Max.y, _AABB.AABB.m_Max.z);
											glm::vec3 cor2(max.x, min.y, min.z);
											glm::vec3 cor3(max.x, min.y, max.z);
											glm::vec3 cor4(min.x, min.y, max.z);
											glm::vec3 cor5(min.x, max.y, max.z);
											glm::vec3 cor6(min.x, max.y, min.z);
											glm::vec3 cor7(max.x, max.y, min.z);
											glm::mat3 TransformMatrix(glm::vec3(_Transform.m[0][0], _Transform.m[1][0], _Transform.m[2][0]),
																	  glm::vec3(_Transform.m[0][1], _Transform.m[1][1], _Transform.m[2][1]),
																	  glm::vec3(_Transform.m[0][2], _Transform.m[1][2], _Transform.m[2][2]));

											glm::vec3 crnr2 = glmPos + cor2 * TransformMatrix;
											glm::vec3 crnr3 = glmPos + cor3 * TransformMatrix;
											glm::vec3 crnr4 = glmPos + cor4 * TransformMatrix;
											glm::vec3 crnr5 = glmPos + cor5 * TransformMatrix;
											glm::vec3 crnr6 = glmPos + cor6 * TransformMatrix;
											glm::vec3 crnr7 = glmPos + cor7 * TransformMatrix;
											min = glmPos + min * TransformMatrix;
											max = glmPos + max * TransformMatrix;

											Vector3 MyHeadPos;
											/*//Hope it's not wrong to post it here, but sometimes the medic is unable to revice someone. https://github.com/Tonyx97/BF1-Internal/issues/7
											Vector3 MyHeadPos = LPSoldier->GetPosition(); // Feet Position, fix for Revive Bug
											//LocalPlayer->GetSoldier()->GetBonePosition(MyHeadPos, HEAD); // Creates Revive Bug*/

											if (Mem::IsValid(LPSoldier) && Mem::IsValid(LPSoldier->prediction))  //local player on feet; GetPosition() works OK
												Vector3 MyHeadPos = LPSoldier->GetPosition(); // Feet Position, fix for Revive Bug
											else     //local player in vehicle; GetPosition() crash! => Take position from HEAD, not from GetPosition() prediction
												LPSoldier->GetBonePositionLP(MyHeadPos, HEAD); // Creates Revive Bug. Not aymore

											float dist = Vector3::Distance(Vector3(min.x, min.y, min.z), MyHeadPos);
											if ( (Mem::IsValid(LocalPlayer->GetCurrentVehicle()) ? true : (dist >= .01f && dist < Features::ESPDistance) ) &&  //if in vehicle show all, no matter distance (bugfix)
												/*dist >= 1.f && dist < Features::ESPDistance &&*/
												Game::W2S(Vector3(min.x, min.y, min.z), minSP) &&
												Game::W2S(Vector3(max.x, max.y, max.z), maxSP) &&
												Game::W2S(Vector3(crnr2.x, crnr2.y, crnr2.z), cor2SP) &&
												Game::W2S(Vector3(crnr3.x, crnr3.y, crnr3.z), cor3SP) &&
												Game::W2S(Vector3(crnr4.x, crnr4.y, crnr4.z), cor4SP) &&
												Game::W2S(Vector3(crnr5.x, crnr5.y, crnr5.z), cor5SP) &&
												Game::W2S(Vector3(crnr6.x, crnr6.y, crnr6.z), cor6SP) &&
												Game::W2S(Vector3(crnr7.x, crnr7.y, crnr7.z), cor7SP) &&
												Game::W2S(Vector3(glmPos.x, glmPos.y /*+ (max.y - min.y)*/, glmPos.z), NameSP))
											{
												DrawDXText(NameSP.x, NameSP.y, 0.8f, true, 0.f, Color(0, 255, 255, 255), true, "%s", Vehicle->GetName().c_str());
												
												DrawLine(minSP.x, minSP.y, cor4SP.x, cor4SP.y, BoxColor);
												DrawLine(minSP.x, minSP.y, cor2SP.x, cor2SP.y, BoxColor);
												DrawLine(minSP.x, minSP.y, cor6SP.x, cor6SP.y, BoxColor);
												DrawLine(maxSP.x, maxSP.y, cor5SP.x, cor5SP.y, BoxColor);
												DrawLine(maxSP.x, maxSP.y, cor7SP.x, cor7SP.y, BoxColor);
												DrawLine(maxSP.x, maxSP.y, cor3SP.x, cor3SP.y, BoxColor);
												DrawLine(cor7SP.x, cor7SP.y, cor6SP.x, cor6SP.y, BoxColor);
												DrawLine(cor7SP.x, cor7SP.y, cor2SP.x, cor2SP.y, BoxColor);
												DrawLine(cor6SP.x, cor6SP.y, cor5SP.x, cor5SP.y, BoxColor);
												DrawLine(cor6SP.x, cor6SP.y, cor7SP.x, cor7SP.y, BoxColor);
												DrawLine(cor2SP.x, cor2SP.y, cor3SP.x, cor3SP.y, BoxColor);
												DrawLine(cor3SP.x, cor3SP.y, cor4SP.x, cor4SP.y, BoxColor);
												DrawLine(cor4SP.x, cor4SP.y, cor5SP.x, cor5SP.y, BoxColor);
											}
										}
									}
									//outputFile.close();  //log
								}
							}
						}
					}
			}
		}
		/////log show on screen/////////////////////////
		if (!SSCleaner->NeedSS && Mem::IsValid(Log.get())) //log got something => alert, show on screen last line
			if (Log->Buf.size() && LastLogSize != Log->Buf.size()){
				if (GetTickCount() - LastTick < 15000) { // Show time 15 sec.
					if (GetTickCount() - LastTick > 1000) //show 1 sec after added log just to be safe when use of Features::giveCleanSS to not capture log printed on screen (although they are one thread sequently called)
						DrawDXText(ScreenSX / 2.f, (ScreenSY * 0.91f) - 25.f, 1.f, true, 0.f, Color(255, 0, 70, 200), true, "%s", Log->GetLastLine());
				}
				else if (!ShowLogAlert) {
					ShowLogAlert = true;
					LastTick = GetTickCount();
				}
				else {
					ShowLogAlert = false;
					LastLogSize = Log->Buf.size();
				}
			}
		/////////////////////////////////////////////
		ImGui::Render();
	}

	DX11End();
	std::lock_guard<std::mutex> lock(DX11_mutex);  //keep mutex on last line because I want hkCopyResource_blocked.bmp to have all hacks drawn on the screenshot, to see what exactly is blocked
}

BOOLEAN _Initialized = FALSE;

HRESULT HookedDX11Renderer(IDXGISwapChain* _SwapChain, UINT SyncInterval, UINT Flags)
{
	
	if (!_Initialized) _Initialized = DX11->InitDX11RenderStuff(_SwapChain);

	DX11->DX11RenderScene();

	return DX11HookPresent(_SwapChain, SyncInterval, Flags);
}

DWORD DX11Renderer::InitDevice(HMODULE _DllModule, const wchar_t* HWNDTarget)
{		
	
	while (true)
	{
		if (GetAsyncKeyState(VK_INSERT)) break;  //VK_END
		Sleep(40);
	}

	HWND hWnd = FindWindow(0, HWNDTarget);
	WTarget = hWnd;

	Target = HWNDTarget;
	DllModule = _DllModule;

	RECT TarDims;
	GetWindowRect(hWnd, &TarDims);
	ScreenX = TarDims.left;
	ScreenY = TarDims.top;
	ScreenSX = TarDims.right - TarDims.left;
	ScreenSY = TarDims.bottom - TarDims.top;

	Crosshair = false;
	//radar_size = 200;

	IDXGISwapChain* sc = Mem::ReadPtr<IDXGISwapChain*>({ OFFSET_DXRENDERER, 0x820, 0x280,0x10 }, true);
	if (!Mem::IsValid(sc))
		exit(1);

	QWORD* EngineSwapChainVT = *(QWORD**)(sc);

	PresentHook = new PLH::Detour();
	PresentHook->SetupHook((BYTE*)EngineSwapChainVT[8], (BYTE*)HookedDX11Renderer);
	PresentHook->Hook();
	DX11HookPresent = PresentHook->GetOriginal<D3D11PresentHook>();

	pfh = NULL;
	pfh = new CVMTHookManager64((QWORD**)BorderInputNode::Get()->input_node);  //disable this if you dont have BORDER_INPUT_NODE
	PerFrameHook = (PerFrameHook_t)pfh->dwGetMethodAddress(3);   //disable this if you dont have BORDER_INPUT_NODE
	pfh->dwHookMethod((DWORD64)HookedPerFrame, 3);    //disable this if you dont have BORDER_INPUT_NODE
	
	IHooks::Initialize(Target);

	while (true)
	{
		if (GetAsyncKeyState(VK_DELETE)) break;
		Sleep(200);
	}

	Game::ThreadState[0] = FALSE;

	while (true)
	{
		if (!Game::ThreadState[0] && !Game::ThreadState[1] && !Game::ThreadState[2]) break;
		Sleep(100);
	}

	Closing = TRUE;
	IHooks::Restore();
	Sleep(1000);
	CleanupDevice();
	while (true)
		Sleep(9999000);  //do not exit, will make game crash	

	return NULL;
}

BOOLEAN DX11Renderer::InitDX11RenderStuff(IDXGISwapChain* _SwapChain)
{
	void const* shaderByteCode;
	size_t byteCodeLength;

	_SwapChain->GetDevice(__uuidof(DX11Device), (void**)&DX11Device);
	_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&DX11BackBuffer);
	DX11SwapChain = _SwapChain;
	//DX11Device->CreateRenderTargetView(DX11BackBuffer, NULL, &DX11RenderTargetView);

	DX11Device->GetImmediateContext(&DX11DeviceContext);
	Dx11RenderTargetView* pDxRenderTargetView = Mem::ReadPtr<Dx11RenderTargetView*>({ OFFSET_DXRENDERER, 0x820, 0x02B8 }, true);
	if (!pDxRenderTargetView) exit(1);

	ID3D11RenderTargetView* pTargetViews[0x8];
	for (int i = 0; i < pDxRenderTargetView->m_targetCount; i++)
		pTargetViews[i] = pDxRenderTargetView->m_renderTargetViews[i];

	DX11DeviceContext->OMSetRenderTargets(pDxRenderTargetView->m_targetCount, pTargetViews, pDxRenderTargetView->m_depthStencilView);


	DX11SpriteBatch.reset(new SpriteBatch(DX11DeviceContext));
	DX11Purista.reset(new SpriteFont(DX11Device, PuristaFont, sizeof(PuristaFont)));
	DX11Batch.reset(new PrimitiveBatch<VertexPositionColor>(DX11DeviceContext));
	DX11CommonStates.reset(new CommonStates(DX11Device));
	DX11BatchEffects.reset(new BasicEffect(DX11Device));
	DX11BatchEffects->SetVertexColorEnabled(true);
	StateSaver = new DXTKStateSaver;

	InitImGUI(WTarget, DX11Device, DX11DeviceContext);

	DXGI_SWAP_CHAIN_DESC sd;
	_SwapChain->GetDesc(&sd);

	/*D3D11_RENDER_TARGET_VIEW_DESC RenderTarget;
	ZeroMemory(&RenderTarget, sizeof(RenderTarget));
	RenderTarget.Format = sd.BufferDesc.Format;
	RenderTarget.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	DX11Device->CreateRenderTargetView(DX11BackBuffer, &RenderTarget, &DX11RenderTargetView);
	DX11DeviceContext->OMSetRenderTargets(1, &DX11RenderTargetView, NULL);*/
	// 	Dx11RenderTargetView* m_pDefaultRenderView; //0x02B8



	DX11BatchEffects->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	DX11Device->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength, &InputLayout);

	XMMATRIX Projection = XMMatrixOrthographicOffCenterRH(0.0f, ScreenSX, ScreenSY, 0.0f, 0.0f, 1.0f);
	DX11BatchEffects->SetProjection(Projection);
	DX11BatchEffects->SetWorld(XMMatrixIdentity());
	DX11BatchEffects->SetView(XMMatrixIdentity());

	SSCleaner->HookBitBlt();
	SSCleaner->HookCopyResource();  //turned it off. The game is calling for many other purposes so cant determine when is for Screanshot and when is not
	SSCleaner->HookCopySubresourceRegion();  //turned it off. The game is calling for many other purposes so cant determine when is for Screanshot and when is not

	Initialized = TRUE;
	return TRUE;
}

void DX11Renderer::DX11Begin()
{
	RestoreState = FALSE;
	StateSaver->saveCurrentState(DX11DeviceContext);

	Dx11RenderTargetView* pDxRenderTargetView = Mem::ReadPtr<Dx11RenderTargetView*>({ OFFSET_DXRENDERER, 0x820, 0x02B8 }, true);
	if (!pDxRenderTargetView) exit(1);

	ID3D11RenderTargetView* pTargetViews[0x8];
	for (int i = 0; i < pDxRenderTargetView->m_targetCount; i++)
		pTargetViews[i] = pDxRenderTargetView->m_renderTargetViews[i];

	DX11DeviceContext->OMSetRenderTargets(pDxRenderTargetView->m_targetCount, pTargetViews, pDxRenderTargetView->m_depthStencilView);


	RestoreState = TRUE;

	DX11SpriteBatch->Begin(SpriteSortMode_Deferred);

	DX11BatchEffects->Apply(DX11DeviceContext);
	DX11DeviceContext->IASetInputLayout(InputLayout);

	DX11Batch->Begin();

	ImGui_ImplDX11_NewFrame();
}

void DX11Renderer::DX11End()
{
	DX11Batch->End();
	DX11SpriteBatch->End();
	if (RestoreState) StateSaver->restoreSavedState();
}

void DX11Renderer::DrawCursor(float x, float y, float size)
{
	DX11SpriteBatch->Draw(CursorTexture.Get(), XMFLOAT2(x + (CursorDesc.Width / 2 * size), y + (CursorDesc.Height / 2 * size)), nullptr, Colors::White, 0.f, XMFLOAT2(CursorDesc.Width / 2, CursorDesc.Height / 2), size);
}

void DX11Renderer::DrawDXText(float x, float y, float scale, bool center, float rot, Color color, bool shadow, const char* Format, ...)
{
	if (!Initialized) return;

	char Buf[1024] = { '\0' };
	va_list VAL;
	va_start(VAL, Format);
	vsprintf_s(Buf, Format, VAL);
	va_end(VAL);
	std::wstring text = ConvertToWStr(Buf);

	XMFLOAT2 pos = { x, y };
	XMFLOAT2 posShadow[4];
	XMVECTOR colora = { color.R() / 255, color.G() / 255, color.B() / 255, color.A() / 255 };
	XMVECTOR coloraShadow = { 0.f, 0.f, 0.f, 0.7f };

	posShadow[0] = { x + 1.f, y + 1.f };
	posShadow[1] = { x - 1.f, y + 1.f };
	posShadow[2] = { x - 1.f, y - 1.f };
	posShadow[3] = { x + 1.f, y - 1.f };

	XMFLOAT2 origin(0, 0);
	if (center)
	{
		XMVECTOR size = DX11Purista->MeasureString(text.c_str());
		float sizeX = XMVectorGetX(size);
		float sizeY = XMVectorGetY(size);
		origin = XMFLOAT2(sizeX / 2, sizeY / 2);
	}

	if (shadow && color.A() / 255.f > 0.f)
	{
		for (unsigned short i = 0; i < 4; i++)
			DX11Purista->DrawString(DX11SpriteBatch.get(), text.c_str(), posShadow[i], coloraShadow, rot, origin, scale * 0.64f, SpriteEffects_None, 0);
	}
	DX11Purista->DrawString(DX11SpriteBatch.get(), text.c_str(), pos, colora, rot, origin, scale * 0.64f, SpriteEffects_None, 0);
}

void DX11Renderer::DrawUnicodeText(float x, float y, float scale, bool center, float rot, Color color, bool shadow, std::wstring Format)
{
	if (!Initialized) return;

	//std::wstring text = ConvertToWStr(Format);

	XMFLOAT2 pos = { x, y };
	XMFLOAT2 posShadow = { x + 1, y + 1 };
	XMVECTOR colora = { color.R() / 255, color.G() / 255, color.B() / 255, color.A() / 255 };
	XMVECTOR coloraShadow = { 0.f, 0.f, 0.f, 255.f };

	XMFLOAT2 origin(0, 0);
	if (center)
	{
		XMVECTOR size = DX11Purista->MeasureString(Format.c_str());
		float sizeX = XMVectorGetX(size);
		float sizeY = XMVectorGetY(size);
		origin = XMFLOAT2(sizeX / 2, sizeY / 2);
	}

	if (shadow) DX11Purista->DrawString(DX11SpriteBatch.get(), Format.c_str(), posShadow, coloraShadow, rot, origin, scale * 0.64f, SpriteEffects_None, 0);
	DX11Purista->DrawString(DX11SpriteBatch.get(), Format.c_str(), pos, colora, rot, origin, scale * 0.64f, SpriteEffects_None, 0);
}


void DX11Renderer::DrawEmptyBox(float x, float y, float x1, float y1, float px, Color color)
{
	FillRGB(x, y, x1, px, color);
	FillRGB(x + x1, y, px, y1, color);
	FillRGB(x, y + y1 - px, x1 + px, px, color);
	FillRGB(x, y, px, y1, color);
}


void DX11Renderer::DrawEmptyBoxTab(float x, float y, float x1, float y1, float px, Color color)
{
	FillRGB(x, y, x1, px, color);
	FillRGB(x + x1, y, px, y1, color);
	FillRGB(x, y, px, y1, color);
}

void DX11Renderer::DrawEmptyBoxTab_2(float x, float y, float x1, float y1, float from, float to, float px, Color color)
{
	FillRGB(x, y, from, px, color);
	FillRGB(x + to, y, x1 - to, px, color);
	FillRGB(x + x1, y, px, y1, color);
	FillRGB(x, y + y1 - px, x1 + px, px, color);
	FillRGB(x, y, px, y1, color);
}

void DX11Renderer::DrawBox(float x, float y, float x1, float y1, Color color)
{
	FillRGB(x, y, x1, y1, color);
}


void DX11Renderer::FillRGB(float x, float y, float x1, float y1, Color color)
{
	FXMVECTOR c1 = { x, y };
	FXMVECTOR c2 = { x + x1, y };
	FXMVECTOR c3 = { x + x1, y + y1 };
	FXMVECTOR c4 = { x, y + y1 };
	FXMVECTOR colora = { color.R() / 255.f, color.G() / 255.f, color.B() / 255.f, color.A() / 255.f };
	VertexPositionColor d1(c1, colora);
	VertexPositionColor d2(c2, colora);
	VertexPositionColor d3(c3, colora);
	VertexPositionColor d4(c4, colora);

	DX11Batch->DrawQuad(d1, d2, d3, d4);
}

void DX11Renderer::Draw3DBox(float x, float y, float x1, float y1, float movement, Color color)
{
	if (!Initialized) return;

	FXMVECTOR c1 = { x, y };
	FXMVECTOR c2 = { x + x1, y };
	FXMVECTOR c3 = { x + x1 - movement, y + y1 };
	FXMVECTOR c4 = { x - movement, y + y1 };
	FXMVECTOR colora = { color.R() / 255.f, color.G() / 255.f, color.B() / 255.f, color.A() / 255.f };
	VertexPositionColor d1(c1, colora);
	VertexPositionColor d2(c2, colora);
	VertexPositionColor d3(c3, colora);
	VertexPositionColor d4(c4, colora);

	DX11Batch->DrawQuad(d1, d2, d3, d4);
}

void DX11Renderer::DrawTriangle(float x, float y, Color color)
{
	if (!Initialized) return;

	FXMVECTOR c1 = { x, y };
	FXMVECTOR c2 = { x - 20.f, y };
	FXMVECTOR c3 = { x, y - 20.f };
	FXMVECTOR colora = { color.R() / 255.f, color.G() / 255.f, color.B() / 255.f, color.A() / 255.f };
	VertexPositionColor d1(c1, colora);
	VertexPositionColor d2(c2, colora);
	VertexPositionColor d3(c3, colora);

	DX11Batch->DrawTriangle(d1, d2, d3);
}


void DX11Renderer::DrawCircle(float x, float y, Color color, float radius, int _s)
{
	if (!Initialized) return;
	float Angle = (360.0f / _s)*(3.1415926f / 180); 

	float Cos = cos(Angle);
	float Sin = sin(Angle);

	XMVECTOR vec = { radius, 0 };
	XMVECTOR pos = { x, y };

	for (unsigned short i = 0; i < _s; ++i)
	{
		XMVECTOR rot = { Cos*XMVectorGetX(vec) - Sin*XMVectorGetY(vec), Sin*XMVectorGetX(vec) + Cos*XMVectorGetY(vec) };
		rot += pos;
		vec += pos;
		DrawLine(XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetX(rot), XMVectorGetY(rot), color);
		vec = rot - pos;
	}
}


void DX11Renderer::DrawLine(float x0, float y0, float x1, float y1, Color color)
{
	if (!Initialized) return;

	FXMVECTOR pos1 = { x0, y0 };
	FXMVECTOR pos2 = { x1, y1 };
	FXMVECTOR colora = { color.R() / 255, color.G() / 255, color.B() / 255, color.A() / 255 };
	VertexPositionColor draw1(pos1, colora);
	VertexPositionColor draw2(pos2, colora);

	DX11Batch->DrawLine(draw1, draw2);
}


void DX11Renderer::CleanupDevice()
{
	DX11Device->Release();
	DX11DeviceContext->Release();
	PresentHook->UnHook();
	//DetourPerFrameHook->UnHook();
	if (pfh != NULL ) pfh->UnHook();
	if (SSCleaner->BitBltState) SSCleaner->BitBltHook->UnHook();
	if (SSCleaner->CopyResourceState) SSCleaner->CopyResourceHook->UnHook();
	if (SSCleaner->CopySubresourceRegionState) SSCleaner->CopySubresourceHook->UnHook();
}


std::wstring DX11Renderer::ConvertToWStr(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

//DX11 STATE SAVER
//DX11 STATE SAVER
//DX11 STATE SAVER
//DX11 STATE SAVER



DXTKStateSaver::DXTKStateSaver() : m_savedState(false), m_featureLevel(D3D_FEATURE_LEVEL_11_0), m_pContext(NULL), m_primitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED), m_pInputLayout(NULL), m_pBlendState(NULL),
m_sampleMask(0xFFFFFFFF), m_pDepthStencilState(NULL), m_stencilRef(0), m_pRasterizerState(NULL), m_pPSSRV(NULL), m_pSamplerState(NULL), m_pVS(NULL), m_numVSClassInstances(0), m_pVSConstantBuffer(NULL), m_pGS(NULL),
m_numGSClassInstances(0), m_pGSConstantBuffer(NULL), m_pGSSRV(NULL), m_pPS(NULL), m_numPSClassInstances(0), m_pHS(NULL), m_numHSClassInstances(0), m_pDS(NULL), m_numDSClassInstances(0), m_pVB(NULL), m_vertexStride(0),
m_vertexOffset(0), m_pIndexBuffer(NULL), m_indexFormat(DXGI_FORMAT_UNKNOWN), m_indexOffset(0)
{
	for (int i = 0; i < 4; ++i)
	{
		m_blendFactor[i] = 0.0f;
	}
	for (int i = 0; i < 256; ++i)
	{
		m_pVSClassInstances[i] = NULL;
		m_pGSClassInstances[i] = NULL;
		m_pPSClassInstances[i] = NULL;
		m_pHSClassInstances[i] = NULL;
		m_pDSClassInstances[i] = NULL;
	}
}

DXTKStateSaver::~DXTKStateSaver()
{
	releaseSavedState();
}

HRESULT DXTKStateSaver::saveCurrentState(ID3D11DeviceContext* pContext)
{
	if (m_savedState) releaseSavedState();
	if (pContext == NULL) return E_INVALIDARG;

	ID3D11Device* pDevice;
	pContext->GetDevice(&pDevice);
	if (pDevice != NULL)
	{
		m_featureLevel = pDevice->GetFeatureLevel();
		pDevice->Release();
	}

	pContext->AddRef();
	m_pContext = pContext;
	m_pContext->IAGetPrimitiveTopology(&m_primitiveTopology);
	m_pContext->IAGetInputLayout(&m_pInputLayout);
	m_pContext->OMGetBlendState(&m_pBlendState, m_blendFactor, &m_sampleMask);
	m_pContext->OMGetDepthStencilState(&m_pDepthStencilState, &m_stencilRef);
	m_pContext->RSGetState(&m_pRasterizerState);
	m_numVSClassInstances = 256;
	m_pContext->VSGetShader(&m_pVS, m_pVSClassInstances, &m_numVSClassInstances);
	m_pContext->VSGetConstantBuffers(0, 1, &m_pVSConstantBuffer);
	m_numPSClassInstances = 256;
	m_pContext->PSGetShader(&m_pPS, m_pPSClassInstances, &m_numPSClassInstances);
	m_pContext->PSGetShaderResources(0, 1, &m_pPSSRV);
	pContext->PSGetSamplers(0, 1, &m_pSamplerState);

	if (m_featureLevel >= D3D_FEATURE_LEVEL_10_0)
	{
		m_numGSClassInstances = 256;
		m_pContext->GSGetShader(&m_pGS, m_pGSClassInstances, &m_numGSClassInstances);
		m_pContext->GSGetConstantBuffers(0, 1, &m_pGSConstantBuffer);

		m_pContext->GSGetShaderResources(0, 1, &m_pGSSRV);

		if (m_featureLevel >= D3D_FEATURE_LEVEL_11_0)
		{
			m_numHSClassInstances = 256;
			m_pContext->HSGetShader(&m_pHS, m_pHSClassInstances, &m_numHSClassInstances);

			m_numDSClassInstances = 256;
			m_pContext->DSGetShader(&m_pDS, m_pDSClassInstances, &m_numDSClassInstances);
		}
	}

	m_pContext->IAGetVertexBuffers(0, 1, &m_pVB, &m_vertexStride, &m_vertexOffset);
	m_pContext->IAGetIndexBuffer(&m_pIndexBuffer, &m_indexFormat, &m_indexOffset);
	m_savedState = true;

	return S_OK;
}

HRESULT DXTKStateSaver::restoreSavedState()
{
	if (!m_savedState) return E_FAIL;

	m_pContext->IASetPrimitiveTopology(m_primitiveTopology);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pContext->OMSetBlendState(m_pBlendState, m_blendFactor, m_sampleMask);
	m_pContext->OMSetDepthStencilState(m_pDepthStencilState, m_stencilRef);

	m_pContext->RSSetState(m_pRasterizerState);

	m_pContext->VSSetShader(m_pVS, m_pVSClassInstances, m_numVSClassInstances);
	m_pContext->VSSetConstantBuffers(0, 1, &m_pVSConstantBuffer);

	m_pContext->PSSetShader(m_pPS, m_pPSClassInstances, m_numPSClassInstances);
	m_pContext->PSSetShaderResources(0, 1, &m_pPSSRV);
	m_pContext->PSSetSamplers(0, 1, &m_pSamplerState);

	if (m_featureLevel >= D3D_FEATURE_LEVEL_10_0)
	{
		m_pContext->GSSetShader(m_pGS, m_pGSClassInstances, m_numGSClassInstances);
		m_pContext->GSSetConstantBuffers(0, 1, &m_pGSConstantBuffer);

		m_pContext->GSSetShaderResources(0, 1, &m_pGSSRV);

		if (m_featureLevel >= D3D_FEATURE_LEVEL_11_0)
		{
			m_pContext->HSSetShader(m_pHS, m_pHSClassInstances, m_numHSClassInstances);

			m_pContext->DSSetShader(m_pDS, m_pDSClassInstances, m_numDSClassInstances);
		}
	}

	m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &m_vertexStride, &m_vertexOffset);

	m_pContext->IASetIndexBuffer(m_pIndexBuffer, m_indexFormat, m_indexOffset);

	return S_OK;
}

void DXTKStateSaver::ZeroShaders(ID3D11DeviceContext* pContext)
{
	pContext->VSSetShader(NULL, NULL, 0);
	pContext->PSSetShader(NULL, NULL, 0);
	pContext->HSSetShader(NULL, NULL, 0);
	pContext->DSSetShader(NULL, NULL, 0);
	pContext->GSSetShader(NULL, NULL, 0);
}

void DXTKStateSaver::releaseSavedState()
{
	m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pBlendState);
	for (int i = 0; i < 4; ++i)
		m_blendFactor[i] = 0.0f;
	m_sampleMask = 0xffffffff;
	SAFE_RELEASE(m_pDepthStencilState);
	m_stencilRef = 0;
	SAFE_RELEASE(m_pRasterizerState);
	SAFE_RELEASE(m_pPSSRV);
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pVS);
	for (UINT i = 0; i < m_numVSClassInstances; ++i)
		SAFE_RELEASE(m_pVSClassInstances[i]);
	m_numVSClassInstances = 0;
	SAFE_RELEASE(m_pVSConstantBuffer);
	SAFE_RELEASE(m_pGS);
	for (UINT i = 0; i < m_numGSClassInstances; ++i)
		SAFE_RELEASE(m_pGSClassInstances[i]);
	m_numGSClassInstances = 0;
	SAFE_RELEASE(m_pGSConstantBuffer);
	SAFE_RELEASE(m_pGSSRV);
	SAFE_RELEASE(m_pPS);
	for (UINT i = 0; i < m_numPSClassInstances; ++i)
		SAFE_RELEASE(m_pPSClassInstances[i]);
	m_numPSClassInstances = 0;
	SAFE_RELEASE(m_pHS);
	for (UINT i = 0; i < m_numHSClassInstances; ++i)
		SAFE_RELEASE(m_pHSClassInstances[i]);
	m_numHSClassInstances = 0;
	SAFE_RELEASE(m_pDS);
	for (UINT i = 0; i < m_numDSClassInstances; ++i)
		SAFE_RELEASE(m_pDSClassInstances[i]);
	m_numDSClassInstances = 0;
	SAFE_RELEASE(m_pVB);
	m_vertexStride = 0;
	m_vertexOffset = 0;
	SAFE_RELEASE(m_pIndexBuffer);
	m_indexFormat = DXGI_FORMAT_UNKNOWN;
	m_indexOffset = 0;

	SAFE_RELEASE(m_pContext);
	m_featureLevel = D3D_FEATURE_LEVEL_11_0;

	m_savedState = false;
}

std::unique_ptr<DX11Renderer> DX11;
std::unique_ptr<CLog> Log;

