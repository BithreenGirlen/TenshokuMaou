

#include <Windows.h>

#include "win_process.h"

bool win_process::ExecuteBatchSync(const char* pzFilePath)
{
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};

    BOOL iRet = ::CreateProcessA(nullptr, const_cast<char*>(pzFilePath), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    if (iRet)
    {
        DWORD dwRet = ::WaitForSingleObject(pi.hProcess, INFINITE);
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);
        return dwRet == WAIT_OBJECT_0;
    }

    return false;
}

bool win_process::ExecuteProcessSync(const char* pzFilePath, const char* pzCommandLine)
{
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};

    BOOL iRet = ::CreateProcessA(const_cast<char*>(pzFilePath), const_cast<char*>(pzCommandLine), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    if (iRet)
    {
        DWORD dwRet = ::WaitForSingleObject(pi.hProcess, INFINITE);
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);
        return dwRet == WAIT_OBJECT_0;
    }

    return false;
}
