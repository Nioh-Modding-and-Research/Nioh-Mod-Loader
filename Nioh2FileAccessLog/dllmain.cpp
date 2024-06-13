// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "structs.h"
#include "Signature.h"
#include "config.h"
#include "mod.h"
#include <stdio.h>
#include <filesystem>
#include <Psapi.h>

#pragma region Signatures

void* ArchiveLoad = sigScan(
    "\x48\x89\x5C\x24\x2A\x55\x56\x41\x54\x41\x56\x41\x57\x48\x81\xEC\x90\x05\x00\x00",
    "xxxx?xxxxxxxxxxxxxxx");

void* LoadInf = sigScan(
    "\x48\x89\x5C\x24\x2A\x48\x89\x6C\x24\x2A\x48\x89\x74\x24\x2A\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\xB0\x05\x00\x00",
    "xxxx?xxxx?xxxx?xxxxxxxxxxxxxxxx");

void* LoadFileNioh2 = sigScan(
    "\x48\x8B\xC4\x55\x57\x41\x56\x48\x8D\xA8\x2A\x2A\x2A\x2A\x48\x81\xEC\xA0\x04\x00\x00",
    "xxxxxxxxxx????xxxxxxx");

void* LoadFileNioh1 = sigScan(
    "\x48\x8B\xC4\x55\x57\x41\x56\x48\x8D\xA8\x2A\x2A\x2A\x2A\x48\x81\xEC\xB0\x04\x00\x00",
    "xxxxxxxxxx????xxxxxxx");

#pragma endregion

#pragma region Functions

bool FileExists(std::string path)
{
    return std::filesystem::exists("..\\" + path);
}

void PrintModulePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    printf("[DebugLog] Module Path: %S\n", buffer);
}

DWORD_PTR GetProcessBaseAddress(DWORD processID)
{
    DWORD_PTR   baseAddress = 0;
    HANDLE      processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    HMODULE* moduleArray;
    LPBYTE      moduleArrayBytes;
    DWORD       bytesRequired;

    if (processHandle)
    {
        if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
        {
            if (bytesRequired)
            {
                moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);

                if (moduleArrayBytes)
                {
                    unsigned int moduleCount;

                    moduleCount = bytesRequired / sizeof(HMODULE);
                    moduleArray = (HMODULE*)moduleArrayBytes;

                    if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
                    {
                        baseAddress = (DWORD_PTR)moduleArray[0];
                    }

                    LocalFree(moduleArrayBytes);
                }
            }
        }

        CloseHandle(processHandle);
    }

    return baseAddress;
}

#pragma endregion

#pragma region Hooks

HOOK(HANDLE, __stdcall, hook_CreateFileW, &CreateFileW, LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE  hTemplateFile)
{
    if (wcsstr(lpFileName, L"Nioh2")) {
        printf("[File Logger] %S\r\n", lpFileName);
    }
    return orig_hook_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HOOK(DWORD, _stdcall, hook_GetFileAttributesW, &GetFileAttributesW, LPCWSTR lpFileName)
{
    printf("[File Logger] %S\r\n", lpFileName);
    return orig_hook_GetFileAttributesW(lpFileName);
}

HOOK(void, __stdcall, hook_ArchiveLoad, ArchiveLoad, u64 param1)
{
    printf("[Debuglog] ArchiveLoad(%p)\n", param1);
    return orig_hook_ArchiveLoad(param1);
}

HOOK(void, _stdcall, hook_LoadInf, LoadInf, u64 param1)
{
    printf("[Debuglog] LoadInf(%p)\n", param1);
    return orig_hook_LoadInf(param1);
}

HOOK(void, __stdcall, hook_LoadFileNioh2, LoadFileNioh2, char* param1, char *param2, u8 param3)
{
    printf("[FileLog] Loaded file %s\n", param2);
    return orig_hook_LoadFileNioh2(param1, param2, param3);
}

HOOK(void, __stdcall, hook_LoadFileNioh1, LoadFileNioh1, char* param1, char* param2, u8 param3)
{
    printf("[FileLog] Loaded file %s\n", param2);
    return orig_hook_LoadFileNioh1(param1, param2, param3);
}

#pragma endregion

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    char* modspath = new char[0x100];

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        config::init();
        if (!GetConsoleWindow() && config::enableConsole) // open console output
        {
            AllocConsole();
            AttachConsole(GetCurrentProcessId());
            freopen("CON", "w", stdout);
        }
        printf("[DebugLog] Base Address: %p\n", GetProcessBaseAddress(GetCurrentProcessId()));
        PrintModulePath();
        
        strcpy(modspath, "..\\");
        strcat(modspath, config::ModsPath.c_str());

        if (std::filesystem::exists(modspath)) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(modspath))
            {
                if (std::filesystem::path(entry.path()).extension() != ".toml")
                    continue;
                printf("[ModConfigLog] processing: %S", entry.path().filename().c_str());
                printf("\n");

                auto overrides = mod::Load(entry.path().string());

                config::fileOverrides.insert(overrides.begin(), overrides.end());
            }
        }
        else {
            printf("[ModConfigLog] Could not find mods path \"%s\" \n", modspath);
        }
        
        if (config::logFileLoading) {
            if (LoadFileNioh2) {
                printf("[FunctionLog] LoadFile found at %p\n", LoadFileNioh2);
                INSTALL_HOOK(hook_LoadFileNioh2);
            }
            else if (LoadFileNioh1) {
                printf("[FunctionLog] LoadFile found at %p\n", LoadFileNioh2);
                INSTALL_HOOK(hook_LoadFileNioh1);
            }
        }

        if (config::fileOverrides.size() > 0 && config::enableFileOverride)
        {
            for (auto kvp : config::fileOverrides)
            {
                if (config::extendedPathLength == false && kvp.second.size() > 64) {
                    printf("[FileReplaceLog] replacement file path for \"%s\" is too long, path is %zi current limit is 64 characters\n", kvp.first.c_str(), kvp.second.size());
                    continue;
                }
                else if (config::extendedPathLength == true && kvp.second.size() > 79) {
                    printf("[FileReplaceLog] replacement file path for \"%s\" is too long, path is %zi current limit is 79 characters\n", kvp.first.c_str(), kvp.second.size());
                    continue;
                }

                if (!FileExists(kvp.second))
                {
                    printf("[FileReplaceLog] File \"%s\" does not exist\n", kvp.second.c_str());
                    continue;
                }

                printf("[FileReplaceLog] replacing filename %s with %s\n", kvp.first.c_str(), kvp.second.c_str());

                void* NameScan = sigScan(
                    kvp.first.c_str(),
                    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

                if (NameScan)
                {
                    //printf("[FileReplaceLog] location of %s found at %p\n", kvp.first.c_str(), NameScan);
                    for (int i = 0; i < kvp.second.size(); i++)
                    {
                        WRITE_MEMORY(((u64)NameScan + i), char, kvp.second[i]);
                    }
                    WRITE_MEMORY(((u64)NameScan + kvp.second.size()), char, "\0");
                }
            }
        }



        return TRUE;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

