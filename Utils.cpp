#include <locale>

#include "Utils.hpp"

std::wstring Utils::HRESULTToWString(HRESULT hr)
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

std::wstring Utils::Utf8ToWide(std::string var)
{
    std::string str = std::move(var);

    int count = MultiByteToWideChar(
        CP_UTF8,
        0,
        str.c_str(),
        static_cast<int>(str.length()),
        nullptr,
        0);

    std::wstring wstr(count, 0);

    MultiByteToWideChar(
        CP_UTF8,
        0,
        str.c_str(),
        static_cast<int>(str.length()),
        &wstr[0],
        count);

    return wstr;
}
