// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

using ThreadHandler = std::function<void(HANDLE)>;

void foreachOtherThread(ThreadHandler cb) {
    auto thisThreadId = GetCurrentThreadId();
    auto currentProcessId = GetCurrentProcessId();

    // https://stackoverflow.com/a/16684288
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, currentProcessId);
    if (h != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te)) {
            do {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID)) {
                    // Suspend all threads EXCEPT the one we want to keep running
                    if (te.th32ThreadID != thisThreadId && te.th32OwnerProcessID == currentProcessId) {
                        HANDLE thread = ::OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
                        if (thread != NULL) {
                            cb(thread);
                            CloseHandle(thread);
                        }
                    }
                }
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }

}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        foreachOtherThread([](HANDLE thread) { SuspendThread(thread); });
        MessageBoxA(NULL, "Press OK to continue.", "Pause on Load", MB_OK);
        foreachOtherThread([](HANDLE thread) { ResumeThread(thread); });
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

