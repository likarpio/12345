#include "memory.h"


DWORD memory::GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{  
        DWORD modBaseAddr = 0;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
        if (hSnap != INVALID_HANDLE_VALUE)
        {
            MODULEENTRY32 modEntry;
            modEntry.dwSize = sizeof(modEntry);
            if (Module32First(hSnap, &modEntry))
            {
                do
                {
                    if (!_wcsicmp(modEntry.szModule, modName))
                    {
                        modBaseAddr = (DWORD)modEntry.modBaseAddr;
                        break;
                    }
                } while (Module32Next(hSnap, &modEntry));
            }
        }

        CloseHandle(hSnap);
        return modBaseAddr;
}



/*
uintptr_t memory::readMemory(DWORD addy, HANDLE handle)
{
    uintptr_t resAddress;
    ReadProcessMemory(handle, (LPCVOID)addy, &resAddress, 4, nullptr);
    
    return resAddress;
}
*/

