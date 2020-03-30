#pragma once

#include <string>

#include <windows.h>

namespace Utils
{

std::wstring HRESULTToWString(HRESULT hr)
{
    LPWSTR messageBuffer = nullptr;
    DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER
                    | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

    size_t size = FormatMessageW(
        dwFlags,
        nullptr,
        hr,
        MAKELANGID
        (LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR) &messageBuffer,
        0,
        nullptr);

    std::wstring message(messageBuffer, size);

    LocalFree(messageBuffer);

    return message;
}

std::string HRESULTToString(HRESULT hr)
{
    LPSTR messageBuffer = nullptr;
    DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER
                    | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

    size_t size = FormatMessageA(
        dwFlags,
        nullptr,
        hr,
        MAKELANGID
        (LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR) &messageBuffer,
        0,
        nullptr);

    std::string message(messageBuffer, size);

    LocalFree(messageBuffer);

    return message;
}

}
