typedef struct IUnknown IUnknown;

#include <Windows.h>

#include <filesystem>

#include "DW2Lang.h"

bool isTextSection(const BYTE* arr, size_t size) {
    const char* textSection = ".text";
    return size >= strlen(textSection) && memcmp(arr, textSection, strlen(textSection)) == 0;
}

bool isDataSection(const BYTE* arr, size_t size) {
    const char* textSection = ".data";
    return size >= strlen(textSection) && memcmp(arr, textSection, strlen(textSection)) == 0;
}

void PatchMemory(uintptr_t targetAddress, BYTE jmpOpcode[], int codeSize)
{
    // Write the modified instruction to the process
    SIZE_T bytesWritten;

    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<LPVOID>(targetAddress), jmpOpcode, codeSize, &bytesWritten);
}

void PatchMemory(uintptr_t targetAddress, void* value, int codeSize)
{
    // Write the modified instruction to the process
    SIZE_T bytesWritten;

    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<LPVOID>(targetAddress), value, codeSize, &bytesWritten);
}

UINT GetProfileInt(LPCSTR key)
{
    return GetPrivateProfileIntA("Settings", key, 0, ".\\wkDLang.ini");
}

std::string GetProfileString(LPCSTR key)
{
    char result[_MAX_PATH];
    GetPrivateProfileStringA("Settings", key, 0, result, _MAX_PATH, ".\\wkDLang.ini");

    if (result != 0)
        return std::string(result);
    else
        return std::string();
}

DW2Lang lang;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        if (!GetProfileInt("Enabled"))
            return 1;

        std::string langPath = GetProfileString("Language");

        if (!std::filesystem::exists(langPath))
            return -1;

        try {
            lang.Read(langPath);

            for (size_t i = 0; i < lang.UserStrings.size(); i++)
            {
                const char* newstr = lang.UserStrings[i].Value.c_str();
                uintptr_t pointer = reinterpret_cast<int>(newstr);

                for (size_t j = 0; j < lang.UserStrings[i].Offsets.size(); j++)
                {
                    PatchMemory(lang.UserStrings[i].Offsets[j], &pointer, 4);
                }
            }
        }
        catch (...)
        {
            return -1;
        }
    }
    break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

