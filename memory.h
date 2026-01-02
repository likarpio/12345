#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>




class memory
{
	
public:
	DWORD GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

	template <typename T>
	static T readMem(DWORD address, HANDLE hProcess)
	{
		T value{};
		ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), nullptr);
		return value;
	}

	template <typename T>
	static void writeMem(DWORD address, const T& value, HANDLE hProcess)
	{
		WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(address), &value, sizeof(T), nullptr);
	}

};

