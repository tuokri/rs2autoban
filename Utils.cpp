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

std::string Utils::HRESULTToString(HRESULT hr)
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

std::string Utils::WideToUtf8(std::wstring var)
{
    std::wstring wstr = std::move(var);

    int count = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        static_cast<int>(wstr.length()),
        nullptr,
        0,
        nullptr,
        nullptr);

    std::string str(count, 0);

    WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        -1,
        &str[0],
        count,
        nullptr,
        nullptr);

    return str;
}
