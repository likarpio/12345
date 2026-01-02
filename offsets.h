#pragma once
#include <iostream>


namespace offsets
{
	constexpr DWORD localPlayer = 0x17E0A8;
	constexpr DWORD health = 0xEC;
	constexpr DWORD name = 0x205;
	constexpr DWORD locJump = 0x5D; // 1 floor - 0 jump ?
	constexpr DWORD teamNum = 0x30C;

	constexpr DWORD entityList = 0x18AC04;

	//entity Positions (gilt auch für locPlayer)
	constexpr DWORD entityPosX = 0x4;
	constexpr DWORD entityPosY = 0x8;
	constexpr DWORD entityPosZ = 0xC; //4.5 head

	constexpr DWORD viewMatrix = 0x57DFD0; //static address

	constexpr DWORD viewAngleYaw = 0x34;
	constexpr DWORD viewAnglePitch = 0x38;

}
